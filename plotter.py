# import matplotlib.pyplot as plt
# import numpy as np
# import wave
# import sys


# spf = wave.open('output.wav','r')

# #Extract Raw Audio from Wav File
# signal = spf.readframes(-1)
# signal = np.fromstring(signal, 'Int16')
# print(signal)


# #If Stereo
# # if spf.getnchannels() == 2:
# #     print ('Just mono files')
# #     sys.exit(0)

# plt.figure(1)
# plt.title('Signal Wave...')
# plt.plot(signal)
# plt.show()

import matplotlib.pyplot as plt
import numpy as np
import wave
import sys


spf = wave.open('output.wav','r')

#Extract Raw Audio from Wav File
signal = spf.readframes(60*30*44100)
signal = np.fromstring(signal, 'Int16')
fs = spf.getframerate()
print(fs)
print(spf.getnchannels())
print(len(signal))

#If Stereo
if spf.getnchannels() == 2:
    print 'Just mono files'
    sys.exit(0)


#Time=np.linspace(0.0, 1+len(signal)/(fs), num = len(signal))

# plt.figure(1)
# plt.title('Sound Intensity')
# plt.ylim([-32677, +32677])
# plt.plot(Time,signal)
# plt.xlabel("time(s)")
# plt.ylabel("amplitude")
# plt.show()

plt.figure(1)
a = plt.subplot(211)
r = 2**16/2
a.set_ylim([-r, r])
a.set_xlabel('time [s]')
a.set_ylabel('sample value [-]')
Time=np.linspace(0.0, 1+len(signal)/(fs), num = len(signal))
plt.plot(Time, signal)
plt.show()
# Load the required libraries:
#   * scipy
#   * numpy
#   * matplotlib


# from scipy.fftpack import fft
# import numpy as np
# N = 600
# T = 1.0/800.0
# x = np.linspace(0.0, N*T, N)
# y = np.sin(50.0*2.0*np.pi*x) + 0.5*np.sin(80.0 * 2.0*np.pi*x)
# yf = fft(y)
# fq = np.fft.fftfreq(len(x), T)
# positive_frequencies = fq[np.where(fq>=0)]
# mg = abs(yf[np.where(fq>=0)])
# print(positive_frequencies)
# xf = np.linspace(0.0, 1.0/(2.0*T), N//2)
# import matplotlib.pyplot as plt
# plt.plot(xf, 2.0/N * np.abs(yf[0:N//2]))
# plt.grid()
# plt.show()
# peak_frequency = np.argmax(mg)
# print(peak_frequency)
