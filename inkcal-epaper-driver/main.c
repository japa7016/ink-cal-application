#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "fb_epd.h"

#define DEVICE_NAME "fb_epd"

static dev_t epd_dev;
static int epd_major;
static int epd_minor;
static struct cdev epd_cdev;

static int epd_open(struct inode *inode, struct file *file)
{
    PDEBUG("fb_epd: device opened\n");
    return 0;
}

static int epd_release(struct inode *inode, struct file *file)
{
    PDEBUG("fb_epd: device closed\n");
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
        PDEBUG("fb_epd: failed to allocate char device region: %d\n", ret);
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
        PDEBUG("fb_epd: failed to add cdev: %d\n", ret);
        return ret;
    }

    PDEBUG("fb_epd: registered char device major=%d minor=%d\n", epd_major, epd_minor);
    return 0;
}

static void epd_exit(void)
{
    cdev_del(&epd_cdev);
    unregister_chrdev_region(MKDEV(epd_major, epd_minor), 1);
    PDEBUG("Goodbye, cruel world\n");
}

module_init(epd_init);
module_exit(epd_exit);

MODULE_AUTHOR("Jainil Patel");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("E-paper framebuffer character driver");

