# MISRA C et qualite de code : avertissements stricts, relecture (phase 25)

## Objectif
Appliquer des principes de qualite de code embarque directement sur le code
deja ecrit, via des avertissements compilateur stricts et une relecture
manuelle guidee, phase entierement pratique, sans dependance materielle.

## Exercice realise
- Ajout de -Wextra -Wconversion -Wshadow -Wundef -Wcast-align -Wswitch-default
  dans CMakeLists.txt (target_compile_options)
- 7 avertissements obtenus au premier build :
  - app_freertos.c : 3x unused parameter 'argument' (StartTaskSensor,
    StartTaskControl, StartTaskMonitor)
  - main.c : 1x cast increases required alignment (-Wcast-align) dans
    compute_crc
  - syscalls.c : 2x avertissements de conversion (-Wconversion,
    -Wsign-conversion)
  - ld.exe : 1x LOAD segment with RWX permissions (deja identifie phase 22)

## Corrections appliquees
- app_freertos.c : ajout de `(void)argument;` explicite en premiere ligne
  des trois fonctions de tache - le parametre est impose par l'API FreeRTOS
  mais non utilise ici, autant le dire clairement au compilateur
- main.c : remplacement du cast direct `(const uint32_t *)data` par une
  lecture via `memcpy`, qui fonctionne quel que soit l'alignement reel du
  pointeur source, un cast direct suppose un alignement non garanti, exactement
  le type de comportement que MISRA C interdit

## Non corrige, et pourquoi
- syscalls.c : fichier entierement genere par ST, sans zone USER CODE
  disponible, le regenerer l'ecraserait de toute facon, aucune action
  pertinente possible de notre cote
- ld.exe (RWX permissions) : avertissement cosmetique deja identifie et
  explique en phase 22, sans impact fonctionnel

## Resultat final
7 avertissements ramenes a 3, les 3 restants etant precisement ceux
identifies comme hors de notre controle ou sans impact reel, aucun
avertissement corrigible laisse de cote.

## Relecture manuelle (main.c de la phase 10, I2C)
- Nombres magiques (0x68, 0x75, 0x6B, 0x3B) : deja nommes via #define des
  la premiere ecriture, rien a corriger
- Boucles d'attente bloquante : deja protegees par timeout suite au
  diagnostic realise a l'epoque de cette meme phase, coherent avec la
  correction apportee alors, pas une regle appliquee seulement apres coup

## Notions apprises
- MISRA C : regles d'interdiction, pas de recommandations de style
- Le C embarque tolere beaucoup moins l'erreur qu'un script personnel :
  pas de systeme d'exploitation pour rattraper un crash, fonctionnement
  parfois non supervise pendant des annees
- Les avertissements du compilateur sont une premiere forme d'analyse
  statique, gratuite et deja disponible sans outil supplementaire
- (void)parametre; comme facon explicite de documenter un parametre
  volontairement inutilise, plutot que de laisser un avertissement silencieux
- Un cast de pointeur vers un type plus large peut violer une contrainte
  d'alignement materiel non garantie, memcpy est la facon sure de
  reinterpreter des octets sans cette hypothese
- Savoir distinguer un avertissement a corriger d'un avertissement a
  documenter comme accepte (code genere, limitation connue) fait partie
  du travail, pas seulement corriger sans discernement

## Limites / ameliorations possibles
Pas d'outil d'analyse statique MISRA dedie installe (type PC-lint, Cppcheck
avec regles MISRA), les avertissements compilateur standards couvrent une
partie seulement des regles MISRA reelles.