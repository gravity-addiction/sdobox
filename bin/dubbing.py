#!/usr/bin/env python3
from guizero import App, Combo, PushButton, Text
from screeninfo import get_monitors
from python_mpv_jsonipc import MPV
from threading import Thread
import socket
import subprocess
import os
import json
import time
import numpy as np
os.environ["DISPLAY"] = ":0.0"


sdobSocketKiller = 1
def thread_sdobSocket():
    global sdobSocketKiller
    global csock
    global dataSet
    print('Start Thread Socket')
    while sdobSocketKiller:
        try:
            (bytes, address) = csock.recvfrom(8196)
            msg = bytes.decode('utf-8')
            print('address:', address, 'recv', msg)
            # msgJson = json.loads(msg)
            # if type(msgJson) is dict:
            #    for k in msgJson.keys():
            #        setattr(dataSet, k, msgJson[k])
            #        print(k, msgJson[k])
            #else:
            #    setattr(dataSet, "socketdata", msg)
        except:
            time.sleep(0.2)
    # print('Stop Thread Socket')


def thread_getFrames(file):
    global dataSet
    global statusText
    statusText.value = "Getting KeyFrame Locations"
    cmd = "ffprobe -loglevel error -select_streams v:0 -show_entries packet=pts_time,flags -of csv=print_section=0 \"{}\" | awk -F',' '/K/ {{print $1}}'".format(file)
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    frameList = []
    for line in proc.stdout.readlines():
        frameList.append(line.decode('utf-8').rstrip())
    setattr(dataSet, "frameList", frameList)
    statusText.value = "Keyframes Found, Finished Looking"
    

def thread_processVideo():
    global dataSet
    global statusText
    
    videoRoot = '/tmp'
    videoFiles = []
    videoExt = '.mp4'
    videoDest = '/home/pi/Videos/120.mp4'

    if (hasattr(dataSet, "fileext")):
        videoExt = getattr(dataSet, "fileext")

    if (hasattr(dataSet, "slate")):
        # Slate
        sTime = float(getattr(dataSet, "slate")) - 2.0
        if (sTime < 0):
            sTime = 0.00
        eTime = float(getattr(dataSet, "slate")) + 5.0
        sTimes = splitVideoFileKeyTimes(sTime, eTime)
        # print("Slate Start Adding", sTime - sTimes[0])
        # print("Slate End Adding", sTimes[1] - eTime)
        videoFileName = 'sdobSlate{}'.format(videoExt)
        videoFile = '{}/{}'.format(videoRoot, videoFileName)
        videoFiles.append(videoFileName)
        splitVideoFile(getattr(dataSet, "filename"), sTimes[0], sTimes[1], videoFile)

    if (hasattr(dataSet, "exit")):
        statusText.value = "Splitting Video Skydive"
        # Skydive
        sTime = float(getattr(dataSet, "exit")) - 2.0
        if (sTime < 0):
            sTime = 0.00
        eTime = float(getattr(dataSet, "exit")) + 40.0
        sTimes = splitVideoFileKeyTimes(sTime, eTime)
        # print("Skydive Start Adding", sTime - sTimes[0])
        # print("Skydive End Adding", sTimes[1] - eTime)
        videoFileName = 'sdobExit{}'.format(videoExt)
        videoFile = '{}/{}'.format(videoRoot, videoFileName)
        videoFiles.append(videoFileName)
        splitVideoFile(getattr(dataSet, "filename"), sTimes[0], sTimes[1], videoFile)

    statusText.value = "Combining Video Slate and Skydive"
    # Recombine
    combineVideoFiles(videoRoot, videoFiles, videoDest)
    statusText.value = "Done Processing Video"

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
    # print("Submitting", getattr(dataSet, "filename"), getattr(dataSet, "slate"), getattr(dataSet, "exit"))
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    if (hasattr(dataSet, "filename") and hasattr(dataSet, "team") and hasattr(dataSet, "round")):
        print('Uploading For {:s} Round: {:d}'.format(getattr(dataSet, "team"), getattr(dataSet, "round")))

    if (hasattr(dataSet, "filename") and hasattr(dataSet, "slate") and hasattr(dataSet, "exit")):
        print('Splitting Video At', getattr(dataSet, "slate"), 'and', getattr(dataSet, "exit"));
        
        # Start to Stop
        # splitVideoFile(getattr(dataSet, "filename"), getattr(dataSet, "slate"), getattr(dataSet, "exit"), "/tmp/test1.mp4")
        statusText.value = "Processing Video"
        processVideoThread = Thread(target = thread_processVideo)
        processVideoThread.setDaemon(True)
        processVideoThread.start()
        
def combineVideoFiles(rootFolder, videoArr, outputFilename):
    videoListPath = rootFolder + "/sdobVideoList.txt"
    f = open(videoListPath, "w")
    for v in videoArr:
        f.write("file '{}'\n".format(v))
    f.close()
    vidCmd = 'ffmpeg -y -f concat -i  "{}" -c copy {}'.format(videoListPath, outputFilename)
    os.system(vidCmd)
    # proc = subprocess.Popen(vidCmd, shell=True, stderr=subprocess.PIPE)
    # for line in proc.stderr.readlines():
    #     statusText.value = line.decode('utf-8').rstrip()

