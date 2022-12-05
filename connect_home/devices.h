#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define LED_OFF 0
#define LED_ON 1

class Led {
  private:
    Adafruit_NeoPixel pixels;
    int pin;
    byte state;
    int delayval;

  public:
    Led(int pin);
    void init();
    void on();
    void off();
    byte getState();
    void setDelay(int delayval);
};


class Blind {
  private:
  int pin;
  int pos;
  Servo myservo;

  public:
    Blind(int pin);
    void on();
    void off();
};
