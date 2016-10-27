#define SAMPLE_RATE 15360// 256*60
#define OUT1    13
unsigned long lastTime = millis();
volatile uint32_t sample = 0;
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
  // Set prescaler to 8 (see Table 17-9)
  TCCR2B = (TCCR2B & 0b11111000) | _BV(CS21);
  // Set the compare register (OCR2A)
  OCR2A = 64; // 16e6 / 8 / 256 / 120Hz (yields 120.19Hz * 256 interrupts)
  // Enable interrupt when TCNT2 == OCR2A
  TIMSK2 |= _BV(OCIE2A);
  sei(); // enable interrupts
}

void setup() {
  Serial.begin(57600);
  Serial.println("inverter.ino");
  Serial.println(F_CPU);
  pinMode(OUT1,OUTPUT);
  setup60hzInterrupt();
}

void loop() {
  if (millis() - lastTime > 1000) {
    lastTime = millis();
    Serial.print("interrupts per second: ");
    Serial.println(sample);
    sample = 0;
  }
}
