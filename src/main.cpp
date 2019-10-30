#include <Arduino.h>
#include <Servo.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

Servo servo;


const char * ssid = "Volta";
const char * pass = "V0lt@2018"; // I don't mind people using my wifi if they are in the area

const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const long MAX_SLEEP_DURATION_IN_MINUTES = 70;
const int SERVO_PIN = 6;

int DAY_HOUR_START = 6;
int DAY_MIN_START = 50;

int DAY_HOUR_END = 19;
int DAY_MIN_END = 0;

int MOVE_DELAY = 90;

typedef struct {
  unsigned long lastTime;
  unsigned long sleepDurationInSeconds;
} stateStruct __attribute__((aligned(4)));
stateStruct state;

int toMinutes(int hours, int minutes) {
  return (hours * 60) + minutes;
}

bool isDayTime() {
  int minutes = toMinutes(hour(), minute());
  return minutes >= toMinutes(DAY_HOUR_START, DAY_MIN_START) && minutes < toMinutes(DAY_HOUR_END, DAY_MIN_END);
}

void moveTo(int from, int to) {
  servo.attach(SERVO_PIN);
  bool isForward = to < from;

  for (int pos = from ; pos != to ; pos += isForward ? -1 : 1) {
    servo.write(pos);
    delay(MOVE_DELAY);
  }
  servo.detach();
}

void updateClock() {
  if(isDayTime()) {
    moveTo(110, 30);
  } else {
    moveTo(30, 110);
  }
}

void setup() {
  Serial.begin(9600);
  delay(200);

  system_rtc_mem_read(64, &state, sizeof(state));
  const unsigned long currentTimeInSeconds = state.lastTime + state.sleepDurationInSeconds;

  if(ESP.getResetInfoPtr()->reason != 5) {
    Serial.print("connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(100); }
    Serial.print("done\n");
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();

    Serial.print("\nrequesting time... ");
    timeClient.begin();
    timeClient.update();
    // TODO: do I need to set time or does the time client do that? setTime();
    timeClient.end();
    Serial.print("done\n");
  } else {
    setTime(currentTimeInSeconds); // this is inacurate and so is the "rtc" of the ESP but close enough
  }

  updateClock();

  const int currentMinutes = toMinutes(hour(), minute());
  const int minutesTillNight = toMinutes(DAY_HOUR_END, DAY_MIN_END) - currentMinutes;
  const int minutesTillMorning = toMinutes(DAY_HOUR_START, DAY_MIN_START) - currentMinutes;
  const int minutesToSleep = minutesTillNight < minutesTillMorning ? minutesTillNight : minutesTillMorning;

  if(minutesToSleep < MAX_SLEEP_DURATION_IN_MINUTES)
    state.sleepDurationInSeconds = minutesToSleep * 1000 * 1000;
  else
    state.sleepDurationInSeconds = MAX_SLEEP_DURATION_IN_MINUTES * 60;

  state.lastTime = now();
  system_rtc_mem_write(64, &state, sizeof(state));
  ESP.deepSleep(state.sleepDurationInSeconds * 1000 * 1000);
}

//https://github.com/dalmatianrex/articles/blob/master/makerpro-esp8266-ntp/ESP8266_Time.ino
//
//https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/NTP-TZ-DST/NTP-TZ-DST.ino
void loop() { }
