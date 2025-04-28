#!/usr/bin/env python3
import os, fcntl, sys
from datetime import datetime
from zoneinfo import ZoneInfo
from ds1307 import read_rtc0
from PIL import Image, ImageDraw, ImageFont

# ———  Drawing exactly as before  ———
W, H = 264, 176              # Waveshare 2.7″ V2
img = Image.new("1", (W, H), 255)
draw = ImageDraw.Draw(img)

font_header = ImageFont.truetype(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24
)
font_sub = ImageFont.truetype(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18
)
font_body = ImageFont.load_default()

tz   = ZoneInfo("America/Denver")
now  = datetime.now(tz)
weekday  = now.strftime("%A")
date_str = now.strftime("%B %d")
now  = read_rtc0()           # DS1307 RTC
time_str = now.strftime("%H:%M")

draw.text((10, 10), weekday,   font=font_header, fill=0)
draw.text((10, 40), date_str,  font=font_sub,    fill=0)
draw.text((10, 65), time_str,  font=font_sub,    fill=0)

y0 = 95
draw.line((5, y0, W-5, y0), fill=0)

events = [
    ("9:00 AM",  "Project meeting"),
    ("11:00 AM", "Doctor's appointment"),
    ("12:00 PM", "Lunch"),
    ("2:30 PM",  "Call with client"),
]
y = y0 + 10
for t, desc in events:
    draw.text((10, y),  t,    font=font_body, fill=0)
    draw.text((80, y),  desc, font=font_body, fill=0)
    y += 20

FBDEV = "/dev/fb1"
FB_EPD_REFRESH_FULL = 0x6500      # _IO('e',0)

# Pillow -> packed 1-bit bytes, MSB first, top-to-bottom
fb_bytes = img.tobytes("raw", "1;R")

if len(fb_bytes) != (W * H // 8):
    sys.exit("Unexpected byte count! Got %d" % len(fb_bytes))

try:
    fd = os.open(FBDEV, os.O_RDWR)
    # Seek to start & write full buffer
    os.lseek(fd, 0, os.SEEK_SET)
    os.write(fd, fb_bytes)
    # Trigger full refresh
    fcntl.ioctl(fd, FB_EPD_REFRESH_FULL)
finally:
    os.close(fd)
