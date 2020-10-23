#!/usr/bin/env bash

if [ -d ~/Downloads/sb-skydiveorbust/dist/sb-clean-blog-angular ]; then
  sudo cp -r ~/Downloads/sb-skydiveorbust/dist/sb-clean-blog-angular /var/www
  sudo rm -rf /var/www/html && sudo mv /var/www/sb-clean-blog-angular /var/www/html
fi
