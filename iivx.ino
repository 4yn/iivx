/* 
 * IIVX
 *  
 * Beatmania IIDX + Sound Voltex hybrid controller
 * 9 buttons, 2 knobs + 1 turntable
 * Using Arduino Due
 * 
 */

#define REPORT_DELAY 2000 // Microseconds to wait between reports, 0 delay gives 4000 reports a second, 2000 gives 250 reports a second
#define REACTIVE_LIGHTING 1 // 0 = Disable LED, 1 = Reactive Lighting, 2 = HID led

uint8_t buttonCount = 9; //Maximum 15
uint8_t buttonPins[] = {23,25,27,29,31,33,35,37,39}; // Array of buttons
uint8_t ledPins[] = {22,24,26,28,30,32,34,36,38}; // Array of LEDs

iivxReport_t report; // Storage for button and knob states

void setup() {
  
  // Pinmode Setup
  
  for(int i=0;i<buttonCount;i++){ // Loop for each set of LED and button
    pinMode(buttonPins[i],INPUT); // Set button pin to output
    digitalWrite(buttonPins[i],HIGH); // Enable pullup resistor on button pins
    pinMode(ledPins[i],OUTPUT); // Set led pin to output
  }
  
}

void loop() {

  // Read buttons
  
  for(int i=0;i<buttonCount;i++){ // Loop for each button
    if(digitalRead(buttonPins[i])==HIGH){ //Button pressed
      if(REACTIVE_LIGHTING==1) digitalWrite(ledPins[i],HIGH); // Turn on LED on button
      report.buttons |= (uint16_t)1 << i; // Set i'th bit to 1
    } else {
      if(REACTIVE_LIGHTING==1) digitalWrite(ledPins[i],LOW); // Turn off LED on button
      report.buttons &= ~((uint16_t)1 << i); // Set i'th bit to 0;
    }
  }

  // Read Encoders

  // Wrap-up
  
  iivx.setState(&report); // Send data
  delayMicroseconds(REPORT_DELAY); // Delay to reduce report rate
}

