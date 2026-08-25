# STM32 Learning — Formation bare-metal STM32, de zéro à FreeRTOS

## Présentation
Formation personnelle en autoformation, structurée en phases progressives,
allant de l'architecture Cortex-M jusqu'à une architecture multi-tâches
FreeRTOS complète. Suit la même logique que le parcours esp32-idf-learning
réalisé au préalable, avec des comparaisons ponctuelles entre les deux
écosystèmes tout au long du parcours.

## Environnement
- STM32CubeIDE for Visual Studio Code (extension pack STMicroelectronics)
- STM32CubeMX (configuration graphique, generation de code CMake)
- Wokwi for VS Code (simulation)
- Un seul projet CMake, écrasé et régénéré à chaque phase, historique
  conservé via les tags Git plutôt que par des dossiers séparés

## Carte utilisée en simulation
STM32C031C6 (Nucleo-64), choisie après un premier essai sur STM32 Blue Pill
(F103C8T6) abandonné pour des soucis de câblage fiable dans l'éditeur
Wokwi. Board officiel, bouton (B1/PC13) et LED (LD4/PA5) embarqués déjà
câblés, évitant la dépendance à des composants externes pour les tests de
base.

## Contrainte posée dès le début, respectée tout du long
Wokwi ne simule que 3 cartes STM32 au total, et aucune ne simule DMA, CAN,
watchdogs, basse consommation, ni les bus USB/Ethernet (absents du silicium
sur cette puce d'entrée de gamme). Ces phases ont été traitées en
documentation uniquement (théorie + code de référence non testé), signalées
clairement, sans jamais prétendre à une validation qui n'a pas eu lieu.

## Matériel recommandé pour la suite
**Nucleo-H563ZI** (STM32H5, Cortex-M33, TrustZone, FDCAN, USB, Ethernet,
accélérateurs cryptographiques), carte unique permettant de compléter en
pratique l'ensemble des phases restées théoriques ici (DMA, watchdogs, CAN,
Ethernet, USB, basse consommation, sécurité avancée), avec un projet complet
de A à Z prévu à ce moment-là.

## Phases réalisées

| Phase | Sujet | Statut |
|---|---|---|
| 00 | Environnement (CubeMX, CubeIDE for VS Code, Wokwi) | Testé |
| 01 | GPIO (bouton + LED) | Testé |
| 02 | Clocks (HSI, HSIDIV) | Testé (verifie par debugger, timing simule non fiable) |
| 03 | Timers (periodique, PWM statique, PWM dynamique) | Testé |
| 04 | Interruptions (EXTI) | Testé |
| 05 | ADC (potentiometre -> PWM) | Testé |
| 06 | DAC | Theorique (absent sur cette puce) |
| 07 | PWM avance (TIM1, sorties complementaires) | Testé |
| 08 | UART | Compile, comportement non verifie (outillage) |
| 09 | SPI (74HC595) | Testé |
| 10 | I2C (MPU6050, temps reel) | Testé |
| 11 | DMA | Theorique |
| 12 | Watchdogs | Theorique |
| 13 | CAN | Theorique |
| 14 | Ethernet | Theorique (absent sur cette puce) |
| 15 | USB | Theorique (absent sur cette puce) |
| 16 | Basse consommation | Theorique |
| 17 | Stockage (CRC pratique, Flash/FatFS theoriques) | Partiellement teste |
| 18 | FreeRTOS (CMSIS-RTOS2, ISR -> Queue -> Task) | Testé |
| 19 | Architecture RTOS (pipeline 3 taches, queue + mutex) | Testé (confirme via debugger, phase 20) |
| 20 | Debugging (session GDB via Wokwi) | Testé |
| 21 | Linker et memoire (MEMORY/SECTIONS, .map) | Testé |
| 22 | Bootloader (repartition memoire, CRC de validation) | Partiellement teste |
| 23 | Firmware update (reception UART, ecriture Flash) | Code complet, non teste (materiel requis) |
| 24 | Securite (modele de menace, RDP, MPU, TrustZone) | Theorique |
| 25 | Qualite de code (avertissements stricts, MISRA) | Testé |
| 26 | Tests (logique extraite, testee hors cible) | Testé |

## Méthodologie retenue
- Accès registre direct systématique, HAL utilisé uniquement quand le
  calcul sous-jacent est trop sensible à reproduire à la main (TIMINGR I2C,
  clock tree)
- Un seul projet, écrasé à chaque phase, historique via tags Git plutôt que
  duplication de dossiers
- Vérification systématique par un retour observable (LED, comptage,
  debugger) plutôt que par supposition, plusieurs bugs réels rencontrés
  (résidus de code BSP, zones USER CODE dupliquées, désynchronisation de
  build) n'auraient jamais été détectés autrement
- Distinction claire, à chaque phase, entre ce qui est prouvé fonctionnel et
  ce qui reste théorique

## Prochaine étape
Acquisition de la Nucleo-H563ZI, puis reprise d'un projet complet de A à Z
couvrant l'ensemble des phases restées théoriques ici (DMA, CAN, Ethernet,
USB, basse consommation, sécurité avancée avec TrustZone), sur du materiel
reel plutot qu'en simulation.