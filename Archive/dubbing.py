#!/usr/bin/env python3
from guizero import App, Combo, PushButton, Text
from screeninfo import get_monitors
from python_mpv_jsonipc import MPV
from threading import Thread
from os.path import basename, expanduser, isfile, join as joined
from pathlib import Path
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


compDataset = []
compComboOptions = ["Select Competition"]
teamComboOptions = []
roundComboOptions = []

def get_comps():
    global compDataset, compComboOptions, teamComboOptions, roundComboOptions
    response = requests.get("https://dev.skydiveorbust.com/api/latest/events/2020_cf_ghost_nationals/comps");
    print(response.status_code)
    print(response.json())
    compDataset = response.json()

    print(type(compDataset))
    for comp in compDataset["comps"]:
        compComboOptions.append(comp["name"])



        
def change_comp(selected_comp):
    global teamCombo, roundCombo
    print(type(compDataset))
    teamCombo.clear()
    roundCombo.clear()
    for compI in range(len(compDataset["comps"])):
        comp = compDataset["comps"][compI]
        if (comp["name"] == selected_comp):
            for teamI in range(len(comp["teams"])):
                teamCombo.insert(teamI, comp["teams"][teamI]["name"])

            for r in range(1, comp["roundCnt"] + 1):
                roundCombo.insert(r, "R" + str(r))
            for r in range(1, comp["exRoundCnt"] + 1):
                roundCombo.insert(r, comp["exRoundPre"] + str(r + comp["roundCnt"]))
            break


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
    pass
    # mpv_player.loop = False
    # mpv_player.play("/home/pi/Videos/120.mp4")
    # csock.sendto(str.encode('{"event":"playing"}'), ssock_file)

def openButton(startPath = "/media/"):
    if (hasattr(dataSet, "playing") and getattr(dataSet, "playing") == "1"):
        return

    filename = app.select_file(folder=startPath, filetypes=[["Media Files", "*.mp4 *.mov *.vob *.wmv *.mpg *.mpeg *.mkv *.m4v *.avi *.ts *.webm"], ["All Files", "*.*"]])
    if (filename != "" and isinstance(filename, str)):
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
        # mpv_player.loop = False
        # mpv_player.play(getattr(dataSet, "filename"))
        # try:
        #     csock.sendto(str.encode('{"event":"playing"}'), ssock_file)
        # except:
        #     print("Cannot send playing to sdobox.socket")
        media_player = vlc.MediaPlayer()
        media = vlc.Media(getattr(dataSet, "filename"))
        media_player.set_media(media)
        media_
        media_player.play()
        
class DataSet(object):
    pass

class TeamBtns(object):
    pass

class RoundBtns(object):
    pass



# Modified from code orignal to
# Author: Patrick Fay Date: 23-09-2015
libtk = "N/A"
C_Key = "Control-"  # shortcut key modifier
class _Tk_Menu(Tk.Menu):
    _shortcuts_entries = {}
    _shortcuts_widget  = None

    def add_shortcut(self, label='', key='', command=None, **kwds):
        '''Like Tk.menu.add_command extended with shortcut key.
           If needed use modifiers like Shift- and Alt_ or Option-
           as before the shortcut key character.  Do not include
           the Command- or Control- modifier nor the <...> brackets
           since those are handled here, depending on platform and
           as needed for the binding.
        '''
        # <https://TkDocs.com/tutorial/menus.html>
        if not key:
            self.add_command(label=label, command=command, **kwds)

        else:  # XXX not tested, not tested, not tested
            self.add_command(label=label, underline=label.lower().index(key),
                                          command=command, **kwds)
            self.bind_shortcut(key, command, label)

    def bind_shortcut(self, key, command, label=None):
        """Bind shortcut key, default modifier Command/Control.
        """
        if self._shortcuts_widget:
            if C_Key.lower() not in key.lower():
                key = "<%s%s>" % (C_Key, key.lstrip('<').rstrip('>'))
            self._shortcuts_widget.bind(key, command)
            # remember the shortcut key for this menu item
            if label is not None:
                item = self.index(label)
                self._shortcuts_entries[item] = key

    def bind_shortcuts_to(self, widget):
        '''Set the widget for the shortcut keys, usually root.
        '''
        self._shortcuts_widget = widget

    def entryconfig(self, item, **kwds):
        """Update shortcut key binding if menu entry changed.
        """
        Tk.Menu.entryconfig(self, item, **kwds)
        # adjust the shortcut key binding also
        if self._shortcuts_widget:
            key = self._shortcuts_entries.get(item, None)
            if key is not None and "command" in kwds:
                self._shortcuts_widget.bind(key, kwds["command"])



