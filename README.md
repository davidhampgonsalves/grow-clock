# Grow Clock
Childrens grow clock powered by a [Arduino MKR1000](https://store.arduino.cc/usa/arduino-mkr1000).

<p style="text-align:center">
  <img src="https://github.com/davidhampgonsalves/grow-clock/blob/master/grow-clock.gif" width=1200px>
</p>

My version was made to be recyclable since these devices are only useful for a year or two while children learn whento wake up. The MRK1000 is well suited to this task since it as low power conumption in standby and a RTC(real time clock).

# Warnings
Once the MKR1000 goes to sleep the USB port will be disabled and Serial communication will be disrupted. Hit the reset button twice to restore the device back to original settings.

# Install
I use the [platform.io cli](https://platformio.org/get-started/cli) to build but the arduino IDE will work just as well.

Connect a servo to ground / vcc and the signal wire to pin 6.
```
pio devices list
pio run --target upload
pio device monitor --port /dev/cu.usbmodem1421
```
