/* 
 *  IIVX
 *  
 * Beatmania IIDX + Sound Voltex hybrid controller
 * 9 buttons, 2 knobs + 1 turntable
 * Using Arduino Due
 * 
 */

//iivxReport_t report;
JoyState_t report;

void setup() {
  Serial.begin(9600);
  Serial.println("Begin Setup");
  for(int i=22;i<40;i+=2){
    pinMode(i,OUTPUT);
    pinMode(i+1,INPUT);
    digitalWrite(i+1,HIGH);
  }
  Serial.println("Begin HID");
}

void loop() {
  // put your main code here, to run repeatedly
  Serial.println("Loop");
  for(int i=22;i<40;i+=2){
    if(digitalRead(i+1)==HIGH){
       digitalWrite(i,HIGH);
       //Keyboard.press(keys[i/2-11]);
       if(i==22){
        //iivx.buttons((uint16_t)3);
        report.buttons = 1;
       }
    } else {
      digitalWrite(i,LOW);
      //Keyboard.release(keys[i/2-11]);
      if(i==22){
        //iivx.buttons((uint16_t)3);
        report.buttons = 0;
      }
    }
  }
  //iivx.setState(&report);
  Joystick.setState(&report);
}

