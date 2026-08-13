# 06 — ADC : lecture potentiometre -> PWM

## Objectif
Lire une tension analogique sur PA0 et l'utiliser directement pour piloter le
rapport cyclique d'un PWM (TIM3_CH1, PA6), sans mise a l'echelle.

## Configuration
- PA0 en mode analogique, ADC1 canal 0
- Calibration + regulateur interne (ADVREGEN) avant activation
- TIM3 : ARR = 4095 (matche la resolution 12 bits de l'ADC)
- CCR1 = valeur brute ADC a chaque conversion

## Structure
- `Core/Src/main.c` : init ADC + PWM (USER CODE BEGIN 2), boucle de conversion continue
- `diagram.json` : potentiometre sur PA0, LED sur PA6

## Test
Tourner le potentiometre fait varier la luminosite de la LED en direct.

## Notions apprises
- Resolution, canal, temps d'echantillonnage, calibration
- Sequence specifique a cette famille : ADVREGEN avant ADCAL
- RCC->APBENR2 pour l'horloge ADC (bus different de GPIO/TIM3)
- Combiner deux peripheriques (ADC + PWM) sans passer par le CPU pour le calcul,
  juste une recopie de registre a registre