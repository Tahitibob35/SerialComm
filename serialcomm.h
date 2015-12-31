/*
  SerialComm.h - Library for serial communications
  Created by David Hautbois.
  Released into the public domain.
*/
#ifndef SerialComm_h
#define SerialComm_h

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
    SerialComm(HardwareSerial &s);
    void begin(void);
    void check_reception(void);
    bool attach(int command, void (*ptrfonction)(void));
    int  getId( void );                                     // retourne l id d un message
    bool sendMessage( byte , bool , const char * , ... );   // Envoi un message
    bool sendAck( byte , const char * , ... );              // Envoi un accuse avec des donnees
    bool sendAck( byte id );                                // Envoi un accuse sans donnees
	bool getData(const char * , ... );                      // Retourne les donnees d un message entrant

     
  private:
    HardwareSerial *_serial;
    byte _commands[ACTIONSLEN];           // Tableau des actions
    void (*_actions[ACTIONSLEN])(void);   // Tableau des fonctions des actions
    int  _actioncount;                     // Nombre d actions definies
    byte _inputMessage[INPUTMSGLEN];      // Tableau receptionnant le message
    byte _intputIndex;                    // Nombre de caracteres recus

    void _checkSum( byte * , byte );        // Calcul du checksum
    void _addCharInInputMessage( char  );          // Ajout du caractere recu au message
    bool _processMessage( void );                  // Traitement du message
    bool _safeWrite( byte );                       // Ecrit un octet en l echappant si necessaire
    bool _waitAck( byte );                         // attend l'arrivee d'un ack
    bool _sendMessage( byte , byte );             // Envoi le message avec id
    bool _read( void );                           // lit les donnees du buffer serie
    byte _inputMessageGetAction( void );           // Retourne l'action du message entrant
    bool _inputMessageValidateChecksum( void );    // Verifie le checksum du message entrant
    bool _sendMessage( byte , byte , const char* , va_list);  // Envoi un message
    int  _getNewMessageId( void );                       // Retourne un nouvel id de message

};

#endif
