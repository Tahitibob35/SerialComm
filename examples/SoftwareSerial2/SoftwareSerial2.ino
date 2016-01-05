#include <SoftwareSerial.h>
#include "serialcomm.h"

SoftwareSerial mySerial(10, 11);

SerialComm s(mySerial);

unsigned long previousMillis = 0;
const long interval = 50;

char myname[] = "nano2";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  s.begin();

  // Attach the action #2 to the actionB function
  s.attach(2, actionB);
}


void loop() {

	if (mySerial.available()) {
		s.check_reception();
	}

	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		if ( s.sendMessage( 2 , false , "s" , myname) ) {
			// Get back the ack and extract an integer
			//int another_integer;
			//s.getData( "i" , &another_integer );
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

}


