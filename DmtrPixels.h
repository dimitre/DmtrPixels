// #define USEFASTLED 1
#define NUM_LEDS 124

#ifdef USEFASTLED
	#define FASTLED_INTERRUPT_RETRY_COUNT 0
	#define FASTLED_ALLOW_INTERRUPTS 0
	#define INTERRUPT_THRESHOLD 1
	#include "FastLED.h"
#else
	#include <Adafruit_NeoPixel.h>
	Adafruit_NeoPixel pixels(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);
#endif

struct rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct pixelHandler {
public:
#ifdef USEFASTLED
	CRGBArray<NUM_LEDS> leds;
#endif	

	// ver isso se faço todas as operacoes no meu pixel...
	rgb ledsRGB[NUM_LEDS];
	uint16_t ledStart = 0;
	uint16_t ledEnd = NUM_LEDS;

	void setup() {
#ifdef USEFASTLED
		FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, 0, ledEnd);
		// FastLED.setDither( 0 );
		FastLED.setDither(BINARY_DITHER);
#else
		pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
		pixels.updateLength(ledEnd); 
		fill(rgb{0,0,0});
		show();
#endif		
		fill(rgb{0,0,0});
		show();
		setBrightness(LED_BRIGHTNESS);
	}

#ifdef USEFASTLED
	static void setBrightness(const uint8_t & b) {		
		FastLED.setBrightness(b);
	}

	void show() {
		for (uint16_t a=ledStart; a<ledEnd; a++) {
			leds[a] = CRGB(ledsRGB[a].r, ledsRGB[a].g, ledsRGB[a].b);
		}
		FastLED.show();
	}

#else 
	static void setBrightness(const uint8_t & b) {		
		pixels.setBrightness(b);
	}

	static rgb getPixelLib (const uint16_t & n) {
		return rgb { 
			pixels.getPixelColor(n) & 0xff0000 >> 16, 
			pixels.getPixelColor(n) & 0x00ff00 >> 8, 
			pixels.getPixelColor(n) & 0x0000ff 
		};
	}

	void show() {
		for (uint16_t a=ledStart; a<ledEnd; a++) {
			pixels.setPixelColor(a, pixels.Color(ledsRGB[a].r, ledsRGB[a].g, ledsRGB[a].b));
		}
		pixels.show();
	}
#endif

	static rgb mix (const rgb & c1, const rgb & c2, const float & a, const float & b) {
		if (a == 0) { 
			return c1;
		}
		else if (a == 1.0) {
			return c2;
		}
		else {
			return rgb{
				(c1.r * a + c2.r * b),
				(c1.g * a + c2.g * b),
				(c1.b * a + c2.b * b)
			};
		}
	}

	void fadePixelColor (const uint16_t & number, const uint8_t & f) {
		uint8_t fac = 256 -f;
		ledsRGB[number].r = ledsRGB[number].r * fac >> 8;
		ledsRGB[number].g = ledsRGB[number].g * fac >> 8;
		ledsRGB[number].b = ledsRGB[number].b * fac >> 8;
	}

	void fadePixelPercent (const uint16_t & number, const float & p) {
		ledsRGB[number].r = uint8_t(float(ledsRGB[number].r) * p);
		ledsRGB[number].g = uint8_t(float(ledsRGB[number].g) * p);
		ledsRGB[number].b = uint8_t(float(ledsRGB[number].b) * p);
	}

	rgb getPixel (const uint16_t & number) {
		return ledsRGB[number];
	}

	void setPixel(const uint16_t & number, const rgb & cor) {
		ledsRGB[number] = cor;
	}

	void fadeToBlackBy(const uint8_t & f) {
		for (uint16_t a=ledStart; a<ledEnd; a++) {
			fadePixelColor(a, f);
			// setPixel(a, fadeColor(getPixel(a), f));
		}
	}

	void fadePercent(const float & p) {
		for (uint16_t a=ledStart; a<ledEnd; a++) {
			fadePixelPercent(a, p);
		}
	}

	void fill(const rgb & cor, const uint8_t & start, const uint8_t & end) {
		for (uint8_t a=start; a<end; a++) {
			setPixel(a, cor);
		}		
	}

	void fill(const rgb & cor) {
		fill(cor, ledStart, ledEnd);
	}
} px;