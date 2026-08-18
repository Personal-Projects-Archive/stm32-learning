# Phase 16 — Basse consommation

## Statut
Non simulable. Le peripherique PWR (Power Control), qui gere les modes basse
consommation, apparait comme non implemente dans le tableau de
fonctionnalites officiel de Wokwi pour cette carte. Cette phase reste
theorique jusqu'a l'acquisition d'une carte physique (voir phase 11 pour
la recommandation materielle : Nucleo-H563ZI).

## Objectif de la phase
Comprendre les differents niveaux de mise en veille disponibles sur STM32,
leurs compromis respectifs (consommation vs temps de reveil vs etat
conserve), et mettre en oeuvre un systeme qui dort la majeure partie du
temps et ne se reveille que lorsque necessaire.

## Notions fondamentales

- **Sleep** : seul le coeur s'arrete, les peripheriques et l'horloge
  principale restent actifs. Reveil quasi instantane (sur n'importe quelle
  interruption), consommation reduite mais modestement
- **Stop** : la plupart des horloges s'arretent (dont l'horloge principale),
  la RAM et les registres sont conserves. Consommation plus basse
  qu'en Sleep, mais reveil plus lent (le temps de reconfigurer l'horloge
  principale) et moins de sources de reveil disponibles (generalement EXTI,
  RTC)
- **Standby** : consommation la plus basse, mais la RAM n'est pas conservee
  - au reveil, le programme redemarre depuis zero, comme apres un reset.
  Seuls quelques registres de sauvegarde survivent, prevus specifiquement
  pour conserver un minimum d'etat entre deux cycles Standby
- **RTC (Real-Time Clock)** : horloge continue meme dans les modes les plus
  profonds (alimentee separement) - source de reveil privilegiee pour un
  reveil periodique programme a l'avance (ex. toutes les heures)
- **LPTIM (Low-Power Timer)** : timer capable de fonctionner avec une
  horloge lente pendant les modes basse consommation, alternative a la RTC
  pour des reveils plus frequents
- **Sources de reveil** : varient selon le mode choisi - un bouton (EXTI)
  fonctionne dans tous les modes, un timer classique non-LP ne fonctionne
  pas en Stop/Standby (son horloge est coupee)

## Compromis a garder en tete
Chaque mode plus profond economise de l'energie au prix d'un temps de
reveil plus long et d'un etat conserve plus reduit. Le choix depend de
l'application : un capteur qui doit reagir en quelques microsecondes a un
evenement ne peut pas se permettre un Standby ; un capteur qui releve une
mesure toutes les heures peut se permettre le mode le plus profond
disponible.

## Configuration a prevoir dans STM32CubeMX (a faire une fois la carte en main)

1. Categorie **System Core > RTC** : activer si un reveil periodique
   programme est necessaire, configurer une alarme ou un wake-up timer
2. Dans le code applicatif, appeler la fonction HAL correspondant au mode
   souhaite (`HAL_PWR_EnterSLEEPMode()`, `HAL_PWR_EnterSTOPMode()`,
   `HAL_PWR_EnterSTANDBYMode()`)
3. Verifier avant mise en veille profonde que toutes les sources de reveil
   necessaires sont bien configurees et actives - un Stop/Standby sans
   source de reveil valide bloque le systeme indefiniment
4. Pour Standby : identifier au demarrage si le reset provient d'un reveil
   Standby (registre de flag dedie) pour adapter le comportement du
   programme en consequence

## Projet prevu (a faire sur materiel reel)
Capteur autonome : releve une mesure (ex. temperature via ADC), l'envoie ou
la stocke, puis repart en Stop ou Standby jusqu'au prochain reveil
periodique programme via la RTC. Mesurer la consommation reelle (a
l'ampèremetre) dans chaque mode pour comparer concretement les compromis
theoriques ci-dessus.

## Notions transferables
Le principe general (niveaux de veille progressifs, sources de reveil
limitees selon la profondeur, RTC comme reference de temps persistante) se
retrouve sur la quasi-totalite des microcontroleurs bas-consommation,
STM32 ou non - seuls les noms de registres et modes exacts different.

## Bilan du bloc theorique (phases 11 a 16)
Ces six phases (DMA, watchdogs, CAN, Ethernet, USB, basse consommation)
cloturent le bloc de la formation non realisable en simulation sur cette
carte. Elles seront reprises en pratique a l'acquisition de la Nucleo-H563ZI
mentionnee en phase 11, qui couvre l'integralite des peripheriques
necessaires. La formation reprend en pratique des la phase 17 (stockage).