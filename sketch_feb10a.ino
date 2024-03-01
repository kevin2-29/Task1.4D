#include <avr/io.h>             // Include AVR IO library
#include <avr/interrupt.h>      // Include AVR interrupt library

#define RED_LED_PIN_1 5         // Define pin number for red LED 1
#define GREEN_LED_PIN_2 6       // Define pin number for green LED 2
#define LED_PIN_3 7             // Define pin number for LED 3
#define PIR_SENSOR_PIN_1 2      // Define pin number for PIR sensor 1
#define PIR_SENSOR_PIN_2 3      // Define pin number for PIR sensor 2
#define TILT_PIN 4     // Define pin number for TILT sensor
#define LLED_PIN 8             // Define pin number for inbuilt LED

volatile bool LEDRunning = false;               // Flag to indicate if motor is running
volatile unsigned long LEDStartTime = 0;        // Time when motor started

void setup() {
  pinMode(RED_LED_PIN_1, OUTPUT);                      // Set LED pin 1 as output
  pinMode(GREEN_LED_PIN_2, OUTPUT);                      // Set LED pin 2 as output
  pinMode(LED_PIN_3, OUTPUT);                      // Set LED pin 3 as output
  pinMode(LLED_PIN, OUTPUT);                      // Set motor pin as output

  digitalWrite(RED_LED_PIN_1, LOW);                    // Turn off LED 1 initially
  digitalWrite(GREEN_LED_PIN_2, LOW);                    // Turn off LED 2 initially
  digitalWrite(LED_PIN_3, LOW);                    // Turn off LED 3 initially
  digitalWrite(LLED_PIN, LOW);                    // Turn off LED initially

  // Enable Pin Change Interrupt for PIR sensors
  PCICR |= (1 << PCIE2);                           // Enable Pin Change Interrupt Control Register 2
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20); // Set mask for specific pins

  // Enable Timer Interrupt
  enableLEDInterrupt(500);                       // Enable LED interrupt with a period of 500 ms

  // Initialize Serial communication
  Serial.begin(9600);                              // Start serial communication with baud rate 9600
  Serial.println("Sensor Simulation Started");      // Print message indicating sensor simulation started
}

void loop() {
  // Your main loop code here
}

void enableLEDInterrupt(int milliseconds) {
  noInterrupts();                                   // Disable interrupts
  uint16_t ocrValue = (uint16_t)((F_CPU / 1024UL) * (milliseconds / 1000.0) - 1); // Calculate timer compare value
  TCCR1A = 0;                                       // Set Timer/Counter1 Control Register A to 0
  TCCR1B = 0;                                       // Set Timer/Counter1 Control Register B to 0
  TCNT1 = 0;                                        // Set Timer/Counter1 to 0
  
  OCR1A = ocrValue;                                // Set Output Compare Register A
  TCCR1B  |= (1 << WGM12);                        // Set Waveform Generation Mode 12
  TCCR1B  |= (1 << CS12) | (1 << CS10);           // Set Clock Select to prescaler of 1024
  TIMSK1 |= (1 << OCIE1A);                        // Enable Timer/Counter1 Output Compare A Match Interrupt
  interrupts();                                   // Enable interrupts
}

// Interrupt Service Routine for Pin Change Interrupt on PCINT2 vector
ISR(PCINT2_vect) {
  if (digitalRead(PIR_SENSOR_PIN_1) == HIGH) {                // If PIR sensor 1 is activated
    digitalWrite(RED_LED_PIN_1, HIGH);                             // Turn on LED 1
    Serial.println("PIR Sensor 1 Activated");                  // Print message indicating PIR sensor 1 activation
  } else {
    digitalWrite(RED_LED_PIN_1, LOW);                              // Turn off LED 1
  }

  if (digitalRead(PIR_SENSOR_PIN_2) == HIGH) {                // If PIR sensor 2 is activated
    digitalWrite(GREEN_LED_PIN_2, HIGH);                             // Turn on LED 2
    Serial.println("PIR Sensor 2 Activated");                  // Print message indicating PIR sensor 2 activation
  } else {
    digitalWrite(GREEN_LED_PIN_2, LOW);                              // Turn off LED 2
  }
  
  if (digitalRead(TILT_PIN) == HIGH) {              // If TILT sensor is activated
    digitalWrite(LED_PIN_3, HIGH);                             // Turn on LED 3
    Serial.println("Tilt Sensor 3 High");                      // Print message indicating TILT sensor activation
  } else {
    digitalWrite(LED_PIN_3, LOW);                              // Turn off LED 3
  }
}

// Interrupt Service Routine for Timer1 compare match interrupt
ISR(TIMER1_COMPA_vect) {
  if (!LEDRunning) {                               // If LED is not running
    digitalWrite(LLED_PIN, HIGH);                   // Turn on LED
    LEDRunning = true;                              // Set ED running flag to true
    LEDStartTime = millis();                        // Record the start time of LED operation
  } else {
    unsigned long elapsedTime = millis() - LEDStartTime; // Calculate elapsed time since LED started
    if (elapsedTime >= 5000) {                           // If 5 seconds have elapsed since LED started
      digitalWrite(LLED_PIN, LOW);                       // Turn off LED
      LEDRunning = false;                                // Set LED running flag to false
    }
  }
}
