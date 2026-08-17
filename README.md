# Phase 13 — CAN / CAN FD

## Statut
Non simulable. Aucune des cartes STM32 disponibles sur Wokwi ne simule le
controleur CAN, quelle que soit la carte choisie. Cette phase reste
theorique jusqu'a l'acquisition d'une carte physique (voir phase 11 pour
la recommandation materielle : Nucleo-H563ZI, qui integre jusqu'a deux
controleurs FDCAN).

## Objectif de la phase
Comprendre le fonctionnement du bus CAN, mettre en oeuvre une communication
entre deux noeuds STM32, puis introduire les bases de CANopen (protocole
applicatif construit au-dessus du CAN, courant en environnement industriel).

## Notions fondamentales

- **Bus multi-maitre, diffusion** : tous les noeuds sur le bus recoivent
  chaque trame emise ; il n'y a pas d'adressage point-a-point comme en I2C,
  chaque trame porte un identifiant (ID) qui indique son contenu/sa priorite,
  pas son destinataire
- **Arbitrage** : si plusieurs noeuds emettent en meme temps, celui dont
  l'identifiant a la priorite la plus haute (valeur numerique la plus basse)
  gagne l'acces au bus sans collision ni perte de donnees - mecanisme
  matriel, pas une simple detection de collision comme sur d'anciens
  reseaux Ethernet
- **Identifiant standard (11 bits) vs etendu (29 bits)** : deux formats de
  trame coexistent sur le meme bus
- **DLC (Data Length Code)** : longueur utile de la trame, jusqu'a 8 octets
  en CAN classique, jusqu'a 64 en CAN FD
- **ACK** : chaque noeud recepteur valide la trame recue, meme sans
  adressage explicite - un mecanisme d'accuse de reception collectif
- **Bus-off** : etat de protection automatique qu'un noeud atteint apres
  trop d'erreurs consecutives - il se deconnecte de lui-meme du bus pour
  eviter de le polluer
- **Terminaison** : deux resistances de 120 ohms aux extremites physiques
  du bus, necessaires pour eviter les reflexions de signal - detail
  materiel a ne pas oublier sur un vrai montage
- **CAN FD** : extension permettant un debit de donnees plus eleve sur la
  partie utile de la trame, et jusqu'a 64 octets de charge utile au lieu de 8

## CANopen (introduction, a approfondir plus tard)
- **PDO (Process Data Object)** : donnees temps reel echangees directement,
  sans confirmation explicite - pense capteurs/actionneurs
- **SDO (Service Data Object)** : acces configuration/parametrage, avec
  confirmation - pense lecture/ecriture d'un parametre de calibration
- **Object Dictionary** : structure standardisee qui decrit les donnees
  exposees par chaque noeud, permettant une interoperabilite entre
  equipements de fabricants differents
- **NMT (Network Management)** : gestion des etats du reseau (demarrage,
  arret, surveillance des noeuds)

## Configuration a prevoir dans STM32CubeMX (a faire une fois la carte en main)

1. Categorie **Connectivity > FDCAN1** (ou CAN selon la puce) : Mode
   activee selon le besoin (normal, boucle interne pour test sans second
   noeud, etc.)
2. Regler le debit nominal (bit timing : prescaler, segments) selon la
   frequence du bus visee et l'horloge peripherique reelle
3. Configurer au moins un filtre de reception (accepter tout, ou filtrer
   par plage d'identifiants)
4. Activer les interruptions souhaitees (reception, erreurs, bus-off)
5. Generer le code, puis ecrire l'emission/reception dans les zones
   USER CODE

## Projet prevu (a faire sur materiel reel)
Deux STM32 communiquant via CAN : un noeud capteur emettant periodiquement
une valeur, un noeud controleur qui la recoit et reagit (ex. pilotage d'un
actionneur en fonction de la valeur recue).

## Prochaine etape
Phase 14 (Ethernet), meme format documentaire prevu.