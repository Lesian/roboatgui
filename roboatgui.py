#!/usr/bin/python

# Lesian Elias Lengare #
# Senseable City Lab #
# Roboat Control Graphical User Interface #

try:
	from Tkinter import *
except(ImportError):
	from tkinter import *
	from tkinter import messagebox

from serial import *
import sys
import glob


#-------Parameters for wireless communication------###
STARTBYTE = b'\xAA'   #first byte
ROBOTID = b'\x01'     #second byte
COMMANDMODE = b'\x04' #third byte 
COMMANDDATALENGTH = b'\x02' # fourth byte
ROBOTSPEED = b'\x00'  #fifth byte 
ROBOTDIRECTION = b'\x00' # sixth byte
ENDBYTE = b'\xFC'     #seventh/last byte 
#-------Parameters for wireless communication-------###



##------Serial port variables-------##
serialPort = ""
baudRate = "9600"   ## default to 9600
ser = Serial()
serBuffer = ""
Flag = False
##------Serial port variables------##

root = Tk()  #main window
root.wm_title("Roboat Motion Control Platform") #Makes the title that will appear in the top left
root.config()
#root.pack(fill = BOTH, expand = YES)

#Left Frame and its contents
leftFrame = Frame(root, width=200, height = 600)
leftFrame.grid(row=0, column=0, padx=0, pady=0, sticky = NW)

# Configuration Frame
ConfigFrame = Frame(leftFrame, width = 100, height = 600)
ConfigFrame.grid(row = 0, column = 0, padx = 0, pady = 2, sticky = W)
Label(ConfigFrame, text="UART Configuration").grid(row=0, column=0, padx=20, pady=0)

# Speed Frame

SpeedFrame = Frame(leftFrame, width = 800, height = 600)
SpeedFrame.grid(sticky = NE, row = 0)
Label(SpeedFrame, text="Speed/Direction").grid()
#Label(SpeedFrame, text = "Speed").grid(row = 1, column = 0, padx = 0, pady = 0)

# Port selection

##--------A function to identify the ports that are currently active ----###
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

try:
	serialPort = serial_port()[0]
except:
	pass

tkvar = StringVar(ConfigFrame)
try:
	serial_port()[0]
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
popupMenu2.grid(row = 4, column = 0)
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
popupMenu3.grid(row = 6, column = 0)
Label(ConfigFrame, text = "Parity Bit").grid(row = 5, column = 0)

def change_dropdown(*args):
	print(tkvar3.get())

tkvar3.trace('w', change_dropdown)

# Bit selection
tkvar4 = StringVar(ConfigFrame)
bitschoices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
tkvar4.set(8)
popupMenu4 = OptionMenu(ConfigFrame, tkvar4, *bitschoices)
popupMenu4.grid(row = 8, column = 0)
Label(ConfigFrame, text = "Data Bits").grid(row = 7, column = 0)

def change_dropdown(*args):
	print(tkvar2.get())

tkvar4.trace('w', change_dropdown)

# Stop bit selection
tkvar5 = StringVar(ConfigFrame)
stopchoices = {0, 1, 2}
tkvar5.set(0)
popupMenu5 = OptionMenu(ConfigFrame, tkvar5, *stopchoices)
popupMenu5.grid(row = 10, column = 0)
Label(ConfigFrame, text = "Data Bits").grid(row = 9, column = 0)

def change_dropdown(*args):
	print(tkvar5.get())

tkvar5.trace('w', change_dropdown)

# Speed selection
tkvar6 = StringVar(SpeedFrame)
speedchoices = {1, 2, 3, 4, 5, 6, 7, 8, 9}
tkvar6.set(1)
popupMenu6 = OptionMenu(SpeedFrame, tkvar6, *speedchoices)
popupMenu6.grid(row = 2, column = 0)
Label(SpeedFrame, text = "Speed").grid(row = 1, column = 0)

def change_dropdown(*args):
	print(tkvar6.get())

tkvar6.trace('w', change_dropdown)

# Direction Selection
tkvar7 = StringVar(SpeedFrame)
dirchoices = {0, 1, 2, 3}
tkvar7.set(1)
popupMenu7 = OptionMenu(SpeedFrame, tkvar6, *dirchoices)
popupMenu7.grid(row = 4, column = 0)
Label(SpeedFrame, text = "Direction").grid(row = 3, column = 0)

