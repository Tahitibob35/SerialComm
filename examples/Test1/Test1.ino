
#include "serialcomm.h"


SerialComm s(Serial);

int i = 0;

int ledstatus = HIGH;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  s.begin();
  s.attach(2, actionB);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial.available()) {
    s.check_reception();
  }


  
  delay(1000);
  if ( s.sendMessage( 2 , true , "is" , i , "from Arduino" ) ) {


	  s.getData( "i" , &i );
  }
  //byte id;
  //s.lockMessageId( &id  );
  //Serial.println(id);


}

void actionA (void) {
  Serial.println("");
  
}

void actionB (void) {

  int j = 0;

  char z[10] = "ack ard";
  s.getData("is" , &i, &z);
  
  //s.sendMessage(2, false, "i", 3);

  s.sendAck(s.getId() , "is", i, "i set");

}

