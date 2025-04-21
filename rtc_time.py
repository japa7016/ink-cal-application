#!/usr/bin/env python3
import datetime
import fcntl
import struct

# ioctl code to read struct rtc_time from /dev/rtc0
RTC_RD_TIME = 0x80247009

def read_rtc0():
    with open('/dev/rtc0', 'rb') as rtc:
        # struct rtc_time is 9 ints (36 bytes)
        buf = fcntl.ioctl(rtc, RTC_RD_TIME, b'\x00' * 36)
    sec, minute, hour, mday, mon, year_off, *_ = struct.unpack('9i', buf)
    return datetime.datetime(year_off + 1900, mon + 1, mday, hour, minute, sec)

if __name__ == "__main__":
    try:
        now = read_rtc0()
        print(now.strftime("%Y-%m-%d %H:%M:%S"))
    except PermissionError:
        print("Permission denied: cannot open /dev/rtc0.")
    except Exception as e:
        print("Error reading RTC:", e)
