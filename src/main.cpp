#include <Arduino.h>
#include <Servo.h>
#include <RTCZero.h>

Servo servo;
RTCZero rtc;

int DAY_HOUR_START = 6;
int DAY_MIN_START = 45;

int DAY_HOUR_END = 19;
int DAY_MIN_END = 0;

int MOVE_DELAY = 90;

int toMinutes(int hours, int minutes) {
  return (hours * 60) + minutes;
}

bool isDayTime() {
  int minutes = toMinutes(rtc.getHours(), rtc.getMinutes());
  return minutes >= toMinutes(DAY_HOUR_START, DAY_MIN_START) && minutes < toMinutes(DAY_HOUR_END, DAY_MIN_END);
}

void moveTo(int from, int to) {
  bool isForward = to < from;

  for (int pos = from ; pos != to ; pos += isForward ? -1 : 1) {
    servo.write(pos);
    delay(MOVE_DELAY);
  }
}

void updateClock() {
  if(isDayTime()) {
    moveTo(110, 30);
  } else {
    moveTo(30, 110);
  }
}

void markStale() {}

void setup() {
  servo.attach(6);

  rtc.begin();
  rtc.setTime(14, 0, 0);
  rtc.setDate(0, 0, 0);

  pinMode(LED_BUILTIN, OUTPUT);

  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(markStale);
}


bool hasDemoRun = false;
void loop() {
  if(!hasDemoRun) {
    // run demo
    moveTo(110, 30);
    delay(1000);
    moveTo(30, 110);
    delay(1000);
    moveTo(110, 80);
    delay(1000);
    hasDemoRun = true;
  }

  updateClock();

  if(isDayTime()) {
    digitalWrite(LED_BUILTIN, HIGH);
    rtc.setAlarmTime(DAY_HOUR_END, DAY_MIN_END, 0);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    rtc.setAlarmTime(DAY_HOUR_START, DAY_MIN_START + 1, 0);
  }

  rtc.standbyMode();
}
