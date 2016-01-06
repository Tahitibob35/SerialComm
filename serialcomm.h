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

    attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
    default min is 544, max is 2400

 */

#include "Arduino.h"

#define START  0x61    //a
#define END    0x62    //b
#define ESC    0x63    //c
#define TSTART 0x64    //d
#define TEND   0x65    //e
#define TESC   0x66    //f

#define INPUTMSGLEN 50        //Taille max d'un message entrant
#define ACTIONSLEN 5          //nombre max d actions
#define ACKTIMEOUT 2000

class SerialComm
{
  public:
    SerialComm(Stream &s);
    void check_reception(void);
    bool attach(int command, void (*ptrfonction)(void));
    bool sendMessage( byte , bool , const char * , ... );   // Envoi un message
    bool sendAck( const char * , ... );              // Envoi un accuse avec des donnees
    bool sendAck( void );                                // Envoi un accuse sans donnees
	bool getData(const char * , ... );                      // Retourne les donnees d un message entrant

     
  private:
	Stream *_serial;
    byte _commands[ACTIONSLEN];           // Tableau des actions
    void (*_actions[ACTIONSLEN])(void);   // Tableau des fonctions des actions
    int  _actioncount;                     // Nombre d actions definies
    byte _inputMessage[INPUTMSGLEN];      // Tableau receptionnant le message
    byte _inputIndex;                    // Nombre de caracteres recus

    void _checkSum( byte * , byte );        // Calcul du checksum
    void _addCharInInputMessage( char  );          // Ajout du caractere recu au message
    bool _processMessage( void );                  // Traitement du message
    bool _safeWrite( byte );                       // Ecrit un octet en l echappant si necessaire
    bool _waitAck( byte );                         // attend l'arrivee d'un ack
    bool _sendMessage( byte , byte );             // Envoi le message avec id
    bool _read( void );                           // lit les donnees du buffer serie
    byte _inputMessageGetAction( void );           // Retourne l'action du message entrant
    int  _inputMessageGetId( void );                                     // retourne l id d un message
    bool _inputMessageValidateChecksum( void );    // Verifie le checksum du message entrant
    bool _sendMessage( byte , byte , const char* , va_list);  // Envoi un message
    int  _getNewMessageId( void );                       // Retourne un nouvel id de message

};

#endif
