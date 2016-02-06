#include "Arduino.h"
#include "serialcomm.h"
#include <stdarg.h>

//#define SCDEBUG

#ifdef SCDEBUG
#define debug( X ) this->debugserial->print( X )
#define debugln( X , ... ) this->debugserial->println( X )
//#define debugln( X , Y ) this->debugserial->println( X , Y )
#include <SoftwareSerial.h>
#else
#define debug( X , ... )
#define debugln( X , ... )
#endif

SerialComm::SerialComm( Stream &s ): _serial( &s ) {
    this->_inputIndex = 0;                   // Nombre d octets recus
    this->_actioncount = 0;                  // Nombre d actions definies
    this->_checksum = 0;
    this->_readindex = 3;
#ifdef SCDEBUG
    this->debugserial = new SoftwareSerial( 10 , 11 );
    this->debugserial->begin( 9600 );
    debugln( "Debug enabled" );
#endif
}

SerialComm::SerialComm( void ) {
  this->_inputIndex = 0;                  // Nombre d octets recus
  this->_actioncount = 0;                  // Nombre d actions definies
  this->_serial = NULL;
  this->_checksum = 0;
  this->_readindex = 3;
#ifdef SCDEBUG
    this->debugserial = NULL;
#endif
}

/******************************************************
 lit les donnees du buffer serie
 *****************************************************/
bool SerialComm::_read( void ) {
	byte c;                                //Caractere recu

	static bool esc = false;               // Caractere suivant special
	static bool receptionstarted = false;

	c = this->_serial->read();
	//Serial.println(c);
	debug("<-Byte received (hex): ");
	debugln(c, HEX);
	//Serial.println(c, HEX);
	//this->serial->print(",");
	//this->serial->println(c, DEC);
	//mySerial.print(c, HEX);
	//mySerial.print(" ");

	// Check for frame start
	if ( c == START ) {                            //Debut d'un message, restauration des donnees
	  debugln("<-START");
	  //Serial.println( "<-START" );
	  receptionstarted = true;
	  this->_inputIndex = 0;
	  esc = false;
	}
	else {
	  if ( receptionstarted ) {
		if ( c == END ) {                          //Fin d'un message
		    debugln("<-END");
		    //Serial.println("<-END");
		    receptionstarted = false;
		    //mySerial.println("");
		    this->_readindex = 3;
		    return true;
		}
		else {
		  if ( c == ESC ) {                        //Detection du caractere d echappement
			esc = true;
		  }
		  else {
			if ( esc == true ) {             //Traitement du caractere echappe
			  if ( c == TSTART ) c = START;        //Conversion du caractere echappe
			  if ( c == TEND ) c = END;
			  if ( c == TESC ) c = ESC;
			  esc = false;
			}
			this->_addCharInInputMessage( c );            //Ajout d'octet converti au message
		  }
		}
	  }
	}
	return false;
}


void SerialComm::check_reception( void ) {
  
  while ( this->_serial->available( ) ) {
    if ( this->_read( ) ) {
		if ( this->_inputMessageValidateChecksum( ) ) {
		    debugln( "CRC OK" );
            //Serial.println( "CRC OK" );
			this->_processMessage( );
			this->_inputIndex = 0;                 //Restauration des parametres par defaut
		}
		else {
			debug( "Bad CRC" );
            //Serial.println( "Bad CRC" );
		}
    }
  }
}

/******************************************************
  Attend un accuse
 *****************************************************/
bool SerialComm::_waitAck( byte id ) {

    unsigned long now = millis( );
    while ( ( millis() - now ) < ACKTIMEOUT ) {
        while ( this->_serial->available() ) {
            if ( this->_read( ) ) {
                if ( this->_inputMessageValidateChecksum( ) ) {
                    debugln( "CRC OK" );
                    //Serial.println( "CRC OK" );
                    if ( this->_inputMessageGetAction( ) != 0 ) {    // Un message
                        this->_processMessage( );
                        this->_inputIndex = 0;                      //Restauration des parametres par defaut
                    }
                    else {                                          // Un ack
                        if ( this->_inputMessageGetId( ) == id ) {// Ack attendu
                            return true;
                        }
                    }
                }
                else {
                    debugln( "Bad CRC" );
                    //Serial.println( "Bad CRC" );
                }
            }
        }
    }
	return false;
}


/******************************************************
 Ajout d'un caractere au message
 *****************************************************/
void SerialComm::_addCharInInputMessage( char c ) {
    if ( this->_inputIndex < INPUTMSGLEN )
        this->_inputMessage[this->_inputIndex++] = c;
}


/******************************************************
 Calcul du checksum
 *****************************************************/
void SerialComm::_checkSum( byte  *checksum , byte data ) {

  *checksum ^= data;
  //this->serial->print("Local checksum : ");
  //this->serial->println(checksum, HEX);
  //return checksum;

}

/******************************************************
 Traitement du message
 *****************************************************/
