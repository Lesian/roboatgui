
# !/usr/bin/python
# skeleton code from github
# open a microphone in pyAudio and listen for taps

# liblaries to make restful requests
import requests
import json
import time
import pyaudio
import struct
import math
import tkMessageBox

ipaddress = "192.168.1.2" 

INITIAL_TAP_THRESHOLD = 0.012
FORMAT = pyaudio.paInt16 
SHORT_NORMALIZE = (1.0/32768.0)
CHANNELS = 2
RATE = 44100  
INPUT_BLOCK_TIME = 5.0
INPUT_FRAMES_PER_BLOCK = int(RATE*INPUT_BLOCK_TIME)
# if we get this many noisy blocks in a row, increase the threshold
OVERSENSITIVE = 15.0/INPUT_BLOCK_TIME                    
# if we get this many quiet blocks in a row, decrease the threshold
UNDERSENSITIVE = 120.0/INPUT_BLOCK_TIME 
# if the noise was longer than this many blocks, it's not a 'tap'
MAX_TAP_BLOCKS = 0.15/INPUT_BLOCK_TIME

def get_rms(block):
	# RMS amplitude is defined as the square root of the 
	# mean over time of the square of the amplitude.
	# so we need to convert this string of bytes into 
	# a string of 16-bit samples...

	# we will get one short out for each 
	# two chars in the string.
	count = len(block)/2
	format = "%dh"%(count)
	shorts = struct.unpack(format, block )
	#print(shorts)

	# iterate over the block.
	sum_squares = 0.0
	for sample in shorts:
		# sample is a signed short in +/- 32768. 
		# normalize it to 1.0
		n = sample * SHORT_NORMALIZE
		sum_squares += n*n
	print(math.sqrt(sum_squares/count))

	return math.sqrt( sum_squares / count )



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
		

hue = Hue("192.168.1.2")
hue.addUser("lesian")
hue.turnOn(hue.getLights()[1])


class TapTester(object):
	def __init__(self):
		self.pa = pyaudio.PyAudio()
		self.stream = self.open_mic_stream()
		self.tap_threshold = INITIAL_TAP_THRESHOLD
		self.noisycount = MAX_TAP_BLOCKS+1 
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
		try:
			block = self.stream.read(INPUT_FRAMES_PER_BLOCK)
		except IOError:
			# dammit. 
			self.errorcount += 1
			print( "(%d) Error recording: %s"%(self.errorcount,e) )
			self.noisycount = 1
			return

		amplitude = get_rms( block )
		if amplitude > self.tap_threshold:
			# noisy block
			self.quietcount = 0
			self.noisycount += 1
			hue.turnOn(hue.getLights()[1]) 
			if amplitude < 0.02:
				if flag > 0:
					flag = 0
				else:
					hue.setColor(hue.getLights()[1], 12750) # yellow
			elif amplitude < 0.03:
				if flag > 1:
					flag = 1
				else:
					hue.setColor(hue.getLights()[1], 25500) # green
			elif amplitude < 0.04:
				if flag > 2:
					flag = 2
				else:
					hue.setColor(hue.getLights()[1], 46920) # blue
			elif amplitude < 0.05:
				if flag > 3:
					hue.setColor(hue.getLights()[1], 56100) # Purple
				flag = 3
			else:
				hue.setColor(hue.getLights()[1], 65280) # Red
				flag = 4
			# if self.noisycount > OVERSENSITIVE:
			# 	# turn down the sensitivity
			# 	self.tap_threshold *= 1.1


		else:            
			# quiet block.
			if 1 <= self.noisycount <= MAX_TAP_BLOCKS:
				self.tapDetected()
			self.noisycount = 0
			self.quietcount += 1
			hue.turnOff(hue.getLights()[1])
			# if self.quietcount > UNDERSENSITIVE:
			# 	# turn up the sensitivity
			# 	self.tap_threshold *= 0.9

if __name__ == "__main__":
	tt = TapTester()

	for i in range(1000):
		tt.listen()