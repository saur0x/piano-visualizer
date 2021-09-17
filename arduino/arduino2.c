#include <FastLED.h>


#define LED_PIN 7
#define NUM_LEDS 88


typedef unsigned char uchar;

CRGB leds[NUM_LEDS];
uchar color[3] = { 56, 128, 244 };


uchar readByte() {
	while (Serial.available() <= 0);
	return Serial.read();
}

void setup() {
    Serial.begin(9600);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
	uchar instruction = readByte();
	uchar event = instruction >> 7;
	int index = (instruction & 0x7F);

	if (index >= 0 && index < NUM_LEDS) {
		leds[index] = CRGB(color[0] * event, color[1] * event, color[2] * event);
		FastLED.show();
	}
}