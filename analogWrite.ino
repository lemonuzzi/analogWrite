//#include <TimerOne.h>

#define PWM_MAX_DUTY      255
#define PWM_MIN_DUTY      50
#define PWM_START_DUTY    255

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
  DDRD = B00011100;    // Configure pins 2, 3 and 4 as outputs (Enables)
  PORTD = B00000000;   // (EN1=Pin 2, EN2=Pin 3, EN3=Pin 4)

  //Input pins
  DDRB = B00001110;    // Configure pins 9, 10 and 11 as outputs (Inputs)
  PORTB = B00000000;   // (IN1=INA=Pin 9, IN2=INB=Pin 10, IN3=INB=Pin 11)

  // Timer1 module setting: set clock source to clkI/O / 1 (no prescaling)
  TCCR1A = 0;
  TCCR1B = 0x01;
  // Timer2 module setting: set clock source to clkI/O / 1 (no prescaling)
  TCCR2A = 0;
  TCCR2B = 0x01;

  //Set PWM Duty cycle
  OCR1A = duty; //Pin 9
  OCR1B = duty; //Pin 10
  OCR2A = duty; //Pin 11

  ACSR   = 0x10;           // Disable and clear (flag bit) analog comparator interrupt

  // inputs from motor using on-chip ADC
  pinMode(analog2, INPUT);
  pinMode(analog3, INPUT);
  pinMode(dig7, INPUT);

  // virtual neutral point for comparator circuit
  pinMode(vnn, OUTPUT);
}

ISR (ANALOG_COMP_vect) {
  Serial.println("ISR Top");
  // BEMF debounce
  for (i = 0; i < 10; i++) {
    if (bldc_step & 1) {
      if (!(ACSR & 0x20)) 
        i -= 1;
    }
    else {
      if ((ACSR & 0x20))  
        i -= 1;
    }
  }
  bldc_move();
  bldc_step++;
  bldc_step %= 6;
  Serial.println("ISR Bottom");
}

// BLDC motor commutation function
void bldc_move() {
  switch (bldc_step) {
    case 0:
      AH_BL();
      BEMF_C_RISING();
      break;
    case 1:
      AH_CL();
      BEMF_B_FALLING();
      break;
    case 2:
      BH_CL();
      BEMF_A_RISING();
      break;
    case 3:
      BH_AL();
      BEMF_C_FALLING();
      break;
    case 4:
      CH_AL();
      BEMF_B_RISING();
      break;
    case 5:
      CH_BL();
      BEMF_A_FALLING();
      break;
  }
}

void loop() {
  // Motor start
  while (i >= 100) {
    delayMicroseconds(i);
    bldc_move();
    bldc_step++;
    bldc_step %= 6;
    i = i - 10;
    Serial.println(i);
  }
  ACSR |= 0x08;                    // Enable analog comparator interrupt
  while (1) {
    Serial.println("While loop");
  }
}

// Vary duty cycle based on closed-loop parameters
void varyDuty() {
}

void BEMF_A_RISING() {
  ADCSRB = (0 << ACME);    // Select AIN1 as comparator negative input
  ACSR |= 0x03;            // Set interrupt on rising edge
}
void BEMF_A_FALLING() {
  ADCSRB = (0 << ACME);    // Select AIN1 as comparator negative input
  ACSR &= ~0x01;           // Set interrupt on falling edge
}
void BEMF_B_RISING() {
  ADCSRA = (0 << ADEN);   // Disable the ADC module
  ADCSRB = (1 << ACME);
  ADMUX = 2;              // Select analog channel 2 as comparator negative input
  ACSR |= 0x03;
}
void BEMF_B_FALLING() {
  ADCSRA = (0 << ADEN);   // Disable the ADC module
  ADCSRB = (1 << ACME);
  ADMUX = 2;              // Select analog channel 2 as comparator negative input
  ACSR &= ~0x01;
}
void BEMF_C_RISING() {
  ADCSRA = (0 << ADEN);   // Disable the ADC module
  ADCSRB = (1 << ACME);
  ADMUX = 3;              // Select analog channel 3 as comparator negative input
  ACSR |= 0x03;
}
void BEMF_C_FALLING() {
  ADCSRA = (0 << ADEN);   // Disable the ADC module
  ADCSRB = (1 << ACME);
  ADMUX = 3;              // Select analog channel 3 as comparator negative input
  ACSR &= ~0x01;
}

// 6-STEP CASES
void AH_CL() {
  PORTD = B00010100;
  PORTB = B00000010;
  TCCR1A = B10000001;
}

void BH_CL() {
  PORTD = B00011000;
  PORTB = B00000100;
  TCCR1A = B00100001;
}

void BH_AL() {
  PORTD = B00001100;
  PORTB = B00000100;
  TCCR1A = B00100001;
}

void CH_AL() {
  PORTD = B00010100;
  PORTB = B00001000;
  TCCR2A = B10000001;
}

void CH_BL() {
  PORTD = B00011000;
  PORTB = B00001000;
  TCCR2A = B10000001;
}

void AH_BL() {
  PORTD = B00001100;
  PORTB = B00000010;
  TCCR1A = B10000001;
}
