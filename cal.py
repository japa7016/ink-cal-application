import requests
from icalendar import Calendar
import datetime
from zoneinfo import ZoneInfo

ICS_URL = "https://calendar.google.com/calendar/ical/jainil8878%40gmail.com/private-251e13d841443b623e03d3679e4ad307/basic.ics"

# Change this to datetime.date.today() if you want today's events instead of tomorrow's
TARGET_DATE = datetime.date.today()

LOCAL_TZ = ZoneInfo("America/Denver")

def fetch_events_for_date(date_to_check):
    print(f"[DEBUG] Fetching ICS URL: {ICS_URL}")
    resp = requests.get(ICS_URL)
    print(f"[DEBUG] HTTP status code: {resp.status_code}")
    cal = Calendar.from_ical(resp.content)
    print(f"[DEBUG] Parsing calendar data...")
    evts = []
    for comp in cal.walk():
        if comp.name != "VEVENT":
            continue

        raw_dt = comp.get("dtstart").dt
        print(f"[DEBUG] Raw VEVENT dt: {raw_dt!r} (tzinfo={raw_dt.tzinfo})")

        # If it's a full datetime, normalize to UTC then convert to local
        if isinstance(raw_dt, datetime.datetime):
            if raw_dt.tzinfo is None:
                raw_dt = raw_dt.replace(tzinfo=ZoneInfo("UTC"))
            local_dt = raw_dt.astimezone(LOCAL_TZ)
            event_date = local_dt.date()
            event_time = local_dt.strftime("%-I:%M %p")
            print(f"[DEBUG]   → local_dt = {local_dt!r}")
        else:
            # All-day event
            event_date = raw_dt
            event_time = ""
            print(f"[DEBUG]   → all-day on {event_date}")

        if event_date == date_to_check:
            summary = comp.get("summary")
            print(f"[DEBUG]   * MATCH: {event_time}   {summary}")
            evts.append((event_time, str(summary)))

    print(f"[DEBUG] Total events found for {date_to_check}: {len(evts)}\n")
    return evts

if __name__ == "__main__":
    print(f"[DEBUG] Target date is {TARGET_DATE}")
    events = fetch_events_for_date(TARGET_DATE)

    label = "Today" if TARGET_DATE == datetime.date.today() else "Tomorrow"
    print(f"{label}'s events:")
    if not events:
        print("  (none)")
    else:
        for t, desc in events:
            print(f"  • {t:>7}   {desc}")
