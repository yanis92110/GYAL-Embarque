#include <stdio.h>
#include <stdlib.h>
#include "perceptron.h"

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

int main(void) {
    Perceptron p;
    double x[NB_PIXELS];
    int d = 0;

    /* 1. Initialisation conforme aux consignes : graine 42, b = -0.5, eta = 0.01 */
    initialiser_perceptron(&p, 42, -0.5, 0.01);
    printf("Initialisation terminee : biais = %.2f, pas = %.2f\n", p.b, p.eta);

    /* 2. Test sur zero.txt */
    if (lire_motif("/home/yanis/Documents/IAEmbarquee/GYAL-Embarque/student-materials/patterns/zero.txt", x, &d) == 0) {
        double a = calcul_score(&p, x);
        int y_chapeau = decision(a);
        int e = d - y_chapeau;

        printf("\n--- Test de lecture : zero.txt ---\n");
        printf("Classe attendue (d) : %d\n", d);
        printf("Score calcule   (a) : %.4f\n", a);
        printf("Prediction      (y) : %d\n", y_chapeau);
        printf("Erreur          (e) : %d\n", e);

        if (e != 0) {
            mise_a_jour(&p, x, e);
            printf("Poids et biais mis a jour. Nouveau biais : %.4f\n", p.b);
        }
    }

    return 0;
}
