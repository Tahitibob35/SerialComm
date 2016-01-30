#include "Arduino.h"
#include "serialcomm.h"
#include <stdarg.h>

#define SCDEBUG

#ifdef SCDEBUG
#define debug( X ) this->debugserial->print( X )
#define debugln( X , ... ) this->debugserial->println( X )
//#define debugln( X , Y ) this->debugserial->println( X , Y )
#include <SoftwareSerial.h>
#else
#define debug( X , ... )
#define debugln( X , ... )
#endif

#include <SoftwareSerial.h>


SerialComm::SerialComm( Stream &s ): _serial( &s ) {
    this->_inputIndex = 0;                   // Nombre d octets recus
    this->_actioncount = 0;                  // Nombre d actions definies
    this->_checksum = 0;
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
	//this->serial->print(",");
	//this->serial->println(c, DEC);
	//mySerial.print(c, HEX);
	//mySerial.print(" ");

	// Check for frame start
	if ( c == START ) {                            //Debut d'un message, restauration des donnees
	  debugln("<-START");
	  receptionstarted = true;
	  this->_inputIndex = 0;
	  esc = false;
	}
	else {
	  if ( receptionstarted ) {
		if ( c == END ) {                          //Fin d'un message
		    debugln("<-END");
		    receptionstarted = false;
		    //mySerial.println("");
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
			this->_processMessage( );
			this->_inputIndex = 0;                 //Restauration des parametres par defaut
		}
		else {
			debug( "Bad CRC" );
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
		//this->serial->println("Invalid checksum");
		return false;                                           // Retour en erreur
	}
	//this->serial->println("V");
	return true;
}


/******************************************************
 Retourne l'action d'un message entrant
 *****************************************************/
byte SerialComm::_inputMessageGetAction( void ) {
	return this->_inputMessage[1];
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
  return _inputMessage[0];
}


/******************************************************
 Ecrit un octet en l echappant si necessaire
 *****************************************************/
bool SerialComm::_safeWrite( byte octet , bool checksum ) {
    if ( checksum ) {
        this->_checksum ^= this->_checksum;
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

	this->_checksum = 0;

	this->sendHeader( id, action );

	while ( *fmt != '\0' ) {
		if ( *fmt == 'i' ) {
			int i = va_arg( args , int );
			this->_safeWrite( i >> 8 , true );
			this->_safeWrite( i & 0xFF , true );
		}
		else if ( *fmt == 's' ) {
			char *s = va_arg( args , char * );
			while ( *s != '\0' ) {
				this->_safeWrite( *s , true );
				s++;
			}
			this->_safeWrite( 0x00 , true );
		}
		fmt++;
	}

	this->sendFooter( this->_checksum );

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

	int readindex = 2;

	while (*fmt != '\0') {
		if ( readindex >= ( this->_inputIndex - 1 ) ) return false;            // Verification de fin de message
		switch ( *fmt ) {
		case 'i' :
			{
				int * i = va_arg( args , int * );
				*i = word( _inputMessage[readindex], _inputMessage[readindex + 1] ); // Recompose l entier en lisant 2 octets
				readindex += 2;
				break;
			}
		case 's' :
			{
				char *s = va_arg( args, char * );
				int slen = va_arg( args , int );

				char c = 0;
				int j = 0;
				do {
					c = _inputMessage[readindex++];
					s[j++] = c;
				} while ( ( c != 0 ) && ( j < slen ) );
				s[slen-1] = 0;
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
Envoi l entete d un message
******************************************************/
void SerialComm::sendHeader( byte id, byte action ) {

    debugln( "-> Send Header" );
    this->_serial->write( START );
    this->_safeWrite( 0 , true );            // Free byte
    this->_safeWrite( id , true );
    this->_safeWrite( action , true );

}


/******************************************************
Envoi la fin d un message
******************************************************/
void SerialComm::sendFooter( byte checksum ) {

    debugln( "-> Send footer" );
    this->_safeWrite( this->_checksum , false);
    this->_serial->write( END );
    debugln( "-> END" );

}
