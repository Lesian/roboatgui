#!/usr/bin/python

#--------Lesian Lengare --------#
#--------Senseable city lab-----#
#--------Roboat gui-------------#

try:
	from Tkinter import *
except(ImportError):
	from tkinter import *

from serial import *
import sys
import glob
import tkMessageBox


#-----flags----- #
uartflag = False
movflag = 0
uartrecflag = True
#-----flags----- #

#---------speed and direction variables-----#
robotspeed = chr(10).encode()
robotdirection = b'\x00'
#---------speed and direction variables-----#


##------Serial port variables-------##
serialPort = ""
baudRate = "115200"  ## default to 9600
ser = Serial()
serBuffer = ""
##------Serial port variables------##

root = Tk()  #main window
root.wm_title("Roboat Motion Control Platform") 
root.config()
root.resizable(width = False, height = False) # fixed size gui


# Configuration Frame
ConfigFrame = Frame(root, width = 10, height = 10)
ConfigFrame.grid(row = 4, column = 0, padx =0, pady = 0, sticky = "W")
Label(root, text = "UART Configuration").grid(row = 0, column = 0, padx = 0, pady = 0, sticky = "N")


#--------A function to identify the ports that are currently active ----#
def serial_port():
	if sys.platform.startswith('win'):
		ports = ['COM%s' %(i+1) for i in range(256)]
	elif sys.platform.startswith("linux") or sys.platform.startwith("cygwin"):
		ports = glob.glob('/dev/tty[A-Za-z]*')
	elif sys.platform.startswith('darwin'):
		ports = glob.glob('/dev/tty.*')
	else:
		raise EnvironmentError('Unsupported Platform')

	result = []
	for port in ports:
		try:
			s = Serial(port)
			s.close()
			result.append(port)
		except(OSError, SerialException):
			pass
	return result



tkvar = StringVar(ConfigFrame)
try:                        # to see if any ports are available
	serialPort = serial_port()[0]
	portchoices = set(serial_port())
	tkvar.set(serial_port()[0])
except:
	portchoices = {"NO PORT"}
	tkvar.set("NO PORT")
popupMenu = OptionMenu(ConfigFrame, tkvar, *portchoices)
Label(ConfigFrame, text = "PORT").grid(row = 1, column = 0)
popupMenu.grid(row = 2, column = 0)

def change_dropdown(*args):
	global serialPort
	serialPort = str(tkvar.get())
	print(tkvar.get())


tkvar.trace('w', change_dropdown)

# Baud selection
tkvar2 = StringVar(ConfigFrame)
baudchoices = sorted({9600, 19200, 38400, 57600, 115200})
tkvar2.set(9600)
popupMenu2 = OptionMenu(ConfigFrame, tkvar2, *baudchoices)
popupMenu2.grid(row = 4, column = 0, ipadx = 10)
Label(ConfigFrame, text = "Baudrate").grid(row = 3, column = 0)

def change_dropdown(*args):
	global baudRate
	baudRate = int(tkvar2.get())
	print(tkvar2.get())

tkvar2.trace('w', change_dropdown)

# Parity bit selection
tkvar3 = StringVar(ConfigFrame)
paritybit = sorted({0, 1, 2})
tkvar3.set(0)
popupMenu3 = OptionMenu(ConfigFrame, tkvar3, *paritybit)
popupMenu3.grid(row = 6, column = 0, ipadx = 14)
Label(ConfigFrame, text = "Parity Bit").grid(row = 5, column = 0)

def change_dropdown(*args):
	print(tkvar3.get())

tkvar3.trace('w', change_dropdown)

# Bit selection
tkvar4 = StringVar(ConfigFrame)
bitschoices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
tkvar4.set(8)
popupMenu4 = OptionMenu(ConfigFrame, tkvar4, *bitschoices)
popupMenu4.grid(row = 8, column = 0, ipadx = 14)
Label(ConfigFrame, text = "Data Bits").grid(row = 7, column = 0)

def change_dropdown(*args):
	print(tkvar4.get())

tkvar4.trace('w', change_dropdown)

# Stop bit selection
tkvar5 = StringVar(ConfigFrame)
stopchoices = {0, 1, 2}
tkvar5.set(0)
popupMenu5 = OptionMenu(ConfigFrame, tkvar5, *stopchoices)
popupMenu5.grid(row = 10, column = 0, ipadx = 14)
Label(ConfigFrame, text = "Data Bits").grid(row = 9, column = 0)

def change_dropdown(*args):
	print(tkvar5.get())

tkvar5.trace('w', change_dropdown)

#-----Opening uart-----#
def uartOpCallback():
	global ser
	global uartflag
	if not uartflag:
		try:
			ser = Serial(serialPort, baudRate, timeout=0, writeTimeout=None, stopbits = 1, parity = 'N', bytesize = 8,  xonxoff=False, rtscts = False, dsrdtr=False) #ensure non-blocking
			uartflag = True
		except:
			uartError()
	else:
		tkMessageBox.showerror("Error", "One of the ports is already open.")


#-----failure uart open message---#
def uartError():
	tkMessageBox.showerror("Error", "Port not open")

	
