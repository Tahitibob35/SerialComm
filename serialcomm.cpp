
#include "Arduino.h"
#include "serialcomm.h"
#include <stdarg.h>

SerialComm::SerialComm(HardwareSerial& s): serial(&s) {
  this->intputIndex = 0;                  // Nombre d octets recus
  this->actioncount = 0;                  // Nombre d actions definies
}


void SerialComm::begin(void) {
  return;  
}

/******************************************************
 lit les donnees du buffer serie
 *****************************************************/
bool SerialComm::_read(void) {
	byte c;                                //Caractere recu

	static bool esc = false;               // Caractere suivant special
	static bool receptionstarted = false;

	c = this->serial->read();
	//this->serial->print("Byte received (hex): ");
	//this->serial->println(c, HEX);
	//this->serial->print(",");
	//this->serial->println(c, DEC);

	// Check for frame start
	if (c == START) {                            //Debut d'un message, restauration des donnees
	  //this->serial->println("START");
	  receptionstarted = true;
	  this->intputIndex = 0;
	  esc = false;
	}
	else {
	  if (receptionstarted) {
		if (c == END) {                          //Fin d'un message
		  //this->serial->println("END");
		  receptionstarted = false;
		  return true;;
		}
		else {
		  if (c == ESC) {                        //Detection du caractere d echappement
			esc = true;
		  }
		  else {
			if (esc == true) {             //Traitement du caractere echappe
			  if (c == TSTART) c = START;        //Conversion du caractere echappe
			  if (c == TEND) c = END;
			  if (c == TESC) c = ESC;
			  esc = false;
			}
			addCharInInputMessage(c);            //Ajout d'octetconverti au message
		  }
		}
	  }
	}
	return false;
}


void SerialComm::check_reception(void) {
  
  while ( this->serial->available() ) {
    if ( this->_read() ) {
		if ( this->inputMessageValidateChecksum( ) ) {
			//this->serial->print("Z");
			this->ProcessMessage();
			this->intputIndex = 0;                 //Restauration des parametres par defaut
		}
    }
  }
}

/******************************************************
  Attend un accuse
 *****************************************************/
