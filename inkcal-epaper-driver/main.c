#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include "fb-epd.h"

#define DRV_NAME  "fb_epd"
#define LO8(x)  ((u8)((x) & 0xFF))
#define HI8(x)  ((u8)(((x) >> 8) & 0xFF))

static void epd_write_cmd(struct epd_device *epd, u8 cmd)
{
        gpiod_set_value_cansleep(epd->dc, 0);
        spi_write(epd->spi, &cmd, 1);
}

static int epd_write_data(struct epd_device *epd, const u8 *buf, size_t len)
{
    size_t max = spi_max_transfer_size(epd->spi);
    int ret = 0;

    gpiod_set_value_cansleep(epd->dc, 1);
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
               msleep(200);
	}
}


static const u8 lut_vcom_dc[44] = {
    0x00, 0x00,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x32, 0x32, 0x00, 0x00, 0x02,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const u8 lut_ww[42] = {
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u8 lut_bw[42] = {
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const u8 lut_bb[42] = {
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u8 lut_wb[42] = {
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static void epd_refresh_full(struct epd_device *epd)
{
    int i;
    gpiod_set_value_cansleep(epd->reset, 0);  msleep(10);
    gpiod_set_value_cansleep(epd->reset, 1);  msleep(200);

    epd_write_cmd(epd, 0x01);                
    epd_write_data(epd,(u8[]){0x03,0x00,0x2B,0x2B,0x09},5);

    epd_write_cmd(epd, 0x06);
    epd_write_data(epd,(u8[]){0x07,0x07,0x17},3);

    epd_write_cmd(epd, 0x04);
    epd_wait_busy(epd);

    epd_write_cmd(epd, 0x00);                      
    epd_write_data(epd,(u8[]){0xAF},1);          

    epd_write_cmd(epd, 0x30);                   
    epd_write_data(epd,(u8[]){0x3A},1);          

    epd_write_cmd(epd, 0x82);                 
    epd_write_data(epd,(u8[]){0x12},1);

    epd_write_cmd(epd, 0x50);                    
    epd_write_data(epd,(u8[]){0x97},1);           

    epd_write_cmd(epd, 0x61);                     
    epd_write_data(epd,(u8[]){HI8(264), LO8(264), HI8(176), LO8(176)},4); 

    epd_write_cmd(epd, 0x20);                  
    for (i = 0; i < 44; i++)
        epd_write_data(epd, (u8[]){lut_vcom_dc[i]}, 1);  

    epd_write_cmd(epd, 0x21);
    for (i = 0; i < 42; i++)
        epd_write_data(epd, (u8[]){lut_ww[i]}, 1);

    epd_write_cmd(epd, 0x22);
    for (i = 0; i < 42; i++)
        epd_write_data(epd, (u8[]){lut_bw[i]}, 1);

    epd_write_cmd(epd, 0x23);
    for (i = 0; i < 42; i++)
        epd_write_data(epd, (u8[]){lut_bb[i]}, 1);

    epd_write_cmd(epd, 0x24);
    for (i = 0; i < 42; i++)
        epd_write_data(epd, (u8[]){lut_wb[i]}, 1);

    epd_write_cmd(epd, 0x4E);  epd_write_data(epd,(u8[]){0x00},1);     
    epd_write_cmd(epd, 0x4F);  epd_write_data(epd,(u8[]){0xB0,0x01},2);    
    epd_wait_busy(epd);

    epd_write_cmd(epd, 0x24);                            
    epd_write_data(epd, epd->vram, epd->vram_size);

    epd_write_cmd(epd, 0x22);  epd_write_data(epd,(u8[]){0xF7},1);
    epd_write_cmd(epd, 0x20);
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
