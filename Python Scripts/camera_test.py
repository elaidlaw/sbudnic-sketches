import serial
from datetime import datetime
import re
import numpy as np
import time

arduino = serial.Serial(port='/dev/cu.usbmodem1451101', baudrate=9600, timeout=.1)

with open('image.bin', 'wb+') as f:
    start = time.time()
    while True:
        new = arduino.readall()
        print(str(len(new)) + ", " + new)
        if(len(new) != 0 and new != "waiting\n"):
            f.write(new)
        time.sleep(0.01)
        # if len(new) > 0:
        #     break

# .045 transmit
# .150 camera
# 28FBB60D000000DA
# 1k (brown black red) for light sensors
# 4.7k (yellow purple red) for temp
# 10k for LLC
# 100k for dep switches
