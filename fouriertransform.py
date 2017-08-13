#import matplotlib.pyplot as plt 
#from scipy.io import wavfile as wave
#from scipy.fftpack import fft 
#import numpy as np 
#rate, data = wave.read("output.wav")
#fft_out = fft(data)
#plt.plot(data, abs(fft_out))
#plt.show()

# import numpy as np
# import matplotlib.pyplot as plt
# from scipy.fftpack import fft
# from scipy.io import wavfile # get the api
# fs, data = wavfile.read('output.wav') # load the data
# a = data.T[1] # this is a two channel soundtrack, I get the first track
# b=[(ele/2**16)*2-1 for ele in a] # this is 16-bit track, b is now normalized on [-1,1)
# c = fft(b) # calculate fourier transform (complex numbers list)
# d = len(c)/2  # you only need half of the fft list (real signal symmetry)
# k = np.arange(len(data))
# T = len(data)/44100  # where fs is the sampling frequency
# frqLabel = k/T  
# plt.plot(abs(c[:(d-1)]),frqLabel) 
# plt.show()


# import matplotlib.pyplot as plt
# from scipy.io import wavfile as wav
# from scipy.fftpack import fft
# import numpy as np
# rate, data = wav.read('output.wav')
# fft_out = fft(data)
# #%matplotlib inline
# plt.plot(data, np.abs(fft_out))
# plt.show()

# import matplotlib.pyplot as plt
# import numpy as np
# from scipy.fftpack import fft
# import wave
# import sys


# spf = wave.open('output.wav','r')

# #Extract Raw Audio from Wav File
# signal = spf.readframes(-1)
# signal = np.fromstring(signal, 'Int16')
# fs = spf.getframerate()

# c = fft(signal)
# d = len(c)/2
# k = np.arange(len(signal))
# T = len(signal)/44100  # where fs is the sampling frequency
# frqLabel = k/T  
# plt.plot(np.abs(c[:(d-1)]), 'r') 
# plt.show()

"""
Plot
"""
#Plots a stereo .wav file
#Decibels on the y-axis
#Frequency Hz on the x-axis

import matplotlib.pyplot as plt
import numpy as np

from pylab import*
from scipy.io import wavfile


def plot(file_name):

	sampFreq, snd = wavfile.read(file_name)

	snd = snd / (2.**15) #convert sound array to float pt. values

	s1 = snd[:,0] #left channel

	s2 = snd[:,1] #right channel

	n = len(s1)
	p = fft(s1) # take the fourier transform of left channel

	m = len(s2) 
	p2 = fft(s2) # take the fourier transform of right channel

	nUniquePts = ceil((n+1)/2.0)
	# p = p[0:nUniquePts]
	p = abs(p)

	mUniquePts = ceil((m+1)/2.0)
	# p2 = p2[0:mUniquePts]
	p2 = abs(p2)

	p = p / float(n) # scale by the number of points so that
			 # the magnitude does not depend on the length 
			 # of the signal or on its sampling frequency  
	p = p**2  # square it to get the power 




# multiply by two (see technical document for details)
# odd nfft excludes Nyquist point
	if n % 2 > 0: # we've got odd number of points fft
		p[1:len(p)] = p[1:len(p)] * 2
	else:
		p[1:len(p) -1] = p[1:len(p) - 1] * 2 # we've got even number of points fft

	freqArray = arange(0, nUniquePts, 1.0) * (sampFreq / n);
	plt.plot(freqArray/1000, 10*log10(p), color='k')
	plt.xlabel('LeftChannel_Frequency (kHz)')
	plt.ylabel('LeftChannel_Power (dB)')
	plt.show()


	p2 = p2 / float(m) # scale by the number of points so that
			 # the magnitude does not depend on the length 
			 # of the signal or on its sampling frequency  
	p2 = p2**2  # square it to get the power 




# multiply by two (see technical document for details)
# odd nfft excludes Nyquist point
	if m % 2 > 0: # we've got odd number of points fft
		 p2[1:len(p2)] = p2[1:len(p2)] * 2
	else:
		 p2[1:len(p2) -1] = p2[1:len(p2) - 1] * 2 # we've got even number of points fft

	freqArray2 = arange(0, mUniquePts, 1.0) * (sampFreq / m);
	plt.plot(freqArray2/1000, 10*log10(p2), color='k')
	plt.xlabel('RightChannel_Frequency (kHz)')
	plt.ylabel('RightChannel_Power (dB)')
	plt.show()


plot('output.wav')