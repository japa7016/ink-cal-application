#!/usr/bin/env python3
import sys
import datetime
import fcntl
import struct
from zoneinfo import ZoneInfo

# add your epd2in7_V2 driver to the path
sys.path.insert(1, './lib')
import epd2in7_V2
from PIL import Image, ImageDraw, ImageFont

# ds1307.py must define read_rtc0() exactly as you have it
from ds1307 import read_rtc0

# ioctl code to read struct rtc_time from /dev/rtc0
RTC_RD_TIME = 0x80247009

def main():
    # Initialize the display
    epd = epd2in7_V2.EPD()
    epd.init()
    epd.Clear()

    # Read RTC time and apply Denver timezone
    now = read_rtc0().replace(tzinfo=ZoneInfo("America/Denver"))
    time_str = now.strftime("%H:%M:%S")

    # Prepare a 1‑bit image canvas
    W, H = epd.width, epd.height
    img = Image.new('1', (W, H), 255)
    draw = ImageDraw.Draw(img)
    font = ImageFont.load_default()

    # Center the time string
    bbox = draw.textbbox((0, 0), time_str, font=font)
    text_w = bbox[2] - bbox[0]
    text_h = bbox[3] - bbox[1]
    x = (W - text_w) // 2
    y = (H - text_h) // 2

    # Draw the RTC time
    draw.text((x, y), time_str, font=font, fill=0)

    # Send to display and put it to sleep
    epd.display(epd.getbuffer(img))
    epd.sleep()

if __name__ == "__main__":
    main()
