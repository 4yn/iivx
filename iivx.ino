/* 
 * IIVX
 *  
 * Beatmania IIDX + Sound Voltex hybrid controller
 * 9 buttons, 2 knobs, 1 turntable (forward + backward movement mapped to buttons 10 and 11)
 * Using Arduino Due
 * 
 * Use "Slider" mode in KSM for knobs to work
 * 
 */

// Microseconds to wait between loops: 0 delay gives 4000 reports a second, 2000 gives 250 reports a second
#define REPORT_DELAY 2000 
// Toggle debug mode: 0 = Disable serial debug, 1 = Enable, 2 = Verbose Output
#define DEBUG_ENABLE 0

// Button Pins
uint8_t buttonCount = 9; //Maximum 15
uint8_t buttonPins[] = {23,25,27,29,31,33,35,37,39}; // Array of buttons
uint8_t ledPins[] = {22,24,26,28,30,32,34,36,38}; // Array of LEDs

// Encoder Pins
// All encoder pins are set on the DUE's C port
// Pin mapping is used to speed up encoder processing, especially for high-performance encoders
// Why did I get the 600 p/r ones
#define ENCODER_SENSITIVITY (double) 2.34375 // I'm using 600 ppr encoders at 1x resolution while HID reports 256 data points per rotation, 600/256 = 2.34375
#define ENCODER_PINPORT PIOC
#define ENCODER_X_A 46
#define ENCODER_X_B 47
#define ENCODER_X_B_REGISTER 16
#define ENCODER_Y_A 48
#define ENCODER_Y_B 49
#define ENCODER_Y_B_REGISTER 14
#define ENCODER_Z_A 50
#define ENCODER_Z_B 51
#define ENCODER_Z_B_REGISTER 12
#define ENCODER_Z_MILLIS_TOLERANCE 100 // Amount of miliseconds to wait and change state of turntable buttons

// System Variables
uint8_t lighting = 0; // Storage for current lighting mode: 0: Lights off, 1: Reactive Lights, 2: HID Lights Press buttons 2, 6, 8 and 9 to enable set, then press 1 for off, 3 for reactive and 5 for HID lights
iivxReport_t report; // Storage for button and knob states
uint16_t hidLed; // Storage for HID led state
volatile int32_t encX = 0, encY = 0, encZ = 0, encZlast = 0, encZmillis = 0; // Storage for encoder states

// Encoder Processing
// Using direct port reading due to large overhead of digitalRead, speeds up especially in case of high resolution encoders
void encXProc(){
  if(bitRead(ENCODER_PINPORT -> PIO_PDSR,ENCODER_X_B_REGISTER)){
    encX -= 1;
  } else {
    encX += 1;
  }
}
void encYProc(){
  if(bitRead(ENCODER_PINPORT -> PIO_PDSR,ENCODER_Y_B_REGISTER)){
    encY -= 1;
  } else {
    encY += 1;
  }
}
void encZProc(){
  if(bitRead(ENCODER_PINPORT -> PIO_PDSR,ENCODER_Z_B_REGISTER)){
    encZ -= 1;
  } else {
    encZ += 1;
  }
}

// Lighting Processing
void lights(uint16_t l){
  for(int i=0;i<buttonCount;i++){
     if(bitRead(l,i)){
         digitalWrite(ledPins[i],HIGH);
     } else {
         digitalWrite(ledPins[i],LOW);
     }
   }
}

void setup() {

  delay(1000);
  
  if(DEBUG_ENABLE) SerialUSB.begin(9600);
  
  // Pinmode Setup
  
  if(DEBUG_ENABLE) SerialUSB.println("Begin Setup");

  // Button Pinmode
  for(int i=0;i<buttonCount;i++){ // Loop for each set of LED and button
    pinMode(buttonPins[i],INPUT); // Set button pin to output
    digitalWrite(buttonPins[i],HIGH); // Enable pullup resistor on button pins
    pinMode(ledPins[i],OUTPUT); // Set led pin to output
  }

  // Encoder Pinmode
  pinMode(ENCODER_X_A,INPUT);
  pinMode(ENCODER_X_B,INPUT);
  pinMode(ENCODER_Y_A,INPUT);
  pinMode(ENCODER_Y_B,INPUT);
  pinMode(ENCODER_Z_A,INPUT);
  pinMode(ENCODER_Z_B,INPUT);
  // Encoder Pullup
  digitalWrite(ENCODER_X_A,HIGH);
  digitalWrite(ENCODER_X_B,HIGH);
  digitalWrite(ENCODER_Y_A,HIGH);
  digitalWrite(ENCODER_Y_B,HIGH);
  digitalWrite(ENCODER_Z_A,HIGH);
  digitalWrite(ENCODER_Z_B,HIGH);

  // Encoder Interrupt Setup

  attachInterrupt(ENCODER_X_A,encXProc,RISING);
  attachInterrupt(ENCODER_Y_A,encYProc,RISING);
  attachInterrupt(ENCODER_Z_A,encZProc,RISING);

  // Reset Lights
  lights(0);
  
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
  report.xAxis = (uint8_t)((int32_t)(encX / ENCODER_SENSITIVITY) % 256);
  report.yAxis = (uint8_t)((int32_t)(encY / ENCODER_SENSITIVITY) % 256);
  report.zAxis = (uint8_t)((int32_t)(encZ / ENCODER_SENSITIVITY) % 256);

  // Read turntable buttons
  if( (int32_t)(encZ / ENCODER_SENSITIVITY) - encZlast > 5) {
    if(millis() - encZmillis > ENCODER_Z_MILLIS_TOLERANCE || bitRead(report.buttons,9)) {
      report.buttons |= (uint16_t)1 << 9;
      report.buttons &= ~((uint16_t)1 << 10);
      encZlast = (encZ / ENCODER_SENSITIVITY);
      encZmillis = millis();
    }
  } else if ( (int32_t)(encZ / ENCODER_SENSITIVITY) - encZlast < -5){
    if(millis() - encZmillis > ENCODER_Z_MILLIS_TOLERANCE || bitRead(report.buttons,10)) {
      report.buttons |= (uint16_t)1 << 10;
      report.buttons &= ~((uint16_t)1 << 9);
      encZlast = (encZ / ENCODER_SENSITIVITY);
      encZmillis = millis();
    }
  } else {
    if(millis() - encZmillis > ENCODER_Z_MILLIS_TOLERANCE) {
      report.buttons &= ~((uint16_t)1 << 9);
      report.buttons &= ~((uint16_t)1 << 10);
    }
  }
  
  if(DEBUG_ENABLE) SerialUSB.println(encZlast);
  if(DEBUG_ENABLE) SerialUSB.println(report.xAxis);

  // Write LED states
  if(lighting){
    if(lighting==1){
      lights(report.buttons);
    } else if (lighting==2){
      hidLed = iivx.readState();
      lights(hidLed);
    }
  }

  // Check if setting lighting mode
  if(report.buttons==419){
    lighting = 0;
    lights(0);
  } else if (report.buttons==422){
    lighting =  1;
  } else if (report.buttons==434){
    lighting = 2;
  }

  // Wrap-up

  iivx.setState(&report);
  
  delayMicroseconds(REPORT_DELAY); // Delay to reduce report rate
}

