# ds1307_dev.py
import datetime, fcntl, struct

# ioctl code to read struct rtc_time from /dev/rtc0
RTC_RD_TIME = 0x80247009  

def read_rtc0():
    with open('/dev/rtc0', 'rb') as rtc:
        # struct rtc_time is 9 ints (36 bytes)
        buf = fcntl.ioctl(rtc, RTC_RD_TIME, b'\x00'*36)
        tm = struct.unpack('9i', buf)
    # tm = (sec, min, hour, mday, mon, year_since_1900, wday, yday, isdst)
    year  = tm[5] + 1900
    month = tm[4] + 1
    day   = tm[3]
    hour  = tm[2]
    minute= tm[1]
    second= tm[0]
    return datetime.datetime(year, month, day, hour, minute, second)
