#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulation.h"
#include "structures.h"

float calculer_energie_transmission(float distance) {
    return 0.05f + 0.01f * distance * distance;
}

int attempt_transmission(Capteur *c) {
    float d;
    float E;

    /* Calcul de la distance euclidienne */
    d = sqrt(c->x * c->x + c->y * c->y);

    /* Calcul de l'énergie consommée */
    E = calculer_energie_transmission(d);

    /* Vérification de la batterie */
    if (c->battery >= E) {
        // Transmission réussie
        c->battery -= E;   // consommation d'énergie

        // Supprimer le paquet transmis du buffer (le plus récent)
        if (c->buffer_tete != NULL) {
            Paquet *paquet_transmis = c->buffer_tete;
            printf("Paquet ID=%d (Temp=%.1f C) en cours de transmission...\n",
                   paquet_transmis->id, paquet_transmis->temperature);

            c->buffer_tete = c->buffer_tete->suivant;
            free(paquet_transmis);
            c->buffer_usage--;
        }

        return 1;          // transmission réussie
    } else {
        // Transmission impossible - batterie insuffisante
        printf("ERREUR: Batterie insuffisante pour transmission !\n");
        printf("  Batterie disponible: %.4f J\n", c->battery);
        printf("  Energie requise: %.4f J\n", E);
        printf("  Deficit: %.4f J\n", E - c->battery);

        c->battery = 0;    // capteur mort
        printf("CAPTEUR HORS SERVICE - Batterie épuisée\n");

        return 0;          // transmission impossible
    }
}
}
