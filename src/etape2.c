#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NB_PIXELS 48
#define GRAINE 42
#define GRAINE_BRUIT 2026
#define ETA 0.01f
#define MAX_EPOQUES 1000
#define SEUIL_L 1e-4f   // Arrêt si L <= 0.0001
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

// 2. Calcul du score réel continu a
float calculer_score(const int x[NB_PIXELS], const float w[NB_PIXELS], float b) {
    float somme = 0.0f;
    for (int j = 0; j < NB_PIXELS; j++) {
        somme += w[j] * (float)x[j];
    }
    return somme + b;
}

// 3. Règle de décision : 1 si a >= 0.5 sinon 0 (Spécifique à l'Étape 2 !)
int predire(float a) {
    return (a >= 0.5f) ? 1 : 0;
}

// 4. Mise à jour de Widrow-Hoff (delta = a - d)
void mise_a_jour_widrow_hoff(const int x[NB_PIXELS], float w[NB_PIXELS], float *b, int d, float a, float eta) {
    float delta = a - (float)d;
    for (int j = 0; j < NB_PIXELS; j++) {
        // w_j <- w_j - eta * delta * x_j
        w[j] -= eta * delta * (float)x[j];
    }
    // b <- b - eta * delta
    *b -= eta * delta;
}

// 5. Initialisation (identique à l'étape 1)
void initialiser_modele(float w[NB_PIXELS], float *b, unsigned int graine) {
    srand(graine);
    for (int j = 0; j < NB_PIXELS; j++) {
        float r = (float)rand() / (float)RAND_MAX;
        w[j] = -0.1f + r * 0.2f;
    }
    *b = -0.5f;
}

// 6. Calcul de la perte L = (1 / 2N) * sum((d_k - a_k)^2) avec N=2
float calculer_perte(Motif base[2], const float w[NB_PIXELS], float b) {
    float somme_carres = 0.0f;
    for (int k = 0; k < 2; k++) {
        float a = calculer_score(base[k].x, w, b);
        float diff = (float)base[k].d - a;
        somme_carres += diff * diff;
    }
    return somme_carres / 4.0f; // 1 / (2 * 2) = 1/4
}

// 7. Entraînement Widrow-Hoff
int entrainer_widrow_hoff(Motif base[2], float w[NB_PIXELS], float *b, float eta, int max_epoques) {
    FILE *f_csv = fopen("experiments/perte_etape2.csv", "w");
    if (f_csv) fprintf(f_csv, "epoque,perte_L\n");

    int epoque_convergence = -1;

    for (int epoque = 1; epoque <= max_epoques; epoque++) {
        int ordre[2] = {0, 1};
        if (rand() % 2 == 1) {
            ordre[0] = 1;
            ordre[1] = 0;
        }

        // Phase d'apprentissage
        for (int k = 0; k < 2; k++) {
            int idx = ordre[k];
            float a = calculer_score(base[idx].x, w, *b);
            mise_a_jour_widrow_hoff(base[idx].x, w, b, base[idx].d, a, eta);
        }

        // Calcul de la perte L avec poids figés
        float L = calculer_perte(base, w, *b);

        if (f_csv) fprintf(f_csv, "%d,%e\n", epoque, L);

        // Détection d'explosion numérique
        if (isnan(L) || isinf(L)) {
            printf("--> ATTENTION : Perte L divergente (explosion numérique) à l'époque %d !\n", epoque);
            break;
        }

        // Arrêt si L <= 10^-4
        if (L <= SEUIL_L && epoque_convergence == -1) {
            epoque_convergence = epoque;
            printf("--> Seuil L <= 1e-4 atteint a l'epoque %d (L = %e) !\n", epoque, L);
            break;
        }
    }

    if (f_csv) fclose(f_csv);
    return (epoque_convergence == -1) ? max_epoques : epoque_convergence;
}

// 8. Génération du bruit (identique à l'étape 1)
void bruiter_motif(const int source[NB_PIXELS], int dest[NB_PIXELS], int n) {
    for (int j = 0; j < NB_PIXELS; j++) dest[j] = source[j];
    if (n <= 0) return;
    if (n >= NB_PIXELS) {
        for (int j = 0; j < NB_PIXELS; j++) dest[j] = 1 - dest[j];
        return;
    }
    int indices[NB_PIXELS];
    for (int j = 0; j < NB_PIXELS; j++) indices[j] = j;
    for (int j = 0; j < n; j++) {
        int r = j + rand() % (NB_PIXELS - j);
        int tmp = indices[j];
        indices[j] = indices[r];
        indices[r] = tmp;
        dest[indices[j]] = 1 - dest[indices[j]];
    }
}

