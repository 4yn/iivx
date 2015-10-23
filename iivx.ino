/* 
 * IIVX
 *  
 * Beatmania IIDX + Sound Voltex hybrid controller
 * 9 buttons, 2 knobs + 1 turntable
 * Using Arduino Due
 * 
 */

int REPORT_DELAY = 25000; // Microseconds to wait between reports, 0 delay gives 4000 reports a second, 2000 gives 250 reports a second
int LIGHTING_MODE = 2; // 0 = Disable LED, 1 = Reactive Lighting, 2 = HID led
int DEBUG_ENABLE = 1; // 0 = Disable serial debug, 1 = Enable, 2 = Verbose Output

uint8_t buttonCount = 9; //Maximum 15
uint8_t buttonPins[] = {23,25,27,29,31,33,35,37,39}; // Array of buttons
uint8_t ledPins[] = {22,24,26,28,30,32,34,36,38}; // Array of LEDs

iivxReport_t report; // Storage for button and knob states
uint16_t hidLed; // Storage for HID led state

void setup() {

  delay(1000);
  
  if(DEBUG_ENABLE) SerialUSB.begin(9600);
  
  // Pinmode Setup
  
  if(DEBUG_ENABLE) SerialUSB.println("Begin Setup");
  for(int i=0;i<buttonCount;i++){ // Loop for each set of LED and button
    pinMode(buttonPins[i],INPUT); // Set button pin to output
    digitalWrite(buttonPins[i],HIGH); // Enable pullup resistor on button pins
    pinMode(ledPins[i],OUTPUT); // Set led pin to output
  }
  
}

void loop() {

  if(DEBUG_ENABLE) SerialUSB.println("Begin Loop");

  // Read buttons
  
  for(int i=0;i<buttonCount;i++){ // Loop for each button
    if(digitalRead(buttonPins[i])==HIGH){ //Button pressed
      report.buttons |= (uint16_t)1 << i; // Set i'th bit to 1
    } else {
      report.buttons &= ~((uint16_t)1 << i); // Set i'th bit to 0;
    }
  }

  // Read Encoders

  // Read HID Led
  hidLed = iivx.readState();
  if(DEBUG_ENABLE) SerialUSB.println( (int) hidLed);

  // Write LED states
  if(LIGHTING_MODE){
    for(int i=0;i<buttonCount;i++){
      if(LIGHTING_MODE==1){ // Reactive Lighting
         if(bitRead(report.buttons,i)){
           digitalWrite(ledPins[i],HIGH);
         } else {
           digitalWrite(ledPins[i],LOW);
         }
      } else if (LIGHTING_MODE==2){ // HID Lighting
        if(bitRead(hidLed,i)){
           digitalWrite(ledPins[i],HIGH);
         } else {
           digitalWrite(ledPins[i],LOW);
         }
      }
    }
  }

  // Wrap-up
  
  iivx.setState(&report); // Send data
  //delayMicroseconds(REPORT_DELAY); // Delay to reduce report rate
  delay(50);
}

