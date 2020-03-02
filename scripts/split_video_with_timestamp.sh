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

posttime=5
totaltime=$(($wt + $leadtime))
videotime=$(($totaltime + $posttime))
ss=$(echo - | awk -v sowt="$sowt" -v leadtime="$leadtime" '{print sowt - leadtime}')

echo 'Getting Duration'
duration=$(ffprobe -loglevel error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 "$input")
ffmpeg -f mpeg -ss $ss -i "$input" -an -t $videotime $bitrate -c:v h264_omx -vf " \
drawtext=fontfile=DejaVuSans.ttf:fontcolor=black:fontsize=96:x=20:y=(h-text_h) - 20 \
:enable='lt(t\, $leadtime)':text='%{eif\\:$wt\\:d\\:2}' \
,drawtext=fontfile=DejaVuSans.ttf:fontcolor=black:fontsize=96:x=20:y=(h-text_h) - 20 \
:enable='gte(t\, $leadtime)':text='%{eif\\:if(between(t\, $leadtime, $totaltime)\, (($duration + $totaltime + .9) - $duration)-t\, 0)\\:d\\:2}' \
,drawtext=fontfile=DejaVuSans.ttf:fontcolor=black:fontsize=96:x=(w-text_w)-20:y=(h-text_h) - 20 \
:enable='between(t\, $totaltime, $duration)':text='15/16' \
,setpts='PTS-STARTPTS + gte(T,$totaltime)*(3/TB)' \
" "$output.mp4"
