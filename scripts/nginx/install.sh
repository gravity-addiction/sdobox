#!/usr/bin/env bash
sudo apt -y install nginx-light
sudo cp ./default /etc/nginx/sites-enabled/default
sudo systemctl restart nginx.service

./copy-website.sh