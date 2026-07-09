#!/usr/bin/env python3
"""
flash application binary to bootloader via UART with Ed25519 signing
usage:   python3 flasher.py <app.bin> [serial_port] [baud] 
example: python3 flasher.py build/main_app.bin /dev/ttyUSB0 115200 0x08002000

format:
[F][SIZE][optional)SIGNATURE][APP]
"""
import re
import serial
import struct
import sys
import time
import os
import glob

PRIVATE_KEY = os.path.join(".secrets", "private.pem")

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ed25519


def list_apps():

    pattern = "build/apps/*.bin"
    bin_files = glob.glob(pattern)
    return bin_files


def sign_app(app_data):
    with open(PRIVATE_KEY, 'rb') as f:
        private_key = serialization.load_pem_private_key(f.read(), password=None)
    return private_key.sign(app_data)


def send_app(bin_file, port='/dev/ttyUSB0', baud=115200):
    FEATURE_SIGNATURE = False
    with open(bin_file, 'rb') as f:
        app_data = f.read()

    ans = input("sign app? [yes/no]: ")
    if ans.lower() == "yes":
        print("signing app (bootloader should have this feature too...)")
        signature = sign_app(app_data)
        data = app_data + signature
        FEATURE_SIGNATURE = True
    else:
        data = app_data

    size = len(data)

    print(f"app size: {len(app_data)} bytes")
    print(f"signature: {"YES" if FEATURE_SIGNATURE else "NO"}")

    if FEATURE_SIGNATURE:
        print(f"data size: {len(data)} bytes")
        print(f"signature: {signature.hex()[:32]}...")

    print("sending app...")
    ser = serial.Serial(port, baud, timeout=5)
    time.sleep(1.5)

    chunk_size=256
    ser.write(b'F')
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(f"\t< {line}")

        if "flash command received" in line:
            match = re.search(r'chunk size:\s*(\d+)', line)
            chunk_size = int(match.group(1)) if match else 256
            break
        if "failed to recv sync byte" in line:
            ser.write(b'F')

    time.sleep(0.1)
    print(f"chunk size recieved: {chunk_size}")

    ser.write(struct.pack('<I', size))
    
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(f"\t< {line}")
        if f"size received: {size}" in line:
            break
        if "invalid size" in line:
            print("invalid size received, sending again...")
            ser.write(struct.pack('<I', size))

    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(f"\t< {line}")
        if "erase done" in line:
            print("erase is done!")
            break

    total_sent = 0
    print("sending data...")
    time.sleep(0.005)
    
    for offset in range(0, len(data), chunk_size):
        ch = data[offset:offset+chunk_size]
        ser.write(ch)
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if "ok" in line:
            print(f"OK: {line}")
        elif "timeout" in line:
            print(f"timeout: {line}")
            break


    print("sending is done")

    print()
    
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(f"\t< {line}")
        if "flashing is done" in line.lower():
            break
        if "aborting" in line.lower():
            print("FLASHING FAILED!")
            break

    ser.close()
    print("done!")

    return 1

def tui_intr():
    print("options:\n\t[1] flash\n\t[2] serial\n\t[3] help\n\t[4] exit")
    i = int(input("> "))
    if i == 3:
        print("usage: python flasher.py <app.bin> [port] [baud]")
        return 
    if i == 4:
        exit(0)

    if i == 1:
        port = input("port: ")
        baud = input("baud: ")
        if not port:
            port = "/dev/ttyUSB0"
        if not baud:
            baud = "115200"
        for indx, app in enumerate(list_apps(), 1):
            print(f"[{indx}] {app}")
        indx = int(input("num of app: "))
        if indx > len(list_apps())+1 or indx < 0:
            print("wrong num!")
            return 
        app = list_apps()[indx-1]
        send_app(app, port, baud)
    if i == 2:
        #TODO add interrupt 
        port = input("port: ")
        baud = input("baud: ")
        if not port:
            port = "/dev/ttyUSB0"
        if not baud:
            baud = "115200"
        ser = serial.Serial(port, baud, timeout=5)
        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(f"\t< {line}")

        ser.close()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("entering tui interface")
        tui_intr()
        sys.exit(1)

    if not os.path.exists(PRIVATE_KEY):
        print(f"error: private key not found at {PRIVATE_KEY}")
        print("run tools/keys.sh first to generate keys")
        sys.exit(1)

    bin_file = sys.argv[1]
    port = sys.argv[2] if len(sys.argv) > 2 else '/dev/ttyUSB0'
    baud = int(sys.argv[3]) if len(sys.argv) > 3 else 115200


    if not os.path.exists(bin_file):
        print(f"couldn't find {bin_file}!")
        exit(-1)
    send_app(bin_file, port, baud)
