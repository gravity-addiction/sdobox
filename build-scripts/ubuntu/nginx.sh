#!/bin/bash

# Get full path to this folder
PWDSRC=`dirname "$(readlink -f "$0")"`
cd "${PWDSRC}"

mkdir -p ~/Downloads
git clone https://github.com/gravity-addiction/jetson-nano-scripts

### Redis Server
sudo adduser --system --group --no-create-home redis && \
sudo mkdir -p /var/lib/redis && sudo chown root:redis /var/lib/redis && sudo chmod 770 /var/lib/redis && \
sudo mkdir -p /etc/redis && sudo chown redis:redis /etc/redis && \
sudo mkdir -p /run/redis && sudo chown root:redis /run/redis && \
sudo chmod g+s /run/redis && \
sudo mkdir -p /var/log/redis && \
sudo chown redis:adm /var/log/redis && \
sudo chmod 775 /var/log/redis && \
sudo chmod g+s /var/log/redis && \
sudo cp jetson-nano-scripts/nginx/redis/redis.conf -O /etc/redis/redis.conf && \
sudo cp jetson-nano-scripts/nginx/redis/redis.service -O /etc/systemd/system/redis.service && \
sudo chown -R redis:redis /etc/redis/redis.conf && sudo chmod 640 /etc/redis/redis.conf && \

cd ~/Downloads && \
wget https://download.redis.io/releases/redis-6.2.5.tar.gz && \
tar zxf redis-6.2.5.tar.gz && \
cd redis-6.2.5 && \
make -j4 && \
sudo make install

sudo systemctl daemon-reload && \
sudo systemctl restart redis && \
sudo systemctl enable redis


### Nginx Config
cd ~/Downloads && \
git clone https://github.com/arut/nginx-rtmp-module && \
sudo mkdir -p /var/www/web /etc/nginx/rtmp.d && \
sudo mkdir -p /var/livestream/hls /var/livestream/dash /var/livestream/recordings /var/livestream/keys && \
sudo ln -s /var/livestream/hls /var/www/web/hls && \
sudo ln -s /var/livestream/dash /var/www/web/dash && \
sudo cp nginx-rtmp-module/stat.xsl /var/www/web/stat.xsl && \
sudo cp jetson-nano-scripts/nginx/crossdomain.xml /var/www/html/ && \
sudo cp jetson-nano-scripts/nginx/crossdomain.xml /var/www/web/ && \
sudo cp -R jetson-nano-scripts/nginx/rtmp.d/* /etc/nginx/rtmp.d/ && \
sudo cp /var/www/html/index.nginx-debian.html /var/www/web/index.html && \
sudo chown -R www-data:www-data /var/livestream /var/www/web /var/www/html && \
sudo mkdir -p /etc/nginx/rtmp.d && \
sudo cp jetson-nano-scripts/nginx/default.conf /etc/nginx/sites-available/ && \
sudo mv /etc/nginx/nginx.conf /etc/nginx/nginx-original.conf && \
sudo cp jetson-nano-scripts/nginx/nginx.conf /etc/nginx/ && \
cd /etc/nginx/sites-enabled && \
sudo ln -s ../sites-available/default.conf default.conf && \
sudo nginx -t && \
sudo systemctl restart nginx.service

