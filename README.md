# 10 — I2C : lecture continue MPU6050 + retour LED PWM

## Objectif
Lire un capteur I2C (MPU6050) en boucle continue, verifier sa presence via le
registre WHO_AM_I, puis piloter une LED en PWM proportionnellement a
l'acceleration lue sur l'axe X, en temps reel.

## Cible
STM32C031C6 (Nucleo-64), simule sur Wokwi.

## Configuration
- I2C1 : PA9 (SCL) / PA10 (SDA) — assignation par defaut de CubeMX pour cette
  puce (differente de PB6/PB7 vu sur d'autres familles/tutoriels)
- TIMINGR (vitesse du bus I2C) genere par STM32CubeMX plutot que calcule a la
  main : formule trop sensible aux erreurs pour la deriver soi-meme de maniere
  fiable ; MX_I2C1_Init() est garde tel que genere, sans modification
- TIM3_CH1 (PA6) en PWM, rapport cyclique pilote directement par la valeur
  ACCEL_XOUT lue sur le capteur
- Aucune pause artificielle dans la boucle de lecture : la transaction I2C
  elle-meme cadence le rythme, pour un retour quasi instantane

## Structure
- `Core/Src/main.c` : MX_I2C1_Init() genere par CubeMX (non modifie) + toute
  la logique applicative (lecture WHO_AM_I, lecture continue ACCEL_XOUT,
  pilotage PWM) ecrite a la main, en acces registre direct
- `diagram.json` : carte + MPU6050 (SCL/SDA sur PA9/PA10) + LED externe sur PA6

## Test
- LD4 (LED embarquee) allumee fixe : confirme que WHO_AM_I a repondu 0x68
- LED externe (PA6) : luminosite qui suit le curseur "Acceleration X" du
  panneau du capteur, en quasi temps reel

## Difficultes rencontrees et solutions
Cette phase a demande plusieurs allers-retours de diagnostic, sans acces
fiable a un debugger ni au moniteur serie dans cet environnement :

1. **Ecriture de reveil du capteur bloquante** — le code initial reveillait
   le MPU6050 (sortie du mode veille, registre PWR_MGMT_1) avant la lecture,
   comme sur un capteur physique reel. Cette ecriture bloquait indefiniment
   en simulation. Cause probable : Wokwi ne modelise pas l'etat de veille
   interne du capteur simule.

2. **Diagnostic sans outil** — face a un blocage silencieux, mise en place
   d'un systeme de clignotements codes (un nombre de clignotements distinct
   a chaque etape du programme) pour localiser precisement le point de
   blocage sans debugger ni terminal. Methode reutilisable pour toute phase
   future si le meme probleme se represente.

3. **Retour visuel trop lent** — une pause de securite (3 millions de cycles)
   en fin de boucle rendait le retour non "temps reel" percu. Retiree : rien
   ne l'imposait, la transaction I2C suffit a cadencer naturellement.

## Notions apprises
- Motif ecriture-puis-lecture (re-START) en I2C : CR2 (adresse, NBYTES,
  START, AUTOEND, RD_WRN), TXDR/RXDR, flags TXIS/RXNE/TC/STOPF
- Timeout systematique sur toute attente de flag bloquante, pour eviter un
  gel silencieux permanent
- Diagnostic par signal visuel code (nombre/vitesse de clignotements) comme
  methode de secours en l'absence de debugger fiable
- Isoler une variable a la fois (test minimal, ajout progressif) plutot que
  de modifier un gros bloc de code d'un coup face a un bug
- Ne pas supposer qu'un capteur simule reproduit tout le comportement d'un
  capteur physique (etat de veille, alimentation)