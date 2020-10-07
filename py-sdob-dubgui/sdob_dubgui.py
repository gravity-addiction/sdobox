#!/usr/bin/env python3
from guizero import App, Combo, PushButton, Text
from screeninfo import get_monitors
from python_mpv_jsonipc import MPV
from threading import Thread
import socket
import os
import time
os.environ["DISPLAY"] = ":0.0"

sdobSocketKiller = 1
def thread_sdobSocket():
    global sdobSocketKiller
    global csock
    print('Start Thread Socket')
    while sdobSocketKiller:
        (bytes, address) = csock.recvfrom(1024)
        msg = bytes.decode('utf-8')
        print('address:', address, 'recv', msg)
    print('Stop Thread Socket')


def globalClicked():
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        mpv_player.stop()
        
def selectButton(t):
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    if (hasattr(dataSet, "team") and hasattr(dataSet, "round") and getattr(dataSet, "round") == t + 1):
        rX = getattr(r, "round_{}".format(str(t)))
        rX.bg = "blue"
        setattr(r, "round_{}".format(str(t)), rX)
        setattr(dataSet, "round", t + 1)
        print('Playing {} {}'.format(getattr(dataSet, "team"), getattr(dataSet, "round")))
    else:
        rKeys = r.__dict__.keys()
        for k in r.__dict__.keys():
            rX = getattr(r, k)
            rX.bg = "white"
            setattr(r, k, rX)
    
        rX = getattr(r, "round_{}".format(str(t)))
        rX.bg = "orange"
        setattr(r, "round_{}".format(str(t)), rX)
        setattr(dataSet, "round", t + 1)

def teamChanged(t):
    setattr(dataSet, "team", t)

def submitButton(t, r):
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    if (hasattr(dataSet, "filename") and hasattr(dataSet, "team") and hasattr(dataSet, "round")):
        print('Uploading For {:s} Round: {:d}'.format(getattr(dataSet, "team"), getattr(dataSet, "round")))

def openButton():
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    filename = app.select_file(folder="/media/pi/", filetypes=[["Media Files", "*.mp4 *.mov *.vob *.wmv *.mpg *.mpeg *.mkv *.m4v *.avi *.ts *.webm"], ["All Files", "*.*"]])
    if (filename == ""):
        openvideo_btn.text = "Open Video"
        openvideo_btn.bg = "#cc33cc"
    else:
        setattr(dataSet, "filename", filename)
        openvideo_btn.text = os.path.basename(filename)
        openvideo_btn.bg = "#cccc00"
        csock.sendto(str.encode('{{"event":"filechange","filename":"{}"}}'.format(filename)), ssock_file)

def playButton():
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    if (hasattr(dataSet, "filename")):
        mpv_player.loop = False
        mpv_player.play(getattr(dataSet, "filename"))
        csock.sendto(str.encode('{"event":"playing"}'), ssock_file)

class DataSet(object):
    pass

class TeamBtns(object):
    pass

class RoundBtns(object):
    pass

r = RoundBtns()
dataSet = DataSet()
selectedTeam = ""
selectedRound = 0

mpv_player = MPV(start_mpv=False, ipc_socket="/tmp/mpv.socket")
monitorList = get_monitors()
for m in monitorList:
    print(str(m))

appWidth = monitorList[0].width
appHeight = monitorList[0].height
appMulti = appWidth / 640
if (appMulti > 1):
    fontSize = int(26 * (appMulti * .85))
    fontSizeMin = int(16 * (appMulti * .85))
else:
    fontSize = 26
    fontSizeMin = 16


app = App(title="Skydive or Bust Dubbing Station", width=appWidth, height=appHeight, layout="grid")

# disc_combo = Combo(app, options=["Select Discipline", "2way Sequential ProAm", "2way Sequential Open", "4way Sequential Open", "4way Rotations"], command=teamChanged, grid=[0,1,8,1], align="top", width="23")
# disc_combo.text_size = fontSize
# disc_combo.bg = "#cc3333"

# team_combo = Combo(app, options=["Select Team", "Team 1", "Team 2", "Team 3", "Team 4"], command=teamChanged, grid=[0,2,6,1], align="top", width="23")
# team_combo.text_size = fontSize
# team_combo.bg = "#33cc33"

openvideo_btn = PushButton(app, command=openButton, text="Open Video", width="17", grid=[0,4,3,1])
openvideo_btn.text_size = fontSizeMin
openvideo_btn.bg = "#cc33cc"

playvideo_btn = PushButton(app, command=playButton, text="Play Video", width="17", grid=[3,4,3,1])
playvideo_btn.text_size = fontSizeMin

for x in range(0, 6):
    rX = PushButton(app, command=selectButton, args=[x], text=str(x + 1), grid=[x, 5])
    rX.text_size = fontSize
    rX.width = 3
    rX.bg = "white"
    setattr(r, "round_{}".format(str(x)), rX)
  
for x in range(6, 10):
    rX = PushButton(app, command=selectButton, args=[x], text=str(x + 1), grid=[x - 6, 6])
    rX.text_size = fontSize
    rX.width = 3
    rX.bg = "white"
    setattr(r, "round_{}".format(str(x)), rX)



update_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Upload Video", grid=[0,7,3,1])
update_btn.text_size = fontSize
update_btn.disabled = True

move_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Move Video", grid=[3,7,3,1])
move_btn.text_size = fontSize
move_btn.disabled = True

# Bind to key press events with a decorator
@mpv_player.on_event("start-file")
def evStartFile_handler(evData):
    setattr(dataSet, "playing", "1")
    
@mpv_player.on_event("end-file")
def evStartFile_handler(evData):
    setattr(dataSet, "playing", "0")

ssock_file = '/tmp/sdobox.socket';
csock_file = '/tmp/sdobox.dubgui.socket';

if os.path.exists(csock_file):
    os.remove(csock_file)
    
csock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
csock.bind(csock_file)

sdobThread = Thread(target = thread_sdobSocket)
sdobThread.setDaemon(True)
sdobThread.start()

app.when_clicked = globalClicked
app.display()

sdobSocketKiller = 0;

try:
    mpv_player.terminate()
except:
    pass

try:
    csock.close()
except:
    pass

if os.path.exists(csock_file):
    os.remove(csock_file)

sdobThread.join()