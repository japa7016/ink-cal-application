#!/usr/bin/env python3
import fcntl, struct, datetime

# ioctl code to read struct rtc_time from /dev/rtc0
RTC_RD_TIME = 0x80247009

def read_rtc():
    with open('/dev/rtc0', 'rb') as rtc:
        # struct rtc_time is 9 ints (36 bytes)
        buf = fcntl.ioctl(rtc, RTC_RD_TIME, b'\x00'*36)
    sec, minute, hour, mday, mon, year_off, *_ = struct.unpack('9i', buf)
    return datetime.datetime(year_off + 1900, mon + 1, mday, hour, minute, sec)

if __name__ == "__main__":
    try:
        rtc_time = read_rtc()
        print("RTC time:", rtc_time.strftime("%Y-%m-%d %H:%M:%S"))
    except PermissionError:
        print("Permission denied: make sure you can read /dev/rtc0 (sudo or rtc group).")
    except Exception as e:
        print("Error reading RTC:", e)
