#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORD_LENGTH 100
#define MAX_ATTEMPTS 10
#define MAX_SCORES 100
#define MAX_AFFICHAGE 5  // Afficher uniquement les 5 meilleurs scores
#define MAX_PLAYERS 10  // Nombre maximum de joueurs

// Couleurs ANSI pour le terminal
#define RESET   "\033[0m"
#define RED     "\033[31m"  // Rouge pour les erreurs
#define GREEN   "\033[32m"  // Vert pour les succès
#define YELLOW  "\033[33m"  // Jaune pour le menu
#define BLUE    "\033[34m"  // Bleu pour les scores
#define CYAN    "\033[36m"  // Cyan pour l'affichage du pendu

// 🔍 Fonction pour trouver un indice dans un fichier
char *trouverIndice(const char *fichier, const char *mot) {
    FILE *f = fopen(fichier, "r");
    if (!f) {
        return NULL;
    }

    char ligne[256];
    char *indice = NULL;

    while (fgets(ligne, sizeof(ligne), f)) {
        char *token = strtok(ligne, ":"); // Séparer le mot et l’indice
        if (token && strcmp(token, mot) == 0) {
            token = strtok(NULL, "\n");
            if (token) {
                indice = strdup(token); // Stocker l’indice trouvé
            }
            break;
        }
    }
    fclose(f);
    return indice; // Retourne l’indice ou NULL si non trouvé
}





// Structure pour stocker les scores
typedef struct {
    char nom[50];
    int score;
} Score;


// Prototypes des fonctions
void afficherMenu();
void jouerPartie();
void afficherScores();
void sauvegarderScore(const char *nom, int score);
char *choisirMot(const char *fichier);
void afficherMotCache(const char *mot, const int lettresTrouvees[]);
int verifierLettre(char lettre, const char *mot, int lettresTrouvees[]);
void afficherPendu(int tentativesRestantes);
void jouerChallenge();  // 🔥 Déclaration du mode Challenge
void jouerContreLaMontre() {
    printf(YELLOW "\n=== Mode Contre-la-montre active ! Vous avez 20 secondes par lettre. ===\n" RESET);
    jouerPartie(); // On utilise la même fonction, mais avec le chrono activé
}

int main() {
    int choix;
    do {
        afficherMenu();
        printf("\nVotre choix : ");
        if (scanf("%d", &choix) != 1) {
            printf("\nEntree invalide. Veuillez entrer un chiffre.\n");
            while (getchar() != '\n'); // Nettoyage du buffer
            continue;
        }
        getchar(); // Pour consommer le \n restant

        switch (choix) {
            case 1:
                jouerPartie();
                break;
            case 2:
                afficherScores();
                break;
            case 3:
                printf("\nMerci d'avoir joue. A bientot !\n");
                break;
                case 4:
                jouerChallenge(); //  Lancer le mode Challenge
                break;
            case 5:
            jouerContreLaMontre(); // ⏳ Mode Contre-la-montre
            break;    
            default:
                printf("\nChoix invalide, veuillez réessayer.\n");
        }
    } while (choix != 3);

    return 0;
}

void afficherMenu() {
    printf(YELLOW "\n===========  Jeu du Pendu  ===========\n" RESET);
    printf(YELLOW "1. Jouer une partie\n" RESET);
    printf(YELLOW "2. Afficher les scores\n" RESET);
    printf(YELLOW "3. Quitter\n" RESET);
    printf(YELLOW "4. Mode Challenge (enchainer plusieurs mots)\n" RESET);
    printf(YELLOW "5. Mode Contre-la-montre  (temps limite)\n" RESET);
    printf(YELLOW "==========================================\n" RESET);
}



// Fonction prototype
void sauvegarderScore(const char *nom, int score);
char *choisirMot(const char *fichier);
void afficherMotCache(const char *mot, const int lettresTrouvees[]);
int verifierLettre(char lettre, const char *mot, int lettresTrouvees[]);
void afficherPendu(int tentativesRestantes);  

