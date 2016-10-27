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
ISR(TIMER1_COMPA_vect) {
  sample+=1;
}

void setup60hzInterrupt() {
  // Set up Timer 1 to send a sample every interrupt.
  cli(); // disable interrupts
  // Set CTC mode (Section 15.9.2 Clear Timer on Compare Match)
  // WGM = 0b0100, TOP = OCR1A, Update 0CR1A Immediate (Table 15-4)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
  // No prescaler, CS = 0b001 (Table 15-5)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = F_CPU / SAMPLE_RATE; // 16e6 / 8000 = 2000
  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);
  sei(); // enable interrupts
}


void setup() {
  Serial.begin(57600);
  Serial.println("inverter.ino");
  Serial.println(F_CPU);
  setPwmFrequency(OUT1,1);
  pinMode(OUT1,OUTPUT);
  sample = 0;
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
