#include <stdio.h>
#include <stdlib.h>
#include "perceptron.h"
#define MAX_EPOQUES 1000

/* Structure simple pour stocker un motif et son label */
typedef struct {
    double x[NB_PIXELS];
    int d;
    const char *nom;
} Motif;

/* Initialise les poids dans [-0.1 ; 0.1], le biais et le pas */
void initialiser_perceptron(Perceptron *p, unsigned int graine, double b_init, double eta_init) {
    p->b = b_init;
    p->eta = eta_init;

    srand(graine);
    for (size_t j = 0; j < NB_PIXELS; ++j) {
        /* Transformation affine : tire un reel uniforme dans [-0.1 ; 0.1] */
        double r = (double)rand() / (double)RAND_MAX;
        p->w[j] = -0.1 + r * 0.2;
    }
}

/* Lit 8 lignes de 6 caracteres ('.' -> 0, '*' -> 1) puis la classe en 9e ligne */
int lire_motif(const char *nom_fichier, double x[NB_PIXELS], int *d) {
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", nom_fichier);
        return -1;
    }

    size_t index = 0;
    char c;

    /* Lecture des 48 pixels en ignorant les retours a la ligne */
    while (index < NB_PIXELS && (c = fgetc(f)) != EOF) {
        if (c == '.') {
            x[index++] = 0.0;
        } else if (c == '*') {
            x[index++] = 1.0;
        }
        /* Les espaces et '\n' sont simplement ignores */
    }

    /* Lecture de la classe attendue d sur la 9e ligne */
    if (fscanf(f, "%d", d) != 1) {
        fprintf(stderr, "Erreur : classe attendue introuvable dans %s\n", nom_fichier);
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

/* Calcule la somme ponderee a = sum(w_j * x_j) + b */
double calcul_score(const Perceptron *p, const double x[NB_PIXELS]) {
    double a = p->b;
    for (size_t j = 0; j < NB_PIXELS; ++j) {
        a += p->w[j] * x[j];
    }
    return a;
}

/* Fonction de decision Heaviside : 1 si a >= 0, sinon 0 */
int decision(double score) {
    return (score >= 0.0) ? 1 : 0;
}

/* Mise a jour standard du perceptron : w_j += eta * e * x_j et b += eta * e */
void mise_a_jour(Perceptron *p, const double x[NB_PIXELS], int erreur) {
    for (size_t j = 0; j < NB_PIXELS; ++j) {
        p->w[j] += p->eta * (double)erreur * x[j];
    }
    p->b += p->eta * (double)erreur;
}

//Question 2
/*
 * Traite un exemple unique :
 * 1. Calcule le score a
 * 2. Evalue la prediction y_chapeau
 * 3. Calcule l'erreur e = d - y_chapeau
 * 4. Met a jour les poids et le biais si e != 0
 * Retourne l'erreur commise (0 ou != 0)
 */
int traiter_exemple(Perceptron *p, const double x[NB_PIXELS], int d, const char *nom_motif) {
    /* Calcul du score et de la prediction */
    double a = calcul_score(p, x);
    int y_chapeau = decision(a);
    int e = d - y_chapeau;

    printf("Motif : %-8s | d = %d | a = %8.4f | y^ = %d | e = %2d", 
           nom_motif, d, a, y_chapeau, e);

    /* Correction si erreur non nulle */
    if (e != 0) {
        mise_a_jour(p, x, e);
        printf(" -> [Correction appliquee, nouveau b = %.4f]\n", p->b);
    } else {
        printf(" -> [Correct, pas de changement]\n");
    }

    return e;
}

int main(void) {
    Perceptron p;
    Motif base[2]; /* base[0] = zero, base[1] = un */

    /* 1. Initialisation demandee : graine 42, b = -0.5, eta = 0.01 */
    initialiser_perceptron(&p, 42, -0.5, 0.01);

    /* 2. Chargement des deux fichiers */
    base[0].nom = "/home/yanis/Documents/IAEmbarquee/GYAL-Embarque/student-materials/patterns/zero.txt";
    base[1].nom = "/home/yanis/Documents/IAEmbarquee/GYAL-Embarque/student-materials/patterns/un.txt";

    if (lire_motif(base[0].nom, base[0].x, &base[0].d) != 0 ||
        lire_motif(base[1].nom, base[1].x, &base[1].d) != 0) {
        return EXIT_FAILURE;
    }

    printf("=== Debut de l'apprentissage (max %d epoques) ===\n\n", MAX_EPOQUES);

    int epoque_succes = -1;

    for (int epoque = 1; epoque <= MAX_EPOQUES; ++epoque) {
        /*
         * PHASE A : Melange de l'ordre de presentation.
         * ordre[0] et ordre[1] contiendront soit {0, 1}, soit {1, 0}.
         */
        int premier = rand() % 2;
        int ordre[2];
        ordre[0] = premier;
        ordre[1] = 1 - premier;

        /*
         * PHASE B : Apprentissage sur les deux motifs dans l'ordre tire.
         */
        for (int i = 0; i < 2; ++i) {
            int idx = ordre[i];
            double a = calcul_score(&p, base[idx].x);
            int y_chapeau = decision(a);
            int e = base[idx].d - y_chapeau;

            /* Si erreur, on met a jour immediatement */
            if (e != 0) {
                mise_a_jour(&p, base[idx].x, e);
            }
        }

        /*
         * PHASE C : Evaluation a la fin de l'epoque (poids figes).
         * On recompte les erreurs sur les deux motifs SANS modifier les poids.
         */
        int total_erreurs = 0;
        for (int i = 0; i < 2; ++i) {
            double a = calcul_score(&p, base[i].x);
            int y_chapeau = decision(a);
            if (y_chapeau != base[i].d) {
                total_erreurs++;
            }
        }

        /* Affichage de suivi (affiche les premieres epoques et quand ca converge) */
        if (epoque <= 5 || total_erreurs == 0 || epoque == MAX_EPOQUES) {
            printf("Epoque %4d | Erreurs a l'evaluation : %d |- Biais : %.4f\n", 
                   epoque, total_erreurs, p.b);
        }

        /*
         * PHASE D : Condition d'arret a zero erreur.
         */
        if (total_erreurs == 0) {
            epoque_succes = epoque;
            break;
        }
    }

    /*
     * BILAN FINAL
     */
    printf("\n=== Bilan ===\n");
    if (epoque_succes != -1) {
        printf("Succes : convergence atteinte a l'epoque %d avec 0 erreur.\n", epoque_succes);
    } else {
        printf("Attention : limite de %d epoques atteinte sans convergence totale.\n", MAX_EPOQUES);
    }

    /* Verification des scores finaux obtenus sur chaque motif */
    printf("\nScores finaux apres apprentissage :\n");
    for (int i = 0; i < 2; ++i) {
        double a = calcul_score(&p, base[i].x);
        int y_chapeau = decision(a);
        printf("- Motif %s (cible %d) : score a = %7.4f -> classe predite = %d\n",
               base[i].nom, base[i].d, a, y_chapeau);
    }

    return EXIT_SUCCESS;
}








