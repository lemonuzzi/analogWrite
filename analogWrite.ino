#include <TimerOne.h>

#define PWM_MAX_DUTY      255
#define PWM_MIN_DUTY      50
#define PWM_START_DUTY    100

//testing commit

int bldc_step = 0;

//Outputs
int en1 = 5;   //EN1 (Pin 5)
int en2 = 11;  //EN2 (Pin 11)
int en3 = 12   //EN3 (Pin 12)
int in1 = 15;  //IN1 (Pin 15)
int in2 = 16;  //IN2 (Pin 16) 
int in3 = 17;  //IN3 (Pin 17)

//Virtual neutral point
int vnn = 6; 

//Inputs
int dig7 = 7;    //ADC1
int analog2 = 2;  //ADC2
int analog3 = 3;  //ADC3

//duty values
int duty = PWM_START_DUTY; 

void setup() {
  // enable
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(en3, OUTPUT);

  // PWM pins 
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);

  // inputs from motor using on-chip ADC 
  pinMode(analog2, INPUT);
  pinMode(analog3, INPUT);
  pinMode(dig7, INPUT);

  // virtual neutral point for comparator circuit
  pinMode(vnn, OUTPUT);
}

// BLDC motor commutation function
void bldc_move(){        
  switch(bldc_step){
    case 0:
      AH_BL();
      break;
    case 1:
      AH_CL();
      break;
    case 2:
      BH_CL();
      break;
    case 3:
      BH_AL();
      break;
    case 4:
      CH_AL();
      break;
    case 5:
      CH_BL();
      break;
  }
}

void loop() {
  bldc_move();
  Serial.println("Case :" + bldc_step);
  bldc_step++;
  bldc_step %= 6;
}

// Vary duty cycle based on closed-loop parameters
void varyDuty(){ 
}

// 6-STEP CASES
void AH_CL(){
  digitalWrite(en1, HIGH);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, LOW);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void BH_CL(){
  digitalWrite(en1, LOW);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, HIGH);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
}

void BH_AL(){
  digitalWrite(en1, HIGH);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void CH_AL(){
  digitalWrite(en1, HIGH);
  digitalWrite(en2, LOW);
  digitalWrite(en3, HIGH);
  digitalWrite(in1, LOW);
  digitalWrite(in3, HIGH);
}

void CH_BL(){
  digitalWrite(en1, LOW);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
}

void AH_BL(){
  digitalWrite(en1, HIGH);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, LOW);
  digitalWrite(in1, HIGH);
  digitalWrite(in3, LOW);
}
