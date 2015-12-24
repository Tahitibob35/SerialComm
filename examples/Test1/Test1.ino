
#include "serialcomm.h"


SerialComm s(Serial);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  s.begin();
  s.attach(2, actionB);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial.available()) {
    s.check_reception();
  }

  

  //s.sendAck2(2, "is", 1, "coucou");
  //delay(5000);
}

void actionA (void) {
  Serial.println("");
  
}

void actionB (void) {
  int i = 0;
  int j = 0;

  char z[10] = "";
  s.getData("i" , &i);
  
//s.sendAck(s.getId() , "i", i);

}

