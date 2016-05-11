/* 
 * File:   CB_TX2.h
 * Author: Valentin
 *
 * Created on 9 mars 2016, 23:21
 */

#ifndef CB_TX2_H
#define	CB_TX2_H

void SendMessage2(unsigned char* message, int length); //recupere le message a envoyer et le fou dans le buffer si il peu.
void CB_TX2_Add(unsigned char value); //rentre un caract dans le buffer.
unsigned char CB_TX2_Get(void); //sort un caract du buffer.
void __attribute__((__interrupt__, __auto_psv__))_U2TXInterrupt(void); //it sur l'envoie.
unsigned char CB_TX2_IsTranmitting(void); //si sa transmet.
unsigned int CB_TX2_RemainingSize(void); //renvoie la taille restant du buffer.
void SendOne2(); //envoie caractere.

#endif	/* CB_TX2_H */

