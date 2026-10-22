#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ==========================================
 * 1. FONCTIONS DEMANDÉES
 * ========================================== */

/* Lecture du fichier : remplit le tableau x (48 entrées) et la cible d[cite: 2] */
int lire_fichier(const char* chemin, int x[48], int* d) {
    FILE* fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", chemin);
        return 0;
    }

    char ligne[256];
    int index = 0;
    
    /* Lecture des 8 premières lignes pour les pixels */
    for (int i = 0; i < 8; i++) {
        if (fgets(ligne, sizeof(ligne), fichier)) {
            for (int j = 0; j < strlen(ligne); j++) {
                if (ligne[j] == '*') {
                    x[index++] = 1;
                } else if (ligne[j] == '.') {
                    x[index++] = 0;
                }
            }
        }
    }
    
    /* Lecture de la 9e ligne pour la classe[cite: 2] */
    if (fgets(ligne, sizeof(ligne), fichier)) {
        *d = atoi(ligne);
    }
    
    fclose(fichier);
    return 1;
}

/* Calcul du score a = somme(wj * xj) + b[cite: 2] */
float calculer_score(const int x[48], const float w[48], float b) {
    float a = b;
    for (int j = 0; j < 48; j++) {
        a += w[j] * x[j];
    }
    return a;
}

/* Décision : ŷ = 1 si a >= 0, sinon 0[cite: 2] */
int decider(float a) {
    return (a >= 0.0f) ? 1 : 0;
}

/* Mise à jour des poids et du biais selon la règle du perceptron[cite: 2] */
void mettre_a_jour(const int x[48], float w[48], float* b, int d, int y_chapeau, float eta) {
    int e = d - y_chapeau;
    
    /* Mise à jour des poids[cite: 2] */
    for (int j = 0; j < 48; j++) {
        float gj = -e * x[j];
        w[j] = w[j] - (eta * gj);
    }
    
    /* Mise à jour du biais (en utilisant un pointeur pour le modifier)[cite: 2] */
    float gb = -e;
    *b = *b - (eta * gb);
}

/* ==========================================
 * 2. INITIALISATION ET TEST
 * ========================================== */
int main() {
    /* Création d'un fichier de test bidon "zero.txt" pour l'exécution */
    FILE* test_file = fopen("zero.txt", "w");
    if (test_file != NULL) {
        fprintf(test_file, "......\n..**..\n.*..*.\n.*..*.\n.*..*.\n.*..*.\n..**..\n......\n0\n");
        fclose(test_file);
    }

    /* Initialisation demandée par l'étape 1[cite: 2] */
    float b = -0.5f;
    float eta = 0.01f;
    float w[48];

    /* Générateur de nombres aléatoires avec la graine 42[cite: 2] */
    srand(42);
    for (int j = 0; j < 48; j++) {
        /* Génère un float entre -0.1 et 0.1 */
        w[j] = ((float)rand() / RAND_MAX) * 0.2f - 0.1f;
    }

    /* Test de l'algorithme */
    int x_zero[48];
    int d_zero;
    
    if (lire_fichier("zero.txt", x_zero, &d_zero)) {
        float score = calculer_score(x_zero, w, b);
        int prediction = decider(score);
        
        printf("--- Test sur zero.txt ---\n");
        printf("Classe attendue (d) : %d\n", d_zero);
        printf("Score calcule (a)   : %.4f\n", score);
        printf("Prediction (y_chap) : %d\n", prediction);
    }

    return 0;
}