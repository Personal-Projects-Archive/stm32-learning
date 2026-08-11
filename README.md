# 04 — Timers : TIM3 periodique + interruption

## Objectif
Configurer TIM3 en mode periodique (PSC/ARR) et declencher une interruption
d'update pour piloter la LED, plutot qu'une boucle logicielle.

## Configuration
- TIM3, horloge APB1 (48 MHz)
- PSC = 47999, ARR = 999 -> update event a 1 Hz
- NVIC_EnableIRQ(TIM3_IRQn)

## Structure
- `Core/Src/main.c` : init timer (USER CODE BEGIN 2), handler d'interruption
  TIM3_IRQHandler (USER CODE BEGIN 4)

## Test
Clignotement LD4 a ~1 Hz, ou verification par debugger (breakpoint dans le handler,
watch sur TIM3->CNT) si le rendu visuel n'est pas fiable en simulation.

## Notions apprises
- PSC / ARR / update event / UIF
- Double activation necessaire : DIER (peripherique) + NVIC (coeur)
- EGR/UG pour precharger les registres a l'init
- Le timer materiel semble mieux respecter le timing simule qu'une boucle logicielle