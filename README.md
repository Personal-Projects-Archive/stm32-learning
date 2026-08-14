# 08 — UART : terminal serie via USART2

## Objectif
Envoyer un message de bienvenue et lire des commandes envoyees depuis le terminal
serie pour piloter la LED, en polling (sans interruption ni DMA).

## Configuration
- USART2, 115200 bauds, 8N1
- PA2 = TX (AF1), PA3 = RX (AF1)
- Relie au port serie virtuel du ST-LINK simule par Wokwi, aucun cablage externe

## Structure
- `Core/Src/main.c` : init USART2 + boucle de lecture/echo/commande (USER CODE BEGIN 2)

## Test
Le terminal serie Wokwi affiche le message de bienvenue. Taper 'o' allume la LED,
'f' l'eteint, chaque caractere tape est renvoye en echo.

## Notions apprises
- BRR, TXE, RXNE, trame 8N1
- RCC->APBENR1 pour USART2 (registre different d'ADC/TIM1 qui sont sur APBENR2)
- Port serie virtuel du ST-LINK simule automatiquement par Wokwi (pas de cablage)

## Limites / ameliorations possibles
Polling bloquant, commandes limitees a un seul caractere — passage a l'interruption
et parsing de chaines completes a prevoir si besoin plus tard.