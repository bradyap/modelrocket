import serial
import time
from tkinter import *

#serial_port = "/dev/tty.HC-02"
serial_port = '/dev/tty.usbmodem13101'
serial_baudrate = 9600


def read_serial(ser):
    data = ser.readline()

    if data != "":
        print(data)
    else:
        print("No response from rocket.")

def test_connection(ser):
    ser.write(b's')
    read_serial(ser)

def open_door(ser):
    ser.write(b"o")
    read_serial(ser)


def close_door(ser):
    ser.write(b"c")
    read_serial(ser)


def main():
    ser = serial.Serial(serial_port, serial_baudrate)

    gui = Tk()
    gui.title("Rocket Control")
    
    test_button = Button(gui, text="Test Connection", command=lambda: test_connection(ser))
    test_button.grid(row=0, column=0)

    open_button = Button(gui, text="Open", command=lambda: open_door(ser))
    open_button.grid(row=1, column=1)

    close_button = Button(gui, text="Close", command=lambda: close_door(ser))
    close_button.grid(row=1, column=2)

    gui.mainloop()


if __name__ == "__main__":
    main()
