"""
flash application binary to bootloader via UART
usage:   python3 flasher.py <app.bin> [serial_port] [baud] [addr]
example: python3 flasher.py build/main_app.bin /dev/ttyUSB0 115200 0x08002000

format:
[F][KEY][SIZE][ADDR][APP]
"""

import serial
import struct
import sys
import time

def send_app(bin_file, port='/dev/ttyUSB0', baud=115200, addr=0x08002000):
    with open(bin_file, 'rb') as f:
        data = f.read()

    size = len(data)

    print(f"addr: {hex(addr)}")
    print(f"app size: {size} bytes")

    ser = serial.Serial(port, baud, timeout=5)
    time.sleep(0.5)

    ser.write(b'F')
    time.sleep(0.1)

    #TODO: implement key here...

    ser.write(struct.pack('<I', size))
    
    while True:
        line = ser.readline().decode().strip()
        print(f"\t>: {line}")
        if f"size received: {size}" in line:
            break
    
    ser.write(struct.pack('<I', addr))
    
    while True:
        line = ser.readline().decode().strip()
        print(f"\t>: {line}")
        if f"addr received: {hex(addr)}" in line:
            break

    total_sent = 0
    print("sending binary data")
    for byte in data:
        ser.write(bytes([byte]))
        time.sleep(0.001)
        total_sent += 1
        if total_sent % 10 == 0:
            print(f"\rprogress: {total_sent}/{size} bytes ({100*total_sent/size:.1f}%)", end='')

    print()
    while True:
        line = ser.readline().decode().strip()
        print(f"\t> UART DATA: {line}")
        if "flashing done" in line.lower():
            break

    ser.close()
    print("done!")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python flasher.py <app.bin> [port] [baud]")
        sys.exit(1)

    bin_file = sys.argv[1]
    port = sys.argv[2] if len(sys.argv) > 2 else '/dev/ttyUSB0'
    baud = int(sys.argv[3]) if len(sys.argv) > 3 else 115200

    send_app(bin_file, port, baud)