bool SerialComm::_processMessage( void ) {

    debug( "_processMessage : actionid : " );
    debugln( this->_inputMessageGetAction( ) );

    if ( this->_inputMessageGetAction( ) >= 100 ) {
        debugln( "_processMessage : user defined action" );
        for( int i=0 ; i < this->_actioncount ; i++ ) {
            //Serial.print("Looking for an action... : ");
            //Serial.println(this->_inputMessageGetAction( ));
            if ( this->_inputMessageGetAction( ) == this->_commands[i] ) {
                //Serial.println("Action found !");
                ( *this->_actions[i] )( );
                break;
            }
        }
        return true;
    }
    else {                       // Internal action
        debugln( "_processMessage : internal action" );
        this->_readindex = 3;
        switch ( this->_inputMessageGetAction( ) ) {
        case A_DIGITALWRITE:
        {
            uint8_t pin = this->getInt();
            int value = this->getInt();
            debugln( "_processMessage : A_DIGITALWRITE" );
            digitalWrite( pin , value );
            break;
        }
        case A_ANALOGWRITE:
        {
            uint8_t pin = this->getInt();
            int value = this->getInt();
            debugln( "_processMessage : A_ANALOGWRITE" );
            analogWrite( pin , value );
            break;
        }
        case A_DIGITALREAD:
        {
            uint8_t pin = this->getInt();
            debugln( "_processMessage : A_DIGITALREAD" );
            this->sendAck( "i" , digitalRead( pin ) );
            break;
        }
        case A_ANALOGREAD:
        {
            uint8_t pin = this->getInt();
            debugln( "_processMessage : A_ANALOGWRITE" );
            this->sendAck( "i" , analogRead( pin ) );
            break;
        }

        }
    }


    return true;

}


/******************************************************
 Verifie le checksum du message entrant
 *****************************************************/
bool SerialComm::_inputMessageValidateChecksum( void ) {

	// Verification du checksum
	//this->serial->print("Message checksum : ");
	//this->serial->println(this->inputMessage[0], HEX);

	byte checksum = 0;
	for ( int i=0 ; i < ( this->_inputIndex - 1 ) ; i++) {
		this->_checkSum( &checksum , this->_inputMessage[i] );
	}

	if ( this->_inputMessage[this->_inputIndex - 1] != checksum ) {
		//Serial.println("Invalid checksum");
		return false;                                           // Retour en erreur
	}
	//this->serial->println("V");
	return true;
}


/******************************************************
 Retourne l'action d'un message entrant
 *****************************************************/
byte SerialComm::_inputMessageGetAction( void ) {
	return this->_inputMessage[2];
}

/******************************************************
 Ajout d une action
 *****************************************************/
bool SerialComm::attach( int command , void ( *ptrfonction )( void )) {
	if ( this->_actioncount < ACTIONSLEN ) {
		this->_actions[this->_actioncount] = ptrfonction;
		this->_commands[this->_actioncount] = command;
		this->_actioncount++;
		//mySerial.println("attach");
		return true;
	}
	return false;
}


/******************************************************
 Retourne l id du message
 *****************************************************/
int SerialComm::_inputMessageGetId( void ) {
  return _inputMessage[1];
}


/******************************************************
 Ecrit un octet en l echappant si necessaire
 *****************************************************/
bool SerialComm::_safeWrite( byte octet , bool checksum ) {
    if ( checksum ) {
        this->_checksum ^= octet;
    }

    switch ( octet ) {
    case START:
        this->_serial->write( ESC );
        this->_serial->write( TSTART );
        //mySerial.print(ESC, HEX);
        //mySerial.print(TSTART, HEX);
        break;
    case END:
        this->_serial->write( ESC );
        this->_serial->write( TEND );
        //mySerial.print(ESC, HEX);
        //mySerial.print(TEND, HEX);
        break;
    case ESC:
        this->_serial->write( ESC );
        this->_serial->write( TESC );
        //mySerial.print(ESC, HEX);
        //mySerial.print(TESC, HEX);
        break;
    default:
        this->_serial->write( octet );
        //mySerial.print(octet, HEX);
        break;
    }
    return true;
}





/******************************************************
Envoi le message
******************************************************/
bool SerialComm::sendMessage( byte action , bool ack , const char * fmt , ... ) {

    debugln( "-> sendMessage - enter" );

	va_list args;
	va_start( args , fmt );

	if ( !ack ) {                                          // Pas d'accuse demande
		return this->_sendMessage( action , 0 , fmt  , args );
	}

	byte id;

	id = this->_getNewMessageId( );

	if ( !this->_sendMessage( action , id  , fmt  , args ) ) {
		return false;                                    // erreur a l'envoi du message
	}

	// ajouter traitement de l ack
    if ( !this->_waitAck( id ) )
    	return false;

   return true;
}


/******************************************************
Envoi le message
******************************************************/
bool SerialComm::sendMessage( byte action , bool ack ) {

	return this->sendMessage( action , ack , "" );
}


