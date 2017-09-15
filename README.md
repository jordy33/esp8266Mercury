### DWIM MERCURY ###

* First Attempt

search your serial port with: ls /dev/cu.*
Access the console with 
```objc
screen /dev/cu.SLAB_USBtoUART 115200
```
To Kill session:
```objc
CTRL-A   K 
```
