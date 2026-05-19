#!/usr/bin/env python3
"""
flash application binary to bootloader via UART with Ed25519 signing
usage:   python3 flasher.py <app.bin> [serial_port] [baud] [addr]
example: python3 flasher.py build/main_app.bin /dev/ttyUSB0 115200 0x08002000

format:
[F][SIZE][ADDR][APP+SIGNATURE]
"""

import serial
import struct
import sys
import time
import os

PRIVATE_KEY = os.path.join(".secrets", "private.pem")

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ed25519

def sign_app(app_data):
    with open(PRIVATE_KEY, 'rb') as f:
        private_key = serialization.load_pem_private_key(f.read(), password=None)
    return private_key.sign(app_data)


def send_app(bin_file, port='/dev/ttyUSB0', baud=115200, addr=0x08002000):
    with open(bin_file, 'rb') as f:
        app_data = f.read()

    print("signing app...")
    signature = sign_app(app_data)
    
    data = app_data + signature
    size = len(data)

    print(f"addr: {hex(addr)}")
    print(f"app size: {len(app_data)} bytes")
    print(f"signature: {signature.hex()[:32]}...")

    print("sending app...")
    ser = serial.Serial(port, baud, timeout=5)
    time.sleep(0.5)

    ser.write(b'F')
    while True:
        line = ser.readline().decode().strip()
        if line:
            print(f"\t< {line}")
        if "flash command" in line:
            break
    time.sleep(0.1)

    ser.write(struct.pack('<I', size))
    
    while True:
        line = ser.readline().decode().strip()
        if line:
            print(f"\t< {line}")
        if f"size received: {size}" in line:
            break
    
    ser.write(struct.pack('<I', addr))
    
    while True:
        line = ser.readline().decode().strip()
        if line:
            print(f"\t< {line}")
        if f"addr received: {hex(addr)}" in line:
            break

    total_sent = 0
    print("sending...")
    for byte in data:
        ser.write(bytes([byte]))
        time.sleep(0.0005)
        total_sent += 1

    print()
    
    while True:
        line = ser.readline().decode().strip()
        if line:
            print(f"\t< {line}")
        if "flashing done" in line.lower():
            break
        if "failed" in line.lower():
            print("FLASHING FAILED!")
            break

    ser.close()
    print("done!")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python flasher.py <app.bin> [port] [baud] [addr]")
        sys.exit(1)

    if not os.path.exists(PRIVATE_KEY):
        print(f"error: private key not found at {PRIVATE_KEY}")
        print("run tools/keys.sh first to generate keys")
        sys.exit(1)

    bin_file = sys.argv[1]
    port = sys.argv[2] if len(sys.argv) > 2 else '/dev/ttyUSB0'
    baud = int(sys.argv[3]) if len(sys.argv) > 3 else 115200
    addr = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0x08002000

    send_app(bin_file, port, baud, addr)
