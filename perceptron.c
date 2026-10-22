#include <stdio.h>
#include <stdlib.h>

// [Les fonctions lire_fichier, calculer_score, decider, mettre_a_jour restent identiques]

int lire_fichier(const char* chemin, int pixels[48], int* d) {
    FILE* fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", chemin);
        return 0;
    }
    char ligne[16]; 
    int index_pixel = 0;
    for (int i = 0; i < 8; ++i) {
        if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
            for (int j = 0; ligne[j] != '\0' && ligne[j] != '\n' && ligne[j] != '\r'; ++j) {
                if (ligne[j] == '*') pixels[index_pixel++] = 1;
                else if (ligne[j] == '.') pixels[index_pixel++] = 0;
            }
        }
    }
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) *d = atoi(ligne);
    fclose(fichier);
    return 1;
}

float calculer_score(int pixels[48], float poids[48], float biais) {
    float a = biais;
    for (int j = 0; j < 48; ++j) a += poids[j] * pixels[j];
    return a;
}

int decider(float a) {
    return (a >= 0.0f) ? 1 : 0;
}

void mettre_a_jour(int pixels[48], float poids[48], float* biais, int d, int y_chapeau, float pas_apprentissage) {
    int erreur = d - y_chapeau;
    for (int j = 0; j < 48; ++j) {
        float gradient_poids = -erreur * pixels[j];
        poids[j] = poids[j] - (pas_apprentissage * gradient_poids);
    }
    float gradient_biais = -erreur;
    *biais = *biais - (pas_apprentissage * gradient_biais);
}

int main() {
    float biais = -0.5f;
    float pas_apprentissage = 0.01f;
    float poids[48];

    srand(42);
    for (int j = 0; j < 48; ++j) {
        poids[j] = ((float)rand() / RAND_MAX) * 0.2f - 0.1f;
    }

    int pixels_zero[48], pixels_un[48];
    int d_zero, d_un;
    
    // On suppose que les fichiers zero.txt et un.txt existent déjà
    if (!lire_fichier("zero.txt", pixels_zero, &d_zero) || !lire_fichier("un.txt", pixels_un, &d_un)) {
        return 1; 
    }

    // Boucle d'apprentissage sur 1000 époques maximum[cite: 2]
    int epoque;
    for (epoque = 1; epoque <= 1000; epoque++) {
        
        // --- PHASE 1 : APPRENTISSAGE ---
        // Mélange de l'ordre de présentation : soit 0 puis 1, soit 1 puis 0[cite: 2]
        int ordre = rand() % 2; 
        
        if (ordre == 0) {
            mettre_a_jour(pixels_zero, poids, &biais, d_zero, decider(calculer_score(pixels_zero, poids, biais)), pas_apprentissage);
            mettre_a_jour(pixels_un, poids, &biais, d_un, decider(calculer_score(pixels_un, poids, biais)), pas_apprentissage);
        } else {
            mettre_a_jour(pixels_un, poids, &biais, d_un, decider(calculer_score(pixels_un, poids, biais)), pas_apprentissage);
            mettre_a_jour(pixels_zero, poids, &biais, d_zero, decider(calculer_score(pixels_zero, poids, biais)), pas_apprentissage);
        }

        // --- PHASE 2 : VÉRIFICATION ---
        // On recompte les erreurs sans modifier les poids[cite: 2]
        int erreurs = 0;
        
        if (decider(calculer_score(pixels_zero, poids, biais)) != d_zero) erreurs++;
        if (decider(calculer_score(pixels_un, poids, biais)) != d_un) erreurs++;

        // --- CONDITION D'ARRÊT ---
        if (erreurs == 0) {
            printf("Apprentissage termine avec succes a l'epoque %d !\n", epoque);
            break; // Le filet de sécurité a fonctionné, on sort de la boucle[cite: 2]
        }
    }

    if (epoque > 1000) {
        printf("Echec : Limite des 1000 epoques atteinte sans convergence.\n");
    }

    return 0;
}