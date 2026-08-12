# 05 — Interruptions : EXTI13 sur bouton PC13

## Objectif
Remplacer la lecture par polling (phase 2) par une interruption EXTI, avec un
controle explicite du front declencheur et un anti-rebond logiciel.

## Configuration
- EXTI13 -> port C (EXTI->EXTICR[3], pas de SYSCFG sur cette famille)
- Front descendant uniquement (EXTI->FTSR1)
- Anti-rebond par horodatage (HAL_GetTick), fenetre de 200ms
- Handler dans Core/Src/stm32c0xx_it.c (deja genere par CubeMX pour ce pin,
  pas dans main.c)

## Structure
- `Core/Src/main.c` : configuration EXTI (USER CODE BEGIN 2)
- `Core/Src/stm32c0xx_it.c` : logique metier dans EXTI4_15_IRQHandler
  (USER CODE BEGIN EXTI4_15_IRQn 1)

## Test
Un appui = un seul basculement LED, stable.

## Difficultes rencontrees
- Handler duplique (une fois genere par CubeMX dans stm32c0xx_it.c, une fois
  ajoute a tort dans main.c) -> erreur de link "multiple definition"
- Rebond observe meme en simulation malgre l'hypothese initiale contraire
  (bouton simule suppose sans rebond mecanique) -> anti-rebond necessaire

## Solution
- Handler laisse dans stm32c0xx_it.c uniquement, retire de main.c
- Anti-rebond logiciel par horodatage (HAL_GetTick, fenetre 200ms)

## Notions apprises
- ISR, NVIC, IMR, RTSR/FTSR, flag pending
- CubeMX genere les handlers dans stm32c0xx_it.c pour les broches qu'il connait
  (bouton, LED) — verifier ce fichier avant d'ecrire un handler dans main.c
- Meme un bouton simule peut necessiter un anti-rebond
- Anti-rebond simple par horodatage (HAL_GetTick), sans timer dedie
