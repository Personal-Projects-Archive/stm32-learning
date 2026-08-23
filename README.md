# Bootloader : principe, repartition memoire, validation CRC (phase 22)

## Statut
Partiellement pratique. Un vrai systeme bootloader + application necessite
deux binaires distincts avec deux scripts linker separes - restructuration
de build hors de portee raisonnable dans ce projet a une seule cible CMake.
Le mecanisme complet (relocation VTOR, saut) est documente en code de
reference, non execute faute de seconde image reelle a cette adresse.

## Realise et verifie
- Calcul CRC materiel sur une plage Flash de 24K (6000 mots), confirmant
  que le mecanisme de validation fonctionne a cette echelle (au-dela des
  quelques octets testes en phase 17) : LD4 s'allume fixe, calcul confirme
- Repartition BOOT (8K @ 0x8000000) / APP (24K @ 0x8002000) testee puis
  documentee comme modele theorique (voir difficulte ci-dessous)

## Difficulte rencontree (et vraie decouverte, pas juste un bug)
Premiere tentative : deplacement reel du programme vers une region APP a
0x8002000 - plus aucun code ne s'executait (LD4 jamais allumee). Cause :
le CPU STM32 lit toujours sa table de vecteurs a l'adresse physique fixe
0x08000000 au reset, quel que soit ce que dit le script linker. En vidant
cette adresse (aucun BOOT reel present), le cœur demarre sur du Flash
efface - echec silencieux total, avant meme HAL_Init(). C'est precisement
le role qu'un vrai bootloader devrait jouer (relocation VTOR + saut), non
implemente ici. Le programme est revenu a un placement normal a 0x8000000 ;
la repartition BOOT/APP reste documentee comme modele theorique, non
testee en execution reelle a cette adresse.

## Non realise ici
- Compilation d'une seconde image (bootloader) distincte
- Execution reelle du saut (jump_to_application), fourni en code de reference
  dans main.c (fonction non appelee, marquee __attribute__((unused)))

## Notions apprises
- Difference bootloader/application, VTOR, relocation de la table de vecteurs
- Le point d'entree du CPU (0x08000000) est fixe en silicium, aucun script
  linker ne peut le deplacer - seul un vrai bootloader present a cette
  adresse peut rediriger vers une autre zone (via VTOR + saut)
- Sequence de saut : lecture SP+Reset_Handler depuis la table de vecteurs
  cible, __set_MSP, branchement
- CRC comme mecanisme de validation avant saut, extensible a version/rollback
- Un warning de linker (type RWX permissions) n'est pas systematiquement une
  erreur a corriger, savoir distinguer un avertissement d'un
  vrai probleme fonctionnel

## Limites / ameliorations possibles
Mise en place complete a deux binaires (deux cibles CMake, deux .ld, un
vrai bootloader a 0x8000000 qui relogerait vers une vraie application) a
prevoir sur un vrai projet, idealement sur materiel physique.