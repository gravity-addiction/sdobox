#!/usr/bin/env python3
from screeninfo import get_monitors
from threading import Thread
from os.path import basename, expanduser, isfile, join as joined
from pathlib import Path
from glob import glob
from subprocess import check_output, CalledProcessError
import time
import socket
import subprocess
import sys
if sys.version_info[0] < 3:
    import Tkinter as Tk
    from Tkinter import ttk
    from Tkinter.filedialog import askopenfilename
    from Tkinter.tkMessageBox import showerror
else:
    import tkinter as Tk
    from tkinter import ttk
    from tkinter.filedialog import askopenfilename
    from tkinter.messagebox import showerror

import os
import json

import requests
import json
import numpy as np
os.environ["DISPLAY"] = ":0.0"

import vlc







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
            msgJson = json.loads(msg)
            if type(msgJson) is dict:
                if (msgJson["event"] and msgJson["event"] == "openvideo"):
                    print("Msg Event", msgJson["event"])
                    if (msgJson["root"] and msgJson["root"] != ""):
                        openButton(msgJson["root"])
                    else:
                        openButton("/media")
                        
                if (msgJson["data"]):
                    print("Msg Data", msgJson["data"])
            #    for k in msgJson.keys():
            #        setattr(dataSet, k, msgJson[k])
            #        print(k, msgJson[k])
            else:
                print("Msg", msg)
                # setattr(dataSet, "socketdata", msg)
        except:
            time.sleep(0.2)
    # print('Stop Thread Socket')


def thread_getFrames(file):
    global dataSet
    global statusText
    # statusText.value = "Getting KeyFrame Locations"
    cmd = "ffprobe -loglevel error -select_streams v:0 -show_entries packet=pts_time,flags -of csv=print_section=0 \"{}\" | awk -F',' '/K/ {{print $1}}'".format(file)
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    frameList = []
    for line in proc.stdout.readlines():
        lineDec = line.decode('utf-8').rstrip()
        frameList.append(lineDec)
    setattr(dataSet, "frameList", frameList)
    # statusText.value = "Keyframes Found, Finished Looking"
    

def thread_processVideo():
    global dataSet
    global statusText
    
    videoRoot = '/tmp'
    videoFiles = []
    videoExt = '.mp4'
    videoDest = '/home/pi/Videos/' + str(time.time)

    player.progressBar['value'] += 10
    root.update_idletasks()
    if (hasattr(dataSet, "fileext")):
        videoExt = getattr(dataSet, "fileext")

    if (hasattr(dataSet, "dest")):
        videoDest = getattr(dataSet, "dest") + videoExt
    else:
        return

 
    if (hasattr(dataSet, "slate")):
        # Slate
        sTime = float(getattr(dataSet, "slate")) - 2.5
        if (sTime < 0):
            sTime = 0.00
        eTime = float(getattr(dataSet, "slate")) + 2.5
        sTimes = splitVideoFileKeyTimes(sTime, eTime)
        player.progressBar['value'] += 10
        # print("Slate Start Adding", sTime - sTimes[0])
        # print("Slate End Adding", sTimes[1] - eTime)
        # print("Slate Chopping", sTimes[0], sTimes[1])
        videoFileName = 'sdobSlate{}'.format(videoExt)
        videoFile = '{}/{}'.format(videoRoot, videoFileName)
        videoFiles.append(videoFileName)
        splitVideoFile(getattr(dataSet, "filename"), sTimes[0], sTimes[1], videoFile)
        player.progressBar['value'] += 10
        root.update_idletasks()
    if (hasattr(dataSet, "exit")):
        #statusText.value = "Splitting Video Skydive"
        # Skydive
        sTime = float(getattr(dataSet, "exit")) - 5.0
        if (sTime < 0):
            sTime = 0.00
        eTime = float(getattr(dataSet, "exit")) + 10.0
        sTimes = splitVideoFileKeyTimes(sTime, eTime)
        player.progressBar['value'] += 10
        # print("Skydive Start Adding", sTime - sTimes[0])
        # print("Skydive End Adding", sTimes[1] - eTime)
        # print("Skydive Chopping", sTimes[0], sTimes[1])
        videoFileName = 'sdobExit{}'.format(videoExt)
        videoFile = '{}/{}'.format(videoRoot, videoFileName)
        videoFiles.append(videoFileName)
        splitVideoFile(getattr(dataSet, "filename"), sTimes[0], sTimes[1], videoFile)
        player.progressBar['value'] += 10
        root.update_idletasks()
    #statusText.value = "Combining Video Slate and Skydive"
    # Recombine
    combineVideoFiles(videoRoot, videoFiles, videoDest)
    player.progressBar['value'] = 100
    root.update_idletasks()
    #statusText.value = "Done Processing Video"



