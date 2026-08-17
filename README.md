# Phase 11 — DMA

## Statut
Non simulable. Cette formation utilise Wokwi (STM32) comme environnement de
simulation, et aucune des cartes STM32 actuellement disponibles sur cette
plateforme ne simule le controleur DMA. Cette phase reste donc theorique
jusqu'a l'acquisition d'une carte physique.

## Objectif de la phase
Comprendre et mettre en oeuvre le transfert de donnees par DMA (Direct Memory
Access) : transferts memoire-vers-memoire, peripherique-vers-memoire et
memoire-vers-peripherique, sans intervention continue du CPU. Notions cles :
canaux, requetes (DMAMUX sur les familles recentes), mode normal vs
circulaire, interruptions half-transfer et transfer-complete.

## Carte physique recommandee pour completer cette partie de la formation

**Nucleo-H563ZI**

Cette carte a ete choisie pour couvrir en une seule acquisition l'ensemble
des phases non simulables restantes de cette formation (DMA, CAN, USB,
Ethernet), plutot que d'acheter une carte differente pour chacune :

- Microcontroleur STM32H563ZI, coeur Arm Cortex-M33 a 250 MHz, avec
  TrustZone (interet pour la securite embarquee : boot securise,
  accelerateurs cryptographiques, isolation memoire)
- 2 Mo de Flash, 640 Ko de RAM
- Jusqu'a 2x FDCAN (CAN FD)
- USB Full-Speed et USB Type-C / Power Delivery
- Interface Ethernet
- DMA multi-canaux avec DMAMUX
- Connecteurs Arduino, ST Zio et ST Morpho (compatibilite shields standards)
- Format Nucleo-144, meme famille d'outils (STM32CubeMX, STM32CubeIDE for
  VS Code) que la carte utilisee jusqu'ici dans cette formation

## Configuration a prevoir dans STM32CubeMX (a faire une fois la carte en main)

Etapes generales, a detailler au moment de l'implementation reelle :

1. Nouveau projet CubeMX cible sur STM32H563ZI (ou import du board Nucleo-H563ZI)
2. Categorie **System Core > DMA** : ajouter une requete DMA associee au
   peripherique concerne (ADC, UART, SPI...), choisir le mode (Normal ou
   Circular selon le besoin)
3. Activer les interruptions DMA souhaitees (Transfer Complete, Half
   Transfer) dans l'onglet NVIC du peripherique DMA
4. Activer le mode DMA correspondant cote peripherique source/destination
   (ex. "DMA Continuous Requests" pour un ADC en acquisition continue)
5. Generer le code, puis ecrire la logique applicative dans les zones
   USER CODE, sans modifier le code d'initialisation genere

## Notions a retenir en attendant la mise en oeuvre reelle

- Le DMA remplace l'attente bloquante du CPU (`while (!flag) {}`) par un
  transfert autonome : le CPU configure une fois, puis est libre
- Le mode circulaire permet un flux continu sans jamais redemarrer le
  transfert manuellement
- Les interruptions half-transfer / transfer-complete permettent un motif
  de double-buffering (traiter une moitie du buffer pendant que l'autre
  se remplit)
- Sur les familles recentes (dont H5), le routage requete-vers-canal passe
  par un DMAMUX configurable, plutot qu'un mapping fixe

## Prochaines etapes
Les phases 12 (Watchdogs), 13 (CAN), 14 (Ethernet) et 15 (USB) suivront le meme format —
README documentant l'objectif et les etapes de configuration, sans code ni
simulation, jusqu'a la mise en oeuvre sur la Nucleo-H563ZI mentionnee ci-dessus.