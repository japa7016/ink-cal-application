#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/fb.h>
#include <linux/vmalloc.h>
#include "fb-epd.h"

#define DEVICE_NAME "fb-epd"

static dev_t epd_dev;
static int epd_major;
static int epd_minor;
static struct cdev epd_cdev;

static struct fb_info *epd_fb;
static void *epd_vram;

static int epd_open(struct inode *inode, struct file *file)
{
    PDEBUG("device opened\n");
    return 0;
}

static int epd_release(struct inode *inode, struct file *file)
{
    PDEBUG("device closed\n");
    return 0;
}

static struct file_operations epd_fops = {
    .owner   = THIS_MODULE,
    
    .open    = epd_open,
    .release = epd_release,
};

static int epd_init(void)
{
    int ret;
    PDEBUG("Hello, world\n");

    ret = alloc_chrdev_region(&epd_dev, 0, 1, DEVICE_NAME);
    if (ret < 0)
    {
        PDEBUG("failed to allocate char device region: %d\n", ret);
        return ret;
    }

    epd_major = MAJOR(epd_dev);
    epd_minor = MINOR(epd_dev);

    cdev_init(&epd_cdev, &epd_fops);
    epd_cdev.owner = THIS_MODULE;
    ret = cdev_add(&epd_cdev, epd_dev, 1);
    if (ret)
    {
        unregister_chrdev_region(epd_dev, 1);
        PDEBUG("failed to add cdev: %d\n", ret);
        return ret;
    }

    PDEBUG("registered char device major=%d minor=%d\n", epd_major, epd_minor);
    
    epd_vram = vzalloc(EPD_STRIDE * EPD_HEIGHT);
    if (!epd_vram) 
    {
        ret = -ENOMEM;
        goto err_chrdev;
    }
 
    epd_fb = framebuffer_alloc(0, NULL);
    if (!epd_fb) 
    {
        ret = -ENOMEM;
        goto err_vram;
    }
    
    
    strscpy(epd_fb->fix.id, "fb_epd", sizeof(epd_fb->fix.id));
    epd_fb->fix.type = FB_TYPE_PACKED_PIXELS;
    epd_fb->fix.visual = FB_VISUAL_MONO01;
    epd_fb->fix.line_length = EPD_STRIDE;
    epd_fb->fix.smem_start = 0;
    epd_fb->fix.smem_len = EPD_STRIDE * EPD_HEIGHT;

    epd_fb->var.xres = EPD_WIDTH;
    epd_fb->var.yres = EPD_HEIGHT;
    epd_fb->var.xres_virtual = EPD_WIDTH;
    epd_fb->var.yres_virtual = EPD_HEIGHT;
    epd_fb->var.bits_per_pixel = EPD_BPP;
    epd_fb->var.red.offset = 0;
    epd_fb->var.red.length = 1;
    epd_fb->var.green = epd_fb->var.red;
    epd_fb->var.blue = epd_fb->var.red;
    epd_fb->var.grayscale = 1;

    epd_fb->fbops = &epd_fbops;
    epd_fb->screen_base = (char __force __iomem *)epd_vram;
    epd_fb->screen_size = EPD_STRIDE * EPD_HEIGHT;

    ret = register_framebuffer(epd_fb);
    if (ret) 
    {
        PDEBUG("register_framebuffer failed: %d\n", ret);
        goto err_fb;
    }

    PDEBUG("registered /dev/fb%d  (%dx%d-%dbpp)\n", epd_fb->node, EPD_WIDTH, EPD_HEIGHT, EPD_BPP);
    
    return 0;
    
err_fb:
    framebuffer_release(epd_fb);
err_vram:
    vfree(epd_vram);
err_chrdev:
    cdev_del(&epd_cdev);
    unregister_chrdev_region(epd_dev, 1);
    return ret;    
}

static void epd_exit(void)
{
    unregister_framebuffer(epd_fb);
    framebuffer_release(epd_fb);
    vfree(epd_vram);
    
    cdev_del(&epd_cdev);
    unregister_chrdev_region(MKDEV(epd_major, epd_minor), 1);
    PDEBUG("Goodbye, cruel world\n");
}

module_init(epd_init);
module_exit(epd_exit);

MODULE_AUTHOR("Jainil Patel");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("E-paper framebuffer character driver");

