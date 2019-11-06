# Grow Clock
ES8266 powered, recyclable grow clock for children.

<p style="text-align:center">
  <img src="https://github.com/davidhampgonsalves/grow-clock/blob/master/grow-clock.gif" width="100%">
</p>

Made out of reused cardboard since these devices are only useful while children learn when to wake. I used an [Arduino originally](https://github.com/davidhampgonsalves/grow-clock/commit/130541bbc1a834789b63a736866fe7e3ff2ab5e8) but switched to an ESP8266 because the clock was still being used and wanted to use NTP time via wifi.

# Install
I use [platform.io cli](https://platformio.org/get-started/cli) but the Arduino IDE will work just as well.

Connect a servo to ground / Vcc and the signal wire to pin D4.
```
pio devices list
pio run
```
