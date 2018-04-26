# Grow Clock
[Arduino](https://store.arduino.cc/usa/arduino-mkr1000) powered, recyclable grow clock for children.

<p style="text-align:center">
  <img src="https://github.com/davidhampgonsalves/grow-clock/blob/master/grow-clock.gif" width="100%">
</p>

My version was made to be recyclable, since these devices are only useful while children learn when to wake. I used an [Arduino MKR1000](https://store.arduino.cc/usa/arduino-mkr1000) (because I had a spare one) but any Arduino compatible board with a RTC(real time clock) will work. The MKR1000 is well suited to this task due to its as low power consumption in standby.

## Note
Once the MKR1000 goes to sleep the USB port will be disabled and Serial communication will be disrupted. Hit the reset button twice to restore the device back to original settings.

# Install
I use [platform.io cli](https://platformio.org/get-started/cli) but the Arduino IDE will work just as well.

Connect a servo to ground / Vcc and the signal wire to pin 6.
```
pio devices list
pio run --target upload
pio device monitor --port /dev/cu.usbmodem1421
```
