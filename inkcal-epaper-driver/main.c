#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include "fb-epd.h"

#define DRV_NAME  "fb_epd"

static void epd_write_cmd(struct epd_device *epd, u8 cmd)
{
        gpiod_set_value_cansleep(epd->dc, 0);
        spi_write(epd->spi, &cmd, 1);
}

static int epd_write_data(struct epd_device *epd, const u8 *buf, size_t len)
{
    int ret = 0;

    gpiod_set_value_cansleep(epd->dc, 1);

    size_t max = spi_max_transfer_size(epd->spi);
    while (len > 0) 
    {
        size_t n = min(len, max);
        ret = spi_write(epd->spi, buf, n);
        if (ret) 
        {
            PDEBUG("spi_write failed at %zu/%zu: %d\n",
                   (size_t)(buf - (u8*)epd->vram), epd->vram_size, ret);
            return ret;
        }
        buf += n;
        len -= n;
    }
    return 0;
}

static void epd_wait_busy(struct epd_device *epd)
{
        while (gpiod_get_value_cansleep(epd->busy))
        {
               usleep_range(1000, 2000);
	}
}

static void epd_refresh_full(struct epd_device *epd)
{
    /* ① hardware reset ------------------------------------------------ */
    gpiod_set_value_cansleep(epd->reset, 0);
    msleep(10);
    gpiod_set_value_cansleep(epd->reset, 1);
    msleep(10);

    /* ② software reset ------------------------------------------------ */
    epd_write_cmd(epd, 0x12);
    epd_wait_busy(epd);

    /* ③ power-up / booster parameters -------------------------------- */
    epd_write_cmd(epd, 0x06);                 /* BOOSTER_SOFT_START       */
    epd_write_data(epd,(u8[]){0x17,0x17,0x17},3);

    epd_write_cmd(epd, 0x04);                 /* POWER_ON                 */
    epd_wait_busy(epd);

    epd_write_cmd(epd, 0x00);                 /* PANEL_SETTING            */
    epd_write_data(epd,(u8[]){0x1F},1);       /* KW-B/W, LUT from OTP     */

    epd_write_cmd(epd, 0x50);                 /* VCOM AND DATA INTERVAL   */
    epd_write_data(epd,(u8[]){0x97},1);       /* border: LUT1, VBD hi-Z   */

    /* ④ resolution & scan direction ---------------------------------- */
    epd_write_cmd(epd, 0x61);                 /* TRES (width-height)      */
    epd_write_data(epd,(u8[]){0x01,0x08,0x00},3);   /* 0x108 = 264, 0x00B0 = 176 */

    epd_write_cmd(epd, 0x15);                 /* DUSPI (data length)      */
    epd_write_data(epd,(u8[]){0x00},1);

    /* ⑤ set RAM pointer top-left ------------------------------------- */
    epd_write_cmd(epd, 0x16);                 /* GSST (gate start)        */
    epd_write_data(epd,(u8[]){0x00},1);

    epd_write_cmd(epd, 0x4E);                 /* X_POINTER */
    epd_write_data(epd,(u8[]){0x00},1);
    epd_write_cmd(epd, 0x4F);                 /* Y_POINTER */
    epd_write_data(epd,(u8[]){0xB0,0x01},2);
    epd_wait_busy(epd);

    /* ⑥ write black-white RAM ---------------------------------------- */
    epd_write_cmd(epd, 0x24);
    epd_write_data(epd, epd->vram, epd->vram_size);

    /* ⑦ display refresh ---------------------------------------------- */
    epd_write_cmd(epd, 0x22);                 /* DISPLAY_UPDATE_CTRL2 */
    epd_write_data(epd,(u8[]){0xF7},1);       /* LUT0 + LUT1 full update  */
    epd_write_cmd(epd, 0x20);                 /* MASTER_ACTIVATION  */
    epd_wait_busy(epd);

    PDEBUG("full refresh done\n");
}




static int epd_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
        struct epd_device *epd = info->par;
        PDEBUG("ioctl 0x%x\n", cmd);

        if (cmd == FB_EPD_REFRESH_FULL) 
        {
                epd_refresh_full(epd);
                return 0;
        }
        return -ENOTTY;
}