def splitVideoFileKeyTimes(startingTime, endingTime):
    ret = np.zeros(2, dtype = float)
    startTime = 0.00
    endTime = 0.00
    checkStart = float(startingTime)
    checkEnd = float(endingTime)
    # print(startingTime, endingTime)
    for f in getattr(dataSet, "frameList"):
        # print('Keyframe', f)
        if (float(f) <= checkStart):
            ret[0] = float(f)
        if (endTime == 0.00 and float(f) >= checkEnd):
            ret[1] = float(f)
        if (ret[0] != 0.00 and ret[1] != 0.00):
            break
    return ret
    
def splitVideoFile(filename, startTime, endTime, outputFilename):
    extraArgs = ""
    if (endTime != 0.00):
        extraArgs = extraArgs + " -t " + str(endTime - startTime)
    
    # print("StartKey", startTime, "EndKey", endTime)
    vid1Cmd = 'ffmpeg -y -i "{}" -ss {} {} -c copy {}'.format(filename, startTime, extraArgs, outputFilename)
    os.system(vid1Cmd)
    statusText.value = "Done!"
    # print(vid1Cmd) 
    # proc = subprocess.Popen(vid1Cmd, shell=True, stderr=subprocess.PIPE)
    # for line in proc.stderr.readlines():
    #     statusText.value = line.decode('utf-8').rstrip()


def playnewVideo():
    mpv_player.loop = False
    mpv_player.play("/home/pi/Videos/120.mp4")
    csock.sendto(str.encode('{"event":"playing"}'), ssock_file)

def openButton():
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    filename = app.select_file(folder="/media/", filetypes=[["Media Files", "*.mp4 *.mov *.vob *.wmv *.mpg *.mpeg *.mkv *.m4v *.avi *.ts *.webm"], ["All Files", "*.*"]])
    if (isinstance(filename, str)):
        setattr(dataSet, "filename", filename)
        openvideo_btn.text = os.path.basename(filename)
        openvideo_btn.bg = "#cccc00"
        try:
            fExt = os.path.splitext(filename)[1]
            setattr(dataSet, "fileext", fExt)
        except:
            pass
        
        keyFrameThread = Thread(target = thread_getFrames, args=[filename])
        keyFrameThread.setDaemon(True)
        keyFrameThread.start()
        try:
            csock.sendto(str.encode('{{"event":"filechange","filename":"{}"}}'.format(filename)), ssock_file)
        except:
            print("Cannot send to sdobox.socket")
        
    else:
    # if (isinstance(filename, (unicode, tuple))):
        openvideo_btn.text = "Open Video"
        openvideo_btn.bg = "#cc33cc"

def playButton():
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    if (hasattr(dataSet, "filename")):
        mpv_player.loop = False
        mpv_player.play(getattr(dataSet, "filename"))
        try:
            csock.sendto(str.encode('{"event":"playing"}'), ssock_file)
        except:
            print("Cannot send playing to sdobox.socket")

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

appWidth = 720 # monitorList[0].width
appHeight = 480 # monitorList[0].height
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

# for x in range(0, 6):
#     rX = PushButton(app, command=selectButton, args=[x], text=str(x + 1), grid=[x, 5])
#     rX.text_size = fontSize
#     rX.width = 3
#     rX.bg = "white"
#     setattr(r, "round_{}".format(str(x)), rX)
  
# for x in range(6, 10):
#     rX = PushButton(app, command=selectButton, args=[x], text=str(x + 1), grid=[x - 6, 6])
#     rX.text_size = fontSize
#     rX.width = 3
#     rX.bg = "white"
#     setattr(r, "round_{}".format(str(x)), rX)



update_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Upload Video", grid=[0,7,3,1])
update_btn.text_size = fontSize
update_btn.disabled = True

playnew_btn = PushButton(app, command=playnewVideo, text="Play New Video", grid=[4,7,3,1])
playnew_btn.text_size = fontSize
playnew_btn.disabled = True

# move_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Move Video", grid=[3,7,3,1])
# move_btn.text_size = fontSize
# move_btn.disabled = True

statusText = Text(app, text="", align="left", size=14, grid=[0,10,7,1])

# Bind to key press events with a decorator
@mpv_player.on_event("start-file")
def evStartFile_handler(evData):
    setattr(dataSet, "playing", "1")
    
@mpv_player.on_event("end-file")
def evStartFile_handler(evData):
    setattr(dataSet, "playing", "0")

ssock_file = '/tmp/sdobox.socket';
csock_file = '/tmp/sdobox.dubbing.socket';

if os.path.exists(csock_file):
    os.remove(csock_file)
    
csock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
csock.bind(csock_file)
csock.setblocking(0)
csock.sendto(str.encode('{"event":"started"}'), ssock_file)

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
    csock.sendto(str.encode('{"event":"stopped"}'), ssock_file)
except:
    pass
finally:
    csock.close()

if os.path.exists(csock_file):
    os.remove(csock_file)

sdobThread.join()