import serial
from datetime import datetime
import re
import numpy as np
import time

arduino = serial.Serial(port='COM5', baudrate=9600, timeout=.1)

with open('image.jpg', 'wb+') as f:
    start = time.time()
    while time.time() - start < 60:
        new = arduino.readall()
        print(len(new))
        f.write(new)
        if len(new) > 0:
            break