# """PyAudio example: Record a few seconds of audio and save to a WAVE file."""

# import pyaudio
# import wave
# from struct import *
# import numpy as np


# CHUNK = 1024 # number of frames the signals are split into. Maybe same as bins. 2^10. Analog read resolution is 10//input frames in one block
# #chunks just means what amount of information I will be reading
# FORMAT = pyaudio.paInt16   #I think this is to represent a 16 bit interger or something
# CHANNELS = 2 # each frame has two samples, each sample is 2 bytes hence 4 bytes per frame
# RATE = 44100  # samples per second. Rate at which analog signals are recorded in order to be converted into digital signals
# RECORD_SECONDS = 5 # record time
# WAVE_OUTPUT_FILENAME = "output.wav"

# p = pyaudio.PyAudio()

# stream = p.open(format=FORMAT,
# 				channels=CHANNELS,
# 				rate=RATE,
# 				input=True,
# 				frames_per_buffer=CHUNK)

# print("* recording")

# frames = []

# for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)): # how many signals in one chunk
# 	data = stream.read(CHUNK)
# 	print(np.frombuffer(data, dtype = 'B'))

# print("* done recording")

# stream.stop_stream()
# stream.close()
# p.terminate()

# wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
# wf.setnchannels(CHANNELS)
# wf.setsampwidth(p.get_sample_size(FORMAT))
# wf.setframerate(RATE)
# wf.writeframes(b''.join(frames))
# wf.close()


# !/usr/bin/python
# skeleton code from github
# open a microphone in pyAudio and listen for taps

# liblaries to make restful requests
import requests
import json

import pyaudio
import struct
import math
import tkMessageBox

INITIAL_TAP_THRESHOLD = 0.010
FORMAT = pyaudio.paInt16 
SHORT_NORMALIZE = (1.0/32768.0)
CHANNELS = 2
RATE = 44100  
INPUT_BLOCK_TIME = 0.05
INPUT_FRAMES_PER_BLOCK = int(RATE*INPUT_BLOCK_TIME)
# if we get this many noisy blocks in a row, increase the threshold
OVERSENSITIVE = 15.0/INPUT_BLOCK_TIME                    
# if we get this many quiet blocks in a row, decrease the threshold
UNDERSENSITIVE = 120.0/INPUT_BLOCK_TIME 
# if the noise was longer than this many blocks, it's not a 'tap'
MAX_TAP_BLOCKS = 0.15/INPUT_BLOCK_TIME

def get_rms( block ):
    # RMS amplitude is defined as the square root of the 
    # mean over time of the square of the amplitude.
    # so we need to convert this string of bytes into 
    # a string of 16-bit samples...

    # we will get one short out for each 
    # two chars in the string.
    count = len(block)/2
    format = "%dh"%(count)
    shorts = struct.unpack( format, block )
    print(shorts)

    # iterate over the block.
    sum_squares = 0.0
    for sample in shorts:
        # sample is a signed short in +/- 32768. 
        # normalize it to 1.0
        n = sample * SHORT_NORMALIZE
        sum_squares += n*n

    return math.sqrt( sum_squares / count )



class Hue(object):
    def __init__(self, ipaddress):
        self.ipaddress = ipaddress
        self.webaddress = "http://"+str(ipaddress)+"/api"

    def addUser(self, username):
        data = {'devicetype':'my_hue_app %s'%(username)}
        r = requests.post(self.webaddress, data = data)
        tkMessageBox.showinfo("", "Press the button")

        #here check and wait until button is pressed, get username
        # and modify the self.webaddress.

        #add a condition to make sure this was achieved, and return error
        #if it wasn't

    def getLights(self):
        r = requests.get(self.webaddress + "/lights")
        if r ==  {}:
            print("No lights were found")
            return
        else:
        #parse the jsons and return the values of lights

    def turnOn(self, lightid):
        data = {"on": true}
        r = requests.put(self.webaddress + "/lights/" + str(lightid)+ "/state", data = data)

    def turnOff(self, lightid):
        data = {"on": false}
        r = requests.put(self.webaddress + "/lights/" + str(lightid) + '/state', data = data)
        

hue = Hue(ipaddress)

class TapTester(object):
    def __init__(self):
        self.pa = pyaudio.PyAudio()
        self.stream = self.open_mic_stream()
        self.tap_threshold = INITIAL_TAP_THRESHOLD
        self.noisycount = MAX_TAP_BLOCKS+1 
        self.quietcount = 0 
        self.errorcount = 0
        onFlag = False

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
        print "Tap!"
        if onFlag == True:
            hue.turnOff(1)
        else:
            hue.turnOn(1)


    def listen(self):
        try:
            block = self.stream.read(INPUT_FRAMES_PER_BLOCK)
        except IOError, e:
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
            if self.noisycount > OVERSENSITIVE:
                # turn down the sensitivity
                self.tap_threshold *= 1.1
        else:            
            # quiet block.

            if 1 <= self.noisycount <= MAX_TAP_BLOCKS:
                self.tapDetected()
            self.noisycount = 0
            self.quietcount += 1
            if self.quietcount > UNDERSENSITIVE:
                # turn up the sensitivity
                self.tap_threshold *= 0.9












if __name__ == "__main__":
    tt = TapTester()

    for i in range(1000):
        tt.listen()