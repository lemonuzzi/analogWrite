//#include <TimerOne.h>

#define PWM_MAX_DUTY      255
#define PWM_MIN_DUTY      50
#define PWM_START_DUTY    100

//testing commit

int bldc_step = 0;
int i = 6000;

//Virtual neutral point
int vnn = 6; //(Pin D4)

//Inputs
int dig7 = 13;    //ADC1 (Pin D7)
int analog2 = 25;  //ADC2 (Pin A2)
int analog3 = 26;  //ADC3 (Pin A3)

//duty values
int duty = PWM_START_DUTY;

void setup() {
  Serial.begin(9600);
    
  //Enable pins
  DDRD = B00011100;    // Configure pins 2, 3 and 4 as outputs
  PORTD = B00000000;

  //Input pins
  DDRB = B00001110;    // Configure pins 9, 10 and 11 as outputs
  PORTB = B00000000;

  // inputs from motor using on-chip ADC
  pinMode(analog2, INPUT);
  pinMode(analog3, INPUT);
  pinMode(dig7, INPUT);

  // virtual neutral point for comparator circuit
  pinMode(vnn, OUTPUT);
}

// BLDC motor commutation function
void bldc_move() {
  switch (bldc_step) {
    case 0:
      AH_CL();
      break;
    case 1:
      BH_CL();
      break;
    case 2:
      BH_AL();
      break;
    case 3:
      CH_AL();
      break;
    case 4:
      CH_BL();
      break;
    case 5:
      AH_BL();
      break;
  }
}

void loop() {
  // Motor start
  while (i >= 20) {
    delayMicroseconds(i);
    bldc_move();
    bldc_step++;
    bldc_step %= 6;
    if (i > 20){
      i = i - 20;
    } 
    Serial.println(i);
  }
}

// Vary duty cycle based on closed-loop parameters
void varyDuty() {
}

// 6-STEP CASES
void AH_CL() {
  PORTD = B00010100;
  PORTB = B00000010;
}

void BH_CL() {
  PORTD = B00011000;
  PORTB = B00000100;
}

void BH_AL() {
  PORTD = B00001100;
  PORTB = B00000100;
}

void CH_AL() {
  PORTD = B00010100;
  PORTB = B00001000;
}

void CH_BL() {
  PORTD = B00011000;
  PORTB = B00001000;
}

void AH_BL() {
  PORTD = B00001100;
  PORTB = B00000010;
}
