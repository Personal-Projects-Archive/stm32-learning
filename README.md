# 01 — Mise en place de l'environnement STM32

## Objectif
Valider la chaine complete : STM32CubeMX + STM32CubeIDE for VS Code + toolchain GCC ARM +
simulation Wokwi, avant d'attaquer le programme de formation STM32.

## Cible
STM32F103C8T6 (Blue Pill), simule sur Wokwi — aucune carte physique.

## Outils installes
- STM32CubeMX (lance depuis le panneau STM32CUBE de VS Code) : utilise uniquement pour fournir
  la structure de projet et les headers CMSIS (Drivers/CMSIS) — aucun peripherique configure via
  l'outil graphique, aucun code HAL utilise.
- STM32CubeIDE for Visual Studio Code (extension pack STMicroelectronics)
- Wokwi for VS Code

## Pourquoi STM32CubeMX ici
Le mode "Create empty project" de l'extension ne fournit pas les headers CMSIS (limitation connue,
signalee sur le forum STMicroelectronics). Passer par STM32CubeMX, sans configurer de peripherique,
recupere Drivers/CMSIS + Core/Inc correctement, sans imposer le HAL.

## Structure
- `Core/Src/main.c` : blink PC13 en acces registre direct (CMSIS RCC/GPIO), ecrit a la main dans
  les sections `USER CODE BEGIN 2` / `USER CODE BEGIN WHILE`
- `Drivers/CMSIS/` : headers generes par CubeMX (non modifies)
- `wokwi.toml` : pointe Wokwi vers le binaire ELF genere
- `diagram.json` : declare la carte Blue Pill simulee

## Compilation
Preset CMake **Debug**

## Test
`Wokwi: Start Simulator` -> la LED PC13 clignote

## Difficultes rencontrees
"Create empty project" : erreur de compilation `stm32f1xx.h: No such file or directory`
(headers CMSIS absents de ce mode).

## Solution
Regeneration du projet via STM32CubeMX (toolchain CMake, aucun peripherique configure) pour
recuperer les headers CMSIS, en gardant un code 100% ecrit a la main (pas de HAL).

## Notions apprises
- Deux chemins de creation de projet dans STM32CubeIDE for VS Code : "empty project" vs projet
  genere par CubeMX, et la limite du premier (pas de CMSIS inclus)
- Acces registre direct via CMSIS (RCC, GPIO) sans passer par HAL
- Sections USER CODE d'un projet genere par CubeMX (preservees a la regeneration)
- Configuration Wokwi pour VS Code (wokwi.toml + diagram.json)