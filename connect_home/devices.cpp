#include "Led.h"

Led::Led(int pin) {
  // Use 'this->' to make the difference between the
  // 'pin' attribute of the class and the 
  // local variable 'pin' created from the parameter.
  this->pixels = Adafruit_NeoPixel(4, pin, NEO_GRB + NEO_KHZ800);
  this->pin = pin;
  init();
}
void Led::init() {
  pixels.begin();
  pinMode(pin, OUTPUT);
  // Always try to avoid duplicate code.
  // Instead of writing digitalWrite(pin, LOW) here,
  // call the function off() which already does that
  on();
}
void Led::on() {
   for(int i=0;i<4;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(150,150,150)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).
  }
}
void Led::off() {
     for(int i=0;i<4;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).
  }
}

byte Led::getState() {
  return state;
}

void Led::setDelay(int delayval){
  this->delayval = delayval;
}

Blind::Blind(int pin){
  this->pin = pin;
  this->myservo.attach(pin);
  this->pos = 170;
  myservo.write(pos); 
  on();
}

void Blind::on() {
  
    for (pos = 50; pos <= 170; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
    }
  
}
void Blind::off() {

  for (pos = 170; pos >= 50; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