//  **Mode multijoueur**
void jouerPartie() {
    char rejouer ;
    int nbJoueurs;
    Score joueurs[MAX_PLAYERS];

    printf("\nNombre de joueurs (1 - %d) : ", MAX_PLAYERS);
    while (scanf("%d", &nbJoueurs) != 1 || nbJoueurs < 1 || nbJoueurs > MAX_PLAYERS) {
        printf("\nVeuillez entrer un nombre valide entre 1 et %d : ", MAX_PLAYERS);
        while (getchar() != '\n'); // Nettoyage du buffer
    }
    getchar(); // Consommer le \n restant

    for (int i = 0; i < nbJoueurs; i++) {
        char *mot;
        int lettresTrouvees[MAX_WORD_LENGTH] = {0};
        int tentativesRestantes = 10;
        int motTrouve = 0;
        char lettre;
        int niveau;

        printf("\nJoueur %d, entrez votre nom : ", i + 1);
        fgets(joueurs[i].nom, sizeof(joueurs[i].nom), stdin);
        joueurs[i].nom[strcspn(joueurs[i].nom, "\n")] = '\0';

        printf("\nChoisissez un niveau (1 = Facile, 2 = Moyen, 3 = Difficile) : ");
        while (scanf("%d", &niveau) != 1 || niveau < 1 || niveau > 3) {
            printf("\nVeuillez entrer un nombre valide entre 1 et 3 : ");
            while (getchar() != '\n');
        }
        getchar(); // Consommer le \n restant

        const char *fichierMots = (niveau == 1) ? "niveau1.txt" : (niveau == 2) ? "niveau2.txt" : "niveau3.txt";

        mot = choisirMot(fichierMots);
           

        if (!mot) {
            printf("\nErreur lors du chargement des mots.\n");
            return;
        }
            // Déterminer le fichier d'indices à utiliser
        const char *fichierIndices = (niveau == 1) ? "indices1.txt" : (niveau == 2) ? "indices2.txt" : "indices3.txt";
        char *indice = trouverIndice(fichierIndices, mot);
        int erreurs = 0;

        while (tentativesRestantes > 0 && !motTrouve) {
            afficherMotCache(mot, lettresTrouvees);
            afficherPendu(tentativesRestantes);
            printf("\nTentatives restantes : %d\n", tentativesRestantes);

                    // 🔥 Début du chronomètre
            time_t start_time = time(NULL);
            time_t current_time;
            
            printf("Proposez une lettre (vous avez 20 secondes) : ");

            // ✅ Attendre l’entrée du joueur avec une limite de temps
            while (1) {
                current_time = time(NULL);
                if (current_time - start_time > 20) {
                    printf(RED "\n⏳ Temps écoulé ! Vous perdez une tentative.\n" RESET);
                    tentativesRestantes--;
                    break;
                }
                
                // ✅ Vérifier si une lettre est entrée
                if (scanf(" %c", &lettre) == 1 && isalpha(lettre)) {
                    getchar(); // Nettoyer le buffer
                    lettre = tolower(lettre);
                    break;
                }
            }

            // ❌ Si le joueur n’a pas entré de lettre, on passe à la prochaine tentative
            if (current_time - start_time > 20) {
                continue;
            }



            printf("Proposez une lettre : ");

            if (scanf(" %c", &lettre) != 1 || !isalpha(lettre)) {
                printf("\nVeuillez entrer une lettre valide.\n");
                while (getchar() != '\n');
                continue;
            }
            getchar();
            lettre = tolower(lettre);

            if (verifierLettre(lettre, mot, lettresTrouvees)) {
                printf(GREEN "\nBonne reponse !\n" RESET);
            } else {
                printf(RED "\nLettre incorrecte.\n" RESET);
                tentativesRestantes--;
                erreurs++;

                        //  Afficher l’indice après 3 erreurs
                if (erreurs == 3 && indice) {
                    printf(YELLOW "\n Indice : %s\n" RESET, indice);
                }
            }
            

            motTrouve = 1;
            for (int j = 0; mot[j] != '\0'; j++) {
                if (!lettresTrouvees[j]) {
                    motTrouve = 0;
                    break;
                }
            }
        }

        if (motTrouve) {
            printf("\nFelicitations, %s, vous avez trouve le mot : %s\n", joueurs[i].nom, mot);
            printf("\nAppuyez sur Entree pour revenir au menu...\n");
            getchar();
        } else {
            printf("\nDommage, %s, vous avez perdu. Le mot etait : %s\n", joueurs[i].nom, mot);
            printf("\nAppuyez sur Entree pour revenir au menu...\n");
            getchar();
        }
        if (indice) {
            free(indice);
        }

        joueurs[i].score = tentativesRestantes; // Score basé sur les tentatives restantes
        sauvegarderScore(joueurs[i].nom, joueurs[i].score);

        free(mot);
        //  Demander au joueur s'il veut rejouer
        printf("\nVoulez-vous rejouer ? (O/N) : ");
        while (scanf(" %c", &rejouer) != 1 || (rejouer != 'O' && rejouer != 'N' && rejouer != 'o' && rejouer != 'n')) {
            printf("\nVeuillez entrer 'O' pour rejouer ou 'N' pour retourner au menu : ");
            while (getchar() != '\n');
        }
        getchar(); // Consommer le \n restant
        
    }while (rejouer == 'O' || rejouer == 'o'); //  Si "O", la partie recommence !

    //  Affichage du classement final
    printf("\n=== Classement Final ===\n");
    for (int i = 0; i < nbJoueurs; i++) {
        printf("%d. %s - Score : %d\n", i + 1, joueurs[i].nom, joueurs[i].score);
    }
}
// Fonction pour jouer le mode Challenge
void jouerChallenge() {
    char nomJoueur[50];
    int totalScore = 0; //  Score total du challenge
    int niveau;
    int continuer = 1; // Variable pour savoir si on continue ou pas

    printf("\nEntrez votre nom : ");
    fgets(nomJoueur, sizeof(nomJoueur), stdin);
    nomJoueur[strcspn(nomJoueur, "\n")] = '\0';

    printf("\nChoisissez un niveau de difficulte (1 = Facile, 2 = Moyen, 3 = Difficile) : ");
    while (scanf("%d", &niveau) != 1 || niveau < 1 || niveau > 3) {
        printf("\nVeuillez entrer un nombre valide entre 1 et 3 : ");
        while (getchar() != '\n');
    }
    getchar(); // Consommer le \n restant

    const char *fichierMots = (niveau == 1) ? "niveau1.txt" : (niveau == 2) ? "niveau2.txt" : "niveau3.txt";
    const char *fichierIndices = (niveau == 1) ? "indices1.txt" : (niveau == 2) ? "indices2.txt" : "indices3.txt";

    while (continuer) {
        char *mot = choisirMot(fichierMots);
        if (!mot) {
            printf("\nErreur lors du chargement des mots.\n");
            return;
        }

        char *indice = trouverIndice(fichierIndices, mot);
        int lettresTrouvees[MAX_WORD_LENGTH] = {0};
        int tentativesRestantes = 10;
        int motTrouve = 0;
        int erreurs = 0;
        char lettre;

        while (tentativesRestantes > 0 && !motTrouve) {
            afficherMotCache(mot, lettresTrouvees);
            afficherPendu(tentativesRestantes);
            printf("\nTentatives restantes : %d\n", tentativesRestantes);
            printf("Proposez une lettre : ");

            if (scanf(" %c", &lettre) != 1 || !isalpha(lettre)) {
                printf("\nVeuillez entrer une lettre valide.\n");
                while (getchar() != '\n');
                continue;
            }
            getchar();
            lettre = tolower(lettre);

            if (verifierLettre(lettre, mot, lettresTrouvees)) {
                printf(GREEN "\nBonne reponse !\n" RESET);
            } else {
                printf(RED "\nLettre incorrecte.\n" RESET);
                tentativesRestantes--;
                erreurs++;

                // Afficher l’indice après 3 erreurs
                if (erreurs == 3 && indice) {
                    printf(YELLOW "\n Indice : %s\n" RESET, indice);
                }
            }

            motTrouve = 1;
            for (int i = 0; mot[i] != '\0'; i++) {
                if (!lettresTrouvees[i]) {
                    motTrouve = 0;
                    break;
                }
            }
        }

        if (motTrouve) {
            printf("\n Felicitations, vous avez trouve le mot : %s\n", mot);
            totalScore += 10; //  +10 points par mot trouvé
            printf(GREEN "\n Score actuel : %d points\n" RESET, totalScore);
        } else {
            afficherPendu(tentativesRestantes);
            printf("\n Dommage, vous avez perdu. Le mot etait : %s\n", mot);
            continuer = 0; //  Fin du challenge
        }

        if (indice) free(indice);
        free(mot);
    }

    printf("\nChallenge termine, %s ! Score final : %d points\n", nomJoueur, totalScore);
    sauvegarderScore(nomJoueur, totalScore);
}



