# 04 partie 2 — Timers : PWM sur TIM3_CH1

## Objectif
Generer un signal PWM materiel (sortie geree par le timer, pas par le CPU) sur PA6.

## Configuration
- PA6 en Alternate Function 1 (TIM3_CH1)
- PSC = 47, ARR = 999 -> PWM 1 kHz
- CCR1 = 500 -> rapport cyclique 50%
- CCMR1 OC1M = PWM mode 1, CCER CC1E active

## Structure
- `Core/Src/main.c` : config GPIO AF + TIM3 PWM (USER CODE BEGIN 2)
- `diagram.json` : LED externe sur PA6/GND

## Test
LED externe a luminosite reduite et stable (pas de clignotement visible a 1 kHz).
Verification alternative par debugger : TIM3->CNT progresse, TIM3->CCR1 = 500.

## Notions apprises
- Output Compare / PWM mode 1, CCMRx, CCER, preload (OC1PE/ARPE)
- Rapport cyclique = CCR/(ARR+1), independant de la frequence (ARR/PSC)
- Le timer pilote la broche directement en materiel, sans intervention CPU en continu
- Configuration GPIO en mode Alternate Function (MODER=10 + AFR)