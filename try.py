import sys


from datetime import datetime
from zoneinfo import ZoneInfo

sys.path.insert(1, './lib')       # path to your epd2in7.py driver
import epd2in7_V2
from PIL import Image, ImageDraw, ImageFont

# 1) Initialize display
epd = epd2in7_V2.EPD()
epd.init()
font = ImageFont.load_default()
mst = ZoneInfo("America/Denver")
# 2) Grab system time
now = datetime.now(mst).strftime("%Y-%m-%d %H:%M:%S")

# 3) Draw it
image = Image.new('1', (epd.width, epd.height), 255)  # white bg
draw  = ImageDraw.Draw(image)
# center vertically
text_w, text_h = draw.textsize(now, font=font)
y = (epd.height - text_h) // 2
draw.text((10, y), now, font=font, fill=0)

# 4) Send to e‑Paper
epd.Clear()
epd.display(epd.getbuffer(image))
epd.sleep()