#----Closing uart-----#	
def uartclCallback():
	global uartflag
	if uartflag:
		ser.close()
		uartflag = False
	else:
		tkMessageBox.showerror("Error", "No open serial port")
	
	

def forwardCallback():
	global movflag
	global robotspeed
	commandmode = b'\x04' 
	robotspeed = chr(10).encode()   
	robotdirection = b'\x00'
	movflag = 0
	sendCommand(commandmode, robotspeed, robotdirection)

def backwardCallback():
	global movflag
	global robotspeed
	commandmode = b'\x05'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 1
	sendCommand(commandmode, robotspeed, robotdirection)

def leftCallback():
	global movflag
	global robotspeed
	commandmode = b'\x06'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 2
	sendCommand(commandmode, robotspeed, robotdirection)

def rightCallback():
	global movflag
	global robotspeed
	commandmode = b'\x07'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 3
	sendCommand(commandmode, robotspeed, robotdirection)

def spotTurnCallback():
	global robotspeed
	global movflag
	commandmode = b'\x08'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 4
	sendCommand(commandmode, robotspeed, robotdirection)

def spotantiTurnCallback():
	global robotspeed
	global movflag
	commandmode = chr(9).encode()
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 5
	sendCommand(commandmode, robotspeed, robotdirection)

def turnLeftCallback():
	global robotspeed
	global movflag
	commandmode = chr(11).encode()
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 6
	sendCommand(commandmode, robotspeed, robotdirection)

def turnRightCallback():
	global robotspeed
	global movflag
	commandmode = chr(10).encode()
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 7
	sendCommand(commandmode, robotspeed, robotdirection)


def leftKey(event):
	global movflag
	global robotspeed
	commandmode = b'\x06'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 2
	sendCommand(commandmode, robotspeed, robotdirection)


def rightKey(event):
	global movflag
	global robotspeed
	commandmode = b'\x07'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 3
	sendCommand(commandmode, robotspeed, robotdirection)

def upKey(event):
	global movflag
	global robotspeed
	commandmode = b'\x04' 
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 0
	sendCommand(commandmode, robotspeed, robotdirection)

def downKey(event):
	global movflag
	global robotspeed
	commandmode = b'\x05'
	robotspeed = chr(10).encode()
	robotdirection = b'\x00'
	movflag = 1
	sendCommand(commandmode, robotspeed, robotdirection)




def plusKey(event):
	global robotspeed
	temp = robotspeed
	temp = temp.decode()
	temp = ord(temp) + 2
	temp = chr(temp)
	temp = temp.encode()
	robotspeed = temp
	if movflag == 0:
		sendCommand(b'\x04', robotspeed, robotdirection)
	elif movflag == 1:
		sendCommand(b'\x05', robotspeed, robotdirection)
	elif movflag == 2:
		sendCommand(b'\x06', robotspeed, robotdirection)
	elif movflag == 3:
		sendCommand(b'\x07', robotspeed, robotdirection)




def minusKey(event):
	global robotspeed
	temp = robotspeed
	temp = temp.decode()
	temp = ord(temp) - 2
	temp = chr(temp)
	temp = temp.encode()
	robotspeed = temp
	if movflag == 0:
		sendCommand(b'\x04', robotspeed, robotdirection)
	elif movflag == 1:
		sendCommand(b'\x05', robotspeed, robotdirection)
	elif movflag == 2:
		sendCommand(b'\x06', robotspeed, robotdirection)
	elif movflag == 3:
		sendCommand(b'\x06', robotspeed, robotdirection)




uartopbtn = Button(ConfigFrame, text = "OPEN UART PORT", width = 16, command = uartOpCallback)
uartopbtn.grid(row = 2, column = 40, padx = 60, pady = 0)

uartclbtn = Button(ConfigFrame, text = "CLOSE UART PORT", width = 16, command = uartclCallback)
uartclbtn.grid(row = 4, column = 40, padx = 60, pady = 0)


# Swimming Behavior Frame
SwimmingFrame = Frame(root, width = 10, height = 10)
SwimmingFrame.grid(row = 28, column = 0, padx =0, pady = 0, sticky = "W")

Label(SwimmingFrame, text = "Swimming Behaviors").grid(row = 0, column = 2, padx = 0, pady = 2, sticky = "NSEW")

Fowbtn = Button(SwimmingFrame, text = "Forward", width = 14, command = forwardCallback)
Fowbtn.grid(row = 1, column = 0, padx = 2, pady = 2)

Backbtn = Button(SwimmingFrame, text = "Backward", width = 14, command = backwardCallback)
Backbtn.grid(row = 2, column = 0, padx = 2, pady = 2)

Leftbtn = Button(SwimmingFrame, text = "Left Sideways", width = 14, command = leftCallback)
Leftbtn.grid(row = 1, column = 2, padx = 2, pady = 2)

Rightbtn = Button(SwimmingFrame, text = "Right Sideways", width = 14, command = rightCallback)
Rightbtn.grid(row = 2, column = 2, padx = 2, pady = 2)

