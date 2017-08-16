
# !/usr/bin/python

import requests
import json
import time
import pyaudio
import struct
import math
import tkMessageBox
import numpy as np
import audioop
import wave

ipaddress = "192.168.1.2"  # IP address of the hue lamp

INITIAL_TAP_THRESHOLD = 0.012
FORMAT = pyaudio.paInt16 
SHORT_NORMALIZE = (1.0/32768.0)
CHANNELS = 1
RATE = 44100  
INPUT_BLOCK_TIME = 5.0
INPUT_FRAMES_PER_BLOCK = int(RATE*INPUT_BLOCK_TIME)


def get_rms(block):
	# RMS amplitude is defined as the square root of the 
	# mean over time of the square of the amplitude.
	# so we need to convert this string of bytes into 
	# a string of 16-bit samples...

	# we will get one short out for each 
	# two chars in the string.
	count = len(block)/2
	format = "%dh"%(count)
	shorts = struct.unpack(format, block)

	# iterate over the block.
	sum_squares = 0.0
	for sample in shorts:
		# sample is a signed short in +/- 32768. 
		# normalize it to 1.0
		n = sample * SHORT_NORMALIZE
		sum_squares += n*n
	print(math.sqrt(sum_squares/count))
	# print(audioop.rms(block, 1))

	return math.sqrt( sum_squares / count )

def ffilter(block):

	#print(block)
	
	signal = np.fromstring(block, 'Int16')

	wf = wave.open("inputwave", 'wb')
	wf.setnchannels(1)
	wf.setsampwidth(2)
	wf.setframerate(44100)
	wf.writeframes(b''.join(block))
	wf.close()


	yf = np.fft.fft(signal)

	fs = 44100

	import matplotlib.pyplot as plt

	plt.figure(1)
	a = plt.subplot(211)
	r = 2**16/2
	a.set_ylim([-r, r])
	a.set_xlabel('time [s]')
	a.set_ylabel('sample value [-]')
	Time=np.linspace(0.0, 1+len(signal)/(fs), num = len(signal))
	print(len(Time))
	print(len(yf))
	plt.plot(Time, signal)

	b = plt.subplot(212)
	b.set_xscale('log')
	b.set_xlabel('frequency [Hz]')
	b.set_ylabel('|amplitude|')
	#xf = np.linspace(0.0, 1.0/1.0*44100, len(signal))
	plt.plot(abs(yf))
	plt.savefig('sample-graph.png')

	lowpass = 20 # Rremove lower frequencies.
	highpass = 20000# Remove higher frequencies.

	yf[:lowpass] = 0 # low pass filter (1)
	#lf[55:66], rf[55:66] = 0, 0 # line noise filter (2)
	yf[highpass:] = 0 # high pass filter (3)

	yf[595:605] = 0

	yf[810:820] = 0

	yf[285:290] = 0

	yf[257:262] = 0

	yf[147:152] = 0

	yf[195:200] = 0

	yf[1198:1202] = 0

	yf[46:48] = 0

	yf[95:98] = 0

	nl = np.fft.ifft(yf) # (4)

	# print(nl)

	ns = (np.column_stack((nl)).ravel().astype(np.int16))

	ns = ns.tobytes()

	# wf = wave.open("outputwave", 'wb')
	# wf.setnchannels(1)
	# wf.setsampwidth(2)
	# wf.setframerate(44100)
	# wf.writeframes(b''.join(ns))
	# wf.close()

	c = plt.subplot(211)
	plt.plot(Time, nl)

	d = plt.subplot(212)
	plt.plot(abs(yf))
	plt.show()

	return ns


	# c = plt.subplot(211)
	# plt.plot(Time, ns)

	# d = plt.subplot(212)
	# plt.plot(abs(yf))
	# plt.show()

	
class Hue(object):
	def __init__(self, ipaddress):
		self.ipaddress = ipaddress
		self.webaddress = "http://"+str(ipaddress)+"/api"
		self.username = ""

	def addUser(self, username):
		# data = {'devicetype':'my_hue_app %s'%(username)}
		# r = requests.post(self.webaddress, data = json.dumps(data))
		# tkMessageBox.showinfo("", "Press the button")

		# while True:
		# 	r = requests.post(self.webaddress, data = json.dumps(data))
		# 	r = r.json()
		# 	try:
		# 		str(r[0][u'error'][u'description']).startswith("link")
		# 	except:
		# 		self.username = str(r[0][u'success'][u'username'])
		# 		self.webaddress = self.webaddress + "/" + self.username
		# 		print("success")
		# 		break
		self.username = 'kgEi7CypuZf8dIPIW3O8QGVmnX1CYBgImAq2yR-r'
		self.webaddress = self.webaddress + "/" + self.username

		#here check and wait until button is pressed, get username
		# and modify the self.webaddress.
		#add a condition to make sure this was achieved, and return error
		#if it wasn't

	def getLights(self):
		r = requests.get(self.webaddress + "/lights")
		r = r.json()
		if r ==  {}:
			print("No lights were found")
			return
		else:
			dictkeys = list(r)
			print(dictkeys)
			return dictkeys

	def getLightState(self, light):
		r = requests.get(self.webaddress + "/lights/" + str(light))
		r = r.json()
		# parse the info received


	def turnOn(self, lightid):
		data = {"on": True}
		r = requests.put(self.webaddress + "/lights/" + str(lightid) + "/state", data = json.dumps(data))

	def turnOff(self, lightid):
		data = {"on": False}
		r = requests.put(self.webaddress + "/lights/" + str(lightid) + '/state', data = json.dumps(data))

	def setBrightness(self, lightid, brightness):
		try:
			assert brightness > 1 and brightness < 254
		except:
			print("Bad input")
			return
		data = {"bri":brightness}
		r = requests.put(self.webaddress + "/lights/" + str(lightid) + '/state', data = json.dumps(data))
	def setColor(self, lightid, color):
		data = {"hue":color}
		r = requests.put(self.webaddress + "/lights/" + str(lightid) + '/state', data = json.dumps(data))
		

