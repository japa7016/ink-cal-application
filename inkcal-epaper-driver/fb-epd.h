 #ifndef FB_EPD_H_
 #define FB_EPD_H_

 #define FB_EPD_DEBUG 1  
 
 #undef PDEBUG             
 #ifdef FB_EPD_DEBUG
 #  ifdef __KERNEL__
 #    define PDEBUG(fmt, args...) printk( KERN_DEBUG "fb_epd: " fmt, ## args)
 #  else
 #    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
 #  endif
 #else
 #  define PDEBUG(fmt, args...) 
 #endif
 
#define EPD_WIDTH   264
#define EPD_HEIGHT  176
#define EPD_BPP     1
#define EPD_STRIDE  (EPD_WIDTH / 8)

#include <linux/fb.h>
#include <linux/vmalloc.h>
#include <linux/gpio/consumer.h>
#include <linux/spi/spi.h>
#include <linux/ioctl.h> 
 
#define FB_EPD_IOC_MAGIC        'e'
#define FB_EPD_REFRESH_FULL     _IO(FB_EPD_IOC_MAGIC, 0) 

struct epd_device {
        struct spi_device *spi;
        struct fb_info    *info;
        void              *vram;
        size_t             vram_size;

        struct gpio_desc  *reset;
        struct gpio_desc  *dc;
        struct gpio_desc  *busy;
}; 

#endif
