import os
import numpy as np
import sys

index = -1
process_raw = False

if len(sys.argv)>1:
    index = int(sys.argv[1])
    
if len(sys.argv)>2:
    process_raw = bool(int(sys.argv[2]))
    
print('index',  index)
print('raw',    process_raw)

path = './'
file_list = []
for file in os.listdir(path):
    if file.endswith('.dat'):
        fullname = os.path.join(path, file)
        file_list.append(fullname)

file_list.sort()
filename = file_list[index]
print(filename)

data = []
i=0
with open(filename, 'rb') as f:
    for line in f:
        line = line[:-1]
        if b'DATA' in line:
            line = line[5:]
        try:
            data.append(int(line.decode("utf-8")))
        except:
            print(i, line)
        i += 1

wave_count    = data[0]
wave_sampling = data[1]
wave_bitrate  = data[2]
wave_channels = data[3]
wave_bitdepth = data[4]
cpu_clock     = data[5]
wave_length   = data[6]
wave_duration = data[7]
wave_data     = data[8:]

print('SYSTEM    ', sys.byteorder+'-endian')
print('TIMER CNT ', wave_count)
print('SAMPLING  ', wave_sampling)
print('BITRATE   ', wave_bitrate)
print('CHANNELS  ', wave_channels)
print('BITDEPTH  ', wave_bitdepth)
print('LENGTH    ', wave_length)
print('CLOCK     ', cpu_clock)
print('DURATION  ', wave_duration/1000, 's')

if process_raw:
    wave_byte     = bytearray(wave_data)
else:
    wave_raw      = np.array(wave_data, dtype=np.ubyte)

    # remove average
    '''
    M = np.mean(wave_raw)
    wave_raw = wave_raw+(255/2-M)
    print('Mean correction', M)
    '''
    
    # smooth
    from scipy.signal import savgol_filter
    wave_smh = savgol_filter(wave_raw, 5, 3) # window size 51, polynomial order 3
    
    '''
    M = np.mean(wave_smh)
    wave_smh = wave_smh+(255/2-M)
    print('Mean correction', 255/2-M)
    '''
    
    wave_smh[wave_smh<0] = 0;
    wave_smh[wave_smh>255] = 255;
    
    import matplotlib.pyplot as plt
    #plt.plot(wave_raw+(255/2-M), '.--')
    plt.plot(wave_raw, '.--')
    plt.plot(wave_smh, '-')
    plt.show()
    
    
    wave_byte = wave_smh.astype(np.ubyte).tobytes()
    
import wave

head, tail = os.path.split(filename)
name_out, ext = os.path.splitext(tail)

with wave.open(name_out+'.wav', 'wb') as wavfile:
    wavfile.setparams((wave_channels, wave_bitdepth//8, wave_sampling, 0, 'NONE', 'NONE'))
    wavfile.writeframes(wave_byte)


