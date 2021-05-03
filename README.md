# poc.vic-hack
POC to test my little friend "vector" features without any sdk

Ultimate goal - being able to write own firmware components without proprietary SDK from DD

# Verified
- LCD
- CAMERA
- SPEAKER
- IMU

# TODO 
- BodyBoard
  - LEDs
  - Sensors
  - Motors
- BLE
- OTA


# How to build using multipass and custom scripts for docker
## build
multipass exec ubuntu-vm -- sudo /home/ubuntu/_make/mk poc.vic-hack
## upload
multipass exec ubuntu-vm -- sudo /home/ubuntu/_make/up poc.vic-hack vic-hack cloud
## build & upload
multipass exec ubuntu-vm -- sudo /home/ubuntu/_make/mkup poc.vic-hack vic-hack
## get logs
multipass exec ubuntu-vm -- sudo /home/ubuntu/_make/logs
## get 
multipass exec ubuntu-vm -- sudo /home/ubuntu/_make/logs
