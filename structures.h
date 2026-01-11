#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#include <time.h>

typedef struct Paquet {
    int id;
    float temperature;
    long timestamp;
    struct Paquet *suivant;
} Paquet;

typedef struct Capteur {
    float x;
    float y;
    float battery;
    Paquet *buffer_tete;
    int buffer_usage;
    int buffer_max;
} Capteur;

#endif // STRUCTURES_H_INCLUDED
