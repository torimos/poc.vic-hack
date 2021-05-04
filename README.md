# poc.vic-hack
POC to test my little friend "vector" features without any sdk

Ultimate goal - being able to write own firmware components without proprietary SDK from DD

# Prerequisites
All /anki/bin/vic-* amd mm-anki-camera demons installed on vector needs to be removed or replaced by dummy executables to eliminate any conflicts with custom apps

# Verified
- LCD
- CAMERA
- SPEAKER
- IMU
- BodyBoard
  - Read Status
  - Control LEDs state
  - Control Motors (Wheels, Head, Lift) & LEDs state
  - Control power (shutdown)
  - Read version of body board firmware
  - Read 4 Motors encoders
  - Read 4 Cliff sensors
  - Read Prox sensor data
  - Read Battery/Charger voltage and battery status
  - Read 4 Microphone data samples

# TODO 
- BLE
- OTA

# BLOCKED
- DFU - seems firmware for body board is encrypted with some cypher (AES-128 maybe). rampost just pass encrypted image on boot if installed and new versions are different. Further work is blocked until ecryption secret/method will be known.
