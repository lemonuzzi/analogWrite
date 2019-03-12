//#include <TimerOne.h>

#define PWM_MAX_DUTY      255
#define PWM_MIN_DUTY      50
#define PWM_START_DUTY    0

//testing commit

int bldc_step = 0, i_prep = 0, i_ramp = 3000, duty = PWM_START_DUTY;
int sensorValue, i;
volatile int i_zEvent = 0;

void setup() {
  Serial.begin(9600);
  
  // Enable pins
  DDRD = B00011100;    // Configure pins 2, 3 and 4 as outputs (Enables)
  PORTD = B00000000;   // (EN1=Pin 2, EN2=Pin 3, EN3=Pin 4)

  // Input pins
  DDRB = B00001110;    // Configure pins 9, 10 and 11 as outputs (Inputs)
  PORTB = B00000000;   // (IN1=INA=Pin 9, IN2=INB=Pin 10, IN3=INB=Pin 11)

  // Timer1 module setting: set clock source to clkI/O / 1 (no prescaling)
  TCCR1A = 0;
  TCCR1B = 0x01;
  // Timer2 module setting: set clock source to clkI/O / 1 (no prescaling)
  TCCR2A = 0;
  TCCR2B = 0x01;
  
  // Disable and clear (flag bit) analog comparator interrupt
  ACSR   = 0x10;           
}

ISR (ANALOG_COMP_vect) {
  // Makes sure there are at least 2 Zero-Crossing events before we switch to autocommutate mode
  if (i_zEvent < 2){
    i_zEvent++;
    return;
  }
  Serial.println("In ISR");
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
  
  // Prespositioning Section
//  while(duty < 230){
//    setDuty();
//    AH_BL_CL();
//    duty = pow(1.2,i_prep);
//    i_prep++;
//    delayMicroseconds(15);
//    Serial.println(duty);
//  }

  // Ramp up sequence (FSM for 24 steps out of 36)
  while (i_ramp >= 200) {
    Serial.println(i_ramp);
    delayMicroseconds(i_ramp);
    bldc_move();
    bldc_step++;
    bldc_step %= 6;
    i_ramp = i_ramp - 20;
    if (i_ramp == 440){
      // Enable analog comparator interrupt
      ACSR |= 0x08;
    }
    if (i_zEvent == 2){
      break;
    }
  }
                  
  while (1) {
    
  }
}

// Vary duty cycle based on closed-loop parameters
void setDuty() {
  //Set PWM Duty cycle
  OCR1A = duty; //Pin 9
  OCR1B = duty; //Pin 10
  OCR2A = duty; //Pin 11
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

//Motor Prepositioning case
void AH_BL_CL() {
  PORTD = B00011100;
  PORTB = B00000010;
  TCCR1A = B10000001;
}
