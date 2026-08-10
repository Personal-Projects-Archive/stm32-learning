# 02 — GPIO : bouton + LED

## Objectif
Lire un bouton en entree (pull-up interne) et piloter la LED embarquee en sortie,
en acces registre direct.

## Cible
STM32C031C6 (Nucleo-64), simule sur Wokwi.

## Configuration
- PA5 (LD4, LED embarquee) : sortie push-pull
- PC13 : entree, pull-up interne, bouton externe simule (wokwi-pushbutton) cable dessus
- Horloge : HSE -> SYSCLK (config par defaut generee par CubeMX pour cette carte)

## Pourquoi un bouton externe
Le bouton utilisateur embarque (B1) de la carte simulee ne reagit pas au clic dans
Wokwi for VS Code (limitation apparente du simulateur, pas du firmware) — bouton
externe cable sur PC13 en remplacement.

## Structure
- `Core/Src/main.c` : lecture/ecriture registre directe, pas d'interruption
- `diagram.json` : Nucleo-C031C6 + bouton poussoir externe sur PC13/GND

## Compilation
Preset CMake Debug

## Test
Wokwi: Start Simulator -> maintenir le bouton = LED allumee, relacher = LED eteinte

## Difficultes rencontrees
Premiere version basee sur un exemple BSP genere (interruption EXTI) : double bascule
par appui (front montant + descendant).

## Solution
Retour a une lecture registre en boucle (polling), sans interruption ni BSP

## Notions apprises
- MODER/PUPDR (GPIOv2, familles C0/G0/L0) vs CRL/CRH (GPIOv1, famille F1)
- RCC_IOPENR vs RCC_APB2ENR selon la famille
- Difference entre logique de niveau (polling) et logique par interruption/toggle
- Le bouton embarque d'une carte simulee n'est pas forcement cliquable dans Wokwi

## Limites / ameliorations possibles
Pas d'anti-rebond — non necessaire ici (lecture de niveau, pas de comptage d'evenements).
Gestion propre par interruption EXTI prevue en phase 3.