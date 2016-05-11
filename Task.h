#ifndef TASK_H
#define	TASK_H

#define ALIGNED_WITH_MOVE 1
#define NOT_ALIGNED 0

#define NO_TASK 0
#define TASK_VIDAGE_CHATEAU 10
#define TASK_FERMETURE_PORTES 20
#define TASK_AVALAGE_CHATEAU 30
#define TASK_PECHE_POISSON 40

#define MATCH_INIT 0
#define MATCH_INIT_EN_COURS 1
#define MATCH_FERMETURE_PORTES 2
#define MATCH_FERMETURE_PORTES_EN_COURS 3
#define MATCH_AVALAGE_CHATEAU 4
#define MATCH_AVALAGE_CHATEAU_EN_COURS 5
#define MATCH_VIDAGE_CHATEAU 6
#define MATCH_VIDAGE_CHATEAU_EN_COURS 7
#define MATCH_PECHE_POISSON 8
#define MATCH_PECHE_POISSON_EN_COURS 9
#define MATCH_NO_TASK 100


#define T_AC_INIT 0
#define T_AC_INIT_EN_COURS 1
#define T_AC_1 2
#define T_AC_1_EN_COURS 3
#define T_AC_5 4
#define T_AC_5_EN_COURS 5
#define T_AC_RECULE_2 6
#define T_AC_RECULE_2_EN_COURS 7
#define T_AC_4 8
#define T_AC_4_EN_COURS 9
#define T_AC_FIN 10
#define T_AC_FIN_EN_COURS 11
#define T_AC_BLOCK 12
#define T_AC_2 13
#define T_AC_2_EN_COURS 14
#define T_AC_3 15
#define T_AC_3_EN_COURS 16

#define T_VC_INIT 20
#define T_VC_INIT_EN_COURS 21
#define T_VC_AVANCE_1 22
#define T_VC_AVANCE_1_EN_COURS 23
#define T_VC_RECULE_1 24
#define T_VC_RECULE_1_EN_COURS 25
#define T_VC_AVANCE_2 26
#define T_VC_AVANCE_2_EN_COURS 27
#define T_VC_RECULE_2 28
#define T_VC_RECULE_2_EN_COURS 29
#define T_VC_FIN 30
#define T_VC_FIN_EN_COURS 31
#define T_VC_AVANCE_1_BIS 32
#define T_VC_AVANCE_1_BIS_EN_COURS 33
#define T_VC_AVANCE_0 34
#define T_VC_AVANCE_0_EN_COURS 35
#define T_VC_TOURNE_1 36
#define T_VC_TOURNE_1_EN_COURS 37
#define T_VC_TOURNE_2 38
#define T_VC_TOURNE_2_EN_COURS 39

#define T_FP_INIT 50
#define T_FP_INIT_EN_COURS 51
#define T_FP_6 52
#define T_FP_6_EN_COURS 53
#define T_FP_7 54
#define T_FP_7_EN_COURS 55
#define T_FP_FIN 56
#define T_FP_FIN_EN_COURS 57
#define T_FP_1 58
#define T_FP_1_EN_COURS 59
#define T_FP_3 60
#define T_FP_3_EN_COURS 61
#define T_FP_5 62
#define T_FP_5_EN_COURS 63
#define T_FP_RECULE_TAS_2 64
#define T_FP_RECULE_TAS_2_EN_COURS 65
#define T_FP_2 66
#define T_FP_2_EN_COURS 67
#define T_FP_4 68
#define T_FP_4_EN_COURS 69


#define T_PP_INIT 70
#define T_PP_INIT_EN_COURS 71
#define T_PP_POS1 72
#define T_PP_POS1_EN_COURS 73
#define T_PP_POS2 74
#define T_PP_POS2_EN_COURS 75
#define T_PP_POS3 76
#define T_PP_POS3_EN_COURS 77
#define T_PP_POS4 78
#define T_PP_POS4_EN_COURS 79
#define T_PP_POS5 80
#define T_PP_POS5_EN_COURS 81
#define T_PP_POS6 82
#define T_PP_POS6_EN_COURS 83
#define T_PP_POS7 84
#define T_PP_POS7_EN_COURS 85
#define T_PP_FIN 86
#define T_PP_FIN_EN_COURS 87
#define T_PP_POS3_BIS 88
#define T_PP_POS3_BIS_EN_COURS 89
#define T_PP_POS0 90
#define T_PP_POS0_EN_COURS 91
#define T_PP_POS3_TER 92
#define T_PP_POS3_TER_EN_COURS 93
#define T_PP_INIT_BIS 94
#define T_PP_INIT_BIS_EN_COURS 95
#define T_PP_INIT_TER 96
#define T_PP_INIT_TER_EN_COURS 97
#define T_PP_POS8 98
#define T_PP_POS8_EN_COURS 99
#define T_PP_POS9 100
#define T_PP_POS9_EN_COURS 101
#define T_PP_POS10 102
#define T_PP_POS10_EN_COURS 103
#define T_PP_POS11 104
#define T_PP_POS11_EN_COURS 105
#define T_PP_POS12 106
#define T_PP_POS12_EN_COURS 107



typedef struct TaskBITS {

    union {
        struct {         
            //unsigned char taskNumber;
            int startingPosX;
            int startingPosY;
            //unsigned char nbCall;
            //unsigned char nbTaskCompleted;
        };
    };
} TASK_BITS;

void StartMove(unsigned int nextState, int X, int Y, unsigned char alignMvt);
void StartTempo(unsigned int nextState);
void IsCurrentMoveFinishedGoToState(unsigned char nextTask, int timeout);
void IsCurrentTempoFinished(unsigned char nextTask, int timeout);

#endif	/* TASK_H */