# hue = Hue("192.168.1.2")
# hue.addUser("lesian")
# hue.turnOn(hue.getLights()[1])


class TapTester(object):
	def __init__(self):
		self.pa = pyaudio.PyAudio()
		self.stream = self.open_mic_stream()
		self.tap_threshold = INITIAL_TAP_THRESHOLD
		# self.noisycount = MAX_TAP_BLOCKS+1 
		self.quietcount = 0 
		self.errorcount = 0
		self.count = 0

	def stop(self):
		self.stream.close()

	def find_input_device(self):
		device_index = None            
		for i in range( self.pa.get_device_count() ):     
			devinfo = self.pa.get_device_info_by_index(i)   
			print( "Device %d: %s"%(i,devinfo["name"]) )

			for keyword in ["mic","input"]:
				if keyword in devinfo["name"].lower():
					print( "Found an input: device %d - %s"%(i,devinfo["name"]) )
					device_index = i
					return device_index

		if device_index == None:
			print( "No preferred input found; using default input device." )

		return device_index

	def open_mic_stream( self ):
		device_index = self.find_input_device()

		stream = self.pa.open(   format = FORMAT,
								 channels = CHANNELS,
								 rate = RATE,
								 input = True,
								 input_device_index = device_index,
								 frames_per_buffer = INPUT_FRAMES_PER_BLOCK)

		return stream

	def tapDetected(self):
		print ("Tap!")
		if self.count == 0:
			hue.setColor(hue.getLights()[1], 0)
			self.count = 1
		else:
			hue.setColor(hue.getLights()[1], 46920)
			self.count = 0

	def listen(self):
		print("I am stuck here")
		try:
			block = self.stream.read(INPUT_FRAMES_PER_BLOCK)
			print(audioop.rms(block, 2))
		except IOError:
			# dammit. 
			self.errorcount += 1
			print( "(%d) Error recording: %s"%(self.errorcount,e) )
			self.noisycount = 1
			return

		filtered = ffilter(block)
		amplitude = get_rms(block)
		filteredamp = get_rms(filtered)
		print('amplitude:', str(amplitude))
		print('filtered amp:', str(filteredamp))
		# if amplitude > self.tap_threshold:
		# 	# noisy block
		# 	print("noisyblock")
		# # 	hue.turnOn(hue.getLights()[1]) 
		# 	if filtered < 0.02:      # really quiet, the room barely goes to this level. Hue turned Off
		# 		if flag > 0:
		# 			flag = 0
		# 		else:
		# 			hue.setColor(hue.getLights()[1], 12750) # yellow
		# 	elif filtered < 0.03:    # 0.02-0.03. The room is quiet. Hue will turn yellow at this point
		# 		if flag > 1:
		# 			flag = 1
		# 		else:
		# 			hue.setColor(hue.getLights()[1], 25500) # green
		# 	elif filtered < 0.04:    # 0.03 - 0.04. The room is on normal reasonable conversations. Hue turns green
		# 		if flag > 2:
		# 			flag = 2
		# 		else:
		# 			hue.setColor(hue.getLights()[1], 46920) # blue
		# 	elif filtered < 0.05:   # 0.03 - 0.04. The room is a bit loud. Hue turns blue.
		# 		if flag > 3:
		# 			hue.setColor(hue.getLights()[1], 56100) # Purple
		# 		else:               # 0.04 - 0.05. The room is louder. People should reduce noise
		# 			flag = 3
		# 	else:
		# 		hue.setColor(hue.getLights()[1], 65280) # Red
		# 		flag = 4           # 0.05 - 0.06. The room is too loud. People should come down.
		# else:            
		# 	# quiet block.
		# 	# if 1 <= self.noisycount <= MAX_TAP_BLOCKS:
		# 	# 	self.tapDetected()
		# 	print("do nothing")
		# # 	hue.turnOff(hue.getLights()[1])
			

if __name__ == "__main__":
	tt = TapTester()
	for i in range(1000):
		tt.listen()