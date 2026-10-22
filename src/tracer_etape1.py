import csv
import matplotlib.pyplot as plt

# 1. Lecture de erreurs_epoques.csv
epoques = []
erreurs = []
with open("experiments/erreurs_epoques.csv", "r") as f:
    lecteur = csv.DictReader(f)
    for ligne in lecteur:
        epoques.append(int(ligne["epoque"]))
        erreurs.append(int(ligne["erreurs"]))

# 2. Lecture de bruit_etape1.csv
p_vals = []
err_zero = []
err_un = []
with open("experiments/bruit_etape1.csv", "r") as f:
    lecteur = csv.DictReader(f)
    for ligne in lecteur:
        p_vals.append(int(ligne["p"]))
        err_zero.append(float(ligne["err_zero_pct"]))
        err_un.append(float(ligne["err_un_pct"]))

# 3. Tracé des deux panneaux
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# Panneau gauche : Erreurs selon l'époque
ax1.plot(epoques, erreurs, marker='o', color='b', linewidth=2)
ax1.set_title("Erreurs sur les 2 motifs selon l'époque")
ax1.set_xlabel("Époque")
ax1.set_ylabel("Nombre d'erreurs (sur 2)")
ax1.set_ylim(-0.1, 2.1)
ax1.grid(True, linestyle="--", alpha=0.6)

# Panneau droit : Taux d'erreur selon le bruit
ax2.plot(p_vals, err_zero, marker='s', label="Chiffre 0", color='green', linewidth=2)
ax2.plot(p_vals, err_un, marker='^', label="Chiffre 1", color='red', linewidth=2)
ax2.set_title("Taux d'erreur selon le pourcentage de bruit")
ax2.set_xlabel("Bruit p (%)")
ax2.set_ylabel("Taux d'erreur (%)")
ax2.set_ylim(-5, 105)
ax2.legend()
ax2.grid(True, linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig("figures/etape1_resultats.png", dpi=150)
print("Succès ! Graphique généré dans 'figures/etape1_resultats.png'")