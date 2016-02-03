/*
  SerialComm.h - Library for serial communications
  Created by David Hautbois.
  Released into the public domain.
*/
#ifndef SerialComm_h
#define SerialComm_h

/*
  A serial channel is activated by creating an instance of the SerialComm class passing
  a Stream object (hard or soft).

  The methods are:
    SerialComm - Class for sending and receiving messages.
    attach( command , function )  - Attaches a function to a command.
    check_reception() - Checks for incomings messages.
    getId( ) - Returns the message id of the incoming message.
               Used to return the ack to the current incoming message.
    sendMessage( command , ack , values ... ) - Send a message with the specified command and values.
                                                Set ack to true to received an ack.
                                                Return true if an ack has been received.
    sendAck( values... ) - Send an ack with values.
    getData(const char * , ... ) - Extract data from incoming message or ack.
                                   Return true if success
 */

#include "Arduino.h"

#define START  0x61    //a
#define END    0x62    //b
#define ESC    0x63    //c
#define TSTART 0x64    //d
#define TEND   0x65    //e
#define TESC   0x66    //f

#define INPUTMSGLEN 100        //Taille max d'un message entrant
#define ACTIONSLEN 20          //nombre max d actions
#define ACKTIMEOUT 2000

#define NSCDEBUG

#ifdef SCDEBUG
#include <SoftwareSerial.h>
#endif

class SerialComm
{
  public:
    SerialComm( Stream &s );
    SerialComm( void );
    void check_reception(void);
    bool attach(int command, void (*ptrfonction)(void));
    bool sendMessage( byte , bool );   // Envoi un message
    bool sendMessage( byte , bool , const char * , ... );   // Envoi un message
    void sendHeader( byte action );                         // Envoi l entete d un message sans ID
    void sendFooter( void );                                // Envoi la fin d un message
    void sendInteger( int  value );                         // Envoi un entier
    void sendcharArray( char * string );                    // Envoi une chaine
    bool sendAck( const char * , ... );              // Envoi un accuse avec des donnees
    bool sendAck( void );                                // Envoi un accuse sans donnees
	bool getData(const char * , ... );                      // Retourne les donnees d un message entrant
	int getInt( void );                                    // Lit un entier dans le message
	void getString( char *buf , int maxsize );              // Lit une chaine dans le message
 #ifdef SCDEBUG
	SoftwareSerial *debugserial;
    #endif

     
  private:
	Stream *_serial;
    byte _commands[ACTIONSLEN];           // Tableau des actions
    void (*_actions[ACTIONSLEN])(void);   // Tableau des fonctions des actions
    int  _actioncount;                    // Nombre d actions definies
    byte _inputMessage[INPUTMSGLEN];      // Tableau receptionnant le message
    byte _inputIndex;                     // Nombre de caracteres recus
    byte _checksum;                       // Checksum du message en cours
    int _readindex;                       // index de lecture

    void _checkSum( byte * , byte );                          // Calcul du checksum
    void _addCharInInputMessage( char  );                     // Ajout du caractere recu au message
    bool _processMessage( void );                             // Traitement du message
    bool _safeWrite( byte octet, bool checksum );             // Ecrit un octet en l echappant si necessaire
    bool _waitAck( byte );                                    // attend l'arrivee d'un ack
    bool _sendMessage( byte , byte );                         // Envoi le message avec id
    bool _read( void );                                       // lit les donnees du buffer serie
    byte _inputMessageGetAction( void );                      // Retourne l'action du message entrant
    int  _inputMessageGetId( void );                          // retourne l id d un message
    bool _inputMessageValidateChecksum( void );               // Verifie le checksum du message entrant
    bool _sendMessage( byte , byte , const char* , va_list);  // Envoi un message
    int  _getNewMessageId( void );                            // Retourne un nouvel id de message
    void _sendHeader( byte id, byte action );                // Envoi l entete d un message

};

#endif
