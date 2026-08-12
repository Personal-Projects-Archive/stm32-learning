# 04 partie 3 — Timers : PWM dynamique, effet respiration

## Objectif
Faire varier CCR1 dans le temps via l'interruption d'update de TIM3, pour un fondu
progressif de luminosite, sans boucle logicielle.

## Decouverte experimentale
Augmenter ARR seul (CCR1 fixe) diminue la luminosite percue -> confirme concretement
que le rapport cyclique est CCR/(ARR+1), pas une valeur absolue. Confirme aussi que
Wokwi modelise correctement le comportement des timers materiels (contrairement a une
boucle logicielle CPU).

## Configuration
- PWM 1 kHz (PSC=47, ARR=999) sur TIM3_CH1 (PA6)
- CCR1 pilote par TIM3_IRQHandler, +-1 tous les 5 update events

## Test
LED externe en fondu progressif continu (~10s par cycle complet)

## Notions apprises
- Rapport cyclique = ratio, pas valeur absolue (confirme par observation directe)
- Pilotage d'un registre CCR depuis une interruption plutot que la boucle principale
- Les timers materiels sont fiables en simulation Wokwi, a la difference des delais
  logiciels bases sur le comptage de cycles CPU