
#include <stdio.h>
#include <stdlib.h>
#include "p33Fxxxx.h"
#include "CB_RX1.h"
#include "CB_TX1.h"
#include "main.h"
#include "Lidar.h"
#include "math.h"
#include "IO.h"
#include "robot.h"

unsigned char DecodeState = QUALITY_BYTE;
unsigned int angleCourant=0;
unsigned int distanceCourante=0;
int qualityCourante;
unsigned char nouveauTour;
unsigned char DistanceTable[ANGLE_MAX - ANGLE_MIN];//type ?
unsigned char DistanceTableToSend[ANGLE_MAX - ANGLE_MIN];
unsigned char ListAngle[ANGLE_MAX - ANGLE_MIN];//type ?
unsigned char ListDistance[ANGLE_MAX - ANGLE_MIN];//type ?
unsigned int i = 0;
unsigned int cpt_angle = 0;
unsigned char flagLidarFrameReady = 0;

void DecodeIrRangeFinder(unsigned char c)
{
    switch (DecodeState)
    {
        case QUALITY_BYTE:
            if ((c & 0x01) == ((c >> 1) & 0x01))
            {
                //Problème, on reste dans le même état
            }
            else
            {                
                nouveauTour = ((c & 0x01) == 0x01);
                qualityCourante = c >> 2;
                if (nouveauTour)
                {
                    nouveauTour = 0;
                    cpt_angle = 0;
                    //RplidarPostProcessing();
                    for (i = 0; i < ANGLE_MAX - ANGLE_MIN; i++) //envoi des donnees brutes pas bon
                    {
                        DistanceTableToSend[i] = DistanceTable[i];
                        DistanceTable[i]=0;
//                        //DistanceTableToSend[i] = DistanceTable[i];
//                        if (DistanceTable[i] != 0)
//                        {
//                            DistanceTableToSend[i] = DistanceTable[i];
//                        }
////                        else
////                        {
////                            if (i > 0)
////                                DistanceTableToSend[i] = DistanceTableToSend[i - 1];
////                            else
////                                DistanceTableToSend[0] = 255;
////                        }
                    }
                    flagLidarFrameReady = 1;
                }
                DecodeState = ANGLE_LSB_BYTE;
            }
            break;

        case ANGLE_LSB_BYTE:
            if ((c & 0x01) != 0x01)
            {
                //pas bon
                DecodeState = QUALITY_BYTE;
            }
            else
            {
                angleCourant = (int)(c >> 1);
                DecodeState = ANGLE_MSB_BYTE;
            }
            break;

        case ANGLE_MSB_BYTE:
            angleCourant = angleCourant | (((int) c) << 7);
            angleCourant = angleCourant >> 6;
            DecodeState = DISTANCE_LSB_BYTE;
            break;

        case DISTANCE_LSB_BYTE:            
            distanceCourante = c>>2;
            DecodeState = DISTANCE_MSB_BYTE;
            break;

        case DISTANCE_MSB_BYTE:
            distanceCourante = distanceCourante | (((int)c) << 6);
            distanceCourante /= 10;
            
            DecodeState = QUALITY_BYTE;
            if ((int) angleCourant >= ANGLE_MIN && (int) angleCourant < ANGLE_MAX)
            {
                if(qualityCourante >= robotState.LidarSensitivity)
                    DistanceTable[(int) angleCourant - ANGLE_MIN] = (unsigned char)distanceCourante;
            }
            break;

        default:
            DecodeState = QUALITY_BYTE;
            break;
    }
}

//void RplidarPostProcessing(void)//problem de type ?
//{
//    int startDetectionAngle = 10;
//    int stopDetectionAngle = 170;
//    int i = 0;
//    int ListAngleSansTrou[ANGLE_MAX - ANGLE_MIN];
//    unsigned char ListDistanceSansTrou[ANGLE_MAX - ANGLE_MIN];
//    int lastAngle = 10;
//    int lastDistance = 255;
//    cpt_angle = 0;
//    for (i = startDetectionAngle; i < stopDetectionAngle; i++)
//    {
//        if (DistanceTable[i] != 0)
//        {
//            ListDistanceSansTrou[cpt_angle] = i;
//            ListDistanceSansTrou[cpt_angle] = DistanceTable[i];
//            cpt_angle++;
//            lastAngle = i;
//            lastDistance = DistanceTable[i];
//        }
//        else
//        {
//            ListAngleSansTrou[i] = lastAngle;
//            ListDistanceSansTrou[i] = lastDistance;
//        }
//
//    }
//
//    //Génération du tableau des distances post traitées
//    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//
//    int listAngleFiltree[ANGLE_MAX - ANGLE_MIN];
//    unsigned char listDistanceFiltree[ANGLE_MAX - ANGLE_MIN];
//    unsigned char currentdistance = 0;
//    int encombrementAngulaireRobot = 0;
//    int currentAngle = 0;
//    int rayonRobot = 150;
//
//    for (i = startDetectionAngle; i < stopDetectionAngle; i++)
//    {
//
//        listDistanceFiltree[i] = 255; //255 cm =2.55m
//        listAngleFiltree[i] = i;
//    }
//
//    for (i = startDetectionAngle; i < stopDetectionAngle; i++)
//    {
//        //Calcule de l'encombrement angulaire du robot
//
//        currentdistance = ListDistanceSansTrou[i - startDetectionAngle];
//        encombrementAngulaireRobot = atanf(rayonRobot / ListDistanceSansTrou[i - startDetectionAngle])*180 / M_Pi; //problem ?
//        currentAngle = i - encombrementAngulaireRobot;
//        while (currentAngle < (i + encombrementAngulaireRobot))
//        {
//            if (currentAngle >= 10 && currentAngle < 170)
//            {
//                //cree la lib Min
//                listDistanceFiltree[currentAngle - startDetectionAngle] = Min(listDistanceFiltree[currentAngle - startDetectionAngle], currentdistance);
//            }
//            currentAngle++;
//        }
//        /*for(i=0; i<ANGLE_MAX-ANGLE_MIN ; i++)
//        {
//           DistanceTableToSend[i] = listDistanceFiltree[i];
//           flagLidarFrameReady = 1;
//        }//probleme il retourne pas tout le temps dans le main
//        UartEncodeAndSendMessage1(0x0060,ANGLE_MAX-ANGLE_MIN, DistanceTableToSend);*/
//
//    }
//    return;
//}
//
//unsigned char Min(unsigned char a, unsigned char b)
//{
//    if (a < b)
//    {
//        return a;
//    }
//    else
//        return b;
//}


