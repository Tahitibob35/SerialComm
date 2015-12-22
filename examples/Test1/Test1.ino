
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

  //s.prepareInt(77);
  //s.prepareStr("cou", 6);
  //s.prepareInt(77);
  //s.prepareStr("cou", 6);
  //s.sendMessage(2 , false);
  //Serial.println("");
  

  //s.sendAck2(2, "is", 1, "coucou");
  //delay(5000);
}

void actionA (void) {
  Serial.println(s.getId());
  s.prepareInt(77);
  s.sendAck(7);
  s.prepareInt(35);
  s.sendAck(s.getId());
  Serial.println("");
  
}

void actionB (void) {
  int i = 0;
  //s.prepareStr("david", 5);
  //s.prepareInt(i);
  //s.prepareStr("pyhtonn", 7);
  //s.readInt(&i);
  //s.prepareInt(i);

  //s.prepareInt(s.getId());

  //s.sendAck(s.getId());
  
  s.sendAck2(s.getId(), "is" , 7 , "David");

}

