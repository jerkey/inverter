// http://extremeelectronics.co.in/avr-tutorials/pwm-signal-generation-by-using-avr-timers-part-ii/
// http://www.embedds.com/programming-16-bit-timer-on-atmega328/
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#define SAMPLE_RATE 15360// 256*60
#define OUT1    13
unsigned long lastTime = millis();
volatile uint16_t sample;
// This is called at SAMPLE_RATE kHz to load the next sample.
ISR(TIMER2_COMPA_vect) {
  sample+=1;
}

void setup60hzInterrupt() {
  // Set up Timer 2 to send a sample every interrupt.
  cli(); // disable interrupts
  // Set CTC mode (Table 17-8)
  TCCR2A = (TCCR2A & ~_BV(WGM20)) | _BV(WGM21);
  TCCR2B = TCCR2B & ~_BV(WGM22);
  // Set prescaler to 1 (No prescaler) see Table 17-9
  TCCR2B = (TCCR2B & 0b11111000) | _BV(CS20);
  // Set the compare register (OCR2A)
  OCR2A = F_CPU / SAMPLE_RATE; // 16e6 / 8000 = 2000
  // Enable interrupt when TCNT2 == OCR2A
  TIMSK2 |= _BV(OCIE2A);
  sei(); // enable interrupts
}

void setup() {
  Serial.begin(57600);
  Serial.println("inverter.ino");
  Serial.println(F_CPU);
  setPwmFrequency(OUT1,1);
  pinMode(OUT1,OUTPUT);
  sample = 0;
  delay(1000);
  setup60hzInterrupt();
}

void loop() {
  if (millis() - lastTime > 1000) {
    lastTime = millis();
    Serial.println(sample);
    sample = 0;
  }
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 64: 
      mode = 0x03; 
      break;
    case 256: 
      mode = 0x04; 
      break;
    case 1024: 
      mode = 0x05; 
      break;
    default: 
      return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } 
    else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } 
  else if(pin == 3 || pin == 11) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 32: 
      mode = 0x03; 
      break;
    case 64: 
      mode = 0x04; 
      break;
    case 128: 
      mode = 0x05; 
      break;
    case 256: 
      mode = 0x06; 
      break;
    case 1024: 
      mode = 0x7; 
      break;
    default: 
      return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
