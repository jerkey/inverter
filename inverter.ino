#include "sinewave.h" // sinewave_length=256  sinewave_data[256]
#define SAMPLE_RATE 15360// 256*60
#define L1_low  9 // OC1A PB1
#define L1_high 8 // PB0
#define L2_low  10// OC1B PB2
#define L2_high 12// PB4

unsigned long lastTime = millis();
volatile uint32_t sample = 0;
volatile byte phase = 0; // 0 = L1 is HIGH, 1 = L2 is HIGH
// This is called at SAMPLE_RATE kHz to load the next sample.
ISR(TIMER2_COMPA_vect) {
  sample+=1;
}

void setup30720hzInterrupt() {
  // Set up Timer 2 to send a sample every interrupt.
  cli(); // disable interrupts
  // Set CTC mode (Table 17-8)
  TCCR2A = (TCCR2A & ~_BV(WGM20)) | _BV(WGM21);
  TCCR2B = TCCR2B & ~_BV(WGM22);
  // Set prescaler to 8 (see Table 17-9)
  TCCR2B = (TCCR2B & 0b11111000) | _BV(CS21);
  // Set the compare register (OCR2A)
  OCR2A = 64; // 16e6 / 8 / 256 / 120Hz (yields 120.19Hz * 256 interrupts)
  // Enable interrupt when TCNT2 == OCR2A
  TIMSK2 |= _BV(OCIE2A);
  sei(); // enable interrupts
}

void setupPWMs() {
  pinMode(L1_low,OUTPUT);
  pinMode(L1_high,OUTPUT);
  pinMode(L2_low,OUTPUT);
  pinMode(L2_high,OUTPUT);
  // timer1 pins 9 and 10

void setup() {
  Serial.begin(57600);
  Serial.println("inverter.ino");
  Serial.println(F_CPU);
  pinMode(OUT1,OUTPUT);
  setup30720hzInterrupt();
}

void loop() {
  if (millis() - lastTime > 1000) {
    lastTime = millis();
    Serial.print("interrupts per second: ");
    Serial.println(sample);
    sample = 0;
  }
}