class Player(Tk.Frame):
    """The main window has to deal with events.
    """
    _geometry = ''
    _stopped  = None

    def __init__(self, parent, title=None, video=''):
        Tk.Frame.__init__(self, parent)

        self.parent = parent  # == root
        self.parent.title(title or "tkVLCplayer")
        self.video = expanduser(video)

        # first, top panel shows video

        self.videopanel = ttk.Frame(self.parent)
        self.canvas = Tk.Canvas(self.videopanel)
        self.canvas.pack(fill=Tk.BOTH, expand=1)
        self.videopanel.pack(fill=Tk.BOTH, expand=1)

        # panel to hold buttons
        self.buttons_panel = Tk.Toplevel(self.parent)
        self.buttons_panel.title("")
        self.is_buttons_panel_anchor_active = False

        buttons = ttk.Frame(self.buttons_panel)
        self.playButton = ttk.Button(buttons, text="Play", command=self.OnPlay)
        stop            = ttk.Button(buttons, text="Stop", command=self.OnStop)
        self.muteButton = ttk.Button(buttons, text="Mute", command=self.OnMute)
        self.playButton.pack(side=Tk.LEFT)
        stop.pack(side=Tk.LEFT)
        self.muteButton.pack(side=Tk.LEFT)

        self.volMuted = False
        self.volVar = Tk.IntVar()
        self.volSlider = Tk.Scale(buttons, variable=self.volVar, command=self.OnVolume,
                                  from_=0, to=100, orient=Tk.HORIZONTAL, length=200,
                                  showvalue=0, label='Volume')
        self.volSlider.pack(side=Tk.RIGHT)
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


        # VLC player
        args = []
        args.append('--no-xlib')
        self.Instance = vlc.Instance(args)
        self.player = self.Instance.media_player_new()

        self.parent.bind("<Configure>", self.OnConfigure)  # catch window resize, etc.
        self.parent.update()

        # After parent.update() otherwise panel is ignored.
        self.buttons_panel.overrideredirect(True)

        # Estetic, to keep our video panel at least as wide as our buttons panel.
        self.parent.minsize(width=502, height=0)

        self.is_buttons_panel_anchor_active = False

        self._AnchorButtonsPanel()

        self.OnTick()  # set the timer up

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

    def _AnchorButtonsPanel(self):
        video_height = self.parent.winfo_height()
        panel_x = self.parent.winfo_x()
        panel_y = self.parent.winfo_y() + video_height + 23 # 23 seems to put the panel just below our video.
        panel_height = self.buttons_panel.winfo_height()
        panel_width = self.parent.winfo_width()
        self.buttons_panel.geometry("%sx%s+%s+%s" % (panel_width, panel_height, panel_x, panel_y))

    def OnConfigure(self, *unused):
        """Some widget configuration changed.
        """
        # <https://www.Tcl.Tk/man/tcl8.6/TkCmd/bind.htm#M12>
        self._geometry = ''  # force .OnResize in .OnTick, recursive?

        if self.is_buttons_panel_anchor_active:
            self._AnchorButtonsPanel()

    def OnFullScreen(self, *unused):
        pass

    def OnMute(self, *unused):
        """Mute/Unmute audio.
        """
        # audio un/mute may be unreliable, see vlc.py docs.
        self.volMuted = m = not self.volMuted  # self.player.audio_get_mute()
        self.player.audio_set_mute(m)
        u = "Unmute" if m else "Mute"
        self.fileMenu.entryconfig(self.muteIndex, label=u)
        self.muteButton.config(text=u)
        # update the volume slider text
        self.OnVolume()

    def OnOpen(self, *unused):
        """Pop up a new dialow window to choose a file, then play the selected file.
        """
        # if a file is already running, then stop it.
        self.OnStop()
        # Create a file dialog opened in the current home directory, where
        # you can display all kind of files, having as title "Choose a video".
        video = askopenfilename(initialdir = Path(expanduser("~")),
                                title = "Choose a video",
                                filetypes = (("all files", "*.*"),
                                             ("mp4 files", "*.mp4"),
                                             ("mov files", "*.mov")))
        self._Play(video)

    def _Pause_Play(self, playing):
        # re-label menu item and button, adjust callbacks
        p = 'Pause' if playing else 'Play'
        c = self.OnPlay if playing is None else self.OnPause
        self.fileMenu.entryconfig(self.playIndex, label=p, command=c)
        # self.fileMenu.bind_shortcut('p', c)  # XXX handled
        self.playButton.config(text=p, command=c)
        self._stopped = False

    def _Play(self, video):
        # helper for OnOpen and OnPlay
        if isfile(video):  # Creation
            m = self.Instance.media_new(str(video))  # Path, unicode
            self.player.set_media(m)
            self.parent.title("tkVLCplayer - %s" % (basename(video),))

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
        else:
            self._Pause_Play(True)
            # set volume slider to audio level
            vol = self.player.audio_get_volume()
            if vol > 0:
                self.volVar.set(vol)
                self.volSlider.set(vol)

    def OnResize(self, *unused):
        """Adjust the window/frame to the video aspect ratio.
        """
        g = self.parent.geometry()
        if g != self._geometry and self.player:
            u, v = self.player.video_get_size()  # often (0, 0)
            if v > 0 and u > 0:
                # get window size and position
                g, x, y = g.split('+')
                w, h = g.split('x')
                # alternatively, use .winfo_...
                # w = self.parent.winfo_width()
                # h = self.parent.winfo_height()
                # x = self.parent.winfo_x()
                # y = self.parent.winfo_y()
                # use the video aspect ratio ...
                if u > v:  # ... for landscape
                    # adjust the window height
                    h = round(float(w) * v / u)
                else:  # ... for portrait
                    # adjust the window width
                    w = round(float(h) * u / v)
                self.parent.geometry("%sx%s+%s+%s" % (w, h, x, y))
                self._geometry = self.parent.geometry()  # actual

    def OnStop(self, *unused):
        """Stop the player, resets media.
        """
        if self.player:
            self.player.stop()
            self._Pause_Play(None)
            # reset the time slider
            self.timeSlider.set(0)
            self._stopped = True

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
        if not self._geometry:
            self.OnResize()

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

    def OnVolume(self, *unused):
        """Volume slider changed, adjust the audio volume.
        """
        vol = min(self.volVar.get(), 100)
        v_M = "%d%s" % (vol, " (Muted)" if self.volMuted else '')
        self.volSlider.config(label="Volume " + v_M)
        if self.player and not self._stopped:
            # .audio_set_volume returns 0 if success, -1 otherwise,
            # e.g. if the player is stopped or doesn't have media
            if self.player.audio_set_volume(vol):  # and self.player.get_media():
                self.showError("Failed to set the volume: %s." % (v_M,))

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


# Download API Data
get_comps()

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

compCombo = Combo(app, options=compComboOptions, grid=[0,5,5,1], command=change_comp, align="left")
compCombo.text_size = fontSize
teamCombo = Combo(app, options=teamComboOptions, grid=[0,6,5,1], align="left")
teamCombo.text_size = fontSize
roundCombo = Combo(app, options=roundComboOptions, grid=[0,7,5,1], align="left")
roundCombo.text_size = fontSize
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



update_btn = PushButton(app, command=submitButton, args=[selectedTeam, selectedRound], text="Upload Video", grid=[0,8,3,1])
update_btn.text_size = fontSize
update_btn.disabled = True

playnew_btn = PushButton(app, command=playnewVideo, text="Play New Video", grid=[4,8,3,1])
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

app.when_clicked = globalClicked

# Add vlc player
root = Tk.Tk()
player = Player(root)
root.protocol("WM_DELETE_WINDOW", player.OnClose)

# Main Looop
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