void afficherPendu(int tentativesRestantes) {
    const char *pendu[] = {
        CYAN "\n +---+\n |   |\n O   |\n/|\\  |\n/ \\  |\n     |\n=========\n" RESET,
        CYAN "\n +---+\n |   |\n O   |\n/|\\  |\n/    |\n     |\n=========\n" RESET,
        CYAN "\n +---+\n |   |\n O   |\n/|\\  |\n     |\n     |\n=========\n" RESET,
        CYAN "\n +---+\n |   |\n O   |\n/|   |\n     |\n     |\n=========\n" RESET,
        CYAN "\n +---+\n |   |\n O   |\n |   |\n     |\n     |\n=========\n" RESET,
        CYAN "\n +---+\n |   |\n O   |\n     |\n     |\n     |\n=========\n" RESET
    };
    int index = 10 - tentativesRestantes;  // Ajuste l'affichage
    if (index >= 0 && index < 6) {
        printf("%s", pendu[index]);
    }
}


// Choisir un mot al�atoire depuis un fichier
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

// Afficher le mot cach� avec les lettres trouv�es
void afficherMotCache(const char *mot, const int lettresTrouvees[]) {
    printf("\nMot : ");
    for (int i = 0; mot[i] != '\0'; i++) {
        if (lettresTrouvees[i]) {
            printf(" %c ", mot[i]); // Ajoute des espaces
        } else {
            printf(" _ ");
        }
    }
    printf("\n");
}