def change_dropdown(*args):
	print(tkvar7.get())

tkvar7.trace('w', change_dropdown)

dirsetbtn = Button(SpeedFrame, text = "Direction Set")
dirsetbtn.grid(row = 4, column = 1, padx = 0, pady = 0, sticky = E)

speedsetbtn = Button(SpeedFrame, text = "Speed Set")
speedsetbtn.grid(row = 2, column = 1, padx = 0, pady = 0, sticky = E)

def uartOpCallback():
	global serialPort
	global baudRate
	global ser
	global Flag
	try:
		ser = Serial(serialPort, baudRate, timeout=0, writeTimeout=0, stopbits = 1, parity = 'N', bytesize = 8) #ensure non-blocking
		Flag = True
		print('Serial port initialization successful')
	except:
		print("Serial port initialization failed")


def uartclCallback():
	global ser
	global Flag
	Flag = False
	ser.close()


uartopbtn = Button(ConfigFrame, text = "OPEN UART PORT", width = 16, command = uartOpCallback)
uartopbtn.grid(row = 2, column = 8, padx = 0, pady = 0)

uartclbtn = Button(ConfigFrame, text = "CLOSE UART PORT", width = 16, command = uartclCallback)
uartclbtn.grid(row = 4, column = 8, padx = 0, pady = 0)


# Right Frame and its contents
rightFrame = Frame(root, width = 200, height = 200)
rightFrame.grid(row = 0, column = 20, padx = 20, pady = 2, sticky = N)
Label(rightFrame, text= "Data Send Area").grid(row = 0, column = 0, padx = 0, pady = 2, sticky = W)

newText = Text(rightFrame, width = 30, height = 10, takefocus = 0)
newText.grid(row = 2, column = 0, padx = 0, pady = 0)

Label(rightFrame, text = "Data Receive Area").grid(row = 11, column = 0, padx = 0, pady = 0, sticky = W)

newText2 = Text(rightFrame, width = 30, height = 20, takefocus = 0)
newText2.grid(row = 15, padx = 0, pady = 0, sticky = E)

def dataClrCallback():
	newText2.delete("1.0", END)

DataClrBtn = Button(rightFrame, text = "Clear Data", command = dataClrCallback)
DataClrBtn.grid(row = 11, column = 0, padx = 0, pady = 0, sticky = E)

Datasendbtn = Button(rightFrame, text = "Send Data")
Datasendbtn.grid(row = 0, padx = 0, pady = 0, sticky = E)

# Thruster speed Control Frame
ThrustFrame = Frame(leftFrame, width = 10, height = 10)
ThrustFrame.grid(row = 16, column = 0, padx = 0, pady = 0, sticky = 'W')

Label(ThrustFrame, text = "Thruster Speed Control").grid(row = 0, column = 1, padx = 0, pady = 2, sticky = "NSEW")


Leftplusbtn = Button(ThrustFrame, text = "LeftThruster+", width = 12)
Leftplusbtn.grid(row = 1, column = 0, padx = 0, pady = 0)

Leftminbtn = Button(ThrustFrame, text = "LeftThruster-", width = 12)
Leftminbtn.grid(row = 2, column = 0, padx = 0, pady = 0)

Rightplusbtn = Button(ThrustFrame, text = "RightThruster+", width = 12)
Rightplusbtn.grid(row = 1, column = 1, padx = 0, pady = 0)

Rightminbtn = Button(ThrustFrame, text = "RightThruster-", width = 12)
Rightminbtn.grid(row = 2, column = 1, padx = 0, pady = 0)

Frontupbtn = Button(ThrustFrame, text = "FrontThruster+", width = 12)
Frontupbtn.grid(row = 1, column = 2, padx = 0, pady = 0)

Frontdwbtn = Button(ThrustFrame, text = "FrontThruster-", width = 12)
Frontdwbtn.grid(row = 2, column = 2, padx = 0, pady = 0)

Rearupbtn = Button(ThrustFrame, text = "RearThruster+", width = 12)
Rearupbtn.grid(row = 1, column = 3, padx = 0, pady = 0)

Reardwbtn = Button(ThrustFrame, text = "RearThruster-", width = 12)
Reardwbtn.grid(row = 2, column = 3, padx = 0, pady = 0)


