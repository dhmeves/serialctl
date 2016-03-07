#include "hw.h"
#include "packet.h"
#include "globals.h"
#include <Servo.h>
void init_pins() {
  //Main motors
  osmc_init();
  //Linear Actuators
  SABERTOOTH12.begin(9600);
  SABERTOOTH34.begin(9600);
  delay(10);
  //failsafes
  for (int i = 0; i < 3; i++) {
    ST12.setTimeout(300);
    ST34.setTimeout(300);
    ST12.motor(1, 0);
    ST34.motor(1, 0);
    ST12.motor(2, 0);
    ST34.motor(2, 0);
  }
  count = 0;
  attachInterrupt(digitalPinToInterrupt(A), isrA, RISING);
}
void print_data() {
  float i1, i2;
  i1 = (analogRead(CURRENT_LEFT) - 512.0 - CURRENT_OFFSET) / 1.28;
  i2 = (analogRead(CURRENT_RIGHT) - 512.0 - CURRENT_OFFSET) / 1.28;
  SerComm.print("I(L):");
  SerComm.print(i1);
  SerComm.print("A   I(C):");
  SerComm.print(i2);
  SerComm.print("A, ");
  SerComm.print("H: ");
  SerComm.print(homed, DEC);
  SerComm.print(", C: ");
  SerComm.print(count);
  SerComm.println();
}

void osmc_init() {
  digitalWrite(13, LOW);
  digitalWrite(ALI1, LOW);
  digitalWrite(BLI1, LOW);
  digitalWrite(AHI1, LOW);
  digitalWrite(BHI1, LOW);
  digitalWrite(ALI2, LOW);
  digitalWrite(BLI2, LOW);
  digitalWrite(AHI2, LOW);
  digitalWrite(BHI2, LOW);
  pinMode(ALI1, OUTPUT);
  pinMode(AHI1, OUTPUT);
  pinMode(BLI1, OUTPUT);
  pinMode(BHI1, OUTPUT);
  pinMode(ALI2, OUTPUT);
  pinMode(AHI2, OUTPUT);
  pinMode(BLI2, OUTPUT);
  pinMode(BHI2, OUTPUT);
  pinMode(13, OUTPUT);
  fast_pwm();
}
void fast_pwm() {
  TCCR1B = (TCCR1B & 0b11111000) | 0x04;
  TCCR4B = (TCCR4B & 0b11111000) | 0x04;
}
// OSMC motor controller stuff
// Low side outputs must be PWM capable and NOT 5 or 6 (on Uno)
// Do not change timer0,
// Pins 7 and 8 use timer4 in phase correct mode
// Pins 11 and 12 use timer1 in phase correct mode
void drive_osmc(int rawpower, unsigned short brake, unsigned short ali, unsigned short bli, unsigned short ahi, unsigned short bhi) {
  int power = constrain(rawpower, -255, 255);
  //Stop!
  if (abs(power) < 2) {
    digitalWrite(ali, LOW);
    digitalWrite(bli, LOW);
    if (brake != 0) {
      digitalWrite(ahi, HIGH);
      digitalWrite(bhi, HIGH);
    } else {
      digitalWrite(ahi, LOW);
      digitalWrite(bhi, LOW);
    }
  }
  //Forward!
  if (power > 0) {
    digitalWrite(ahi, HIGH);
    digitalWrite(bhi, LOW);
    digitalWrite(ali, LOW);
    analogWrite(bli, power);
  }
  //Reverse!
  if (power < 0) {
    digitalWrite(ahi, LOW);
    digitalWrite(bhi, HIGH);
    digitalWrite(bli, LOW);
    analogWrite(ali, abs(power));
  }
}