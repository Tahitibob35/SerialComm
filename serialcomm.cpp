
#include "Arduino.h"
#include "serialcomm.h"


SerialComm::SerialComm(HardwareSerial& s): serial(&s) {
  this->intputIndex = 0;                  // Nombre d octets recus
  this->actioncount = 0;                  // Nombre d actions definies
  this->esc = false;                      // Flag de caractere d'echappement recu
  this->receptionstarted = false;         // Message en cours de reception
  this->outputindex = 2;                  // Nombre d octets a emettre (commence a 2 pour les donnees, 0 -> action)
  this->readindex = 3;					  // Offset de lecture
}


void SerialComm::begin(void) {
  return;  
}


void SerialComm::check_reception(void) {
  byte c;                                //Caractere recu
  
  while (this->serial->available()) {
    c = this->serial->read();
    //this->serial->print("Byte received (hex): ");
    //this->serial->println(c, HEX);
    //this->serial->print(",");
    //this->serial->println(c, DEC);
    
    // Check for frame start
    if (c == START) {                            //Debut d'un message, restauration des donnees
      //this->serial->println("START");
      this->receptionstarted = true;
      this->intputIndex = 0;
      this->esc = false;
    }
    else {
      if (this->receptionstarted) {
        if (c == END) {                          //Fin d'un message
          //this->serial->println("END");
          ProcessMessage();                      
          this->intputIndex = 0;                 //Restauration des parametres par defaut
          this->receptionstarted = false;
        }
        else {
          if (c == ESC) {                        //Detection du caractere d echappement
            this->esc = true;
          }
          else {
            if (this->esc == true) {             //Traitement du caractere echappe
              if (c == TSTART) c = START;        //Conversion du caractere echappe
              if (c == TEND) c = END;
              if (c == TESC) c = ESC;
              this->esc = false;
            }
            addCharInInputMessage(c);            //Ajout d'octetconverti au message
          }
        }
      }
    }
  }
}


/******************************************************
 Ajout d'un caractere au message
 *****************************************************/
void SerialComm::addCharInInputMessage( char c ) {
  if (this->intputIndex < INPUTMSGLEN) 
    this->inputMessage[this->intputIndex++] = c;
}


/******************************************************
 Ajout du message recu
 *****************************************************
void SerialComm::printInputMessage( void ) {
  this->serial->print("Message : ");
  for(int i=1; i<intputIndex; i++) {
    this->serial->print(this->inputMessage[i]);
  }
  this->serial->println("");
  if (this->inputMessage[0] == this->CalculChecksum()) {
    this->serial->println("Valid checksum");
  }
  else {
    this->serial->println("Invalid checksum");
  }
  this->serial->println("");
}*/

/******************************************************
 Calcul du checksum
 *****************************************************/
byte SerialComm::CalculChecksum( byte * data, int dstart, int dend ) {

  byte checksum = 0;

  for(int i=dstart; i<dend; i++) {
    checksum = checksum ^ data[i];    
  }
  //this->serial->print("Local checksum : ");
  //this->serial->println(checksum, HEX);
  return checksum;

}


/******************************************************
 Traitement du message
 *****************************************************/
bool SerialComm::ProcessMessage( void ) {

  int action = 0;        
  byte id = 0;
  this->readindex = 3;    

  // Verification du checksum
  //this->serial->print("Message checksum : ");
  //this->serial->println(this->inputMessage[0], HEX);
  if (this->inputMessage[0] != this->CalculChecksum( this->inputMessage ,1 ,this->intputIndex )) {
    //this->serial->println("Invalid checksum");
    return false;                                           // Retour en erreur
  }
  // Recuperation de l id
  id = inputMessage[1];
  //this->serial->print("Id : ");
  //this->serial->println(id);
  
  // Recuperation de l action
  action = inputMessage[2];
  //this->serial->print("Action : ");
  //this->serial->println(action);
  
  //this->serial->print("Data : ");
  for(int i=2; i<intputIndex; i++) {
    //this->serial->print(this->inputMessage[i], HEX);
    //this->serial->print(" ");
  }
  //this->serial->println("");

  // Execution des actions definies
  for(int i=0; i < this->actioncount; i++) {
    if (action == this->commands[i]) {
      (*this->actions[i])();
      break;
    }
  }
  
  return true;  

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
 Retourne un entier depuis le message
 *****************************************************/
bool SerialComm::readInt( int * val ) {  
  if (this->readindex >= this->intputIndex) return false;            // Verification de fin de message
  *val = word(inputMessage[readindex], inputMessage[readindex + 1]); // Recompose l entier en lisant 2 octets
  readindex += 2;                                                    // Avance le pointeur de lecture de 2 octets
  return true;
}


/******************************************************
 Retourne l id du message
 *****************************************************/
int SerialComm::getId( void ) {  
  return inputMessage[1];
}


/******************************************************
 Retourne une chaine depuis le message
 *****************************************************/
bool SerialComm::readStr( char * val, int slen ) {  
  if (this->readindex >= this->intputIndex) return false;            // Verification de fin de message

  char c = 0;
  int i = 0;
  memset(val, 0, slen);
  do {
    c = inputMessage[readindex++];
    val[i++] = c;    
    if (i > slen) {
      val[i - 1] = 0;                                            // Fin de chaine       
      while (inputMessage[readindex++] != 0);                    // On avance le pointeur de lecture jusqu a la fin de la chaine
      return false;                                              // On force la sortie
    }
  } while (c != 0);


  return true;
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
 Ajoute un octet au message a envoyer
 *****************************************************/
bool SerialComm::addByte(byte octet) {

  this->outputmessage[this->outputindex++] = octet;      
  return true;
}


/******************************************************
Ajoute un entier au prochain message a envoyer
******************************************************/
bool SerialComm::prepareInt( int value ) {
  if ( (this->outputindex + 2) > OUTPUTMSGLEN ) {                                   // Test s'il reste assez de place
    return false;
  }
  if (addByte(value >> 8) && addByte(value & 0xFF)) return true;
  return false;
}


/******************************************************
Ajoute une chaine au prochain message a envoyer
******************************************************/
bool SerialComm::prepareStr( char * mystring, int slen ) {
  if ( (this->outputindex + (slen + 1)) > OUTPUTMSGLEN ) {                                   // Test s'il reste assez de place
    return false;
  }
  for (int i=0; i<slen; i++) {
    if (!addByte(mystring[i])) return false;
  }
  if (!addByte(0)) return false;
  return true;
}


/******************************************************
Envoi le message avec id
******************************************************/
bool SerialComm::sendMessage( byte action , byte id) {
  this->serial->write(START);                                                  // Debut de trame
  
  this->outputmessage[0] = action;                                             // Ajout de l action au debut du message
  this->outputmessage[1] = id;                                                 // Ajout de l id au debut du message

  int checksum;       
  checksum = this->CalculChecksum(this->outputmessage, 0, this->outputindex);   // Calcul du checksum

  this->safeWrite(checksum);                                                   // Ecriture des octets des donnees
  for ( int i=0; i < this->outputindex; i++)
    this->safeWrite(this->outputmessage[i]);

  this->serial->write(END);                                                    // Fin de trame
  this->outputindex = 2;
  return true;  
}


/******************************************************
Envoi le message sans id
******************************************************/
bool SerialComm::sendMessage( byte action ) {
  return this->sendMessage( action, 0);
}


/******************************************************
Envoi un accuse
******************************************************/
bool SerialComm::sendAck( byte id ) {
  return this->sendMessage( 0, id);
}
