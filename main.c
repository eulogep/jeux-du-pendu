#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Constantes
#define MAX_MOTS 100
#define TAILLE_MAX_MOT 50
#define MAX_ESSAIS 10

// Prototypes
void afficherMotCache(const char* motSecret, const int* lettresDevin�es, int taille);
void afficherPendu(int essaisRestants);
char lireLettre();
int validerLettre(const char* motSecret, char lettre, int* lettresDevin�es, int taille);
void jouerUnePartie(int niveau, int* score);
void sauvegarderScore(const char* nomJoueur, int score);
void chargerMots(const char* fichier, char mots[][TAILLE_MAX_MOT], int* nbMots);
int choisirDifficulte();
char* choisirMot(char mots[][TAILLE_MAX_MOT], int nbMots);

int main() {
    int choix;
    int score = 0;
    char nomJoueur[50];
    char mots[MAX_MOTS][TAILLE_MAX_MOT];
    int nbMots = 0;

    printf("Bienvenue dans le jeu du pendu !\n");
    printf("Entrez votre nom : ");
    scanf("%s", nomJoueur);

    // Charger les mots depuis un fichier
    chargerMots("mots.txt", mots, &nbMots);

    // Menu principal
    do {
        printf("\nMenu :\n");
        printf("1. Jouer une partie\n");
        printf("2. Afficher les scores\n");
        printf("3. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: {
                int niveau = choisirDifficulte();
                jouerUnePartie(niveau, &score);
                sauvegarderScore(nomJoueur, score);
                break;
            }
            case 2:
                printf("Affichage des scores (� impl�menter).\n");
                break;
            case 3:
                printf("Merci d'avoir jou� !\n");
                break;
            default:
                printf("Choix invalide, r�essayez.\n");
        }
    } while (choix != 3);

    return 0;
}

// Fonction pour afficher le mot cach�
void afficherMotCache(const char* motSecret, const int* lettresDevin�es, int taille) {
    for (int i = 0; i < taille; i++) {
        if (lettresDevin�es[i])
            printf("%c ", motSecret[i]);
        else
            printf("_ ");
    }
    printf("\n");
}

// Fonction pour afficher l'�tat du pendu
void afficherPendu(int essaisRestants) {
    const char* pendu[] = {
        "  ----\n  |  |\n  |   \n  |   \n  |   \n /|\\ \n",
        "  ----\n  |  |\n  |  O\n  |   \n  |   \n /|\\ \n",
        "  ----\n  |  |\n  |  O\n  |  |\n  |   \n /|\\ \n",
        "  ----\n  |  |\n  |  O\n  | /|\n  |   \n /|\\ \n",
        "  ----\n  |  |\n  |  O\n  | /|\\\n  |   \n /|\\ \n",
        "  ----\n  |  |\n  |  O\n  | /|\\\n  | / \n /|\\ \n",
        "  ----\n  |  |\n  |  O\n  | /|\\\n  | / \\\n /|\\ \n"
    };
    int index = 6 - essaisRestants;
    printf("%s\n", pendu[index]);
}

// Fonction pour lire une lettre
char lireLettre() {
    char lettre;
    printf("Entrez une lettre : ");
    scanf(" %c", &lettre);
    return tolower(lettre);
}

// Fonction pour valider une lettre
int validerLettre(const char* motSecret, char lettre, int* lettresDevin�es, int taille) {
    int correct = 0;
    for (int i = 0; i < taille; i++) {
        if (motSecret[i] == lettre && !lettresDevin�es[i]) {
            lettresDevin�es[i] = 1;
            correct = 1;
        }
    }
    return correct;
}

// Fonction pour jouer une partie
void jouerUnePartie(int niveau, int* score) {
    char mots[MAX_MOTS][TAILLE_MAX_MOT];
    int nbMots = 0;
    int essaisRestants;
    int lettresDevin�es[TAILLE_MAX_MOT] = {0};

    // Charger les mots et choisir la difficult�
    chargerMots("mots.txt", mots, &nbMots);
    essaisRestants = MAX_ESSAIS - niveau * 2;

    // Choisir un mot
    char* motSecret = choisirMot(mots, nbMots);
    int tailleMot = strlen(motSecret);

    // D�but de la partie
    printf("\nLe mot contient %d lettres.\n", tailleMot);
    while (essaisRestants > 0) {
        afficherMotCache(motSecret, lettresDevin�es, tailleMot);
        afficherPendu(essaisRestants);

        char lettre = lireLettre();
        if (!validerLettre(motSecret, lettre, lettresDevin�es, tailleMot)) {
            essaisRestants--;
            printf("Lettre incorrecte. Il vous reste %d essais.\n", essaisRestants);
        }

        // V�rifier si toutes les lettres sont trouv�es
        int toutesTrouvees = 1;
        for (int i = 0; i < tailleMot; i++) {
            if (!lettresDevin�es[i]) {
                toutesTrouvees = 0;
                break;
            }
        }
        if (toutesTrouvees) {
            printf("F�licitations ! Vous avez trouv� le mot : %s\n", motSecret);
            *score += essaisRestants * 10;
            return;
        }
    }
    printf("Dommage, vous avez perdu ! Le mot �tait : %s\n", motSecret);
}

// Fonction pour sauvegarder un score
void sauvegarderScore(const char* nomJoueur, int score) {
    FILE* fichier = fopen("scores.txt", "a");
    if (fichier) {
        fprintf(fichier, "%s : %d\n", nomJoueur, score);
        fclose(fichier);
    }
}

// Fonction pour charger les mots depuis un fichier
void chargerMots(const char* fichier, char mots[][TAILLE_MAX_MOT], int* nbMots) {
    FILE* f = fopen(fichier, "r");
    if (f) {
        while (fscanf(f, "%s", mots[*nbMots]) != EOF) {
            (*nbMots)++;
        }
        fclose(f);
    }
}

// Fonction pour choisir la difficult�
int choisirDifficulte() {
    int niveau;
    printf("Choisissez un niveau de difficult� :\n");
    printf("1. Facile\n");
    printf("2. Moyen\n");
    printf("3. Difficile\n");
    printf("Votre choix : ");
    scanf("%d", &niveau);
    return niveau - 1;
}

// Fonction pour choisir un mot al�atoirement
char* choisirMot(char mots[][TAILLE_MAX_MOT], int nbMots) {
    srand(time(NULL));
    return mots[rand() % nbMots];
}
