/*
 * main.c
 *
 *  Created on: April 15, 2025
 *      Author: Jainil Patel
 */

#include <linux/module.h>
#include <linux/init.h>
#include "fb_epd.h"
 #define DEVICE_NAME "fb_epd"
 
MODULE_AUTHOR("Jainil");
MODULE_LICENSE("Dual BSD/GPL");
 
static int hello_init(void)
{
     printk(KERN_ALERT "Hello, world\n");
     return 0;
}

static void hello_exit(void)
{
     printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
