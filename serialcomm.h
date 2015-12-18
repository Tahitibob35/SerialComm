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

#define INPUTMSGLEN 21        //Taille max d'un message entrant
#define OUTPUTMSGLEN 21        //Taille max d'un message entrant
#define ACTIONSLEN 5          //nombre max d actions
#define ACKTIMEOUT 2000

class SerialComm
{
  public:
    SerialComm(HardwareSerial &s);
    void begin(void);
    void check_reception(void);
    bool attach(int command, void (*ptrfonction)(void));
    bool readInt(int * val);                            // Lecture d un entier
    bool readStr(char * val, int slen);                 // Lecture d une chaine
    bool prepareInt(int value);                         // Ajoute un entier au message a envoyer
    bool prepareStr( char * , int );
    bool sendMessage( byte , bool);                           // Envoi le message sans id
    bool sendAck( byte );                               // Envoi un ack
    int  getId( void );                                 // retourne l id d un message
    bool lockMessageId( byte * );                       // Retourne un nouvel id de message
    void releaseMessageId( byte );                     // Libere un id de message
    byte messageids;                           // Id de messages disponibles

     
  private:
    HardwareSerial *serial;    
    byte commands[ACTIONSLEN];            // Tableau des actions
    void (*actions[ACTIONSLEN])(void);   // Tableau des fonctions des actions
    int actioncount;                     // Nombre d actions definies
    bool esc;                            // Flag de caractere d'echappement recu
    bool receptionstarted;               // Message en cours de reception
    byte inputMessage[INPUTMSGLEN];      // Tableau receptionnant le message
    byte intputIndex;                    // Nombre de caracteres recus
    byte readindex;                      // Offset des donnes en cours de lecture
    byte outputmessage[OUTPUTMSGLEN];    // Tableau receptionnant le message
    byte outputindex;                    // Nombre de caracteres recus
    
    bool addByte( byte octet );                   // Ajoute un octet au message a envoyer
    byte CalculChecksum(  byte * , int, int );    // Calcul du checksum
    void printInputMessage( void );
    void addCharInInputMessage( char  );          // Ajout du caractere recu au message
    bool ProcessMessage( void );                  // Traitement du message
    bool safeWrite( byte );                       // Ecrit un octet en l echappant si necessaire
    bool waitAck( byte );            // attend l'arrivee d'un message
    bool _sendMessage( byte , byte );             // Envoi le message avec id
    bool _read( void );                           // lit les donnees du buffer serie
    byte inputMessageGetId( void );               // Retourne l'id du message entrant
    byte inputMessageGetAction( void );           // Retourne l'action du message entrant
    bool inputMessageValidateChecksum( void );    // Verifie le checksum du message entrant
};

#endif
