#include <stdio.h>
#include <stdlib.h>

#define NB_PIXELS 48
#define GRAINE 42
#define ETA 0.01f

// Structure d'un motif (image de 48 pixels + classe attendue d)
typedef struct {
    int x[NB_PIXELS];
    int d;
} Motif;

// 1. Lecture du fichier motif (8 lignes x 6 pixels + 1 ligne label)
int lire_motif(const char *chemin, Motif *m) {
    FILE *f = fopen(chemin, "r");
    if (!f) {
        printf("Erreur : impossible d'ouvrir %s\n", chemin);
        return 0;
    }

    int idx = 0;
    char buffer[64];

    for (int ligne = 0; ligne < 8; ligne++) {
        if (fgets(buffer, sizeof(buffer), f) == NULL) break;
        for (int col = 0; buffer[col] != '\0' && buffer[col] != '\n' && buffer[col] != '\r'; col++) {
            if (col < 6 && idx < NB_PIXELS) {
                if (buffer[col] == '*') {
                    m->x[idx++] = 1;
                } else if (buffer[col] == '.') {
                    m->x[idx++] = 0;
                }
            }
        }
    }

    if (fgets(buffer, sizeof(buffer), f) != NULL) {
        m->d = atoi(buffer);
    }

    fclose(f);
    return 1;
}

// 2. Calcul du score : a = sum(w_j * x_j) + b
float calculer_score(const int x[NB_PIXELS], const float w[NB_PIXELS], float b) {
    float somme = 0.0f;
    for (int j = 0; j < NB_PIXELS; j++) {
        somme += w[j] * (float)x[j];
    }
    return somme + b;
}

// 3. Règle de décision : 1 si a >= 0 sinon 0
int predire(float a) {
    return (a >= 0.0f) ? 1 : 0;
}

// 4. Mise à jour des poids et du biais
int mise_a_jour(const int x[NB_PIXELS], float w[NB_PIXELS], float *b, int d, int y_pred, float eta) {
    int e = d - y_pred;
    if (e != 0) {
        for (int j = 0; j < NB_PIXELS; j++) {
            w[j] += eta * (float)e * (float)x[j];
        }
        *b += eta * (float)e;
    }
    return e;
}

// 5. Initialisation : w dans [-0.1; 0.1], b = -0.5
void initialiser_modele(float w[NB_PIXELS], float *b, unsigned int graine) {
    srand(graine);
    for (int j = 0; j < NB_PIXELS; j++) {
        float r = (float)rand() / (float)RAND_MAX;
        w[j] = -0.1f + r * 0.2f;
    }
    *b = -0.5f;
}

// Programme principal de test
int main(void) {
    float w[NB_PIXELS];
    float b;
    Motif zero, un;

    initialiser_modele(w, &b, GRAINE);
    printf("Modèle initialisé. Biais initial b = %.2f\n", b);

    if (!lire_motif("zero.txt", &zero) || !lire_motif("un.txt", &un)) {
        return 1;
    }
    printf("Motifs zero.txt et un.txt charges avec succes !\n");

    float score_zero = calculer_score(zero.x, w, b);
    float score_un = calculer_score(un.x, w, b);

    printf("Score '0' : %.4f -> Predit: %d (Attendu: %d)\n", score_zero, predire(score_zero), zero.d);
    printf("Score '1' : %.4f -> Predit: %d (Attendu: %d)\n", score_un, predire(score_un), un.d);

    return 0;
}