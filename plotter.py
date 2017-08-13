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
signal = spf.readframes(-1)
signal = np.fromstring(signal, 'Int16')
fs = spf.getframerate()
print(fs)
print(spf.getnchannels())

#If Stereo
# if spf.getnchannels() == 2:
#     print 'Just mono files'
#     sys.exit(0)


Time=np.linspace(0.0, len(signal)/(2*fs), len(signal))

plt.figure(1)
plt.title('Sound Intensity')
plt.plot(Time,signal)
plt.xlabel("time(s)")
plt.ylabel("amplitude")
plt.show()

# Load the required libraries:
#   * scipy
#   * numpy
#   * matplotlib
