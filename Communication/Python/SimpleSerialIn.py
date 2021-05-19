import serial
import time

ser = serial.Serial(port='/dev/tty.usbmodem142101', baudrate=9600, timeout=1)

while (True):
    num = input("Enter command in the form of <index>,<value (0-255)>: ")
    strNum = str.encode(num)
    print("Sending...", strNum)
    ser.write(strNum)
    time.sleep(0.05) # sleep for 0.05s

    echoLine = ser.readline()

    print(echoLine)
    print() # empty line