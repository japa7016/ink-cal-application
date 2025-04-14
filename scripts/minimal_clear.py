
#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os
import time


sys.path.append(os.path.join(os.path.dirname(__file__), '../epaper_driver'))

from epd2in7 import EPD 

def main():
    try:
        epd = EPD()
        print("Initializing display...")
        epd.init()         # Initialize with default settings
        print("Clearing display to white...")
        epd.Clear(0xFF)    # Clear the screen (0xFF for white)
        time.sleep(2)
        print("Putting display to sleep.")
        epd.sleep()
    except Exception as e:
        print("Error:", e)

if __name__ == '__main__':
    main()

