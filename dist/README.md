# Package Versioning
sdobox_<major version>.<minor version>-<package revision>

## Unpack .deb package
- Change to dist folder
cd dist/
apt download sdobox
dpkg-deb -xv sdobox_1.0-1_armhf.deb sdobox_1.0-1/
mkdir sdobox_1.0-1/DEBIAN
ar p sdobox_1.0-1_armhf.deb control.tar.xz | tar -xvJ -C sdobox_1.0-1/DEBIAN/
cp -Rp sdobox_1.0-1 sdobox_1.0-2
- Modify sdobox_1.0-2/DEBIAN/control update Version:




### aptly config
- export aws keys, requires `sudo apt install awscli`

- append to ~/.aptly.conf root json
  "S3PublishEndpoints":{
    "sdobox.skydiveorbust.com":{
      "region":"us-west-2",
      "bucket":"sdobox.skydiveorbust.com",
      "acl":"public-read"
    }
  }




## repository packaging
dpkg-deb --build sdobox_1.0-9


### publishing to S3
aptly repo add sdobox sdobox_1.0-9.deb
aptly repo show -with-packages sdobox

aptly snapshot create sdobox_1.0-9 from repo sdobox
aptly snapshot list

// aptly publish snapshot sdobox_1.0-1 s3:sdobox.skydiveorbust.com:

export AWS_ACCESS_KEY_ID="AKIA4SPMMSJ4T3NGBAU2"
export AWS_SECRET_ACCESS_KEY="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"
aptly publish switch sdobox s3:sdobox.skydiveorbust.com: sdobox_1.0-9
