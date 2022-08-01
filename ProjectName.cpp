/**
 * Mr Benino 2022 07 17
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "servoObject.h"


bool direction = true;
int currentMillis = 400;
int servoPin = 0;

Servo myServo(servoPin);

/*
  Showing numbers, chars and phrases
                             A (seg[0] in this project)
                            ---
F (seg[5] in this project) |   | B (seg[1] in this project)
                           |   |
                            --- G (seg[6] in this project)
E (seg[4] in this project) |   |
                           |   | C (seg[2] in this project)
                            ---  . dot or dicimal (seg[7] in this project)
                             D (seg[3] in this project)
 */
#define A 12
#define B 10
#define C 20
#define D 18
#define E 17
#define F 11
#define G 21
#define DP 19
//#define D5 18
//#define D6 18
#define DIG1 15
#define DIG2 14
#define DIG3 13
#define DIG4 16

//#define CLK 20
//#define DT 21
//#define SW 22
//
//#define l_eye 27
//#define r_eye 26

long pos = 0;
int lastState = 0;
const int8_t increment[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

int seg[] = {A, B, C, D, E, F, G, DP}; // segment pins
char chars = 37; // max value in the array "Chars"

char Chars[37][9] = {
    {'0', 1, 1, 1, 1, 1, 1, 0, 0},//0
    {'1', 0, 1, 1, 0, 0, 0, 0, 0},//1
    {'2', 1, 1, 0, 1, 1, 0, 1, 0},//2
    {'3', 1, 1, 1, 1, 0, 0, 1, 0},//3
    {'4', 0, 1, 1, 0, 0, 1, 1, 0},//4
    {'5', 1, 0, 1, 1, 0, 1, 1, 0},//5
    {'6', 1, 0, 1, 1, 1, 1, 1, 0},//6
    {'7', 1, 1, 1, 0, 0, 0, 0, 0},//7
    {'8', 1, 1, 1, 1, 1, 1, 1, 0},//8
    {'9', 1, 1, 1, 1, 0, 1, 1, 0},//9
    {'a', 1, 1, 1, 0, 1, 1, 1, 0},//A/10
    {'b', 0, 0, 1, 1, 1, 1, 1, 0},//b/11
    {'c', 1, 0, 0, 1, 1, 1, 0, 0},//C/12
    {'d', 0, 1, 1, 1, 1, 0, 1, 0},//d/13
    {'e', 1, 0, 0, 1, 1, 1, 1, 0},//E/14
    {'f', 1, 0, 0, 0, 1, 1, 1, 0},//F/15
    {'g', 1, 0, 1, 1, 1, 1, 0, 0},//G/16
    {'h', 0, 1, 1, 0, 1, 1, 1, 0},//H/17
    {'i', 0, 0, 0, 0, 1, 1, 0, 0},//I/18
    {'j', 0, 1, 1, 1, 1, 0, 0, 0},//J/19
    {'l', 0, 0, 0, 1, 1, 1, 0, 0},//L/20
    {'n', 0, 0, 1, 0, 1, 0, 1, 0},//n/21
    {'o', 0, 0, 1, 1, 1, 0, 1, 0},//o/22
    {'p', 1, 1, 0, 0, 1, 1, 1, 0},//P/23
    {'q', 1, 1, 1, 0, 0, 1, 1, 0},//q/24
    {'r', 0, 0, 0, 0, 1, 0, 1, 0},//r/25
    {'s', 1, 0, 1, 1, 0, 1, 1, 0},//S/26   looks like number 5
    {'t', 0, 0, 0, 1, 1, 1, 1, 0},//t/27
    {'u', 0, 1, 1, 1, 1, 1, 0, 0},//U/28
    {'y', 0, 1, 1, 1, 0, 1, 1, 0},//y/29
    {'-', 0, 0, 0, 0, 0, 0, 1, 0},//-/30
    {'.', 0, 0, 0, 0, 0, 0, 0, 1},//./31
    {']', 1, 1, 1, 1, 0, 0, 0, 0},//]/32
    {'[', 1, 0, 0, 1, 1, 1, 0, 0},//[/33
    {'_', 0, 0, 0, 1, 0, 0, 0, 0},//_/34
    {'*', 1, 1, 0, 0, 0, 1, 1, 0},//*/35
    {':', 0, 0, 0, 0, 0, 0, 0, 1},//*/36
};

void render(char simbul) {
  int o;

  for (int i = 0; i < chars; i++) {
    if (Chars[i][0] == simbul) {
      o = i;
      break;
    }
  }

  for (int i = 0; i < 8; i++) {
    gpio_put(seg[i], Chars[o][i + 1]);
  }
}

void _render(int simbul) {
  for (int i = 0; i < 8; i++) {
    gpio_put(seg[i], Chars[simbul][i + 1]);
  }
}

void cls() {
  for (int i = 0; i < 8; i++) {
    gpio_put(seg[i], 1);
  }
}

void priloader() {
  for (int i = 0; i < 6; i++) {
    if (i == 0) gpio_put(seg[5], 1);
    if (i > 0) gpio_put(seg[i - 1], 1);
    gpio_put(seg[i], 0);
    sleep_ms(200);
  }
}

void set_pwm_pin(uint pin, uint freq, uint duty_c) { // duty_c between 0..10000
  gpio_set_function(pin, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(pin);
  pwm_config config = pwm_get_default_config();
  float div = (float) clock_get_hz(clk_sys) / (freq * 10000);
  pwm_config_set_clkdiv(&config, div);
  pwm_config_set_wrap(&config, 10000);
  pwm_init(slice_num, &config, true); // start the pwm running according to the config
  pwm_set_gpio_level(pin, duty_c); //connect the pin to the pwm engine and set the on/off level.
};

int main() {
  gpio_init(seg[0]);
  gpio_init(seg[1]);
  gpio_init(seg[2]);
  gpio_init(seg[3]);
  gpio_init(seg[4]);
  gpio_init(seg[5]);
  gpio_init(seg[6]);
  gpio_init(seg[7]);
  gpio_init(DIG1);
  gpio_init(DIG2);
  gpio_init(DIG3);
  gpio_init(DIG4);

  gpio_set_dir(seg[0], GPIO_OUT);
  gpio_set_dir(seg[1], GPIO_OUT);
  gpio_set_dir(seg[2], GPIO_OUT);
  gpio_set_dir(seg[3], GPIO_OUT);
  gpio_set_dir(seg[4], GPIO_OUT);
  gpio_set_dir(seg[5], GPIO_OUT);
  gpio_set_dir(seg[6], GPIO_OUT);
  gpio_set_dir(seg[7], GPIO_OUT);
  gpio_set_dir(DIG1, GPIO_OUT);
  gpio_set_dir(DIG2, GPIO_OUT);
  gpio_set_dir(DIG3, GPIO_OUT);
  gpio_set_dir(DIG4, GPIO_OUT);

  gpio_put(DIG1, 0);
  gpio_put(DIG2, 0);
  gpio_put(DIG3, 0);
  gpio_put(DIG4, 0);

  while (true) {
    currentMillis += (direction) ? 5 : -5;
    if (currentMillis >= 2400) direction = false;
    if (currentMillis <= 400) direction = true;
    myServo.write(currentMillis);
    sleep_ms(10);
  }

//  serva
//  set_pwm_pin(0,50,1);
//
//  set_pwm_pin(0,50,1000);

//     blink eye
//  char slovko[5] = {'c', 'h', ':','l', 'l'};
//  int direction = 1;
//  int duti_min = 1000;
//  int duti_max = 10000;
//  int duti = duti_min;
//  while (true) {
//
//    duti =+ direction;
//    if (duti > 10000) {
//      duti = duti_min;
//    }
//    pwm_set_gpio_level(0, duti);
//    printf(reinterpret_cast<const char *>(duti));
//
//    for (int i = 0; i < 5; i++) {
//      if(i == 0) {
//        gpio_put(DIG1, 0);
//        gpio_put(DIG2, 1);
//        gpio_put(DIG3, 1);
//        gpio_put(DIG4, 1);
//      }
//      if(i == 1) {
//        gpio_put(DIG1, 1);
//        gpio_put(DIG2, 0);
//        gpio_put(DIG3, 1);
//        gpio_put(DIG4, 1);
//      }
//      if(i == 2) {
//        gpio_put(DIG1, 1);
//        gpio_put(DIG2, 0);
//        gpio_put(DIG3, 1);
//        gpio_put(DIG4, 1);
//      }
//      if(i == 3) {
//        gpio_put(DIG1, 1);
//        gpio_put(DIG2, 1);
//        gpio_put(DIG3, 0);
//        gpio_put(DIG4, 1);
//      }
//      if(i == 4) {
//        gpio_put(DIG1, 1);
//        gpio_put(DIG2, 1);
//        gpio_put(DIG3, 1);
//        gpio_put(DIG4, 0);
//      }
//      cls();
//      render(slovko[i]);
//      printf(reinterpret_cast<const char *>(slovko[i]));
//      sleep_ms(1);
//    }
//  }

//line name
//  char slovko[5] = {'f', 'r', 'a', 'n', 'h'};
//  for (int i = 0; i < 5; i++) {
//    priloader();
//  }
//  for (int i = 0; i < 5; i++) {
//    render(slovko[i]);
//    sleep_ms(1000);
//    cls();
//  }
//  while (true) {
//    char state = gpio_get(CLK) | (gpio_get(DT) << 1);
//
//    if (state != lastState) {
//      pos += increment[state | (lastState << 2)];
//      lastState = state;
//      printf(reinterpret_cast<const char *>(pos / 4));
//      if ((pos / 4) >= 0 && (pos / 4) <= 36)
//        _render(pos / 4);
//    }
//  }
}