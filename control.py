import serial
from tkinter import *
import asyncio
from threading import Thread

serial_port = "/dev/tty.rocket"
serial_baudrate = 9600


async def read_serial(ser):
    while True:
        data = ser.readline()
        print(data.decode("utf-8"))
        await asyncio.sleep(0.01)


def ping(ser):
    ser.write(b'p')


def open_door(ser):
    ser.write(b"o")


def close_door(ser):
    ser.write(b"c")

async def gui(ser):
    gui = Tk()
    gui.title("Rocket Control")

    test_button = Button(gui, text="Ping", command=lambda: ping(ser))
    test_button.grid(row=0, column=0)

    open_button = Button(gui, text="Open", command=lambda: open_door(ser))
    open_button.grid(row=1, column=1)

    close_button = Button(gui, text="Close", command=lambda: close_door(ser))
    close_button.grid(row=1, column=2)
    
    gui.mainloop()


def main():
    ser = serial.Serial(serial_port, serial_baudrate)
    asyncio.run(gui(ser))
    

if __name__ == "__main__":
    main()
