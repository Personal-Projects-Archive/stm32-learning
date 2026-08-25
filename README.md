# Tests : unitaires, hote seul, sans dependance materielle (phase 26)

## Objectif
Extraire une logique pure (sans effet de bord materiel) du code deja ecrit,
et la tester directement sur PC avec un compilateur C standard, aucune
dependance a Wokwi ni au toolchain ARM.

## Fonctions testees
- led_should_be_on : logique de seuil de la phase 19 (TaskMonitor), extraite
  du GPIO qui l'entourait
- firmware_size_is_valid : logique de validation de la phase 23, extraite
  de l'ecriture Flash qui la suivait

## Structure
- tests/test_logic.c : fichier autonome, compilable avec n'importe quel
  compilateur C, sans rapport avec le projet STM32 lui-meme

## Notions apprises
- Separer la decision (pure, testable) de l'action materielle (non testable
  hors cible) est la technique de base pour rendre du code embarque
  testable sur PC
- assert() suffit pour des tests simples, pas besoin d'un framework de
  test degre pour commencer
- Tester les limites exactes (valeur egale au seuil, taille egale au maximum
  autorise) revele des bugs qu'un test "au hasard" manquerait facilement
- Une suite de tests ecrite une fois peut etre rejouee a volonte (regression)
  a chaque modification future du code source original

## Limites / ameliorations possibles
- Pas de framework de test formel (Unity, CMock) mis en place, assert()
  suffit pour la taille actuelle du projet mais deviendrait limitant sur
  un projet plus consequent
- Pas de test d'integration reel (chaine complete queue/tache) automatise,
  fait manuellement via le debugger en phase 20