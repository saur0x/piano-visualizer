#include <FastLED.h>


#define NUM_LEDS 88
#define DATA_PIN 7
#define LED_TYPE WS2812B

#define BRIGHTNESS 255	// brightness
#define SATURATION 255	// saturation


typedef unsigned char uchar;

CRGB leds[NUM_LEDS];

void setup() {
	FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
}

uchar readByte() {
	while (Serial.available() <= 0);
	return Serial.read();
}

void loop() {
	uchar instruction = readByte();
	uchar event = instruction >> 7;

	int index = (instruction & 0x7F);

	if (index >= 0 && index < NUM_LEDS) {
		// leds[index] = CRGB(color[0] * event, color[1] * event, color[2] * event);
		leds[index] = CHSV(255, SATURATION, BRIGHTNESS * event);
		FastLED.show();
	}

	/*
	for (int j = 0; j < 255; j++) {
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i] = CHSV(i - (j * 2), SATURATION, BRIGHTNESS);
		}
		FastLED.show();
		delay(25);
	}
	*/
}