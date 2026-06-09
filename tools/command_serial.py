#
# command line interface 
# made to communicate with the device
#

from flash import send_app
import serial 


class Command:
    def __init__(self, port, baud):
        self.port = port
        self.baud = baud

    def flash(self, bin_file, addr=0x08002000):
        try:
            send_app(bin_file, self.port, self.baud, addr)
        except Exception as err:
            print("error in flash command: ", err)


    def execute_apps(self, verbose=False):
        try:
            ser = serial.Serial(self.port, self.baud, timeout=5)
            ser.write(b'S')
            if verbose:
                while(1):
                    line = ser.readline().decode().strip()
                    if "NO MORE APPS" in line:
                        break
                    if line:
                        print(f"\t< {line}")

            print("scan done!")
            ser.close()

        except Exception as err:
            print("error in scan command: ", err)

    def serial_watch(self):
        try:
            ser = serial.Serial(self.port, self.baud, timeout=5)
            ser.write(b'W')
            if verbose:
                line = ser.readline().decode().strip()
                if line:
                    print(f"\t< {line}")
            ser.close()

        except Exception as err:
            print("error in serial watch: ", err)
        

if "__main__" == __name__:
    cmd = Command(port='/dev/ttyUSB0', baud=115200)
    
    data = input("write command\n> ")

    if data == "flash" or data == "f":
        path = input("path to app\n> ")
        addr = input("addr\n> ")
        if not addr:
            cmd.flash(path)
        else: 
            cmd.send_app(path, addr)
    if data == "exec" or data == "e":
        cmd.execute_apps(True)