def submitButton():
    if (hasattr(dataSet, "filename") and hasattr(dataSet, "slate") and hasattr(dataSet, "exit")):
        print('Splitting Video At', getattr(dataSet, "slate"), 'and', getattr(dataSet, "exit"))
        
        # Start to Stop
        # splitVideoFile(getattr(dataSet, "filename"), getattr(dataSet, "slate"), getattr(dataSet, "exit"), "/tmp/test1.mp4")
        # statusText.value = "Processing Video"
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
    player.progressBar['value'] += 10
    ret = np.zeros(2, dtype = float)
    startTime = 0.00
    endTime = 0.00
    checkStart = float(startingTime)
    checkEnd = float(endingTime)
    print(startingTime, endingTime)
    for f in getattr(dataSet, "frameList"):
        print('Keyframe', f)
        if (float(f) <= checkStart):
            ret[0] = float(f)

        if (endTime == 0.00 and float(f) >= checkEnd):
            ret[1] = float(f)
        
        if (ret[1] != 0.00):
            break
    player.progressBar['value'] += 10
    return ret
    
def splitVideoFile(filename, startTime, endTime, outputFilename):
    extraArgs = ""
    if (endTime != 0.00):
        extraArgs = extraArgs + " -t " + str(endTime - startTime)
    
    # print("StartKey", startTime, "EndKey", endTime)
    vid1Cmd = 'ffmpeg -y -i "{}" -ss {} {} -c copy {}'.format(filename, startTime, extraArgs, outputFilename)
    os.system(vid1Cmd)
    # statusText.value = "Done!"
    # print(vid1Cmd) 
    # proc = subprocess.Popen(vid1Cmd, shell=True, stderr=subprocess.PIPE)
    # for line in proc.stderr.readlines():
    #     statusText.value = line.decode('utf-8').rstrip()


class DataSet(object):
    pass

class TeamBtns(object):
    pass

class RoundBtns(object):
    pass


def get_usb_devices():
    sdb_devices = map(os.path.realpath, glob('/sys/block/sd*'))
    usb_devices = []
    for dev in sdb_devices:
        print(dev)
        if any(['usb' in dev.split('/')[5]]):
            usb_devices.append(dev)
    return dict((os.path.basename(dev), dev) for dev in usb_devices)

def get_mount_points(devices=None):
    devices = get_usb_devices()
    print(devices)
    output = check_output(['mount']).splitlines()
    is_usb = lambda path: any(dev in path for dev in devices)
    usb_info = (line for line in output if is_usb(line.split()[0]))
    return [(info.split()[0], info.split()[2]) for info in usb_info]

# Modified from code orignal to
# Author: Patrick Fay Date: 23-09-2015
libtk = "N/A"
C_Key = "Control-"  # shortcut key modifier


