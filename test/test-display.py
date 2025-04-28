#!/usr/bin/env python3
import fcntl, os, sys

# Must match your driver’s fb-epd.h: _IO('e', 0)
FB_EPD_REFRESH_FULL = 0x6500

def refresh(fbdev="/dev/fb1"):
    try:
        fd = os.open(fbdev, os.O_RDWR)
    except OSError as e:
        print(f"Error opening {fbdev}: {e}", file=sys.stderr)
        return 1

    try:
    	print("Executing Ioctl")
    	fcntl.ioctl(fd, FB_EPD_REFRESH_FULL)
    	print(f"{fbdev} refresh ioctl sent")
        except OSError as e:
        print(f"ioctl failed: {e}", file=sys.stderr)
        return 1
    finally:
        os.close(fd)
    return 0

if __name__ == "__main__":
    sys.exit(refresh(sys.argv[1] if len(sys.argv) > 1 else "/dev/fb1"))

