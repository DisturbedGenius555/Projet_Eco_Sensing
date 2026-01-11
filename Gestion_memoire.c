#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "gestion_memoire.h"
#include "structures.h"

static int id_specifique = 0;

void initialiser_buffer(Capteur *b, int max) {
    b->buffer_tete = NULL;
    b->buffer_usage = 0;
    b->buffer_max = max;
}

void ajouter_en_tete(Capteur *b, Paquet *nouveau) {
    nouveau->suivant = b->buffer_tete;
    b->buffer_tete = nouveau;
    b->buffer_usage++;
}

void supprimer_dernier(Capteur *b) {
    if (b->buffer_tete == NULL) return;

    if (b->buffer_tete->suivant == NULL) {
        printf("Suppression du paquet ID=%d (seul element)\n", b->buffer_tete->id);
        free(b->buffer_tete);
        b->buffer_tete = NULL;
        b->buffer_usage--;
        return;
    }

    Paquet *courant = b->buffer_tete;
    while (courant->suivant != NULL && courant->suivant->suivant != NULL) {
      courant = courant->suivant;
    }

    if (courant->suivant != NULL) {
        printf("ALERTE : Memoire saturee. Suppression du paquet ID=%d pour liberer de l'espace\n",
               courant->suivant->id);
        free(courant->suivant);
        courant->suivant = NULL;
        b->buffer_usage--;
    }
}

void produire_paquet(Capteur *b) {
    Paquet *p = malloc(sizeof(Paquet));
    if (!p) {
        printf("Erreur d'allocation memoire\n");
        return;
    }

    p->id = id_specifique++;
    p->temperature = 20.0f + (float)(rand() % 300) / 10.0f;
    p->timestamp = time(NULL);
    p->suivant = NULL;

    printf("PAQUET CREE: ID=%d, Temperature=%.1f C\n", p->id, p->temperature);

    ajouter_en_tete(b, p);

    if (b->buffer_usage > b->buffer_max) {
        supprimer_dernier(b);
    }
}

void afficher_buffer(Capteur *b) {
    printf("=== BUFFER (%d/5 paquets) ===\n", b->buffer_usage);
    printf("Plus recent au Plus ancien:\n");

    if (b->buffer_usage == 0) {
        printf("(vide)\n");
        printf("=========================\n");
        return;
    }

    Paquet *courant = b->buffer_tete;
    int position = 1;

    while (courant != NULL) {
        printf("%d. ID=%d | Temperature: %.1f C | Temps: %ld\n",
               position++, courant->id, courant->temperature, courant->timestamp);
        courant = courant->suivant;
    }
    printf("=========================\n");
}

void liberer_buffer(Capteur *b) {
    Paquet *courant = b->buffer_tete;
    Paquet *suivant;

    while (courant != NULL) {
        suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }

    b->buffer_tete = NULL;
    b->buffer_usage = 0;
}

void afficher_info_capteur(Capteur *c) {
    printf("\INFOS CAPTEUR\n");
    printf("Position: (%.2f, %.2f)\n", c->x, c->y);
    printf("Batterie: %.4f J\n", c->battery);
    float distance = sqrt(c->x * c->x + c->y * c->y);
    printf("Distance a la station: %.2f\n", distance);
    printf("Energie requise pour transmission: %.4f J\n",
           calculer_energie_transmission(distance));
    printf("Buffer: %d/5 paquets\n", c->buffer_usage);
    printf("=========================\n");
}