// 9. Évaluation au bruit
void evaluer_bruit_etape2(Motif base[2], const float w[NB_PIXELS], float b) {
    FILE *f_csv = fopen("experiments/bruit_etape2.csv", "w");
    if (f_csv) fprintf(f_csv, "p,n_pixels,err_zero_pct,err_un_pct\n");

    printf("\n=== RESISTANCE AU BRUIT : WIDROW-HOFF ===\n");
    printf("%-5s | %-8s | %-15s | %-15s\n", "p (%)", "n pixels", "Erreur 0 (%)", "Erreur 1 (%)");
    printf("------------------------------------------------------------\n");

    srand(GRAINE_BRUIT); // Même graine de bruit qu'à l'étape 1 !

    for (int p = 0; p <= 100; p += 10) {
        int n = (int)roundf((float)(NB_PIXELS * p) / 100.0f);
        int mauvaises_zero = 0;
        int mauvaises_un = 0;

        for (int essai = 0; essai < NB_ESSAIS; essai++) {
            int copie[NB_PIXELS];

            // Test 0
            bruiter_motif(base[0].x, copie, n);
            float a0 = calculer_score(copie, w, b);
            if (predire(a0) != base[0].d) mauvaises_zero++;

            // Test 1
            bruiter_motif(base[1].x, copie, n);
            float a1 = calculer_score(copie, w, b);
            if (predire(a1) != base[1].d) mauvaises_un++;
        }

        float err_zero_pct = (100.0f * (float)mauvaises_zero) / (float)NB_ESSAIS;
        float err_un_pct = (100.0f * (float)mauvaises_un) / (float)NB_ESSAIS;

        printf("%-5d | %-8d | %13.1f%% | %13.1f%%\n", p, n, err_zero_pct, err_un_pct);
        if (f_csv) fprintf(f_csv, "%d,%d,%.2f,%.2f\n", p, n, err_zero_pct, err_un_pct);
    }

    if (f_csv) fclose(f_csv);
}

int main(void) {
    float w[NB_PIXELS];
    float b;
    Motif base[2];

    initialiser_modele(w, &b, GRAINE);
    printf("=== ETAPE 2 : APPRENTISSAGE WIDROW-HOFF ===\n");

    if (!lire_motif("zero.txt", &base[0]) || !lire_motif("un.txt", &base[1])) {
        return 1;
    }

    // Scores initiaux
    float a0 = calculer_score(base[0].x, w, b);
    float a1 = calculer_score(base[1].x, w, b);
    printf("[Avant entraînement] Score 0: %.4f | Score 1: %.4f | Perte L = %e\n", a0, a1, calculer_perte(base, w, b));

    // Entraînement
    int ep = entrainer_widrow_hoff(base, w, &b, ETA, MAX_EPOQUES);

    // Scores finaux
    a0 = calculer_score(base[0].x, w, b);
    a1 = calculer_score(base[1].x, w, b);
    printf("\n[Après entraînement (époque %d)]\n", ep);
    printf("Chiffre 0 : score = %7.4f -> Predit = %d (Cible idéale: 0.0)\n", a0, predire(a0));
    printf("Chiffre 1 : score = %7.4f -> Predit = %d (Cible idéale: 1.0)\n", a1, predire(a1));
    printf("Biais final b = %.4f | Perte finale L = %e\n", b, calculer_perte(base, w, b));

    // Évaluation au bruit
    evaluer_bruit_etape2(base, w, b);

    // Sauvegarde des poids
    FILE *f_poids = fopen("experiments/poids_etape2.csv", "w");
    if (f_poids) {
        fprintf(f_poids, "pixel,poids\n");
        for (int j = 0; j < NB_PIXELS; j++) fprintf(f_poids, "%d,%f\n", j, w[j]);
        fprintf(f_poids, "biais,%f\n", b);
        fclose(f_poids);
    }

    return 0;
}