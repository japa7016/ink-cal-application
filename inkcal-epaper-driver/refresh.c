// refresh.c – user-space test, no kernel headers
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// This must match your driver’s _IO('e',0):
#define FB_EPD_REFRESH_FULL 0x6500

int main(void) {
    int fd = open("/dev/fb1", O_RDWR);
    if (fd < 0) {
        perror("open /dev/fb1");
        return 1;
    }

    printf("Sending FB_EPD_REFRESH_FULL (0x%X)\n", FB_EPD_REFRESH_FULL);
    if (ioctl(fd, FB_EPD_REFRESH_FULL) < 0) {
        perror("ioctl FB_EPD_REFRESH_FULL");
        close(fd);
        return 1;
    }

    printf("ioctl succeeded!\n");
    close(fd);
    return 0;
}

