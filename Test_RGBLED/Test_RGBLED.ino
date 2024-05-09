#include "Freenove_WS2812B_RGBLED_Controller.h"

#define I2C_ADDRESS  0x20
#define LEDS_COUNT   10

Freenove_WS2812B_Controller strip(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB);

u8 m_color[5][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 255}, {0, 0, 0}};
u32 yellow = 0xFFFF00;
u32 red = 0xFF0000;
u32 red_dark = 0xC80000;
u32 white = 0xFFFFFF;


int delayval = 100;

void setup() {
  while (!strip.begin());
}

void loop() {

  strip.setLedColor(1, white);
  strip.setLedColor(2, white);
  strip.setLedColor(3, white);
  strip.setLedColor(6, red_dark);
  strip.setLedColor(7, red_dark);
  strip.setLedColor(8, red_dark);
  delay(1000);

  for (int j = 0; j < 5; j++) {
    strip.setLedColor(0, yellow);
    strip.setLedColor(1, yellow);
    strip.setLedColor(8, yellow);
    strip.setLedColor(9, yellow);

    delay(500);
    strip.setLedColor(0, 0);
    strip.setLedColor(1, white);
    strip.setLedColor(8, red_dark);
    strip.setLedColor(9, 0);
    delay(500);
  }

  //strip.setAllLedsColor(white);
  //delay(1000);

  for (int j = 0; j < 5; j++) {
    strip.setLedColor(3, yellow);
    strip.setLedColor(4, yellow);
    strip.setLedColor(5, yellow);
    strip.setLedColor(6, yellow);

    delay(500);
    strip.setLedColor(4, 0);
    strip.setLedColor(3, white);
    strip.setLedColor(6, red_dark);
    strip.setLedColor(5, 0);
    delay(500);
  }

return;
  // for (int j = 0; j < 5; j++) {
  //   for (int i = 0; i < LEDS_COUNT; i++) {
  //     strip.setLedColor(i, m_color[j][0], m_color[j][1], m_color[j][2]);
  //     delay(delayval);
  //   }
  //   delay(500);
  // }
}
