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
 
static const struct fb_ops epd_fbops = {
        .owner          = THIS_MODULE,
        .fb_read        = fb_sys_read,
        .fb_write       = fb_sys_write,
        .fb_fillrect    = sys_fillrect,
        .fb_copyarea    = sys_copyarea,
        .fb_imageblit   = sys_imageblit,
};
 #endif
