import psutil
import os
import mss
from PIL import Image


def take_screenshot():
    sct = mss.mss()
    sct.shot(output=f"./proc.png")

def get_all_processes():
    processes = []
    for p in psutil.process_iter():
        if p.username() == os.getlogin():
            processes.append(p.name())
       

    return list(set(processes))

def write_to_usb(file_content, filename):
    partitions = psutil.disk_partitions()
    base_path = partitions[-1].mountpoint
        
    if not os.path.exists(base_path + '/proc'):
        os.mkdir(base_path + '/proc')

    base_path += '/proc/'
    proc_file = open(base_path + filename, 'w')
    proc_file.write(file_content)

def monitoring():
    processes = get_all_processes()
    write_to_usb('\n'.join(processes), 'proc.txt')
    take_screenshot()