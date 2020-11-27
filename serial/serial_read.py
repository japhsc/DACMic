import serial
import time

filename = time.strftime('%Y_%m_%d-%H_%M_%S.dat')

ser = serial.Serial(
    port='/dev/ttyUSB0',\
    baudrate=115200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS)
    #timeout=0

print("connected to: " + ser.portstr)

i=0
with open(filename, 'wb') as f:
    while True:
        line = ser.readline()[:-2]
        if line==b'EOF':
            break
        elif line==b'START':
            print(i, 'START REC')
        elif line==b'ERROR':
            print(i, 'ERROR REC')
        elif line==b'BEGIN':
            f.seek(0)
            f.truncate(0)
            print(i, 'RESET')
            i = 0
        else:
            if b'DATA' in line:
                print(i, 'END REC', int(line[4:]))
            f.write(line)
            f.write(b'\n')
        i += 1

ser.close()

print('lines: ', i)

