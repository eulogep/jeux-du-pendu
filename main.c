#include <stdio.h>
#include <string.h>

//AFFICHER LE MOTS DE PASSE

void afficherMotCache(char* motSecret, int* lettresDevinees) {
    int longueur = strlen(motSecret);
    for (int i = 0; i < longueur; i++) {
        if (lettresDevinees[i]) {
            printf("%c ", motSecret[i]);
        } else {
            printf("_ ");
        }
    }
    printf("\n");
}

//Affichage du pendu

void afficherPendu(int essaisRestants) {
    switch (essaisRestants) {
        case 6:
            printf(" +---+\n     |\n     |\n     |\n    ===\n");
            break;
        case 5:
            printf(" +---+\n O   |\n     |\n     |\n    ===\n");
            break;
        case 4:
            printf(" +---+\n O   |\n |   |\n     |\n    ===\n");
            break;
        case 3:
            printf(" +---+\n O   |\n/|   |\n     |\n    ===\n");
            break;
        case 2:
            printf(" +---+\n O   |\n/|\\  |\n     |\n    ===\n");
            break;
        case 1:
            printf(" +---+\n O   |\n/|\\  |\n/    |\n    ===\n");
            break;
        case 0:
            printf(" +---+\n O   |\n/|\\  |\n/ \\  |\n    ===\n");
            break;
    }
}



// Gestion des entr�es utilisateur
char lireLettre() {
    char lettre;
    printf("Entrez une lettre : ");
    scanf(" %c", &lettre);
    // Validation de l'entr�e
    if ((lettre >= 'a' && lettre <= 'z') || (lettre >= 'A' && lettre <= 'Z')) {
        return lettre;
    } else {
        printf("Entr�e invalide. Veuillez entrer une lettre.\n");
        return lireLettre(); // Recommence si l'entr�e n'est pas valide
    }
}

//Exemple d'utilisation

int main() {
    char motSecret[] = "programmation";
    int lettresDevinees[14] = {0}; // Initialiser � 0, aucune lettre devin�e
    int essaisRestants = 6;

    // Exemple de boucle de jeu simple
    while (essaisRestants > 0) {
        afficherMotCache(motSecret, lettresDevinees);
        afficherPendu(essaisRestants);

        char lettre = lireLettre();
        // Ajoute ici la logique pour v�rifier la lettre et mettre � jour lettresDevinees

        // Exemple de d�compte d'essais si la lettre est incorrecte
        essaisRestants--; // � ajuster selon la logique r�elle de ton jeu
    }

    return 0;
}
