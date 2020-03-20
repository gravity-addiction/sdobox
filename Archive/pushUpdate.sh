if [ $# -eq 0 ]
then
  echo "Need IP Address as Argument"
fi
if [ -z "$1" ]
then
  echo "Need IP Address as Argument"
fi
./upload.sh $1
ssh -t $1 'sudo killall -SIGINT touchapp; sudo systemctl stop advert.service'
ssh -t $1 'sudo systemctl start advert.service'
