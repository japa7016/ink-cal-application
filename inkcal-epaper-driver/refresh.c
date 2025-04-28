// refresh.c
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>    // <- for _IO and friends
#include "fb-epd.h"

int main(void) {
    printf("FB_EPD_REFRESH_FULL = 0x%X\n", FB_EPD_REFRESH_FULL);
    int fd = open("/dev/fb1", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    if (ioctl(fd, FB_EPD_REFRESH_FULL) < 0) {
        perror("ioctl");
        return 1;
    }
    close(fd);
    return 0;
}

