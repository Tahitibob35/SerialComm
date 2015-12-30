
#include "serialcomm.h"


SerialComm s(Serial);

int i = 0;

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


  
  //delay(1000);
  //s.sendMessage(2, false, "is", i++, "from Arduino");
}

void actionA (void) {
  Serial.println("");
  
}

void actionB (void) {
  int i = 0;
  int j = 0;

  char z[10] = "";
  s.getData("is" , &i, &z);
  
  //s.sendMessage(2, false, "i", 3);
  delay(5000);
  //s.sendAck(s.getId() , "is", i, z);
  s.sendAck(6 , "is", i, z);

}

