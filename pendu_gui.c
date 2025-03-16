#include <gtk/gtk.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 100

char motSecret[MAX_WORD_LENGTH] = "ordinateur"; // 🔥 À remplacer par un mot aléatoire plus tard
int lettresTrouvees[MAX_WORD_LENGTH] = {0};
int tentativesRestantes = 10;
GtkWidget *labelMot;
GtkWidget *labelTentatives;
GtkWidget *imagePendu;

// 🔍 Met à jour l'affichage du mot caché (_ _ _ _)
void afficherMotCache() {
    char affichage[MAX_WORD_LENGTH * 2] = "";
    for (int i = 0; motSecret[i] != '\0'; i++) {
        if (lettresTrouvees[i]) {
            char lettre[3] = {motSecret[i], ' ', '\0'};
            strcat(affichage, lettre);
        } else {
            strcat(affichage, "_ ");
        }
    }
    gtk_label_set_text(GTK_LABEL(labelMot), affichage);
}

// 🔥 Met à jour l’image du pendu
void afficherPendu() {
    char chemin[20];
    sprintf(chemin, "pendu%d.png", 10 - tentativesRestantes);
    gtk_image_set_from_file(GTK_IMAGE(imagePendu), chemin);
}

// 🎯 Vérifie si la lettre est correcte
void verifierLettre(GtkWidget *widget, gpointer data) {
    char lettre = *(char *)data;
    int trouve = 0;
    
    for (int i = 0; motSecret[i] != '\0'; i++) {
        if (motSecret[i] == lettre) {
            lettresTrouvees[i] = 1;
            trouve = 1;
        }
    }

    if (!trouve) {
        tentativesRestantes--;
    }

    afficherMotCache();
    afficherPendu();

    char texteTentatives[50];
    sprintf(texteTentatives, "Tentatives restantes : %d", tentativesRestantes);
    gtk_label_set_text(GTK_LABEL(labelTentatives), texteTentatives);

    if (tentativesRestantes == 0) {
        gtk_label_set_text(GTK_LABEL(labelMot), "Perdu ! Le mot était : ordinateur");
    } else {
        int motTrouve = 1;
        for (int i = 0; motSecret[i] != '\0'; i++) {
            if (!lettresTrouvees[i]) {
                motTrouve = 0;
                break;
            }
        }
        if (motTrouve) {
            gtk_label_set_text(GTK_LABEL(labelMot), "Bravo, vous avez trouvé !");
        }
    }
}

// 🖥️ Interface GTK
void creerFenetre(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    GtkWidget *box;
    
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Jeu du Pendu");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    labelMot = gtk_label_new("_ _ _ _ _ _ _ _ _");
    gtk_box_pack_start(GTK_BOX(box), labelMot, FALSE, FALSE, 10);

    imagePendu = gtk_image_new_from_file("pendu0.png");
    gtk_box_pack_start(GTK_BOX(box), imagePendu, FALSE, FALSE, 10);

    labelTentatives = gtk_label_new("Tentatives restantes : 10");
    gtk_box_pack_start(GTK_BOX(box), labelTentatives, FALSE, FALSE, 10);

    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 10);

    char lettres[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 26; i++) {
        char *lettre = g_malloc(2);
        lettre[0] = lettres[i];
        lettre[1] = '\0';

        button = gtk_button_new_with_label(lettre);
        g_signal_connect(button, "clicked", G_CALLBACK(verifierLettre), lettre);
        gtk_grid_attach(GTK_GRID(grid), button, i % 7, i / 7, 1, 1);
    }

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.pendu", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(creerFenetre), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
