/*
Base sketch for the Bug Bounty Hunters Sweden Challenge Card.
Button 1 turns on the green LED for a set amount of time, DIP1&2 doubles the on-time respectively.
Button 2 turns on the red LED for a set amount of time, DIP3&4 doubles the on-time respectively.

Known issue: Clock rollover after 49 days runtime will make LEDs turn on "for good", deal with it.
*/

// DIP switches: 1-PD3, 2-PD2, 3-PD1, 4-PD0
// Buttons: 1-PD6-D12, 2-PD4-D4
// LEDs: Green-PB0, Red-PD5

// For explanation of port manipulation see: https://www.arduino.cc/en/Reference/PortManipulation

#include <Bounce2.h>  // Used for bounce-free buttons

// Bit masks for each feature
#define DIP1 B00001000
#define DIP2 B00000100
#define DIP3 B00000010
#define DIP4 B00000001
#define BUTTON1 B01000000
#define BUTTON2 B00010000
#define LED_GREEN B00000001
#define LED_RED B00100000

// Debouncer objects
Bounce bouncer1 = Bounce();
Bounce bouncer2 = Bounce();

// Function declarations
bool getButton(byte);
bool getDIP(byte);
void setLED(byte, bool);

// Parameters
const byte debounceInterval = 20;
const int baseLedOnTime = 1000;

// Variables
bool ledGreenState = false;
bool ledRedState = false;
unsigned long ledGreenOffTime = 0;
unsigned long ledRedOffTime = 0;

void setup() {  
  // DIP switches
  DDRD &=  ~(DIP1 | DIP2 | DIP3 | DIP4);  // Set as inputs
  PORTD |=  (DIP1 | DIP2 | DIP3 | DIP4);  // Activate input pullups

  // Buttons
  DDRD &=  ~(BUTTON1 | BUTTON2);        // Set Button 1&2 as inputs
  PORTD |=  (BUTTON1 | BUTTON2);        // Activate input pullups
  bouncer1.attach(12);                  // Attach debouncer to button 1
  bouncer1.interval(debounceInterval);  // Set debounce deadzone
  bouncer2.attach(4);                   // Attach debouncer to button 2
  bouncer2.interval(debounceInterval);  // Set debounce deadzone

  // LEDs
  DDRB |=   LED_GREEN;  // Set Green LED as output
  PORTB &= ~LED_GREEN;  // Set Green LED default low
  DDRD |=   LED_RED;    // Set Red LED as output
  PORTD &= ~LED_RED;    // Set Red LED default low
}

void loop() {
  // Update bouncers
  bouncer1.update();
  bouncer2.update();
  
  // If button 1 was pressed, turn on green LED
  if(bouncer1.fell() == HIGH){
    setLED(1, HIGH);
    ledGreenState = true;
    int duration = baseLedOnTime;

    // Double the LED on time for each of DIP1&2 that is ON (bit shift left <=> multiply by 2)
    if(getDIP(1) == LOW){
      duration = duration << 1;
    }
    if(getDIP(2) == LOW){
      duration =  duration << 1;
    }

    // Set planned time to turn off
    ledGreenOffTime = millis() + duration;    
  }

  // If button 2 was pressed, turn on red LED
  if(bouncer2.fell() == HIGH){
    setLED(2, HIGH);
    ledRedState = true;
    int duration = baseLedOnTime;

    // Double the LED on time for each of DIP3&4 that is ON (bit shift left <=> multiply by 2)
    if(getDIP(3) == LOW){
      duration = duration << 1;
    }
    if(getDIP(4) == LOW){
      duration =  duration << 1;
    }

    // Set planned time to turn off
    ledRedOffTime = millis() + duration;    
  }

  // Check if LEDs are due to turn off
  if(ledGreenState){  // If Green LED is lit
    if(millis() >= ledGreenOffTime){
      setLED(1, false);
      ledGreenState = false;
    }
  }
  if(ledRedState){  // If Red LED is lit
    if(millis() >= ledRedOffTime){
      setLED(2, false);
      ledRedState = false;
    }
  }
}

// Returns the state of buttons 1 and 2, LOW = button pressed
bool getButton(byte i){
  switch(i){
    case 1:
      return PIND &= BUTTON1;
    case 2:
      return PIND &= BUTTON2;
    default:
      return false;
  }
}

// Returns the state of DIP switch 1-4, LOW = ON
bool getDIP(byte i){
  switch(i){
    case 1:
      return PIND &= DIP1;
    case 2:
      return PIND &= DIP2;
    case 3:
      return PIND &= DIP3;
    case 4:
      return PIND &= DIP4;
    default:
      return false;
  }
}

// Sets the state of LED 1&2 (green&red), HIGH = ON
void setLED(byte i, bool value){
  switch(i){
    case 1: // Green LED
      if(value == HIGH){
        PORTB |= LED_GREEN;
      }
      else {
        PORTB &= ~LED_GREEN;
      }
      break;
    case 2: // Red LED
      if(value == HIGH){
        PORTD |= LED_RED;
      }
      else{
        PORTD &= ~LED_RED;
      }
      break;
    default:
      break;
  }
}

