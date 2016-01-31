
#include "serialcomm.h"

SerialComm s(Serial);

unsigned long previousMillis = 0;
const long interval = 1000;
int localcounter = 0;

void setup( ) {
    Serial.begin( 9600 );

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
        char string[] = "From arduino";
        if ( s.sendMessage( 2 , true , "is" , an_integer , string ) ) {
            // Get back the ack and extract an integer
            int another_integer;
            s.getData( "i" , &another_integer );
            localcounter = another_integer;
        }
    }
}

void actionB ( void ) {

	// Extract the data of the incoming message
	char a_string[20] = "";
	int an_integer = 0;
	s.getData( "is" , &an_integer , &a_string , sizeof( a_string ) );

	// Send an ack, with values
	char another_string[20] = "The response";
	s.sendAck( "is" , localcounter , another_string );

}

