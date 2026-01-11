#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "main.h"
#include "structures.h"
#include "simulation.h"
#include "gestion_memoire.h"
#include "persistence.h"

void lancer_simulation() {
    Capteur capteur;
    int choix_chargement;
    int temps = 0;
    int paquets_produits = 0;
    int paquets_transmis = 0;  // COMPTEUR DE PAQUETS TRANSMIS

    // Initialisation
    srand(time(NULL));

    printf("\nSimulateur de Reseau de Capteurs Contraints\n\n");

    // Demander la position et la batterie
    printf("Initialisation du capteur:\n");
    printf("Entrez la position x: ");
    scanf("%f", &capteur.x);
    printf("Entrez la position y: ");
    scanf("%f", &capteur.y);
    printf("Entrez la batterie initiale (Joules): ");
    scanf("%f", &capteur.battery);

    // Buffer fixé à 5
    initialiser_buffer(&capteur, 5);

    printf("\n--- Configuration initiale du capteur ---\n");
    printf("Position: (%.2f, %.2f)\n", capteur.x, capteur.y);
    printf("Batterie initiale: %.4f J\n", capteur.battery);

    float distance = sqrt(capteur.x * capteur.x + capteur.y * capteur.y);
    float energie_transmission = calculer_energie_transmission(distance);
    printf("Distance a la station: %.2f\n", distance);
    printf("Energie par transmission: %.4f J\n", energie_transmission);

    int transmissions_possibles = (int)(capteur.battery / energie_transmission);
    printf("Transmissions possibles initialement: %d\n", transmissions_possibles);
    printf("0/5 paquets en attentes\n\n");

    // Demander si on veut charger des paquets précédents
    printf("Voulez-vous charger les 5 derniers paquets dune sauvegarde ?\n");
    printf("1. Oui\n");
    printf("2. Non (generer de nouveaux paquets)\n");
    printf("Choix: ");
    scanf("%d", &choix_chargement);

    if (choix_chargement == 1) {
        charger_etat(&capteur);
        printf("\nEtat charge depuis la sauvegarde.\n");
        afficher_buffer(&capteur);
    } else {
        printf("\nDemarrage avec un buffer vide.\n");
    }

    printf("\nDebut de simulation\n");
    printf("Le capteur produit et transmet des paquets jusqua epuisement de la batterie.\n");
    printf("A chaque cycle: 1. Production dun paquet, 2. Transmission dun paquet\n");
    printf("Condition darret: Batterie = 0 J (capteur 'mort')\n");
    printf("--------------------------------------------------\n");

    // Production et transmission jusqu'à épuisement de la batterie
    int buffer_virtuel = 0;
    while (capteur.battery > 0) {
        printf("\n Cycle %d \n", temps + 1);
        printf("Batterie au debut du cycle: %.4f J\n", capteur.battery);

        // 1. PRODUCTION D'UN PAQUET
        printf("\nPRODUCTION\n");
        produire_paquet(&capteur);
        paquets_produits++;
         // Calcul des paquets en attente (simulation)
        int paquets_en_attente;
        if (paquets_produits <= 5) {
            // Pour les 5 premiers cycles : 5 - paquets_transmis
            paquets_en_attente = 5 - paquets_transmis;
            if (paquets_en_attente < 0) paquets_en_attente = 0;
        } else {
            // À partir du 6ème cycle : toujours 5 (simulation)
            paquets_en_attente = 5;
        }

        printf("Paquets produits totaux: %d\n", paquets_produits);
        printf("Paquets en attente (simulation): %d/5\n", paquets_en_attente);

        // Afficher l'alerte de mémoire saturée à partir du 6ème cycle
        if (paquets_produits > 5) {
            // L'ID supprimé virtuellement est : paquets_produits - 6
            // Car si on a produit 6 paquets, le premier (ID 0) est supprimé
            int id_supprime = paquets_produits - 6;
            printf("ALERTE : Mémoire saturée. Suppression du paquet ID=%d pour libérer de l'espace.\n",
                   id_supprime);
        }


        // 2. TRANSMISSION D'UN PAQUET (si buffer non vide)
        printf("\nTRANSMISSION\n");
        if (capteur.buffer_usage > 0) {
            printf("Tentative de transmission du paquet le plus recent...\n");

            float batterie_avant = capteur.battery;
            printf("Batterie avant transmission: %.4f J\n", batterie_avant);

            // Tenter la transmission
            int transmission_reussie = attempt_transmission(&capteur);

            if (transmission_reussie) {
                paquets_transmis++;  // INCrémenter le compteur
                printf("TRANSMISSION REUSSIE !\n");
                printf("Paquet transmis avec succes.\n");
                printf("Energie consommee: %.4f J\n", batterie_avant - capteur.battery);
                printf("Total paquets transmis: %d\n", paquets_transmis);
            } else {
                printf(" ECHEC DE TRANSMISSION - Batterie insuffisante\n");
                printf("Batterie restante: %.4f J\n", capteur.battery);
                printf("Energie requise: %.4f J\n", energie_transmission);

                // Si échec, la batterie est mise à 0 dans attempt_transmission()
                // On sort de la boucle
                break;
            }

            printf("Batterie apres transmission: %.4f J\n", capteur.battery);
        } else {
            printf("Aucun paquet a transmettre (buffer vide)\n");
        }

        // Affichage de l'état courant
        printf("\nETAT COURANT\n");
        printf("Cycle: %d\n", temps + 1);
        printf("Batterie: %.4f J\n", capteur.battery);
        printf("Buffer: %d/5 paquets\n", capteur.buffer_usage);
        printf("Paquets produits totaux: %d\n", paquets_produits);
        printf("Paquets transmis totaux: %d\n", paquets_transmis);

        // Afficher le contenu du buffer
        afficher_buffer(&capteur);

        temps++;

        // Pause d'une seconde entre les cycles
        sleep(5);

        // Avertissement si batterie faible
        if (capteur.battery < energie_transmission * 1.5) {
            printf("\n  ATTENTION: Batterie faible !\n");
            int transmissions_restantes = (int)(capteur.battery / energie_transmission);
            printf("   Transmissions possibles restantes: %d\n", transmissions_restantes);
        }
    }

    // ============ AFFICHAGE FINAL ============
    printf("\n");
    printf("========================================\n");
    printf("         CAPTEUR HORS SERVICE\n");
    printf("         (Batterie epuisee)\n");
    printf("========================================\n");

    printf("\n=== RAPPORT FINAL ===\n");
    printf("Durée de fonctionnement: %d cycles\n", temps);
    printf("Batterie initiale: %.4f J\n", capteur.battery + (paquets_transmis * energie_transmission));
    printf("Batterie finale: %.4f J\n", capteur.battery);
    printf("Paquets produits au total: %d\n", paquets_produits);
    printf("Paquets transmis avec succes: %d\n", paquets_transmis);
    printf("Paquets non transmis: %d\n", paquets_produits - paquets_transmis);printf("Paquets non transmis: %d\n", paquets_produits - paquets_transmis);
    if (paquets_produits > 0) {
        float taux_transmission = (paquets_transmis * 100.0) / paquets_produits;
        printf("Taux de transmission: %.1f%%\n", taux_transmission);
    }

    printf("Paquets restants dans le buffer: %d/5\n", capteur.buffer_usage);
    printf("Distance a la station: %.2f\n", distance);
    printf("Energie consommee par transmission: %.4f J\n", energie_transmission);
    printf("Energie totale consommee: %.4f J\n", paquets_transmis * energie_transmission);

    // Demander si on veut sauvegarder les 5 derniers paquets
    int choix_sauvegarde;
    printf("\nVoulez-vous sauvegarder les 5 derniers paquets non transmis ?\n");
    printf("1. Oui\n");
    printf("2. Non\n");
    printf("Choix: ");
    scanf("%d", &choix_sauvegarde);

    if (choix_sauvegarde == 1) {
        sauvegarder_etat(&capteur);
        printf(" Sauvegarde effectuee. %d paquets sauvegardes.\n", capteur.buffer_usage);
    }

    // Nettoyage final
    liberer_buffer(&capteur);

    printf("\n=== SIMULATION TERMINEE ===\n");
}

int main() {
    int choix = 0;


    printf(" SIMULATEUR DE CAPTEUR INTELLIGENT\n");
    printf("========================================\n");
    printf("1. Lancer une nouvelle simulation\n");
    printf("2. Quitter\n");
    printf("Choix : ");
    scanf("%d", &choix);

    if (choix == 1) {
        lancer_simulation();
    } else {
        printf("Au revoir !\n");
    }

    return 0;
}
