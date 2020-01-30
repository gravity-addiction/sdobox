sudo killall -SIGINT touchapp
sudo systemctl stop advert.service
make clean
make -j4
sudo systemctl start advert.service