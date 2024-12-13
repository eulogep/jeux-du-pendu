#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORD_LENGTH 100
#define MAX_ATTEMPTS 10

// Prototypes des fonctions
void afficherMenu();
void jouerPartie();
void afficherScores();
void sauvegarderScore(const char *nom, int score);
char *choisirMot(const char *fichier);
void afficherMotCache(const char *mot, const int lettresTrouvees[]);
int verifierLettre(char lettre, const char *mot, int lettresTrouvees[]);

// Main
int main() {
    int choix;
    do {
        afficherMenu();
        printf("\nVotre choix : ");
        scanf("%d", &choix);
        getchar(); // Pour consommer le \n restant

        switch (choix) {
            case 1:
                jouerPartie();
                break;
            case 2:
                afficherScores();
                break;
            case 3:
                printf("\nMerci d'avoir joué. A bientôt !\n");
                break;
            default:
                printf("\nChoix invalide, veuillez réessayer.\n");
        }
    } while (choix != 3);

    return 0;
}

// Affichage du menu principal
void afficherMenu() {
    printf("\n=========== Jeu du Pendu ===========\n");
    printf("1. Jouer une partie\n");
    printf("2. Afficher les scores\n");
    printf("3. Quitter\n");
    printf("====================================\n");
}

// Fonction pour jouer une partie
void jouerPartie() {
    char nomJoueur[50];
    char *mot;
    int lettresTrouvees[MAX_WORD_LENGTH] = {0};
    int tentativesRestantes = MAX_ATTEMPTS;
    int motTrouve = 0;
    char lettre;

    printf("\nEntrez votre nom : ");
    fgets(nomJoueur, sizeof(nomJoueur), stdin);
    nomJoueur[strcspn(nomJoueur, "\n")] = '\0'; // Supprimer le \n
    printf("\nChoisissez un niveau de difficulté (1 = Facile, 2 = Moyen, 3 = Difficile) : ");
    int niveau;
    scanf("%d", &niveau);
    getchar(); // Consommer le \n restant

    const char *fichierMots;
    switch (niveau) {
        case 1:
            fichierMots = "niveau1.txt";
            break;
        case 2:
            fichierMots = "niveau2.txt";
            break;
        case 3:
            fichierMots = "niveau3.txt";
            break;
        default:
            printf("Niveau invalide. Par défaut, niveau Facile sera utilisé.\n");
            fichierMots = "niveau1.txt";
    }

    mot = choisirMot(fichierMots);
    if (!mot) {
        printf("Erreur lors du chargement des mots.\n");
        return;
    }

    while (tentativesRestantes > 0 && !motTrouve) {
        printf("\nMot à deviner : ");
        afficherMotCache(mot, lettresTrouvees);

        printf("\nTentatives restantes : %d\n", tentativesRestantes);
        printf("Proposez une lettre : ");
        scanf(" %c", &lettre);
        getchar();
        lettre = tolower(lettre);

        if (!isalpha(lettre)) {
            printf("\nVeuillez entrer une lettre valide.\n");
            continue;
        }

        if (verifierLettre(lettre, mot, lettresTrouvees)) {
            printf("\nBonne réponse !\n");
        } else {
            printf("\nLettre incorrecte.\n");
            tentativesRestantes--;
        }

        // Vérifier si le mot est entièrement découvert
        motTrouve = 1;
        for (int i = 0; mot[i] != '\0'; i++) {
            if (!lettresTrouvees[i]) {
                motTrouve = 0;
                break;
            }
        }
    }

    if (motTrouve) {
        printf("\nFélicitations, vous avez trouvé le mot : %s\n", mot);
        sauvegarderScore(nomJoueur, tentativesRestantes);
    } else {
        printf("\nDommage, vous avez perdu. Le mot était : %s\n", mot);
    }

    free(mot);
}

// Afficher les scores depuis un fichier
void afficherScores() {
    FILE *fichier = fopen("scores.txt", "r");
    if (!fichier) {
        printf("\nAucun score disponible pour le moment.\n");
        return;
    }

    char ligne[100];
    printf("\n=== Scores ===\n");
    while (fgets(ligne, sizeof(ligne), fichier)) {
        printf("%s", ligne);
    }
    fclose(fichier);
}

// Sauvegarder un score dans un fichier
void sauvegarderScore(const char *nom, int score) {
    FILE *fichier = fopen("scores.txt", "a");
    if (!fichier) {
        printf("\nErreur lors de la sauvegarde du score.\n");
        return;
    }

    fprintf(fichier, "%s : %d\n", nom, score);
    fclose(fichier);
}

// Choisir un mot aléatoire depuis un fichier
char *choisirMot(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (!f) {
        return NULL;
    }

    char **mots = NULL;
    size_t nombreMots = 0;
    char buffer[MAX_WORD_LENGTH];

    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Supprimer le \n
        char *mot = strdup(buffer);
        mots = realloc(mots, (nombreMots + 1) * sizeof(char *));
        mots[nombreMots++] = mot;
    }

    fclose(f);

    if (nombreMots == 0) {
        return NULL;
    }

    srand(time(NULL));
    int index = rand() % nombreMots;
    char *motChoisi = strdup(mots[index]);

    for (size_t i = 0; i < nombreMots; i++) {
        free(mots[i]);
    }
    free(mots);

    return motChoisi;
}

// Afficher le mot caché avec les lettres trouvées
void afficherMotCache(const char *mot, const int lettresTrouvees[]) {
    for (int i = 0; mot[i] != '\0'; i++) {
        if (lettresTrouvees[i]) {
            printf("%c", mot[i]);
        } else {
            printf("_");
        }
    }
    printf("\n");
}

// Vérifier si une lettre est dans le mot
int verifierLettre(char lettre, const char *mot, int lettresTrouvees[]) {
    int trouve = 0;
    for (int i = 0; mot[i] != '\0'; i++) {
        if (mot[i] == lettre && !lettresTrouvees[i]) {
            lettresTrouvees[i] = 1;
            trouve = 1;
        }
    }
    return trouve;
}