class Player(Tk.Frame):
    """The main window has to deal with events.
    """
    _geometry = ''
    _stopped  = None
        

    def __init__(self, parent, title=None, video=''):
        Tk.Frame.__init__(self, parent)

        print(get_usb_devices())
        self.compDataset = []
        self.compComboOptions = ["Select Competition"]
        self.teamComboOptions = []
        self.teamComboOptionsI = []
        self.roundComboOptions = []
        
        style = ttk.Style()
        style.configure("TButton", font=("Helvetica", "14"))
        style.configure("Player.TButton", font=("Helvetica", "20"))
        style.configure("Split.TButton", padding=30, font=("Helvetica", "20"))
        style.configure("TProgressbar", pady=5)
        style.configure("SplitSelected.TButton", padding=30, background="green", activebackground="yellow", highlightbackground="yellow", font=("Helvetica", "20"))
        self.parent = parent  # == root
        self.parent.title(title or "Skydive Or Bust Dubbing 1.0")
        self.video = expanduser(video)

        # first, top panel shows vide
        self.vWrapper = ttk.Frame(self.parent)
        self.videopanel = ttk.Frame(self.vWrapper)
        self.canvas = Tk.Canvas(self.videopanel)
        self.canvas.pack(fill=Tk.BOTH, expand=1)
        self.videopanel.pack(fill=Tk.BOTH, expand=1)
        self.vWrapper.pack(side=Tk.LEFT)
        # panel to hold buttons
        self.buttons_panel = Tk.Frame(self.vWrapper)
        self.splitting_panel = Tk.Frame(self.parent)

        compcombo = ttk.Frame(self.buttons_panel)
        self.compCombo = ttk.Combobox(compcombo, width=50)
        self.compCombo['values'] = self.compComboOptions

        self.teamCombo = ttk.Combobox(compcombo, width=50)
        self.roundCombo = ttk.Combobox(compcombo, width=50)
        self.compCombo.pack(side=Tk.TOP)
        self.teamCombo.pack(side=Tk.TOP)
        self.roundCombo.pack(side=Tk.TOP)
        compcombo.pack(side=Tk.BOTTOM, fill=Tk.X)
        
        self.compCombo.bind('<<ComboboxSelected>>', lambda event: self.change_comp())

        buttons = ttk.Frame(self.buttons_panel)
        self.openButton = ttk.Button(buttons, text="Open", command=self.OnOpen, style="Player.TButton")
        self.playButton = ttk.Button(buttons, text="Play", command=self.OnPlay, style="Player.TButton")
        stop            = ttk.Button(buttons, text="Stop", command=self.OnStop, style="Player.TButton")
        self.slateButton = ttk.Button(self.splitting_panel, text="Mark Slate", command=self.onMarkSlate, style="Split.TButton")
        self.slateButton.configure(state="yellow")
        self.exitButton = ttk.Button(self.splitting_panel, text="Mark Exit", command=self.onMarkExit, style="Split.TButton")
        self.uploadButton = ttk.Button(self.splitting_panel, text="Upload", command=self.onUpload, style="Split.TButton")
        self.progressBar = ttk.Progressbar(self.splitting_panel, orient = Tk.HORIZONTAL, length = 200, mode = 'determinate', style="TProgressbar")
        self.openButton.pack(side=Tk.LEFT)
        self.playButton.pack(side=Tk.LEFT)
        stop.pack(side=Tk.LEFT)
        self.slateButton.pack(side=Tk.TOP)
        self.exitButton.pack(side=Tk.TOP)
        self.uploadButton.pack(side=Tk.TOP)
        self.progressBar.pack(side=Tk.TOP)
        buttons.pack(side=Tk.BOTTOM, fill=Tk.X)


        # panel to hold player time slider
        timers = ttk.Frame(self.buttons_panel)
        self.timeVar = Tk.DoubleVar()
        self.timeSliderLast = 0
        self.timeSlider = Tk.Scale(timers, variable=self.timeVar, command=self.OnTime,
                                   from_=0, to=1000, orient=Tk.HORIZONTAL, length=500,
                                   showvalue=0)  # label='Time',
        self.timeSlider.pack(side=Tk.BOTTOM, fill=Tk.X, expand=1)
        self.timeSliderUpdate = time.time()
        timers.pack(side=Tk.BOTTOM, fill=Tk.X)


        self.buttons_panel.pack(fill=Tk.BOTH, expand=1)
        self.splitting_panel.pack(fill=Tk.BOTH, expand=1)

        # VLC player
        args = []
        args.append('--no-xlib')
        self.Instance = vlc.Instance(args)
        self.player = self.Instance.media_player_new()
        self.player.audio_set_volume(0)

        self.parent.bind("<Configure>", self.OnConfigure)  # catch window resize, etc.
        self.parent.update()

        # After parent.update() otherwise panel is ignored.
        # self.buttons_panel.overrideredirect(True)

        # Estetic, to keep our video panel at least as wide as our buttons panel.
        self.parent.minsize(width=720, height=480)

        self.is_buttons_panel_anchor_active = False

        # Download API Data
        self.get_comps()
        self.list_comps()

        self.OnTick()  # set the timer up


    def clean_all(self):
        print("Cleaning")
        self.OnStop()
        setattr(dataSet, "filename", "")
        setattr(dataSet, "dest", "")
        self.compComboOptions.clear()
        self.teamComboOptions.clear()
        self.teamComboOptionsI.clear()
        self.roundComboOptions.clear()
        self.list_comps()

        setattr(dataSet, "slate", "")
        self.slateButton.configure(style="Split.TButton")
        setattr(dataSet, "exit", "")
        self.exitButton.configure(style="Split.TButton")
        player.progressBar['value'] = 0


    def list_comps(self):
        for comp in self.compDataset["comps"]:
            self.compComboOptions.append(comp["name"])
        self.compCombo['values'] = self.compComboOptions

    def get_comps(self):
        response = requests.get("https://dev.skydiveorbust.com/api/latest/events/2020_cf_ghost_nationals/comps");
        # print(response.status_code)
        # print(response.json())
        self.compDataset = response.json()



    def change_comp(self):
        compInd = self.compCombo.current()
        for compI in range(len(self.compDataset["comps"])):
            comp = self.compDataset["comps"][compI]

            if (comp["name"] == self.compComboOptions[compInd]):
 
                self.teamComboOptions.clear()
                self.teamComboOptionsI.clear()
                self.roundComboOptions.clear()
                for teamI in range(len(comp["teams"])):
                    self.teamComboOptions.append(comp["teams"][teamI]["name"])
                    if (hasattr(comp["teams"][teamI], "teamNumber")):
                        self.teamComboOptionsI.append(comp["teams"][teamI]["teamNumber"])
                    else:
                        self.teamComboOptionsI.append(str(teamI))
                    # self.teamCombo.insert(teamI, comp["teams"][teamI]["name"])

                for r in range(1, comp["roundCnt"] + 1):
                    self.roundComboOptions.append("R" + str(r))
                    
                for r in range(1, comp["exRoundCnt"] + 1):
                    self.roundComboOptions.append(comp["exRoundPre"] + str(r + comp["roundCnt"]))

                self.teamCombo['values'] = self.teamComboOptions
                self.roundCombo['values'] = self.roundComboOptions
                break


    def OnClose(self, *unused):
        """Closes the window and quit.
        """
        # print("_quit: bye")
        self.parent.quit()  # stops mainloop
        self.parent.destroy()  # this is necessary on Windows to avoid
        # ... Fatal Python Error: PyEval_RestoreThread: NULL tstate

    def _DetectButtonsPanelDragging(self, _):
        """If our last click was on the boarder
           we disable the anchor.
        """
        if self.has_clicked_on_buttons_panel:
            self.is_buttons_panel_anchor_active = False
            self.buttons_panel.unbind("<Button-1>")
            self.buttons_panel.unbind("<B1-Motion>")
            self.buttons_panel.unbind("<ButtonRelease-1>")

    def OnConfigure(self, *unused):
        """Some widget configuration changed.
        """
        # <https://www.Tcl.Tk/man/tcl8.6/TkCmd/bind.htm#M12>
        self._geometry = ''  # force .OnResize in .OnTick, recursive?


    def OnFullScreen(self, *unused):
        pass


    def OnOpen(self, *unused):
        """Pop up a new dialow window to choose a file, then play the selected file.
        """
        self.clean_all()

        # Create a file dialog opened in the current home directory, where
        # you can display all kind of files, having as title "Choose a video".
        video = askopenfilename(initialdir = Path(expanduser("/media/pi")),
                                title = "Choose a video",
                                filetypes = (("all files", "*.*"),
                                             ("mp4 files", "*.mp4"),
                                             ("mov files", "*.mov")))

        setattr(dataSet, "filename", video)
        self._Play(video)

        keyFrameThread = Thread(target = thread_getFrames, args=[video])
        keyFrameThread.setDaemon(True)
        keyFrameThread.start()

    def _Pause_Play(self, playing):
        # re-label menu item and button, adjust callbacks
        p = 'Pause' if playing else 'Play'
        c = self.OnPlay if playing is None else self.OnPause
        
        self.playButton.config(text=p, command=c)
        self._stopped = False

    def _Play(self, video):
        # helper for OnOpen and OnPlay
        if isfile(video):  # Creation
            m = self.Instance.media_new(str(video))  # Path, unicode
            self.player.set_media(m)
            self.parent.title("Skydive Or Bust Dubbing 1.0")

            # set the window id where to render VLC's video output
            h = self.videopanel.winfo_id()  # .winfo_visualid()?
            self.player.set_xwindow(h)  # fails on Windows
            # FIXME: this should be made cross-platform
            self.OnPlay()

    def OnPause(self, *unused):
        """Toggle between Pause and Play.
        """
        if self.player.get_media():
            self._Pause_Play(not self.player.is_playing())
            self.player.pause()  # toggles

    def OnPlay(self, *unused):
        """Play video, if none is loaded, open the dialog window.
        """
        # if there's no video to play or playing,
        # open a Tk.FileDialog to select a file
        if not self.player.get_media():
            if self.video:
                self._Play(expanduser(self.video))
                self.video = ''
            else:
                self.OnOpen()
        # Try to play, if this fails display an error message
        elif self.player.play():  # == -1
            self.showError("Unable to play the video.")
        # else:
        self._Pause_Play(True)
        # set volume slider to audio level
        self.player.audio_set_volume(0)

    def OnStop(self, *unused):
        """Stop the player, resets media.
        """
        if self.player:
            self.player.stop()
            self._Pause_Play(None)
            # reset the time slider
            self.timeSlider.set(0)
            self._stopped = True

    def onMarkSlate(self, *unused):
        if self.player:
            cTime = self.player.get_time()
            if (cTime == -1):
                return
            setattr(dataSet, "slate", float(cTime / 1000))
            self.slateButton.configure(style="SplitSelected.TButton")

    def onMarkExit(self, *unused):
        if self.player:          
            cTime = self.player.get_time()
            if (cTime == -1):
                return
            setattr(dataSet, "exit", float(cTime / 1000))
            self.exitButton.configure(style="SplitSelected.TButton")

    def onUpload(self, *unused):
        compInd = player.compCombo.current()
        teamInd = player.teamCombo.current()
        roundInd = player.roundCombo.current()
        destFile = "/home/pi/Videos3/" + str(compInd) + "_" + str(teamInd) + "_" + str(roundInd)
        self.OnStop()
        setattr(dataSet, "dest", destFile)
        submitButton()

    def OnTick(self):
        """Timer tick, update the time slider to the video time.
        """
        if self.player:
            # since the self.player.get_length may change while
            # playing, re-set the timeSlider to the correct range
            t = self.player.get_length() * 1e-3  # to seconds
            if t > 0:
                self.timeSlider.config(to=t)

                t = self.player.get_time() * 1e-3  # to seconds
                # don't change slider while user is messing with it
                if t > 0 and time.time() > (self.timeSliderUpdate + 2):
                    self.timeSlider.set(t)
                    self.timeSliderLast = int(self.timeVar.get())
        # start the 1 second timer again
        self.parent.after(1000, self.OnTick)
        # adjust window to video aspect ratio, done periodically
        # on purpose since the player.video_get_size() only
        # returns non-zero sizes after playing for a while
        # if not self._geometry:
        #     self.OnResize()

    def OnTime(self, *unused):
        if self.player:
            t = self.timeVar.get()
            if self.timeSliderLast != int(t):
                # this is a hack. The timer updates the time slider.
                # This change causes this rtn (the 'slider has changed' rtn)
                # to be invoked.  I can't tell the difference between when
                # the user has manually moved the slider and when the timer
                # changed the slider.  But when the user moves the slider
                # tkinter only notifies this rtn about once per second and
                # when the slider has quit moving.
                # Also, the tkinter notification value has no fractional
                # seconds.  The timer update rtn saves off the last update
                # value (rounded to integer seconds) in timeSliderLast if
                # the notification time (sval) is the same as the last saved
                # time timeSliderLast then we know that this notification is
                # due to the timer changing the slider.  Otherwise the
                # notification is due to the user changing the slider.  If
                # the user is changing the slider then I have the timer
                # routine wait for at least 2 seconds before it starts
                # updating the slider again (so the timer doesn't start
                # fighting with the user).
                self.player.set_time(int(t * 1e3))  # milliseconds
                self.timeSliderUpdate = time.time()

    def showError(self, message):
        """Display a simple error dialog.
        """
        self.OnStop()
        showerror(self.parent.title(), message)



