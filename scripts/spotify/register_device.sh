
# SERIAL=$(cat /proc/cpuinfo | grep Serial | cut -d ' ' -f 2)
patternSerial='^Serial.*([[:xdigit:]]{16})$'
patternModel='^Model.*: (.*)$'
patternRevision='^Revision.*: (.*)$'

while read -r line
do
    if [[ $line =~ $patternSerial ]]
    then
        SERIAL="${BASH_REMATCH[1]}"
    fi
    if [[ $line =~ $patternModel ]]
    then
        MODEL="${BASH_REMATCH[1]}"
    fi
    if [[ $line =~ $patternRevision ]]
    then
        REVISION="${BASH_REMATCH[1]}"
    fi
done < /proc/cpuinfo

echo "${SERIAL} - ${MODEL} - ${REVISION}"

curl -X "POST" -H "Content-Type: application/json" -d "{\"s\":\"${SERIAL}\",\"m\":\"${MODEL}\",\"r\":\"${REVISION}\"}" http://localhost:4004/rpi/spotify # https://spotify.skydiveorbust.com/rpi/spotify