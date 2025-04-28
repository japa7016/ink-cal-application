#!/usr/bin/env python3
import os, fcntl, sys
from datetime import datetime
from PIL import Image, ImageDraw, ImageFont

# ─── Frame-buffer parameters ───────────────────────────────
FBDEV  = "/dev/fb1"         # kernel driver registered this node
WIDTH  = 264
HEIGHT = 176
BYTES  = WIDTH * HEIGHT // 8   # 5 808

# ioctl number from fb-epd.h   (_IO('e', 0))
FB_EPD_REFRESH_FULL = 0x6500

# ─── Create a blank 1-bit canvas, MSB first ────────────────
img  = Image.new("1", (WIDTH, HEIGHT), 255)        # 255 = white
draw = ImageDraw.Draw(img)

# fonts present if you installed ttf-dejavu or similar
font_header = ImageFont.truetype(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24
)
font_sub = ImageFont.truetype(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18
)
font_body = ImageFont.load_default()

# ─── Header: weekday + date ────────────────────────────────
today    = datetime.now()
weekday  = today.strftime("%A")
date_str = today.strftime("%B %d")

draw.text((10, 10), weekday,  font=font_header, fill=0)
draw.text((10, 40), date_str, font=font_sub,    fill=0)

# ─── Separator line ────────────────────────────────────────
y0 = 65
draw.line((5, y0, WIDTH-5, y0), fill=0)

# ─── Schedule items ────────────────────────────────────────
events = [
    ("9:00 AM",  "Project meeting"),
    ("11:00 AM", "Doctor's appointment"),
    ("12:00 PM", "Lunch"),
    ("2:30 PM",  "Call with client"),
]
y = y0 + 10
for t, desc in events:
    draw.text((10,  y), t,    font=font_body, fill=0)
    draw.text((80,  y), desc, font=font_body, fill=0)
    y += 20

# ─── Convert to packed 1-bit bytes ─────────────────────────
fb_bytes = img.tobytes("raw", "1;R")   # MS-bit first order
assert len(fb_bytes) == BYTES, "Byte count mismatch"

# ─── Write to /dev/fb1 and trigger full refresh ───────────
fd = os.open(FBDEV, os.O_RDWR)
try:
    os.lseek(fd, 0, os.SEEK_SET)
    os.write(fd, fb_bytes)                 # copy into VRAM
    fcntl.ioctl(fd, FB_EPD_REFRESH_FULL)   # tell driver to refresh
finally:
    os.close(fd)
