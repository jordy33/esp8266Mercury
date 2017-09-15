### DWIM MERCURY ###

* First Attempt

![](https://github.com/jordy33/esp8266Mercury/blob/master/esp-201.png?raw=true)

search your serial port with: ls /dev/cu.*
Access the console with 
```objc
screen /dev/cu.SLAB_USBtoUART 115200
```
To Kill session:
```objc
CTRL-A   K 
```

* Test MQTT
* publish to: dwim/feeds/onoff
* message: on   <--- To turn onf
* message: off  <--- To turn off
* Subscribe to: dwim/feeds/button
* Receive either 1 or 0 (1 for Pressed 0 for not pressed)
