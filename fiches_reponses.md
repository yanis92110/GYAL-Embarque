# GYAL — IA pour les systèmes embarqués
## Fiches de travail : Prédis avant de lancer

---

## Étape 1 · Un neurone

### Question 1
**Données :**  
- $x = [1, 0]$  
- $w = [0{,}2 ; -0{,}1]$  
- $b = -0{,}5$  
- Cible attendue : $d = 1$  
- Pas d'apprentissage : $\eta = 0{,}01$

**Calculs et résultats :**
1. **Score $a$ :**  
   $$a = (w_0 \cdot x_0 + w_1 \cdot x_1) + b = (0{,}2 \times 1 + (-0{,}1) \times 0) - 0{,}5 = -0{,}3$$

2. **Décision $\hat{y}$ et erreur $e$ :**  
   Comme $a = -0{,}3 < 0$, la prédiction est $\mathbf{\hat{y} = 0}$.  
   $$e = d - \hat{y} = 1 - 0 = \mathbf{1}$$

3. **Mise à jour des poids et du biais :**  
   - $w_0 \leftarrow w_0 + \eta \cdot e \cdot x_0 = 0{,}2 + (0{,}01 \times 1 \times 1) = \mathbf{0{,}21}$
   - $w_1 \leftarrow w_1 + \eta \cdot e \cdot x_1 = -0{,}1 + (0{,}01 \times 1 \times 0) = \mathbf{-0{,}10}$
   - $b \leftarrow b + \eta \cdot e = -0{,}5 + (0{,}01 \times 1) = \mathbf{-0{,}49}$

---

### Question 2
> *Une erreur nulle sur les deux motifs garantit-elle une erreur nulle à 10 % de bruit ? Pourquoi ?*

**Réponse :**  
**Non, pas forcément.** Avoir une erreur nulle sur les motifs parfaits prouve seulement que le perceptron a mémorisé ces deux exemples précis. Le bruit peut inverser des entrées clés (pixels discriminants) et faire basculer le score du mauvais côté de la frontière de décision, provoquant une erreur de classification.

---

### Question 3
> *À 100 %, combien de pixels changent ? Est-ce encore une petite perturbation ?*

**Réponse :**  
À 100 % de bruit, **les 48 pixels changent** ($n = \text{arrondi}(48 \times 100 / 100) = 48$).  
**Non**, ce n'est plus du tout une petite perturbation : l'image est totalement inversée en son négatif photographique.

---

### Question 4
> *Un classifieur qui répond toujours 0 sur 50 zéros et 50 uns bruités à 50 % reconnaît quelle proportion ?*

**Réponse :**  
Le classifieur reconnaît **50 %** (proportion de **1/2**, soit 50 bonnes réponses sur 100).  
Il a 50/50 sur les zéros et 0/50 sur les uns. C'est la performance du pur hasard sur deux classes équilibrées (classe majoritaire) : un score de 50 % ne signifie donc pas que le modèle a appris.

---

## Étape 2 · Widrow–Hoff

### Question 1
> *Si la classe est correcte mais que le score vaut 0,6 au lieu de 1, la correction s’arrête-t-elle ? Comparer les deux règles.*

**Réponse :**
- **Règle 1 (Perceptron - Étape 1) :**  
  Le score $a = 0{,}6 \ge 0 \implies \hat{y} = 1$.  
  L'erreur vaut $e = d - \hat{y} = 1 - 1 = 0$.  
  → **La correction s'arrête**, le perceptron ne modifie plus ses poids.

- **Règle 2 (Widrow-Hoff - Étape 2) :**  
  La correction est fondée sur la valeur réelle du score :  

  $$\delta = a - d = 0{,}6 - 1{,}0 = -0{,}4 \neq 0$$

  → **La correction continue.** Le neurone continue d'ajuster ses poids pour forcer le score à se rapprocher au plus près de la cible numérique $1{,}0$.
---

### Question 2
> *Que signifie une perte $L$ faible ? Est-ce un pourcentage de bonnes classes ?*

