#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <stddef.h>

#define NB_PIXELS 48 /* 8 lignes * 6 colonnes */

typedef struct {
    double w[NB_PIXELS]; /* 48 poids synaptiques */
    double b;            /* Biais */
    double eta;          /* Pas d'apprentissage */
} Perceptron;

/* Prototypes obligatoires pour la question 1 */
void initialiser_perceptron(Perceptron *p, unsigned int graine, double b_init, double eta_init);
int  lire_motif(const char *nom_fichier, double x[NB_PIXELS], int *d);
double calcul_score(const Perceptron *p, const double x[NB_PIXELS]);
int  decision(double score);
void mise_a_jour(Perceptron *p, const double x[NB_PIXELS], int erreur);

#endif /* PERCEPTRON_H */
