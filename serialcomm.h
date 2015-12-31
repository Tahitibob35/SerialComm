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
    int  getNewMessageId( void );                       // Retourne un nouvel id de message

     
  private:
    HardwareSerial *serial;    
    byte commands[ACTIONSLEN];           // Tableau des actions
    void (*actions[ACTIONSLEN])(void);   // Tableau des fonctions des actions
    int actioncount;                     // Nombre d actions definies
    byte inputMessage[INPUTMSGLEN];      // Tableau receptionnant le message
    byte intputIndex;                    // Nombre de caracteres recus

    void _checkSum( byte * , byte );        // Calcul du checksum
    void addCharInInputMessage( char  );          // Ajout du caractere recu au message
    bool ProcessMessage( void );                  // Traitement du message
    bool safeWrite( byte );                       // Ecrit un octet en l echappant si necessaire
    bool waitAck( byte );                         // attend l'arrivee d'un ack
    bool _sendMessage( byte , byte );             // Envoi le message avec id
    bool _read( void );                           // lit les donnees du buffer serie
    byte inputMessageGetId( void );               // Retourne l'id du message entrant
    byte inputMessageGetAction( void );           // Retourne l'action du message entrant
    bool inputMessageValidateChecksum( void );    // Verifie le checksum du message entrant
    bool _sendMessage( byte , byte , const char* , va_list);  // Envoi un message
};

#endif