# START App Main()
r = RoundBtns()
dataSet = DataSet()
selectedTeam = ""
selectedRound = 0

# mpv_palyer = MPV(start_mpv=False, ipc_socket="/tmp/mpv.socket")
monitorList = get_monitors()
for m in monitorList:
    print(str(m))

appWidth = 480 # monitorList[0].width
appHeight = 320 # monitorList[0].height
appMulti = appWidth / 640
if (appMulti > 1):
    fontSize = int(26 * (appMulti * .85))
    fontSizeMin = int(16 * (appMulti * .85))
else:
    fontSize = 26
    fontSizeMin = 16


# app = App(title="Skydive or Bust Dubbing Station", width=appWidth, height=appHeight, layout="grid")



# disc_combo = Combo(app, options=["Select Discipline", "2way Sequential ProAm", "2way Sequential Open", "4way Sequential Open", "4way Rotations"], command=teamChanged, grid=[0,1,8,1], align="top", width="23")
# disc_combo.text_size = fontSize
# disc_combo.bg = "#cc3333"

# team_combo = Combo(app, options=["Select Team", "Team 1", "Team 2", "Team 3", "Team 4"], command=teamChanged, grid=[0,2,6,1], align="top", width="23")
# team_combo.text_size = fontSize
# team_combo.bg = "#33cc33"