CCturnbtn = Button(SwimmingFrame, text = "Spot Turn CC", width = 14, command = spotTurnCallback)
CCturnbtn.grid(row = 3, column = 0, padx = 2, pady = 2)

ACCturnbtn = Button(SwimmingFrame, text = "Spot Turn AntiCC", width = 14, command = spotantiTurnCallback)
ACCturnbtn.grid(row = 4, column = 0, padx = 2, pady = 2)

LeftCirbtn = Button(SwimmingFrame, text = "Turn Left Circle", width = 14, command = turnLeftCallback)
LeftCirbtn.grid(row = 3, column = 2, padx = 2, pady = 2)

RightCirbtn = Button(SwimmingFrame, text = "Turn Right Circle", width = 14, command = turnRightCallback)
RightCirbtn.grid(row = 4, column = 2, padx = 2, pady = 2)

#---Keyboard Roboat stops---#
def spaceKey(event):
	global robotspeed
	commandmode = b'\x00'
	robotspeed = b'\x00'
	robotdirection = b'\x00'
	sendCommand(commandmode, robotspeed, robotdirection)
#---Robot stops---#

#---Gui roboat stops----#
def stopbtnCallback():
	global robotspeed
	commandmode = b'\x00'
	robotspeed = b'\x00'
	robotdirection = b'\x00'
	sendCommand(commandmode, robotspeed, robotdirection)
#---Gui roboat stops----#

Stopbtn = Button(SwimmingFrame, text = "STOP", fg = 'red', command = stopbtnCallback, width = 14)
Stopbtn.grid(row = 1, column = 8, padx = 0, pady = 0)

RecFrame = Frame(root, height = 10, width = 10)
RecFrame.grid(row = 30, column = 0, sticky = W)

Label(RecFrame, text = "").grid(row = 0, column = 0, pady = 2)

newText = Text(SwimmingFrame, width = 4, height = 1)
newText.grid(row = 2, column = 8, padx = 0, pady = 0, sticky = 'W')

def speedSelectCallback():
	global robotspeed
	global robotdirection
	ans = int(newText.get('1.0', END))
	print(ans)
	temp = chr(int(ans))
	robotspeed = temp.encode()
	if movflag == 0:
		sendCommand(b'\x04', robotspeed, robotdirection)
	elif movflag == 1:
		sendCommand(b'\x05', robotspeed, robotdirection)
	elif movflag == 2:
		sendCommand(b'\x06', robotspeed, robotdirection)
	elif movflag == 3:
		sendCommand(b'\x07', robotspeed, robotdirection)


speedSelect = Button(SwimmingFrame, text = "Speed Select", width = 8, command = speedSelectCallback)
speedSelect.grid(row = 2, column = 8, sticky = 'E')


##---function to read the output of serial ---##
##---used only for debugging and not in the main program ###
def startRecCallback():
	global ser
	global uartrecflag
	def readSerial():
		global ser
		while uartrecflag:
			c = ser.read() # attempt to read a character from Serial
			try:
				c = str(c, 'utf-8')
			except:
				pass
			#was anything read?
			if len(c) == 0:
				break
			
			# get the buffer from outside of this function
			global serBuffer
			
			# check if character is a delimeter
			if c == '\r':
				c = '' # don't want returns. chuck it
				
			if c == '\n':
				serBuffer += "\n" # add the newline to the buffer
				
				#add the line to the TOP of the lonewText2.insert('0.0', serBuffer)
				print(serBuffer)
				serBuffer = "" # empty the buffer
			else:
				serBuffer += c # add to the buffer
		root.after(10, readSerial) # check serial again soon
	root.after(100, readSerial)

def sendCommand(commandmode, robotspeed, robotdirection):
	try:
		assert uartflag == True
		STARTBYTE = b'\xAA'   #first byte
		ROBOTID = b'\x01'     #second byte
		COMMANDMODE = commandmode #third byte 
		COMMANDDATALENGTH = b'\x02' # fourth byte
		ROBOTSPEED = robotspeed  #fifth byte 
		ROBOTDIRECTION = robotdirection # sixth byte
		ENDBYTE = b'\xFC'     #seventh/last byte 
		ser.write(STARTBYTE)
		time.sleep(0.13)
		ser.write(ROBOTID)
		time.sleep(0.13)
		ser.write(COMMANDMODE)
		time.sleep(0.13)
		ser.write(COMMANDDATALENGTH)
		time.sleep(0.13)
		ser.write(ROBOTSPEED)
		time.sleep(0.13)
		ser.write(ROBOTDIRECTION)
		time.sleep(0.13)
		ser.write(ENDBYTE)
		newText.delete(1.0, END)
		newText.insert(END, str(ord(robotspeed.decode())))
		time.sleep(0.13)

	except:
		uartError()

speedSelect.bind("<1>", lambda event: speedSelect.focus_set())

root.bind('<Left>', leftKey)
root.bind('<Right>', rightKey)
root.bind('<Up>', upKey)
root.bind('<Down>', downKey)
root.bind('+', plusKey)
root.bind('-', minusKey)
root.bind('=', plusKey)
root.bind('<space>', spaceKey)
root.focus_set()
root.mainloop() 