# Swimming Behavior Frame
SwimmingFrame = Frame(leftFrame, width = 10, height = 10)
SwimmingFrame.grid(row = 28, column = 0, padx = 0, pady = 0, sticky = "W")

Label(SwimmingFrame, text = "Swimming Behaviors").grid(row = 0, column = 2, padx = 0, pady = 2, sticky = "NSEW")

Fowbtn = Button(SwimmingFrame, text = "Forward", width = 12)
Fowbtn.grid(row = 1, column = 0, padx = 0, pady = 0)

Backbtn = Button(SwimmingFrame, text = "Backward", width = 12)
Backbtn.grid(row = 2, column = 0, padx = 0, pady = 0)

Leftbtn = Button(SwimmingFrame, text = "Left Sideways", width = 12)
Leftbtn.grid(row = 1, column = 2, padx = 0, pady = 0)

Rightbtn = Button(SwimmingFrame, text = "Right Sideways", width = 12)
Rightbtn.grid(row = 2, column = 2, padx = 0, pady = 0)

CCturnbtn = Button(SwimmingFrame, text = "Spot Turn CC", width = 12)
CCturnbtn.grid(row = 1, column = 4, padx = 0, pady = 0)

ACCturnbtn = Button(SwimmingFrame, text = "Spot Turn AntiCC", width = 12)
ACCturnbtn.grid(row = 2, column = 4, padx = 0, pady = 0)

LeftCirbtn = Button(SwimmingFrame, text = "Turn Left Circle", width = 12)
LeftCirbtn.grid(row = 1, column = 6, padx = 0, pady = 0)

RightCirbtn = Button(SwimmingFrame, text = "Turn Right Circle", width = 12)
RightCirbtn.grid(row = 2, column = 6, padx = 0, pady = 0)


Stopbtn = Button(SwimmingFrame, text = "STOP", fg = 'red')
Stopbtn.grid(row = 2, column = 8, padx = 0, pady = 0)


RecFrame = Frame(leftFrame, height = 10, width = 10)
RecFrame.grid(row = 30, column = 0, sticky = W)

Label(RecFrame, text = "").grid(row = 0, column = 0, pady = 2)

def startRecCallback():
	global ser
	global Flag
	Flag = True
	def readSerial():
		global ser
		while Flag:
			c = ser.read() # attempt to read a character from Serial
			c = str(c, 'utf-8')
			
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
				
				#add the line to the TOP of the log
				newText2.insert('0.0', serBuffer)
				serBuffer = "" # empty the buffer
			else:
				serBuffer += c # add to the buffer
		root.after(10, readSerial) # check serial again soon
	root.after(100, readSerial)

def stopRecCallback():
	global ser
	global Flag
	Flag = False


Startrec = Button(RecFrame, text = "START RECORDING", command = startRecCallback)
Startrec.grid(row = 2, column = 0, pady = 2, sticky = W)

Stoprec = Button(RecFrame, text = "STOP RECORDING", fg = "red", command = stopRecCallback)
Stoprec.grid(row = 2, column = 3, pady = 2, sticky = W)
#start monitoring and updating the GUI


def leftKey(event):
	print("Left key pressed")

def rightKey(event):
	print("Right key pressed")

def upKey(event):
	global ser
	ser.write(STARTBYTE)
	
	ser.write(ROBOTID)
	
	ser.write(COMMANDMODE)
	
	ser.write(COMMANDDATALENGTH)
	
	ser.write(ROBOTSPEED)
	
	ser.write(ROBOTDIRECTION)

	ser.write(ENDBYTE)

def downKey(event):
	print("Down key pressed")

def plusKey(event):
	print("Plus key pressed")

def minusKey(event):
	print('Minus key pressed')

###------Communication between the higher level controller and the microcontroller #####

def Send_float_to_byte():
	pass

def Receive_byte_to_float():
	pass

##------End of communication to higher level controller---###

root.bind('<Left>', leftKey)
root.bind('<Right>', rightKey)
root.bind('<Up>', upKey)
root.bind('<Down>', downKey)
root.bind('+', plusKey)
root.bind('-', minusKey)
root.bind('=', plusKey)
root.focus_set()
root.mainloop() 