#openvideo_btn = PushButton(app, command=openButton, text="Open Video", width="17", grid=[0,4,3,1])
#openvideo_btn.text_size = fontSizeMin
#openvideo_btn.bg = "#cc33cc"

#playvideo_btn = PushButton(app, command=playButton, text="Play Video", width="17", grid=[3,4,3,1])
#playvideo_btn.text_size = fontSizeMin

#compCombo = Combo(app, options=compComboOptions, grid=[0,5,5,1], command=change_comp, align="left")
#compCombo.text_size = fontSize
#teamCombo = Combo(app, options=teamComboOptions, grid=[0,6,5,1], align="left")
#teamCombo.text_size = fontSize
#roundCombo = Combo(app, options=roundComboOptions, grid=[0,7,5,1], align="left")
#roundCombo.text_size = fontSize
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



#update_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Upload Video", grid=[0,8,3,1])
#update_btn.text_size = fontSize
#update_btn.disabled = True

#playnew_btn = PushButton(app, command=playnewVideo, text="Play New Video", grid=[4,8,3,1])
#playnew_btn.text_size = fontSize
#playnew_btn.disabled = True

# move_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Move Video", grid=[3,7,3,1])
# move_btn.text_size = fontSize
# move_btn.disabled = True

#statusText = Text(app, text="", align="left", size=14, grid=[0,10,7,1])

