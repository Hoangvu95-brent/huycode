from time import sleep
import serial
 
ser = serial.Serial("/dev/ttyS0")
ser.baudrate = 9600
ser.flushInput()
print("Raspberry's receiving : ") 
 
try:
    while True:
        received_data = ser.read()              #read serial port
        sleep(0.03)
        data_left = ser.inWaiting()             #check for remaining byte
        #data_left = received_data.decode('utf-8')           # decode s
        received_data += ser.read(data_left)
        decodebyte=float(received_data[0:len(received_data)-2].decode("utf-8"))
        #data_left = received_data.decode('utf-8')           # decode s
        #data = data.rstrip()            # cut "\r\n" at last of string
        print(received_data)             # print string
        print(decodebyte)
except KeyboardInterrupt:
    ser.close()