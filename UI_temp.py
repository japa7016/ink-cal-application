#!/usr/bin/env python3
import sys
from datetime import datetime
from zoneinfo import ZoneInfo    
from ds1307 import read_rtc0
# add your epd2in7_V2 driver folder to the path
sys.path.insert(1, './lib')

import epd2in7_V2
from PIL import Image, ImageDraw, ImageFont

# ——— Initialize the V2 display ———
epd = epd2in7_V2.EPD()
epd.init()
epd.Clear()

# ——— Create a blank 1‑bit canvas ———
W, H = epd.width, epd.height
img = Image.new('1', (W, H), 255)
draw = ImageDraw.Draw(img)

# ——— Load fonts ———
font_header = ImageFont.truetype(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24
)
font_sub = ImageFont.truetype(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18
)
font_body = ImageFont.load_default()

# ——— Header: weekday, date, and Denver time ———
tz = ZoneInfo("America/Denver")             # MST/MDT aware
now = datetime.now(tz)
weekday  = now.strftime("%A")
date_str = now.strftime("%B %d")
#time_str = now.strftime("%H:%M")

now = read_rtc0()
time_str = now.strftime("%H:%M")

draw.text((10, 10), weekday,   font=font_header, fill=0)
draw.text((10, 40), date_str,  font=font_sub,    fill=0)
draw.text((10, 65), time_str,  font=font_sub,    fill=0)

# ——— Separator line ———
y0 = 95
draw.line((5, y0, W - 5, y0), fill=0)

# ——— Schedule items ———
events = [
    ("9:00 AM",  "Project meeting"),
    ("11:00 AM", "Doctor's appointment"),
    ("12:00 PM", "Lunch"),
    ("2:30 PM",  "Call with client"),
]
y = y0 + 10
for t, desc in events:
    draw.text((10, y),   t,    font=font_body, fill=0)
    draw.text((80, y),   desc, font=font_body, fill=0)
    y += 20

# ——— Send to display ———
epd.display(epd.getbuffer(img))
epd.sleep()