# Bind to key press events with a decorator
#@mpv_player.on_event("start-file")
#def evStartFile_handler(evData):
#    setattr(dataSet, "playing", "1")
    
#@mpv_player.on_event("end-file")
#def evStartFile_handler(evData):
#    setattr(dataSet, "playing", "0")

ssock_file = '/tmp/sdobox.socket';
csock_file = '/tmp/sdobox.dubbing.socket';

if os.path.exists(csock_file):
    os.remove(csock_file)
    
csock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
csock.bind(csock_file)
csock.setblocking(0)

try:
  csock.sendto(str.encode('{"event":"started"}'), ssock_file)
except:
  pass

# print('Num Args', len(sys.argv), 'arguments')
# print('Arg list', str(sys.argv))
# statusText.value = sys.argv[1]

sdobThread = Thread(target = thread_sdobSocket)
sdobThread.setDaemon(True)
sdobThread.start()

# Add vlc player
root = Tk.Tk()
player = Player(root)
root.protocol("WM_DELETE_WINDOW", player.OnClose)


# Main Looop
root.mainloop()

sdobSocketKiller = 0

#try:
#    mpv_player.terminate()
#except:
#    pass

try:
    csock.sendto(str.encode('{"event":"stopped"}'), ssock_file)
except:
    pass
finally:
    csock.close()

if os.path.exists(csock_file):
    os.remove(csock_file)

sdobThread.join()