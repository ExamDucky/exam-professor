from time import sleep
from serial import Serial
from serial.tools.list_ports import comports
from process_handler import get_all_processes
from process_handler import take_screenshot
from PIL import Image
import io
import sys
import os
from pathlib import Path
import binascii

def send_exam(path, s):
    s.write('EXSTART\r\n'.encode())
    sleep(2)
    with open(path, 'rb') as f:
        zip_data = f.read()

    chunk_size = 128
    print(f"Sending {len(zip_data)} bytes of image data...")

    for i in range(0, len(zip_data), chunk_size):
        chunk = zip_data[i:i + chunk_size]
        s.write(chunk)
        sleep(0.1)

    sleep(2)
    s.write('EXEND\r\n'.encode())
    print("Finished transfering exam")

def send_processes(s):
    sleep(2)

    print("Sending processes")
    message = 'PSTART\r\n'
    s.write(message.encode())
    for p in get_all_processes():
        proc = p + "\r\n";
        s.write(proc.encode())
    s.write('PEND\r\n'.encode())
    print("Sent processes")


def send_scrt():
    sleep(2)

    ports = comports()
    for port, _, hwid in ports:
        print(hwid)
        if '58CD185432' in hwid:
            with Serial(port) as s:
                s.baudrate = 115200
                s.setDTR(False)
                s.setRTS(False)

                take_screenshot()
                s.write('SCSTART\r\n'.encode())
                sleep(2)

                image = Image.open("./proc.png")

                img_byte_array = io.BytesIO()
                image.save(img_byte_array, format='PNG')
                img_byte_array = img_byte_array.getvalue()

                chunk_size = 128
                print(f"Sending {len(img_byte_array)} bytes of image data...")

                for i in range(0, len(img_byte_array), chunk_size):
                    chunk = img_byte_array[i:i + chunk_size]
                    s.write(chunk)
                    sleep(0.1)
                
                sleep(2)
                s.write('SCEND\r\n'.encode())
        
    raise Exception('TinyUSB COM port not found')

def send_exam(path):
    sleep(2)

    ports = comports()
    for port, _, hwid in ports:
        print(hwid)
        if '58CD185432' in hwid:
            with Serial(port) as s:
                s.baudrate = 115200
                s.setDTR(False)
                s.setRTS(False)

                take_screenshot()
                s.write('EXSUB\r\n'.encode())
                sleep(2)

                with open(path, 'rb') as f:
                    zip_data = f.read()

                chunk_size = 128
                print(f"Sending {len(zip_data)} bytes of image data...")

                for i in range(0, len(zip_data), chunk_size):
                    chunk = zip_data[i:i + chunk_size]
                    s.write(chunk)

                sleep(2)
                s.write('EXFIN\r\n'.encode())
        
    raise Exception('TinyUSB COM port not found')

def convert_to_bytes(number):
    number_str = str(number)
    
    if len(number_str) % 2 != 0:
        raise ValueError("Number must have an even number of digits")
    
    byte_array = []
    for i in range(0, len(number_str), 2):
        pair = number_str[i:i+2]
        ascii_value = int(pair)
        byte_array.append(ascii_value)
    
    return bytes(byte_array)

def handle_serial():
    sleep(2)
    ports = comports()
    for port, _, hwid in ports:
        print(hwid)
        if '58CD185432' in hwid:
            with Serial(port) as s:
                s.baudrate = 115200
                s.setDTR(False)
                s.setRTS(False)
                
                while True:
                    if s.in_waiting > 0:
                        data_unsplit = s.read(s.in_waiting)
                        try:
                            if True:
                                data_split = data_unsplit.decode().split('\r\n')

                                for data in data_split:
                                    print(data)
                                    if 'PROCGET' in data:
                                        send_processes(s)
                        except:
                            pass
        
    raise Exception('TinyUSB COM port not found')


def download_exam():
    sleep(2)
    downloading = False
    ports = comports()
    for port, _, hwid in ports:
        print(hwid)
        if '58CD185432' in hwid:
            with Serial(port) as s:
                s.baudrate = 115200
                s.setDTR(False)
                s.setRTS(False)
                
                home = Path.home()
                exam = open(home.as_posix() + "/exam.zip", 'wb')
                count = 0
                while True:
                    if s.in_waiting > 0:
                        data_unsplit = s.read(s.in_waiting)
                        try:
                            if True:
                                data_split = data_unsplit.decode().split('\r\n')
                                if downloading and not 'EXEND' in data_unsplit.decode():
                                    print(bytes([int(data_unsplit)]))
                                    exam.write(bytes([int(data_unsplit)]))
                                else:
                                    for data in data_split:
                                        print(data)
                                        if 'EXDOW' in data:
                                            downloading = True
                                        elif 'EXEND' in data:
                                            exam.close()
                                            return                                
                        except:
                            pass
        
    raise Exception('TinyUSB COM port not found')


if __name__ == '__main__':
    download_exam()
    handle_serial()
    # send_scrt()
    if sys.argv[1] == "submit":
        send_exam(sys.argv[2])