# 03 — Clocks : HSI, HSIDIV, HCLK/PCLK1

## Objectif
Comprendre l'arbre d'horloge du STM32C0 (pas de PLL, un seul bus APB) et verifier
concretement l'effet d'un changement de frequence sur un delai logiciel.

## Cible
STM32C031C6 (Nucleo-64), simule sur Wokwi.

## Configuration CubeMX
- SYSCLK source : HSI (48 MHz fixe)
- HSIDIV : teste a 1 (SYSCLK 48 MHz) puis 8 (SYSCLK 6 MHz)
- HPRE / PPRE1 : /1 (HCLK = PCLK1 = SYSCLK)

## Structure
- `Core/Src/main.c` : blink PA5 par boucle logicielle (delai en cycles CPU, pas HAL_Delay)

## Test
Comparaison visuelle de la vitesse de clignotement entre HSIDIV=1 et HSIDIV=8
(rapport attendu : x8)

## Notions apprises
- HSI/HSE/LSI/LSE, HSIDIV, SYSCLK, HCLK, PCLK1
- Pas de PLL sur la famille C0 (contrairement a F1/F4)
- Un seul bus APB sur cette famille (pas de split APB1/APB2)
- Pourquoi HAL_Delay() ne convient pas pour observer un changement d'horloge
  (recalibrage automatique via SysTick) — boucle logicielle utilisee a la place