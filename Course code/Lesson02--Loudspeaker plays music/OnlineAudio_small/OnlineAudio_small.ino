#include <Wire.h>
#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"

#define I2S_DOUT 12
#define I2S_BCLK 13
#define I2S_LRC 11

Audio audio;
WiFiMulti wifiMulti;

String ssid = "yanfa1";
String password = "1223334444yanfa";

void setup() {
  Serial.begin(115200);

  pinMode(21, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  digitalWrite(21, HIGH);

  Wire.begin(15, 16);
  delay(50);
  Serial.printf("[LINE--%d]\n", __LINE__);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid.c_str(), password.c_str());
  wifiMulti.run();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect(true);
    wifiMulti.run();
  }
  Serial.printf("[LINE--%d]\n", __LINE__);
  Serial.println("WL_CONNECTED !");
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(20);  // 0...21
  digitalWrite(21, LOW);
  //  audio.connecttoFS(SD, "/320k_test.mp3");
  //  audio.connecttoFS(SD, "test.wav");
  // audio.connecttohost("https://www.growcube.cc/software/HMI/test.mp3"); //陪你去流浪 .mp3
  // audio.connecttohost("http://music.163.com/song/media/outer/url?id=66285.mp3");//葡萄成熟时 .mp3
  audio.connecttohost("http://music.163.com/song/media/outer/url?id=2086327879.mp3"); //鲜花.mp3
  // audio.connecttohost("http://music.163.com/song/media/outer/url?id=5103312.mp3"); //Empire state of mine.mp3
  Serial.printf("[LINE--%d]\t ready to play!!\n", __LINE__);
}

void loop() {
  audio.loop();
  if (Serial.available()) {  // put streamURL in serial monitor
    audio.stopSong();
    String r = Serial.readString();
    r.trim();
    if (r.length() > 5) audio.connecttohost(r.c_str());
    log_i("free heap=%i", ESP.getFreeHeap());
  }
}

// optional
void audio_info(const char *info) {
  Serial.print("info        ");
  Serial.println(info);
}
void audio_id3data(const char *info) {  //id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}
void audio_eof_mp3(const char *info) {  //end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
}
void audio_showstation(const char *info) {
  Serial.print("station     ");
  Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle ");
  Serial.println(info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     ");
  Serial.println(info);
}
void audio_commercial(const char *info) {  //duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info) {  //homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info) {  //stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}
