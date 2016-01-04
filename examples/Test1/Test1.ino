
#include "serialcomm.h"


SerialComm s(Serial);

int i = 0;

int ledstatus = HIGH;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  s.begin();
  s.attach(2, actionB);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial.available()) {
    s.check_reception();
  }

  /*unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
	  // save the last time you blinked the LED
	  previousMillis = currentMillis;


	  //delay(500);
	  if ( s.sendMessage( 2 , true , "i" , i) ) {
		  s.getData( "i" , &i );
	  }
	}*/
  //byte id;
  //s.lockMessageId( &id  );
  //Serial.println(id);


}

void actionA (void) {
  Serial.println("");
  
}

void actionB (void) {

  char z[3] = "";
  s.getData("is" , &i, &z, sizeof(z));
  
  //s.sendMessage(2, false, "i", 3);

  s.sendAck(s.getId() , "is", i, z);

}

