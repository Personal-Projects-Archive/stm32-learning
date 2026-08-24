# Firmware update : reception UART, ecriture Flash securisee (phase 23)

## Statut
Code complet et compilable, non teste (necessite un vrai port serie et une
vraie carte - voir phase 11 pour la recommandation materielle). Rien a
simuler dans Wokwi pour cette phase, aucun diagram.json fourni.

## Objectif
Recevoir un firmware via USART2, verifier son integrite (CRC32) avant toute
ecriture, l'ecrire dans une zone Flash reservee, puis revalider apres
ecriture - sans jamais toucher au code actuellement en cours d'execution.

## Ou se situe ce code exactement
Trois methodes distinctes existent pour mettre du code sur cette puce, a ne
pas confondre :
1. Bootloader ROM d'usine (grave par ST, jamais modifie) - active via la
   broche BOOT0, non utilise dans cette formation
2. ST-Link/SWD - la methode utilisee depuis la phase 00 pour tout ce projet,
   ecrit directement en Flash sans passer par aucun bootloader
3. Bootloader personnalise (phases 22/23) - un programme C ordinaire, ecrit
   et flashe comme n'importe quel autre via la methode 2, dont le role
   special est de gerer le lancement/la mise a jour d'une application separee

Le code de ce fichier correspond a la methode 3 : c'est le contenu du
bootloader personnalise lui-meme, pas un ajout a l'application normale.
Tel quel, il boucle indefiniment en attente d'une mise a jour, il ne saute
jamais vers une application (cette partie, deja donnee en phase 22 sous forme
de reference, n'est pas appelee ici). Sur un vrai montage a deux binaires, 
ce code serait complete par une logique de choix (sauter vers l'application 
existante par defaut, rester en attente de mise a jour seulement sur un signal explicite). 
Deux moyens concrets et courants pour ce signal : un bouton maintenu enfonce 
pendant le demarrage (lu via GPIO avant le saut vers l'application), ou un drapeau 
ecrit dans un registre de sauvegarde RAM qui survit a un reset logiciel (mais pas a une
coupure d'alimentation), l'application elle-meme peut ecrire ce drapeau
juste avant de provoquer volontairement son propre reset, pour demander a
etre reflashee au redemarrage suivant.

## Cas d'usage reel (pourquoi un bootloader personnalise, alors que ST-Link
## fonctionne deja tres bien)
Utile uniquement quand un acces physique regulier a la carte n'est pas
envisageable : produit deploye chez un client, capteur installe sur site,
mise a jour a distance ou par un utilisateur final sans outil de
programmation. Sans ce besoin precis, la methode 2 (ST-Link) suffit et reste
ce qui a ete utilise pour tout le reste de cette formation.

## Protocole (simple, auto-suffisant)
1. PC -> carte : taille du firmware (4 octets, little-endian)
2. PC -> carte : donnees du firmware
3. PC -> carte : CRC32 de reference (4 octets, little-endian)
4. Carte : verifie le CRC en RAM avant d'ecrire quoi que ce soit
5. Carte : efface puis programme la zone reservee (0x8007000, 4 Ko)
6. Carte : relit le CRC directement depuis la Flash ecrite, confirme ou signale l'echec

## Zone reservee
Derniers 4 Ko de la puce (pages 14-15 sur 16 pages de 2 Ko), choisis pour ne
jamais recouvrir le programme actuellement execute (~6-7 Ko utilises sur 32).

## Structure
- `Core/Src/main.c` : fichier complet, autonome, pret a flasher tel quel sur
  materiel reel

## Test prevu (sur materiel reel)
Terminal serie ou petit script cote PC envoyant le protocole ci-dessus,
suivi d'une verification manuelle (relecture memoire) que la zone reservee
contient bien les octets envoyes.

## Notions apprises
- Ne jamais ecrire en Flash sans verifier l'integrite des donnees recues au
  prealable, en RAM
- HAL_FLASH_Unlock/Lock, effacement par page, programmation par double-mot
- Revalider apres ecriture (pas seulement avant) : une ecriture peut echouer
  silencieusement en cours de route
- Choisir une zone reservee qui ne recouvre jamais le code en cours
  d'execution - prealable indispensable avant tout test reel
- Un bootloader ROM programme en usine existe deja sur cette puce (broche
  BOOT0 + STM32CubeProgrammer) pour les besoins basiques, sans code a ecrire

## Limites / ameliorations possibles
- Type de programmation Flash (DOUBLEWORD) a reverifier contre le header HAL
  exact au moment du test reel, pas verifie par compilation dans Wokwi
- Pas de mecanisme de saut vers le firmware ecrit (necessiterait le vrai
  bootloader de la phase 22, non finalise pour les raisons deja documentees)