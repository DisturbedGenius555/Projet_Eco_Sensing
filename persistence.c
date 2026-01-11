#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>

void sauvegarder_etat(Capteur *c) {
    FILE *f = fopen("save.bin", "wb");

    if (f == NULL) {
        printf("Erreur : impossible douvrir le fichier de sauvegarde.\n");
        return;
    }

    fwrite(&(c->battery), sizeof(float), 1, f);
    fwrite(&(c->x), sizeof(float), 1, f);
    fwrite(&(c->y), sizeof(float), 1, f);
    fwrite(&(c->buffer_usage), sizeof(int), 1, f);

    Paquet *temp = c->buffer_tete;

    while (temp != NULL) {
        fwrite(&(temp->id), sizeof(int), 1, f);
        fwrite(&(temp->temperature), sizeof(float), 1, f);
        fwrite(&(temp->timestamp), sizeof(long), 1, f);

        temp = temp->suivant;
    }

    fclose(f);
    printf("Sauvegarde effectuee avec succes dans 'save.bin'.\n");
}

void charger_etat(Capteur *c) {
    FILE *f = fopen("save.bin", "rb");

    if (f == NULL) {
        printf("Aucune sauvegarde trouvee.\n");
        return;
    }

    fread(&(c->battery), sizeof(float), 1, f);
    fread(&(c->x), sizeof(float), 1, f);
    fread(&(c->y), sizeof(float), 1, f);
    fread(&(c->buffer_usage), sizeof(int), 1, f);

    c->buffer_tete = NULL;
    Paquet *dernier = NULL;

    int paquets_a_charger = c->buffer_usage;
    if (paquets_a_charger > 5) paquets_a_charger = 5;

    for (int i = 0; i < paquets_a_charger; i++) {
        Paquet *p = (Paquet *)malloc(sizeof(Paquet));

        fread(&(p->id), sizeof(int), 1, f);
        fread(&(p->temperature), sizeof(float), 1, f);
        fread(&(p->timestamp), sizeof(long), 1, f);

        p->suivant = NULL;

        if (c->buffer_tete == NULL) {
            c->buffer_tete = p;
        } else {
            dernier->suivant = p;
        }

        dernier = p;
    }

    fclose(f);
    printf("Chargement effectue avec succes depuis 'save.bin'.\n");
    printf("Paquets charges: %d/5\n", c->buffer_usage);
}
