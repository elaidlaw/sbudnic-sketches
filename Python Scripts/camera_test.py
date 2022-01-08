import serial
import time
arduino = serial.Serial(port='COM3', baudrate=9600, timeout=.1)

img = bytes()
with open('image.bin', 'wb+') as f:
    f.write('hello'.encode())
start = time.time()
img = bytes()
while time.time() < start + 20:
    new = arduino.readline()
    while len(new) > 0:
        img += new
        new = arduino.readall()
    print(len(img))
with open('image.bin', 'wb+') as f:
    f.write(img)