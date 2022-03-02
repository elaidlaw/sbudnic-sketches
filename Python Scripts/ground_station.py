import serial
from datetime import datetime
import re
import numpy as np

arduino = serial.Serial(port='COM11', baudrate=500000, timeout=.1)

def read_packet():
    new = arduino.readall()
    while len(new) < 258:
        new += arduino.readall()
    return (new[:7].decode("utf-8"), new[7:])

images = {}
image_start_times = {}

telemetry_data = None
try:
    telemetry_data = np.load('telemetry.npy')
except:
    pass

while True:
    try:
        packet = read_packet()
        print(packet[0])
        if packet[0][0] == 'P':
            image_id = int(packet[0][2:3])
            packet_number = int(packet[0][3:6])
            if packet[0][1] == 'S':
                camera_id = int(packet[0][6:])
                images[image_id] = [None]*packet_number
                image_start_times[image_id] = datetime.now().strftime("%m-%d-%Y-%H-%M-%S")
                print(f'Receiving photo {image_id} from camera {camera_id} with {packet_number} packets; will be saved as {image_start_times[image_id]}.bin')
            if packet[0][1] == 'D':
                if image_id not in images:
                    images[image_id] = []
                    image_start_times[image_id] = datetime.now().strftime("%m-%d-%Y-%H-%M-%S")
                    print(f'Missed start command for image {image_id} but receiving data; will be saved as {image_start_times[image_id]}.bin')
                while len(images[image_id]) <= packet_number:
                    images[image_id].append(None)
                images[image_id][packet_number] = packet[1][:128]
                with open(f'images/{image_start_times[image_id]}.bin', 'wb+') as f:
                    for i in range(0, len(images[image_id]) - 1, 2):
                        if images[image_id][i] is not None and images[image_id][i + 1] is not None:
                            f.write(images[image_id][i])
                            f.write(images[image_id][i + 1])
                print(f'Received data packet {packet_number} for image {image_id}')
            if packet[0][1] == 'E':
                camera_id = int(packet[0][6:])
                if image_id not in images:
                    images[image_id] = []
                    image_start_times[image_id] = datetime.now().strftime("%m-%d-%Y-%H:%M:%S")
                    print(f'Missed start command and all data for image {image_id}')
                else:
                    with open(f'images/{image_start_times[image_id]}.bin', 'wb+') as f:
                        for i in range(0, len(images[image_id]) - 1, 2):
                            if images[image_id][i] is not None and images[image_id][i + 1] is not None:
                                f.write(images[image_id][i])
                                f.write(images[image_id][i + 1])
                    print(f'Done receiving image {image_id} with {packet_number} packets; saving as {image_start_times[image_id]}.bin')
                    del images[image_id]
        elif packet[0][0] == 'T':
            telemetry = re.split('[|:]', packet[1].decode('utf-8'))
            data = ['0', telemetry[1], telemetry[2], telemetry[3], telemetry[5], telemetry[6], telemetry[7], telemetry[8], telemetry[9], telemetry[10]]
            data = [float(x) for x in data]
            data[0] = datetime.now().timestamp()
            if telemetry_data is None:
                telemetry_data = np.array([data])
            else:
                telemetry_data = np.concatenate([telemetry_data, np.array([data])], 0)
            np.save('telemetry.npy', telemetry_data)
            print(f'Received telemetry packet {packet[0][1:]}')
        elif packet[0][0] == 'R':
            with open('restarts.txt', 'a') as f:
                print('Arduino restart')
                f.write(datetime.now().strftime("%m-%d-%Y-%H-%M-%S") + '\n')
        else:
            print(f'Couldn\'t parse packet {packet}')
    except:
        print('except')
        pass