static const struct fb_ops epd_fbops = {
        .owner        = THIS_MODULE,
        .fb_read      = fb_sys_read,
        .fb_write     = fb_sys_write,
        .fb_fillrect  = sys_fillrect,
        .fb_copyarea  = sys_copyarea,
        .fb_imageblit = sys_imageblit,
        .fb_ioctl     = epd_fb_ioctl,
};

static int epd_probe(struct spi_device *spi)
{
        struct fb_info *info;
        struct epd_device *epd;
        int ret;

	PDEBUG("probe\n");

        info = framebuffer_alloc(sizeof(*epd), &spi->dev);
        if (!info)
        {
                return -ENOMEM;
        }
        epd          = info->par;
        epd->info    = info; 

	spi->mode           = SPI_MODE_0;
	spi->bits_per_word  = 8;
	spi->max_speed_hz   = 2000000;
	ret = spi_setup(spi);
	if (ret)
	{
	    return dev_err_probe(&spi->dev, ret, "Failed to setup SPI\n");
	}
	
        epd->spi = spi;
        spi_set_drvdata(spi, epd);

        epd->reset = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
        epd->dc    = devm_gpiod_get(&spi->dev, "dc",    GPIOD_OUT_LOW);
        epd->busy  = devm_gpiod_get(&spi->dev, "busy",  GPIOD_IN);
        
        if (IS_ERR(epd->reset) || IS_ERR(epd->dc) || IS_ERR(epd->busy))
        {
            return dev_err_probe(&spi->dev,-EINVAL,"Missing GPIOs");
	}
	
        epd->vram_size = EPD_STRIDE * EPD_HEIGHT;
        epd->vram = vzalloc(epd->vram_size);
        if (!epd->vram)
        {
            return -ENOMEM;
	}
	
        strscpy(info->fix.id,"fb_epd",sizeof(info->fix.id));
        info->fix.type        = FB_TYPE_PACKED_PIXELS;
        info->fix.visual      = FB_VISUAL_MONO01;
        info->fix.line_length = EPD_STRIDE;
        info->fix.smem_len    = epd->vram_size;

        info->var.xres = info->var.xres_virtual = EPD_WIDTH;
        info->var.yres = info->var.yres_virtual = EPD_HEIGHT;
        info->var.bits_per_pixel = 1;
        info->var.grayscale      = 1;

        info->screen_base = (char __force __iomem *)epd->vram;
        info->screen_size = epd->vram_size;
        info->fbops       = &epd_fbops;

        ret = fb_alloc_cmap(&info->cmap, 2, 0);
        if (ret)
        {
            goto err_vram;
	}
        ret = register_framebuffer(info);
        if (ret)
        {
            goto err_cmap;
        }
        dev_info(&spi->dev,"/dev/fb%d ready (%dx%d-1bpp)\n", info->node, EPD_WIDTH, EPD_HEIGHT);
        return 0;

err_cmap:
        fb_dealloc_cmap(&info->cmap);
err_vram:
        vfree(epd->vram);
        framebuffer_release(info);
        return ret;
}

static int epd_remove(struct spi_device *spi)
{
	PDEBUG("remove\n");
        struct epd_device *epd = spi_get_drvdata(spi);
        unregister_framebuffer(epd->info);
        fb_dealloc_cmap(&epd->info->cmap);
        vfree(epd->vram);
        framebuffer_release(epd->info);
        return 0;
}



static const struct of_device_id epd_dt_ids[] = {
        { .compatible = "waveshare,epd" },
        {}
};
MODULE_DEVICE_TABLE(of, epd_dt_ids);

static struct spi_driver epd_driver = {
        .driver = {
                .name = DRV_NAME,
                .of_match_table = epd_dt_ids,
        },
        .probe  = epd_probe,
        .remove = epd_remove,
};
module_spi_driver(epd_driver);

MODULE_AUTHOR("Jainil Patel");
MODULE_DESCRIPTION("Framebuffer driver for Waveshare 2.7-inch e-paper");
MODULE_LICENSE("Dual BSD/GPL");
