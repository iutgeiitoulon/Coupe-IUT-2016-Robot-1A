#ifndef EVENTS_H
#define	EVENTS_H

#define COLLISION_AVANT 0b01
#define COLLISION_ARRIERE 0b10

__extension__ typedef struct tagSystemEvents {

    union {

        struct {
            unsigned QeiDataEvent : 1;
            unsigned PositionDataEvent : 1;
            unsigned AsservissementVitesseEvent : 1;
            unsigned AsservissementPositionEvent : 1;
            unsigned AsservissementAngleEvent : 1;
            unsigned UltrasonicRangeFinderEvent : 1;
            unsigned UltrasonicObjectDetectionEvent : 1;
            unsigned CollisionMoteurGaucheEvent:2;
            unsigned CollisionMoteurDroitEvent:2;
            unsigned TargetProcheEvent:1;
            unsigned TargetAtteinteEvent:1;   
            unsigned DisableDetectionObstacle:1;
        };

        struct {
            unsigned All : 11;
        };
    };
} SystemEvents;

extern volatile SystemEvents sysEvents;

#endif	/* EVENTS_H */

