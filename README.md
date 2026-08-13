# 07 — PWM avance : TIM1, sorties complementaires + temps mort

## Objectif
Configurer TIM1 (timer avance, seul a avoir des sorties complementaires sur cette
puce) avec CH1/CH1N et un temps mort de securite, base commune au controle moteur.

## Configuration
- PA8 = TIM1_CH1 (AF2), PA7 = TIM1_CH1N (AF2)
- PSC = 47, ARR = 999 -> PWM 1 kHz, CCR1 = 500 (50%)
- CCER : CC1E + CC1NE (les deux sorties actives)
- BDTR : DTG = 100 (temps mort), MOE (active physiquement les sorties)

## Structure
- `Core/Src/main.c` : config GPIO AF + TIM1 (USER CODE BEGIN 2)
- `diagram.json` : deux LED, une par sortie (CH1 et CH1N)

## Test
Deux LED a intensite stable, verification rigoureuse du mecanisme temps mort
par debugger (TIM1->BDTR, TIM1->CCER) plutot que par l'oeil (duree trop courte
pour etre visible).

## Notions apprises
- Difference TIM1 (avance, sorties N) vs TIM3 (general, pas de sorties N)
- BDTR : DTG (temps mort) et MOE (activation/coupure globale, aussi fonction
  de securite "break")
- RCC->APBENR2 pour TIM1, mais meme domaine d'horloge (PCLK1) que TIM3
- AF2 pour TIM1_CH1/CH1N sur cette puce (different de AF1 vu sur d'autres
  familles type F4)

## Limites / ameliorations possibles
Pas de veritable moteur ni pont en H simule ici (composant non disponible pour
cette carte sur Wokwi) — demonstration limitee aux deux LED de sortie.