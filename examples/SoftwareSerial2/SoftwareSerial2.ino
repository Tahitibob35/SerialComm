/************************************************************
 * Sample code with SoftwareSerial library.
 *
 ***********************************************************/

#include <SoftwareSerial.h>
#include "serialcomm.h"

SoftwareSerial mySerial(10, 11);

SerialComm s(mySerial);

unsigned long previousMillis = 0;
const long interval = 1000;

char myname[] = "nano2";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  s.begin();

  // Attach the action #2 to the actionB function
  s.attach(2, actionB);
  Serial.println("Ready to receive....");
}


void loop() {

	if (mySerial.available()) {
		s.check_reception();
	}

	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		if ( s.sendMessage( 2 , true , "s" , myname) ) {
			char a_string[20] = "";
			s.getData( "s" , &a_string, sizeof(a_string));
			Serial.print("I'm ");
			Serial.print(myname);
			Serial.print(" and I sent a message to ");
			Serial.println(a_string);
	  }
	}
}


void actionB (void) {

	// Extract the data of the incoming message
	char a_string[20] = "";
	s.getData("s", &a_string, sizeof(a_string));

	Serial.print("I'm ");
	Serial.print(myname);
	Serial.print(" and I received a message from ");
	Serial.println(a_string);

	s.sendAck(s.getId() , "s", myname);

}


