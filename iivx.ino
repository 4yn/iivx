/* 
 * IIVX
 *  
 * Beatmania IIDX + Sound Voltex hybrid controller
 * 9 buttons, 2 knobs + 1 turntable
 * Using Arduino Due
 * 
 */

#define REPORT_DELAY 25000 // Microseconds to wait between reports, 0 delay gives 4000 reports a second, 2000 gives 250 reports a second
#define LIGHTING_MODE 2 // 0 = Disable LED, 1 = Reactive Lighting, 2 = HID led
#define DEBUG_ENABLE 1 // 0 = Disable serial debug, 1 = Enable, 2 = Verbose Output

//Button Pins
uint8_t buttonCount = 9; //Maximum 15
uint8_t buttonPins[] = {23,25,27,29,31,33,35,37,39}; // Array of buttons
uint8_t ledPins[] = {22,24,26,28,30,32,34,36,38}; // Array of LEDs

// Encoder Pins
// All encoder pins are set on the DUE's C port
// Pin mapping is used to speed up encoder processing, especially for high-performance encoders
// Why did I get the 600 p/r ones
#define ENCODER_PINPORT PIOC
#define ENCODER_X_A 46
#define ENCODER_X_B 47
#define ENCODER_X_B_REGISTER 16
#define ENCODER_Y_A 48
#define ENCODER_Y_B 48
#define ENCODER_Y_B_REGISTER 14
#define ENCODER_Z_A 50
#define ENCODER_Z_B 49
#define ENCODER_Z_B_REGISTER 12

// Encoder Variables
volatile int16_t encX = 0, encY = 0, encZ = 0;

// System Variables 
iivxReport_t report; // Storage for button and knob states
iivxReport_t lastReport;
uint16_t hidLed; // Storage for HID led state

//Encoder Processing

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
  //if(DEBUG_ENABLE) SerialUSB.println( (int) encX);

  // Read HID Led
  hidLed = iivx.readState();
  //if(DEBUG_ENABLE) SerialUSB.println( (int) hidLed);

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

  if(report!=lastReport){
    iivx.setState(&report); // Send data only if data has changed
    lastReport = report;
  }
  
  delayMicroseconds(REPORT_DELAY); // Delay to reduce report rate
}

