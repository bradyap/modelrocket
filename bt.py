import serial
import time

serial_port = '/dev/tty.HC-02'
serial_baudrate = 9600

def read_serial(ser):
    print("recieving message from arduino ...")
    data = ser.readline()

    if (data != ""):
        print("arduino says: %s" % data)
    else:
        print("arduino doesnt respond")

def main():
    ser = serial.Serial(serial_port, serial_baudrate)
    
    while True:
        print("msg for set pin high")
        ser.write(b'1')
        read_serial(ser)
        time.sleep(2)
        
        print("msg for set pin low")
        ser.write(b'0')
        read_serial(ser)
        time.sleep(2)

if __name__ == '__main__':
    main()