// V�rifier si une lettre est dans le mot
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
// Fonction de comparaison pour qsort (ordre décroissant)
int comparerScores(const void *a, const void *b) {
    return ((Score *)b)->score - ((Score *)a)->score;
}
// Fonction pour sauvegarder les scores dans un fichier CSV
void sauvegarderScore(const char *nom, int score) {
    FILE *fichier = fopen("scores.csv", "a"); // Ouverture en mode ajout
    if (!fichier) {
        printf("\nErreur lors de la sauvegarde du score.\n");
        return;
    }

    fprintf(fichier, "%s,%d\n", nom, score);
    fclose(fichier);
}
// Fonction pour afficher uniquement les 5 meilleurs scores
void afficherScores() {
    FILE *fichier = fopen("scores.csv", "r");
    if (!fichier) {
        printf(RED "\nAucun score disponible pour le moment.\n" RESET);
        return;
    }

    Score scores[MAX_SCORES];
    int count = 0;

    while (fscanf(fichier, "%[^,],%d\n", scores[count].nom, &scores[count].score) == 2) {
        count++;
        if (count >= MAX_SCORES) break;
    }
    fclose(fichier);

    qsort(scores, count, sizeof(Score), comparerScores);

    printf(BLUE "\n==================================\n" RESET);
    printf(YELLOW "|    TOP 5 Meilleurs Scores    |\n" RESET);
    printf(BLUE "==================================\n" RESET);
    printf(BLUE "| %-3s | %-15s | %-5s |\n" RESET, "Num", "Joueur", "Score");
    printf(BLUE "----------------------------------\n" RESET);

    for (int i = 0; i < count && i < MAX_AFFICHAGE; i++) {
        printf(BLUE "| %-3d | %-15s | %-5d |\n" RESET, i + 1, scores[i].nom, scores[i].score);
    }

    printf(BLUE "==================================\n" RESET);
}
