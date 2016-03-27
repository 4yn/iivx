#include "iivx_leo.h"
iivxReport_t report; 

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  iivx.setState(&report);
}
