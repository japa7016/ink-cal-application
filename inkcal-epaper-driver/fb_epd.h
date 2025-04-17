 #ifndef FB_EPD_H_
 #define FB_EPD_H_
 
 #define FB_EPD_DEBUG 1  
 
 #undef PDEBUG             
 #ifdef FB_EPD_DEBUG
 #  ifdef __KERNEL__
 #    define PDEBUG(fmt, args...) printk( KERN_DEBUG "aesdchar: " fmt, ## args)
 #  else
 #    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
 #  endif
 #else
 #  define PDEBUG(fmt, args...) 
 #endif
 #endif
