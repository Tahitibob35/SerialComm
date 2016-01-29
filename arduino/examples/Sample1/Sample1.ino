
#include "serialcomm.h"


SerialComm s(Serial);

SoftwareSerial mySerial( 10 , 11 );

unsigned long previousMillis = 0;
const long interval = 1000;
int localcounter = 0;

void setup( ) {
  Serial.begin( 9600 );
  mySerial.begin( 9600 );
  s.debugserial = &mySerial;

  // Attach the action #2 to the actionB function
  s.attach( 2 , actionB );

  mySerial.println( "Setup !" );
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
			s.debugserial->print( "Ack contains : " );
			s.debugserial->println( another_integer );
			localcounter = another_integer;
	  }
	}
}


void actionB ( void ) {

	mySerial.println( "ActionB !" );

	// Extract the data of the incoming message
	char a_string[20] = "";
	int an_integer = 0;
	s.getData( "is" , &an_integer , &a_string , sizeof( a_string ) );
	 s.debugserial->println( a_string );
	 s.debugserial->println( an_integer );

	// Send an ack, with values
	int another_integer = 30;
	char another_string[20] = "The response";
	s.sendAck( "is" , localcounter , another_string );

}

