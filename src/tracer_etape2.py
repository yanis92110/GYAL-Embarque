import csv
import matplotlib.pyplot as plt

# 1. Lecture de la perte L
epoques_L = []
perte_L = []
with open("experiments/perte_etape2.csv", "r") as f:
    lecteur = csv.DictReader(f)
    for ligne in lecteur:
        epoques_L.append(int(ligne["epoque"]))
        perte_L.append(float(ligne["perte_L"]))

# 2. Lecture du bruit Etape 1
p_vals = []
err0_e1 = []
err1_e1 = []
with open("experiments/bruit_etape1.csv", "r") as f:
    lecteur = csv.DictReader(f)
    for ligne in lecteur:
        p_vals.append(int(ligne["p"]))
        err0_e1.append(float(ligne["err_zero_pct"]))
        err1_e1.append(float(ligne["err_un_pct"]))

# 3. Lecture du bruit Etape 2
err0_e2 = []
err1_e2 = []
with open("experiments/bruit_etape2.csv", "r") as f:
    lecteur = csv.DictReader(f)
    for ligne in lecteur:
        err0_e2.append(float(ligne["err_zero_pct"]))
        err1_e2.append(float(ligne["err_un_pct"]))

# 4. Tracé des figures
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(13, 5))

# Panneau gauche : Perte L en échelle logarithmique
ax1.plot(epoques_L, perte_L, color='purple', linewidth=2)
ax1.axhline(y=1e-4, color='black', linestyle='--', label="Seuil arrêt 1e-4")
ax1.set_yscale('log')
ax1.set_title("Étape 2 : Décroissance de la perte L (Widrow-Hoff)")
ax1.set_xlabel("Époque")
ax1.set_ylabel("Perte L (MSE)")
ax1.legend()
ax1.grid(True, linestyle="--", alpha=0.6)

# Panneau droit : Comparaison Perceptron vs Widrow-Hoff
ax2.plot(p_vals, err1_e1, 'r--', marker='^', label="Étape 1 : Chiffre 1 (Perceptron)")
ax2.plot(p_vals, err1_e2, 'r-', marker='^', linewidth=2, label="Étape 2 : Chiffre 1 (Widrow-Hoff)")
ax2.plot(p_vals, err0_e1, 'g--', marker='s', label="Étape 1 : Chiffre 0 (Perceptron)")
ax2.plot(p_vals, err0_e2, 'g-', marker='s', linewidth=2, label="Étape 2 : Chiffre 0 (Widrow-Hoff)")

ax2.set_title("Comparaison de la résistance au bruit")
ax2.set_xlabel("Bruit p (%)")
ax2.set_ylabel("Taux d'erreur (%)")
ax2.set_ylim(-5, 105)
ax2.legend()
ax2.grid(True, linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig("figures/etape2_resultats.png", dpi=150)
print("Succès ! Figure sauvegardée dans 'figures/etape2_resultats.png'")