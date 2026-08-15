# 09 — SPI : registre a decalage 74HC595

## Objectif
Envoyer un octet via SPI1 vers un 74HC595, verifier le resultat par un motif
lumineux visible (pas de moniteur serie ni debugger necessaires).

## Configuration
- SPI1, maitre, CS logiciel (SSM/SSI), trame 8 bits, ~6 MHz
- PA5 = SCK, PA7 = MOSI (AF0), PA4 = latch STCP (GPIO classique)

## Structure
- `Core/Src/main.c` : config SPI1 (USER CODE BEGIN 2), boucle d'envoi + latch
- `diagram.json` : 74HC595 + 4 LED sur Q0-Q3

## Test
Une LED allumee a la fois, se deplace de Q0 a Q3 en boucle.

## Notions apprises
- SPI maitre/esclave, CPOL/CPHA, gestion logicielle du CS (SSM/SSI)
- Registre CR2.DS : la taille de trame (8 bits) doit etre configuree explicitement,
  pas de valeur par defaut fiable a supposer
- RCC->APBENR2 pour SPI1 (meme registre qu'ADC et TIM1)
- Verification par retour visuel