/******************************************************
Retourne un nouvel id de message
******************************************************/
int SerialComm::_getNewMessageId( void  ) {
	static unsigned int id = 0;
	return id++;
}


/******************************************************
Envoi un message
******************************************************/
bool SerialComm::_sendMessage( byte action , byte id , const char *fmt , va_list args ) {

	this->_sendHeader( id, action );

	while ( *fmt != '\0' ) {
		if ( *fmt == 'i' ) {
			int i = va_arg( args , int );
			this->sendInteger( i );
		}
		else if ( *fmt == 's' ) {
			char *s = va_arg( args , char * );
			this->sendcharArray( s );
		}
		fmt++;
	}

	this->sendFooter( );

	return true;
}


/******************************************************
Envoi un accuse avec des donnes
******************************************************/
bool SerialComm::sendAck( const char *fmt , ... ) {
	va_list args;
	va_start( args , fmt );
	return this->_sendMessage( 0 , this->_inputMessageGetId( ) , fmt  , args );
}


/******************************************************
Envoi un accuse sans donnees
******************************************************/
bool SerialComm::sendAck( void ) {
	return this->sendAck( 0 , this->_inputMessageGetId( ) , "" , 0 );

}


/******************************************************
Retourne les donnees d un message entrant
******************************************************/
bool SerialComm::getData( const char * fmt , ... ) {
	va_list args;
	va_start( args, fmt );

	this->_readindex = 3;

	while (*fmt != '\0') {
		if ( this->_readindex >= ( this->_inputIndex - 1 ) ) return false;            // Verification de fin de message
		switch ( *fmt ) {
		case 'i' :
			{
				int * i = va_arg( args , int * );
				*i = this->getInt();
				this->_readindex += 2;
				break;
			}
		case 's' :
			{
				char *s = va_arg( args, char * );
				int slen = va_arg( args , int );

				char c = 0;
				int j = 0;
				do {
					c = _inputMessage[this->_readindex++];
					s[j++] = c;
				} while ( ( c != 0 ) && ( j < slen ) );
				s[slen-1] = 0;
				Serial.print( "A str : " );
				Serial.println( s );
				break;
			}
		default:
			return false;
		}
		++fmt;
	}

	va_end( args );

	return true;
}


/******************************************************
Envoi l entete d un message avec id
******************************************************/
void SerialComm::_sendHeader( byte id, byte action ) {

    debugln( "-> Send Header" );
    this->_checksum = 0;
    this->_serial->write( START );
    this->_safeWrite( 0 , true );            // Free byte
    this->_safeWrite( id , true );
    this->_safeWrite( action , true );

}


/******************************************************
Envoi l entete d un messagesans id
******************************************************/
void SerialComm::sendHeader( byte action ) {

    this->_sendHeader( 0, action );

}


/******************************************************
Envoi la fin d un message
******************************************************/
void SerialComm::sendFooter( void ) {

    debugln( "-> Send footer" );
    this->_safeWrite( this->_checksum , false);
    this->_serial->write( END );
    debugln( "-> END" );

}


/******************************************************
Envoi un entier
******************************************************/
void SerialComm::sendInteger( int  value ) {

    debugln( "-> Send an integer" );
    this->_safeWrite( value >> 8 , true );
    this->_safeWrite( value & 0xFF , true );

}

/******************************************************
Envoi un entier
******************************************************/
void SerialComm::sendcharArray( char * string ) {

    debugln( "-> Send a string" );
    while ( *string != '\0' ) {
        this->_safeWrite( *string , true );
        string++;
    }
    this->_safeWrite( 0x00 , true );

}


/******************************************************
Lit un entier dans le message
******************************************************/
int SerialComm::getInt( void ) {
    int i = word( _inputMessage[this->_readindex], _inputMessage[this->_readindex + 1] ); // Recompose l entier en lisant 2 octets
    this->_readindex += 2;
    return i;
}


/******************************************************
Lit une chaine dans le message
******************************************************/
void SerialComm::getString( char *buf , int maxsize ) {
    char c = 0;
    int j = 0;
    do {
        c = _inputMessage[this->_readindex++];
        buf[j++] = c;
    } while ( ( c != 0 ) && ( j < maxsize ) );
    buf[maxsize-1] = 0;
    Serial.print( "A str : " );
    Serial.println( buf );
}


/******************************************************
Ecrit sur une sortie numerique
******************************************************/
void SerialComm::sendDigitalWrite( uint8_t pin , uint8_t value) {
    digitalWrite( pin , value);
}


/******************************************************
Ecrit sur une sortie analogique
******************************************************/
void SerialComm::sendAnalogWrite( uint8_t pin , int value) {
    analogWrite( pin , value);
}


/******************************************************
Lit sur une sortie numerique
******************************************************/
uint8_t SerialComm::sendDigitalRead( uint8_t pin , int value) {
    return digitalRead( pin );
}

