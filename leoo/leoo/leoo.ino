#include "iivx_leo.h"
iivxReport_t report; 

#define REPORT_DELAY 2000
// Number of microseconds between HID reports
// 2000 = 500hz

int tmp;
uint8_t buttonCount = 9;
uint8_t lightMode = 0;
// 0 = reactive lighting, 1 = HID lighting
uint8_t ledPins[] = {1,2,3,4,5,6,7,8,9,10};
uint8_t buttonPins[] = {11,12,13,18,19,20,21,22,23};
uint8_t sysPin = 0;
/* current pin layout
 *  pins 1 to 9 = LED 1 to 9
 *    connect pin to + termnial of LED
 *    connect ground to resistor and then - terminal of LED
 *  pins 11 to 13, A0 to A5 = Button input 1 to 9
 *    connect button pin to ground to trigger button press
 *  pins 0 = system pin
 *    connect system pin to ground 
 *      together with other buttons to change lighting scheme
 *    system button + button 1 = reactive lighting
 *    system button + button 3 = HID lighting
 */

void lights(uint16_t lightDesc){
  for(int i=0;i<buttonCount;i++){
     if((lightDesc>>i)&1){
         digitalWrite(ledPins[i],HIGH);
     } else {
         digitalWrite(ledPins[i],LOW);
     }
  }
}

void setup() {
  delay(1000);
  // Setup I/O for pins
  for(int i=0;i<buttonCount;i++){
    pinMode(buttonPins[i],INPUT_PULLUP);
    pinMode(ledPins[i],OUTPUT);
  }
  pinMode(sysPin,INPUT_PULLUP);
}

void loop() {
  // Read buttons
  for(int i=0;i<buttonCount;i++){
    if(digitalRead(buttonPins[i])!=HIGH){
      report.buttons |= (uint16_t)1 << i;
    } else {
      report.buttons &= ~((uint16_t)1 << i);
    }
  }
  // Light LEDs
  if(lightMode==0){
    lights(report.buttons);
  } else {
    lights(iivx_led);
  }
  // Detect lighting changes
  if(digitalRead(sysPin)!=HIGH){
    if(report.buttons == 1){
      lightMode = 0;
    } else if (report.buttons == 4){
      lightMode = 1;
    } else if (report.buttons == 16){
      report.buttons = 0;
      report.buttons = (uint16_t)1 << 9;
    } else if (report.buttons == 64){
      report.buttons = 0;
      report.buttons = (uint16_t)1 << 10;
    } else {
      report.buttons = 0;
    }
  } else {
    report.buttons &= ~((uint16_t)1 << 9);
    report.buttons &= ~((uint16_t)1 << 10);
}
  // Send report and delay
  iivx.setState(&report);
  delayMicroseconds(REPORT_DELAY);
}

