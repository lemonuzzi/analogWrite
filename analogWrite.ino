//#include <TimerOne.h>
//No PWM, motor constant spin code
//DP-group29-Leo&Fred

#define PWM_MAX_DUTY      255
#define PWM_MIN_DUTY      50
#define PWM_START_DUTY    100

//testing commit

int bldc_step = 0;
int i = 10000;

//Outputs
int en1 = 2;  //EN1 (Pin D3)
int en2 = 3;  //EN2 (Pin D5)
int en3 = 4;  //EN3 (Pin D6)
int in1 = 9 ;  //IN1 (Pin D9)
int in2 = 10;  //IN2 (Pin D10)
int in3 = 11;  //IN3 (Pin D11)

//Virtual neutral point
int vnn = 6; //(Pin D4)

//Inputs
int dig7 = 13;    //ADC1 (Pin D7)
int analog2 = 25;  //ADC2 (Pin A2)
int analog3 = 26;  //ADC3 (Pin A3)

//Timers and Interrupts


//duty values
int duty = PWM_START_DUTY;

void setup() {
  Serial.begin(9600);
  
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

ISR(TIMER1_COMPA_vect) {
  
}

void loop() {
  // Motor start
  while (i >= 1000) {
    delayMicroseconds(i);
    bldc_move();
    bldc_step++;
    bldc_step %= 6;
    if (i > 1000){
      i = i - 20;
    } 
  }
  
  //delayMicroseconds(280);
}

// Vary duty cycle based on closed-loop parameters
void varyDuty() {
}

// 6-STEP CASES
void AH_CL() {
  digitalWrite(en1, HIGH);
  digitalWrite(en2, LOW);
  digitalWrite(en3, HIGH);
  digitalWrite(in1, HIGH);
  digitalWrite(in3, LOW);
}

void BH_CL() {
  digitalWrite(en1, LOW);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, HIGH);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
}

void BH_AL() {
  digitalWrite(en1, HIGH);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void CH_AL() {
  digitalWrite(en1, HIGH);
  digitalWrite(en2, LOW);
  digitalWrite(en3, HIGH);
  digitalWrite(in1, LOW);
  digitalWrite(in3, HIGH);
}

void CH_BL() {
  digitalWrite(en1, LOW);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
}

void AH_BL() {
  digitalWrite(en1, HIGH);
  digitalWrite(en2, HIGH);
  digitalWrite(en3, LOW);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}
