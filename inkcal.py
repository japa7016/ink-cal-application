#!/usr/bin/env python3
import time
import requests
from icalendar import Calendar
import datetime
from zoneinfo import ZoneInfo
from inkcal_lib import epd2in7_V2
from PIL import Image, ImageDraw, ImageFont

ICS_URL  = "https://calendar.google.com/calendar/ical/jainil8878%40gmail.com/private-251e13d841443b623e03d3679e4ad307/basic.ics"
LOCAL_TZ = ZoneInfo("America/Denver")

def fetch_events(date_to_check):
    resp = requests.get(ICS_URL)
    cal = Calendar.from_ical(resp.content)
    matches = []
    for comp in cal.walk():
        if comp.name != "VEVENT":
            continue
        raw = comp.get("dtstart").dt
        if isinstance(raw, datetime.datetime):
            if raw.tzinfo is None:
                raw = raw.replace(tzinfo=ZoneInfo("UTC"))
            local = raw.astimezone(LOCAL_TZ)
            ev_date = local.date()
            ev_time = local.strftime("%-I:%M %p")
        else:
            ev_date, ev_time = raw, ""
        if ev_date == date_to_check:
            matches.append((ev_time, str(comp.get("summary"))))
    return matches

def redraw(events):
    epd = epd2in7_V2.EPD()
    epd.init()
    epd.Clear()

    W, H = epd.width, epd.height
    im = Image.new("1", (W, H), 255)
    draw = ImageDraw.Draw(im)

    fh = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24)
    fs = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18)
    fb = ImageFont.load_default()

    now = datetime.datetime.now(LOCAL_TZ)
    # Header
    draw.text((10, 10), now.strftime("%A"),      font=fh, fill=0)
    draw.text((10, 40), now.strftime("%B %d"),    font=fs, fill=0)
    draw.line((5, 65, W-5, 65), fill=0)

    # Events
    y = 75
    if not events:
        events = [("", "(no events)")]
    for t, desc in events[:6]:
        draw.text((10, y), t,    font=fb, fill=0)
        draw.text((80, y), desc, font=fb, fill=0)
        y += 20

    # Footer: current time (HH:MM:SS)
    timestamp = now.strftime("%-I:%M:%S %p")
    tw, th = draw.textsize(timestamp, font=fb)
    draw.text((W - tw - 10, H - th - 5), timestamp, font=fb, fill=0)

    epd.display(epd.getbuffer(im))
    epd.sleep()

def main():
    target = datetime.date.today()
    last = None
    while True:
        evts = fetch_events(target)
        if evts != last:
            redraw(evts)
            last = evts
        time.sleep(60)

if __name__ == "__main__":
    main()
