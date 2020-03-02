#!/bin/sh


if [ "$#" -ne 6 ]; then
  echo "Usage: split_video_with_timestamp.sh input.mp4 output sowt workingtime leaduptime crf"
  echo "Example: split_video_with_timestamp.sh ./input.mp4 ./output 12.345678 35 5 \"-crf 35\""
  echo "Example: split_video_with_timestamp.sh ./input.mpg ./output 12.345678 50 5 \"-b:v 10M\""
  
  exit 1
fi

input=$1
output=$2
sowt=$3
wt=$4
leadtime=$5
bitrate=$6


totaltime=$(($wt + $leadtime))
ss=$(echo - | awk '{print 22.039289 - 5}')

echo $ss
echo 'Getting Duration'
duration=$(ffprobe -loglevel error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 "$input")
ffmpeg -f mpeg -ss $ss -i "$input" -an -t $totaltime $bitrate -c:v h264_omx -vf "drawtext=fontfile=OpenSans-Regular.ttf:text='%{eif\\:if(between(t\, 0\, $leadtime)\, $wt\, (($duration + $totaltime + .9) - $duration)-t)\\:d\\:2}':fontcolor=white:fontsize=24:x=w-tw-20:y=th:box=1:boxcolor=black@0.5:boxborderw=10" "$output.mp4"
