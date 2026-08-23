# Linker et memoire : script, sections, fichier .map (phase 21)

## Objectif
Comprendre le script linker (MEMORY/SECTIONS) et le fichier .map en les
manipulant directement, plutot qu'en theorie seule (deja vue en phase 1/3).

## Exercice realise
- Reduction volontaire de LENGTH(RAM) de 12K a 1K dans STM32C031xx_FLASH.ld
- Erreur de link obtenue : "RAM: 8760 B / 1 KB - 855.47% Used",
  collect2.exe: error: ld returned 1 exit status
- Restauration a 12K, build reussi confirme : RAM 8760 B / 12 KB (71.29%)

## Verification dans le .map
- _estack = 0x20003000 (= ORIGIN(RAM) 0x20000000 + LENGTH(RAM) 12K, coherent)
- _sstack = 0x20002c00 (= _estack - _Min_Stack_Size, donc _Min_Stack_Size = 0x400)
- valeur_partagee (phase 19) = 0x20001bd4, bien a l'interieur de la plage RAM

## Difficulte rencontree
Apres modification du .ld, un premier rebuild a affiche "ninja: no work to do"
sans recompiler quoi que ce soit (Build completed en 0.3s, trop rapide pour
etre reel). Ninja ne detecte pas toujours un changement dans le script linker comme il le ferait
pour un fichier .c. Solution : suppression complete du dossier build/ avant
de relancer, systematique des qu'un changement hors fichier .c/.h ne semble
pas pris en compte.

## Notions apprises
- MEMORY declare les regions physiques (ORIGIN/LENGTH), SECTIONS dit ou va
  chaque partie du programme
- _estack = ORIGIN(RAM) + LENGTH(RAM) : une valeur fausse ici corromprait
  le fonctionnement de la pile des le demarrage
- Le fichier .map est la source de verite sur le placement reel, plus fiable
  qu'une supposition basee sur la theorie seule
- Une region memoire mal declaree donne une erreur de link claire (pourcentage
  d'usage superieur a 100%).
- Ninja peut ignorer un changement dans un fichier .ld sans supprimer build/.

## Limites / ameliorations possibles
Separation bootloader/application (deux regions Flash distinctes) non
illustree ici - sujet direct de la phase 22.