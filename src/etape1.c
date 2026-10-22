#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NB_PIXELS 48
#define GRAINE 42
#define GRAINE_BRUIT 2026
#define ETA 0.01f
#define MAX_EPOQUES 1000
#define NB_ESSAIS 50

typedef struct {
    int x[NB_PIXELS];
    int d;
} Motif;

// 1. Lecture du motif
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

// 2. Score
float calculer_score(const int x[NB_PIXELS], const float w[NB_PIXELS], float b) {
    float somme = 0.0f;
    for (int j = 0; j < NB_PIXELS; j++) {
        somme += w[j] * (float)x[j];
    }
    return somme + b;
}

// 3. Prédiction
int predire(float a) {
    return (a >= 0.0f) ? 1 : 0;
}

// 4. Mise à jour
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

// 5. Initialisation
void initialiser_modele(float w[NB_PIXELS], float *b, unsigned int graine) {
    srand(graine);
    for (int j = 0; j < NB_PIXELS; j++) {
        float r = (float)rand() / (float)RAND_MAX;
        w[j] = -0.1f + r * 0.2f;
    }
    *b = -0.5f;
}

// 6. Entraînement
int entrainer(Motif base[2], float w[NB_PIXELS], float *b, float eta, int max_epoques) {
    FILE *f_csv = fopen("experiments/erreurs_epoques.csv", "w");
    if (f_csv) fprintf(f_csv, "epoque,erreurs\n");

    int epoque_convergence = -1;

    for (int epoque = 1; epoque <= max_epoques; epoque++) {
        int ordre[2] = {0, 1};
        if (rand() % 2 == 1) {
            ordre[0] = 1;
            ordre[1] = 0;
        }

        for (int k = 0; k < 2; k++) {
            int idx = ordre[k];
            float a = calculer_score(base[idx].x, w, *b);
            int y_pred = predire(a);
            mise_a_jour(base[idx].x, w, b, base[idx].d, y_pred, eta);
        }

        int erreurs = 0;
        for (int k = 0; k < 2; k++) {
            float a = calculer_score(base[k].x, w, *b);
            if (predire(a) != base[k].d) erreurs++;
        }

        if (f_csv) fprintf(f_csv, "%d,%d\n", epoque, erreurs);

        if (erreurs == 0) {
            epoque_convergence = epoque;
            printf("--> Convergence atteinte avec succes a l'epoque %d (0 erreur) !\n", epoque);
            break;
        }
    }

    if (f_csv) fclose(f_csv);
    return (epoque_convergence == -1) ? max_epoques : epoque_convergence;
}

// 7. Fonction pour bruiter une copie d'un motif
// Inverse exactement n pixels distincts au hasard
void bruiter_motif(const int source[NB_PIXELS], int dest[NB_PIXELS], int n) {
    for (int j = 0; j < NB_PIXELS; j++) {
        dest[j] = source[j];
    }
    if (n <= 0) return;
    if (n >= NB_PIXELS) {
        for (int j = 0; j < NB_PIXELS; j++) dest[j] = 1 - dest[j];
        return;
    }

    // Mélange de Fisher-Yates pour tirer n indices distincts
    int indices[NB_PIXELS];
    for (int j = 0; j < NB_PIXELS; j++) indices[j] = j;
    for (int j = 0; j < n; j++) {
        int r = j + rand() % (NB_PIXELS - j);
        int tmp = indices[j];
        indices[j] = indices[r];
        indices[r] = tmp;
        // Inverser le pixel sélectionné
        dest[indices[j]] = 1 - dest[indices[j]];
    }
}

// 8. Évaluation sur le bruit (Point 4)
void evaluer_bruit(Motif base[2], const float w[NB_PIXELS], float b) {
    FILE *f_csv = fopen("experiments/bruit_etape1.csv", "w");
    if (f_csv) {
        fprintf(f_csv, "p,n_pixels,err_zero_pct,err_un_pct\n");
    }

    printf("\n=== EVALUATION DE LA RESISTANCE AU BRUIT (50 copies par chiffre) ===\n");
    printf("%-5s | %-8s | %-15s | %-15s\n", "p (%)", "n pixels", "Erreur 0 (%)", "Erreur 1 (%)");
    printf("------------------------------------------------------------\n");

    // On fixe la graine pour le bruit pour garantir la reproductibilité
    srand(GRAINE_BRUIT);

    for (int p = 0; p <= 100; p += 10) {
        // n = arrondi(48 * p / 100)
        int n = (int)roundf((float)(NB_PIXELS * p) / 100.0f);

        int mauvaises_zero = 0;
        int mauvaises_un = 0;

        for (int essai = 0; essai < NB_ESSAIS; essai++) {
            int copie[NB_PIXELS];

            // Tester copie bruitée du '0'
            bruiter_motif(base[0].x, copie, n);
            float a0 = calculer_score(copie, w, b);
            if (predire(a0) != base[0].d) {
                mauvaises_zero++;
            }

            // Tester copie bruitée du '1'
            bruiter_motif(base[1].x, copie, n);
            float a1 = calculer_score(copie, w, b);
            if (predire(a1) != base[1].d) {
                mauvaises_un++;
            }
        }

        float err_zero_pct = (100.0f * (float)mauvaises_zero) / (float)NB_ESSAIS;
        float err_un_pct = (100.0f * (float)mauvaises_un) / (float)NB_ESSAIS;

        printf("%-5d | %-8d | %13.1f%% | %13.1f%%\n", p, n, err_zero_pct, err_un_pct);

        if (f_csv) {
            fprintf(f_csv, "%d,%d,%.2f,%.2f\n", p, n, err_zero_pct, err_un_pct);
        }
    }

    if (f_csv) {
        fclose(f_csv);
        printf("\nRésultats de bruit sauvegardés dans 'experiments/bruit_etape1.csv'\n");
    }
}

int main(void) {
    float w[NB_PIXELS];
    float b;
    Motif base[2];

    initialiser_modele(w, &b, GRAINE);
    printf("=== ETAPE 1 : PERCEPTRON SIMPLE ===\n");

    if (!lire_motif("zero.txt", &base[0]) || !lire_motif("un.txt", &base[1])) {
        return 1;
    }

    // Apprentissage
    entrainer(base, w, &b, ETA, MAX_EPOQUES);

    // Évaluation au bruit
    evaluer_bruit(base, w, b);

    // Sauvegarde des poids
    FILE *f_poids = fopen("experiments/poids_etape1.csv", "w");
    if (f_poids) {
        fprintf(f_poids, "pixel,poids\n");
        for (int j = 0; j < NB_PIXELS; j++) {
            fprintf(f_poids, "%d,%f\n", j, w[j]);
        }
        fprintf(f_poids, "biais,%f\n", b);
        fclose(f_poids);
    }

    return 0;
}