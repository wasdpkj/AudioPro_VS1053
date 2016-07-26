/*
 * http://wiki.microduino.cn/index.php/Microduino-Module_AudioPro/zh
 */
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, SDA, NEO_RGB + NEO_KHZ800);

#include <SPI.h>
#include "Audio.h"
uint8_t toneNum[12] = {57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76};
int pin_light[7] = {
  4, 5, 7, 6, 8, 9, A0
};

int mode = 0;
boolean sta[7] = {0, 0, 0, 0, 0, 0, 0};
boolean sta_cache[7] = {0, 0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  for (int a = 0; a < 7; a++) {
    pinMode(pin_light[a], INPUT_PULLUP);
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.println("Adafruit VS1053 Simple Test");
  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));
  musicPlayer.applyPatch(MIDIPatch, sizeof(MIDIPatch) / sizeof(uint16_t));

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(0, 0);
  musicPlayer.GPIO_pinMode(4, OUTPUT);  //LM4863,EN
  musicPlayer.GPIO_digitalWrite(4, LOW);
  uint8_t _bit = 0;
  for (int a = 0; a < 7; a++) {
    bitWrite(_bit, a, !digitalRead(pin_light[a]));
  }
  Serial.println(_bit);
  midiSetChannelBank(0, VS1053_BANK_MELODY);
  delay(100);
  midiSetInstrument(0, _bit);
  delay(100);
}

void loop() {
  for (int a = 0; a < 7; a++) {
    int b = digitalRead(pin_light[a]);
    sta[a] = b;
    if (!sta[a] && sta_cache[a]) {
      strip.setPixelColor(a % 2, 0, 100, 100);
      strip.show(); // Initialize all pixels to 'off'
      Serial.print(a);
      Serial.println("on");
      midiNoteOn(0, toneNum[a + 2], 127);
    }
    else if (sta[a] && !sta_cache[a]) {
      strip.setPixelColor(a % 2, 100, 100, 100);
      strip.show(); // Initialize all pixels to 'off'
      Serial.print(a);
      Serial.println("off");
      midiNoteOff(0, toneNum[a + 2], 127);
    }
    sta_cache[a] = sta[a];
  }
  delay(1);
}

