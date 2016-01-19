
#include "serialcomm.h"

SerialComm s(Serial);

unsigned long previousMillis = 0;
const long interval = 1000;

void setup( ) {
  Serial.begin( 115200 );

  // Attach the action #2 to the actionB function
  s.attach( 2 , actionB );
}


void loop( ) {

	if ( Serial.available( ) ) {
		s.check_reception( );
	}

	unsigned long currentMillis = millis();

	if ( currentMillis - previousMillis >= interval ) {
		previousMillis = currentMillis;

		// Send a message with an integer
		int an_integer = 10;
		if ( s.sendMessage( 2 , true , "i" , an_integer) ) {
			// Get back the ack and extract an integer
			int another_integer;
			s.getData( "i" , &another_integer );
	  }
	}
}


void actionB ( void ) {

	// Extract the data of the incoming message
	char a_string[20] = "";
	int an_integer = 0;
	s.getData( "is" , &an_integer , &a_string , sizeof( a_string ) );

	// Send an ack, with values
	int another_integer = 30;
	char another_string[20] = "The response";
	s.sendAck( "is" , another_integer , another_string );

}