bool SerialComm::waitAck( byte id ) {

	unsigned long now = millis( );
	while ( ( millis() - now ) < ACKTIMEOUT ) {
		while ( this->serial->available() ) {
			if ( this->_read() ) {
				if ( this->inputMessageValidateChecksum( ) ) {
					if ( this->inputMessageGetAction( ) != 0 ) {    // Un message
						this->ProcessMessage();
						this->intputIndex = 0;                      //Restauration des parametres par defaut
					}
					else {                                          // Un ack
						if ( this->inputMessageGetId( ) == id ) {// Ack attendu
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


/******************************************************
 Ajout d'un caractere au message
 *****************************************************/
void SerialComm::addCharInInputMessage( char c ) {
  if (this->intputIndex < INPUTMSGLEN) 
    this->inputMessage[this->intputIndex++] = c;
}


/******************************************************
 Calcul du checksum
 *****************************************************/
void SerialComm::_checkSum( byte  * checksum , byte data) {

  * checksum =  * checksum ^ data;
  //this->serial->print("Local checksum : ");
  //this->serial->println(checksum, HEX);
  //return checksum;

}

/******************************************************
 Traitement du message
 *****************************************************/
bool SerialComm::ProcessMessage( void ) {

	for(int i=0; i < this->actioncount; i++) {
		if (this->inputMessageGetAction( ) == this->commands[i]) {
			(*this->actions[i])();
			break;
		}
	}
	return true;

}


/******************************************************
 Verifie le checksum du message entrant
 *****************************************************/
bool SerialComm::inputMessageValidateChecksum( void ) {

	// Verification du checksum
	//this->serial->print("Message checksum : ");
	//this->serial->println(this->inputMessage[0], HEX);

	byte checksum = 0;
	for ( int i=1 ; i < this->intputIndex ; i++) {
		this->_checkSum( &checksum , this->inputMessage[i] );
	}

	if ( this->inputMessage[0] != checksum ) {
		//this->serial->println("Invalid checksum");
		return false;                                           // Retour en erreur
	}
	//this->serial->println("V");
	return true;
}

/******************************************************
 Retourne l'id d'un message entrant
 *****************************************************/
byte SerialComm::inputMessageGetId( void ) {
	return this->inputMessage[1];
}

/******************************************************
 Retourne l'action d'un message entrant
 *****************************************************/
byte SerialComm::inputMessageGetAction( void ) {
	return this->inputMessage[2];
}

/******************************************************
 Ajout d une action
 *****************************************************/
bool SerialComm::attach(int command, void (*ptrfonction)(void)) {
  if (actioncount < ACTIONSLEN) {
    actions[actioncount] = ptrfonction;
    commands[actioncount] = command;
    actioncount++;
    return true;
  }
  return false;
}


/******************************************************
 Retourne l id du message
 *****************************************************/
int SerialComm::getId( void ) {  
  return inputMessage[1];
}


/******************************************************
 Ecrit un octet en l echappant si necessaire
 *****************************************************/
bool SerialComm::safeWrite(byte octet) {
  switch (octet) {
    case START:
      this->serial->write(ESC);
      this->serial->write(TSTART);
      break;
    case END:
      this->serial->write(ESC);
      this->serial->write(TEND);
      break;
    case ESC:
      this->serial->write(ESC);
      this->serial->write(TESC);
      break;
    default:
      this->serial->write(octet);
      break;
  }      
  return true;
}





/******************************************************
Envoi le message
******************************************************/
bool SerialComm::sendMessage( byte action , bool ack , const char * fmt , ... ) {

	va_list args;
	va_start(args, fmt);

	if (!ack) {                                          // Pas d'accuse demande
		return this->_sendMessage( action , 0 , fmt  , args );
	}

	byte id;

	id = this->getNewMessageId();

	if (!this->_sendMessage( action , id  , fmt  , args )) {
		return false;                                    // erreur a l'envoi du message
	}

	// ajouter traitement de l ack
    if (!this->waitAck( id ))
    	return false;

   return true;
}


/******************************************************
Retourne un nouvel id de message
******************************************************/
int SerialComm::getNewMessageId( void  ) {
	static unsigned int id = 0;
	return id++;
}


/******************************************************
Envoi un message
******************************************************/
bool SerialComm::_sendMessage( byte action , byte id , const char * fmt , va_list args) {

	byte checksum = 0;
	const char * tmpfmt = fmt;

	this->_checkSum( &checksum , action );                   // Checksum de l'action
	this->_checkSum( &checksum , id );                       // Checksum de l'id

	va_list args2;
    va_copy(args2,args);

	while ( *fmt != '\0' ) {
		if ( *fmt == 'i' ) {
			int i = va_arg( args , int );
			this->_checkSum( &checksum , i >> 8 );
			this->_checkSum( &checksum , i & 0xFF );
		}
		else if ( *fmt == 's' ) {
			char * s = va_arg( args , char * );
			while ( *s != '\0' ) {
				this->_checkSum( &checksum , *s );
				s++;
			}
		}
		fmt++;
	}

	this->serial->write(START);
	this->safeWrite(checksum);
	this->safeWrite(action);
	this->safeWrite(id);

	fmt = tmpfmt;



	while ( *fmt != '\0' ) {
		if ( *fmt == 'i' ) {
			int i = va_arg( args2 , int );
			this->safeWrite(i >> 8);
			this->safeWrite(i & 0xFF);
		}
		else if ( *fmt == 's' ) {
			char * s = va_arg( args2 , char * );
			while ( *s != '\0' ) {
				this->safeWrite( *s );
				s++;
			}
			this->safeWrite( 0x00 );
		}
		fmt++;
	}

	this->serial->write(END);


	return true;
}


/******************************************************
Envoi un accuse avec des donnes
******************************************************/
bool SerialComm::sendAck( byte id  , const char * fmt , ... ) {
	va_list args;
	va_start(args, fmt);
	return this->_sendMessage( 0 , id , fmt  , args);
}


/******************************************************
Envoi un accuse sans donnees
******************************************************/
bool SerialComm::sendAck( byte id ) {
	return this->_sendMessage( 0 , id , "", NULL);
}


/******************************************************
Retourne les donnees d un message entrant
******************************************************/
bool SerialComm::getData(const char * fmt , ... ) {
	va_list args;
	va_start(args, fmt);

	int readindex = 3;

	while (*fmt != '\0') {
		if (readindex >= this->intputIndex) return false;            // Verification de fin de message
		switch (*fmt) {
		case 'i' :
			{
				int * i = va_arg(args, int * );
				*i = word(inputMessage[readindex], inputMessage[readindex + 1]); // Recompose l entier en lisant 2 octets
				readindex += 2;
				break;
			}
		case 's' :
			{
				char * s = va_arg(args, char * );
				char c = 0;
				int j = 0;
				do {
					c = inputMessage[readindex++];
					s[j++] = c;
				} while (c != 0);
				break;
			}
		default:
			return false;
		}
		++fmt;
	}

	va_end(args);

	return true;
}
