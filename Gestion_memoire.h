#ifndef GESTION_MEMOIRE_H_INCLUDED
#define GESTION_MEMOIRE_H_INCLUDED

#include "structures.h"

void initialiser_buffer(Capteur *b, int max);
void ajouter_en_tete(Capteur *b, Paquet *nouveau);
void supprimer_dernier(Capteur *b);
void produire_paquet(Capteur *b);
void afficher_buffer(Capteur *b);
void liberer_buffer(Capteur *b);
void afficher_info_capteur(Capteur *c);

#endif // GESTION_MEMOIRE_H_INCLUDED
