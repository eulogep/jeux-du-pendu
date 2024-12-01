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
void afficherMotCache(const char* motSecret, const int* lettresDevinées, int taille);
void afficherPendu(int essaisRestants);
char lireLettre();
int validerLettre(const char* motSecret, char lettre, int* lettresDevinées, int taille);
void jouerUnePartie(int niveau, int* score);
void sauvegarderScore(const char* nomJoueur, int score);
void chargerMotsParNiveau(int niveau, char mots[][TAILLE_MAX_MOT], int* nbMots); // Modification : Chargement par niveau
char* choisirMot(char mots[][TAILLE_MAX_MOT], int nbMots);
int choisirDifficulte();
int genererNombreAleatoire(); // Modification : Ajout de la génération aléatoire

int main() {
    int choix;
    int score = 0;
    char nomJoueur[50];

    printf("Bienvenue dans le jeu du pendu !\n");
    printf("Entrez votre nom : ");
    scanf("%s", nomJoueur);

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
                printf("Affichage des scores (à implémenter).\n");
                break;
            case 3:
                printf("Merci d'avoir joué !\n");
                break;
            default:
                printf("Choix invalide, réessayez.\n");
        }
    } while (choix != 3);

    return 0;
}

// Fonction pour afficher le mot caché
void afficherMotCache(const char* motSecret, const int* lettresDevinées, int taille) {
    for (int i = 0; i < taille; i++) {
        if (lettresDevinées[i])
            printf("%c ", motSecret[i]);
        else
            printf("_ ");
    }
    printf("\n");
}

// Fonction pour afficher l'état du pendu
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
int validerLettre(const char* motSecret, char lettre, int* lettresDevinées, int taille) {
    int correct = 0;
    for (int i = 0; i < taille; i++) {
        if (motSecret[i] == lettre && !lettresDevinées[i]) {
            lettresDevinées[i] = 1;
            correct = 1;
        }
    }
    return correct;
}

// Fonction pour générer un nombre aléatoire (Ajouté)
int genererNombreAleatoire() {
    srand(time(NULL));
    return rand() % 100;
}

// Fonction pour charger les mots selon le niveau (Modification)
void chargerMotsParNiveau(int niveau, char mots[][TAILLE_MAX_MOT], int* nbMots) {
    char nomFichier[20];
    snprintf(nomFichier, sizeof(nomFichier), "niveau%d.txt", niveau); // Charge un fichier spécifique au niveau

    FILE* fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", nomFichier);
        exit(1);
    }

    *nbMots = 0;
    while (fscanf(fichier, "%s", mots[*nbMots]) != EOF) {
        (*nbMots)++;
    }
    fclose(fichier);
}

// Fonction pour choisir un mot selon le niveau (Modification)
char* choisirMot(char mots[][TAILLE_MAX_MOT], int nbMots) {
    int indexAleatoire = genererNombreAleatoire() % nbMots;
    return mots[indexAleatoire];
}

// Fonction pour jouer une partie
void jouerUnePartie(int niveau, int* score) {
    char mots[MAX_MOTS][TAILLE_MAX_MOT];
    int nbMots = 0;
    int essaisRestants;
    int lettresDevinées[TAILLE_MAX_MOT] = {0};

    // Charger les mots pour le niveau choisi
    chargerMotsParNiveau(niveau, mots, &nbMots);
    essaisRestants = MAX_ESSAIS - niveau * 2;

    // Choisir un mot
    char* motSecret = choisirMot(mots, nbMots);
    int tailleMot = strlen(motSecret);

    printf("\nLe mot contient %d lettres.\n", tailleMot);

    while (essaisRestants > 0) {
        afficherMotCache(motSecret, lettresDevinées, tailleMot);
        afficherPendu(essaisRestants);

        char lettre = lireLettre();
        if (!validerLettre(motSecret, lettre, lettresDevinées, tailleMot)) {
            essaisRestants--;
            printf("Lettre incorrecte. Il vous reste %d essais.\n", essaisRestants);
        }

        int toutesTrouvees = 1;
        for (int i = 0; i < tailleMot; i++) {
            if (!lettresDevinées[i]) {
                toutesTrouvees = 0;
                break;
            }
        }
        if (toutesTrouvees) {
            printf("Félicitations ! Vous avez trouvé le mot : %s\n", motSecret);
            *score += essaisRestants * 10;
            return;
        }
    }
    printf("Dommage, vous avez perdu ! Le mot était : %s\n", motSecret);
}

// Fonction pour choisir la difficulté
int choisirDifficulte() {
    int niveau;
    printf("Choisissez un niveau de difficulté :\n");
    printf("1. Facile\n");
    printf("2. Moyen\n");
    printf("3. Difficile\n");
    printf("Votre choix : ");
    scanf("%d", &niveau);
    return niveau;
}

// Fonction pour sauvegarder un score
void sauvegarderScore(const char* nomJoueur, int score) {
    FILE* fichier = fopen("scores.txt", "a");
    if (fichier) {
        fprintf(fichier, "%s : %d\n", nomJoueur, score);
        fclose(fichier);
    }
}
