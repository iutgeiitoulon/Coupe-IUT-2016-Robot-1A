#ifndef SERVO_H
#define	SERVO_H

void ManageServos(void);
void SetConsigneServoBrasGauche(unsigned int angle);
void SetConsigneServoBrasDroit(unsigned int angle);
void SetConsigneServoPinceGauche(unsigned int angle);
void SetConsigneServoPinceDroit(unsigned int angle);
void SetConsigneServoBras(unsigned int angle);
void SetConsigneServoBrasInactif(unsigned int angle);
void SetConsigneServoPince(unsigned int angle);
void SetConsigneServoPinceInactif(unsigned int angle);
void SetConsigneServo5(unsigned int angle);
void SetConsigneServoCreveBallon(unsigned char ouvert);

#endif	/* SERVO_H */