**Réponse :**  
**Non, ce n'est pas un pourcentage de bonnes classes** (ce n'est pas une mesure d'*accuracy*).  
Une perte $L$ faible signifie que l'erreur quadratique moyenne $\frac{1}{2N}\sum (d_k - a_k)^2$ est proche de 0. Cela indique que les scores continus prédits sont numériquement très proches des cibles 0 et 1, ce qui constitue une mesure de précision continue et non une simple classification binaire.

---

### Question 3
> *Combien vaut $\eta \|[x, 1]\|^2$ pour un motif choisi ? Pourquoi compter aussi le biais ?*

**Réponse :**  
La norme au carré du vecteur étendu $\|[x, 1]\|^2$ correspond au nombre de pixels allumés ($=1$) plus $1^2$ pour le terme de biais :
- **Pour le chiffre 0 (`zero.txt`, 16 pixels allumés) :**  
  $$\|[x, 1]\|^2 = 16 + 1 = 17 \implies \eta \|[x, 1]\|^2 = 0{,}01 \times 17 = \mathbf{0{,}17}$$
- **Pour le chiffre 1 (`un.txt`, 8 pixels allumés) :**  
  $$\|[x, 1]\|^2 = 8 + 1 = 9 \implies \eta \|[x, 1]\|^2 = 0{,}01 \times 9 = \mathbf{0{,}09}$$

**Pourquoi compter le biais ?**  
Le biais $b$ se met à jour exactement comme un poids connecté à une entrée constante égale à $1$. Il intervient donc dans le pas effectif de la descente de gradient. La condition de stabilité locale $\eta \|[x, 1]\|^2 < 2$ est largement respectée ($0{,}17 \ll 2$), garantissant la convergence sans divergence numérique.

---

### Question 4
> *Pourquoi garder les mêmes poids initiaux, copies bruitées et ordre des exemples pour comparer deux règles ?*

**Réponse :**  
Pour assurer la **reproductibilité scientifique** (*ceteris paribus* : toutes choses étant égales par ailleurs). En figeant la graine d'initialisation, le jeu de test bruité et l'ordre des présentations, on s'assure que tout écart de performance provient **uniquement du changement de règle d'apprentissage** (Perceptron vs Widrow-Hoff) et non d'un tirage aléatoire favorable.

---

### Résultats observés et comparaison expérimentale

#### Tableau récapitulatif :
| Méthode | Chiffre 0 propre (score, préd.) | Chiffre 1 propre (score, préd.) | Erreur 10 % (0 / 1) | Erreur 30 % (0 / 1) | Erreur 50 % (0 / 1) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Perceptron (Étape 1)** | -0,1587 (0) | +0,0240 (1) | 8,0 % / 54,0 % | 10,0 % / 68,0 % | 14,0 % / 86,0 % |
| **Widrow–Hoff (Étape 2)** | +0,0086 (0) | +0,9826 (1) | 4,0 % / 2,0 % | 4,0 % / 44,0 % | 12,0 % / 84,0 % |

#### Les quatre phrases de conclusion :
1. **Règle la plus performante :**  
   *« La règle de Widrow-Hoff est nettement plus robuste que celle du perceptron, particulièrement pour les faibles et moyens niveaux de bruit (10 % à 30 %). »*
2. **Gain par chiffre :**  
   *« Le chiffre 1 bénéficie d'un gain spectaculaire de robustesse à 10 % de bruit (l'erreur chute de 54 % à 2 %) car son score appris (0,9826) se situe désormais très loin du seuil de décision (0,5). »*
3. **Comportement du chiffre 0 :**  
   *« Le chiffre 0 conserve une excellente résistance au bruit sur les deux méthodes (erreur inférieure à 12 % jusqu'à 30 % de bruit), mais ses scores sont plus équilibrés avec Widrow-Hoff. »*
4. **Limites observées :**  
   *« Au-delà de 60 % de bruit, les deux règles s'effondrent de manière similaire, Widrow-Hoff réduisant une erreur quadratique sans pour autant garantir une symétrie parfaite ni une immunité totale aux très fortes corruptions. »*