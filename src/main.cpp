#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

Servo servo;

const char * ssid = "BELL502";
const char * pass = "2167ACD3E6AF"; // I don't mind people using my wifi if they are in the area

const long utcOffsetInSeconds = 3600;

const unsigned long MAX_SLEEP_DURATION = 70 * 60;
const int SERVO_PIN = D4;

int MOVE_DELAY = 90;

const unsigned long DAY_START = ((6 * 60) + 50) * 60;
const unsigned long DAY_END = 19 * 60 * 60;

IPAddress timeServerIP;
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;
unsigned int localPort = 2390;

typedef struct {
  unsigned long lastTime;
  unsigned long sleepDurationInSeconds;
} stateStruct __attribute__((aligned(4)));
stateStruct state;

unsigned long toSecondsInDay(unsigned long t) {
  return (hour(t) * 60 * 60) + (minute(t) * 60) + second(t);
}

bool isDayTime(unsigned long c) {
  bool isDayTime = c >= DAY_START && c < DAY_END;

  Serial.print("is day time? ");
  Serial.print(isDayTime);
  Serial.print(" @ ");
  Serial.print(c);
  Serial.println(" seconds.");

  return isDayTime;
}

void moveTo(int from, int to) {
  servo.attach(SERVO_PIN);
  servo.write(to);
  delay(1000);
  servo.detach();
}

// from sketch examples
void sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// from sketch examples
unsigned long fetchNTPTime() {
  Serial.print("syncing time");
  udp.begin(localPort);
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(200);

  int cb;
  int counter = 0;
  while(!(cb = udp.parsePacket())) {
    Serial.print(".");
    delay(100);
    counter ++;
    if(counter > 50) {
      Serial.println("failed.");
      return fetchNTPTime();
    }
  }
  Serial.println("done.");

  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  const unsigned long seventyYears = 2208988800UL;
  unsigned long epoch = secsSince1900 - seventyYears;

  return epoch;
}

void setupSystemTime() {
  if(ESP.getResetInfoPtr()->reason != 5) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.print("connecting to wifi");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(50);
    }
    Serial.println("done.");

    const unsigned long currentTime = fetchNTPTime();
    setTime(currentTime);
    adjustTime(60 * 60 * -4); // apply TZ (but not DST)
  } else {
    Serial.print("using time from RTC mem: ");
    Serial.print(state.lastTime);
    Serial.print(", ");
    Serial.println(state.sleepDurationInSeconds);

    setTime(state.lastTime + state.sleepDurationInSeconds); // this is inacurate and so is the "rtc" of the ESP but close enough
  }
}

void setup() {
  Serial.begin(9600);
  delay(200);

  system_rtc_mem_read(64, &state, sizeof(state));

  setupSystemTime();

  // if there was a day / night change between last run and now then update clock position
  unsigned long current = toSecondsInDay(now());
  if(isDayTime(toSecondsInDay(state.lastTime)) != isDayTime(current)) {
    if(isDayTime(current)) {
      Serial.println("its day time.");
      moveTo(110, 30);
    } else {
      Serial.println("its night time.");
      moveTo(30, 110);
    }
  }

  unsigned long secToSleep;
  if(current >= DAY_END)
    secToSleep = MAX_SLEEP_DURATION;
  else if(current >= DAY_START)
    secToSleep = DAY_END - current;
  else
    secToSleep = DAY_START - current;

  state.sleepDurationInSeconds = secToSleep < MAX_SLEEP_DURATION ? secToSleep : MAX_SLEEP_DURATION;

  Serial.print("sleeping for ");
  Serial.print(state.sleepDurationInSeconds);
  Serial.print(" / ");
  Serial.print(MAX_SLEEP_DURATION);
  Serial.println(" seconds.");

  state.lastTime = now();

  Serial.print("writing in time to RTC: ");
    Serial.print(state.lastTime);
    Serial.print(", ");
    Serial.println(state.sleepDurationInSeconds);

  system_rtc_mem_write(64, &state, sizeof(state));
  ESP.deepSleep(state.sleepDurationInSeconds * 1000 * 1000);
}

void loop() { }
