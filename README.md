# Debugging : breakpoints, watch, session GDB via Wokwi (phase 20)

## Objectif
Apprendre a utiliser le debugger integre (breakpoints, watch, call stack)
plutot que de diagnostiquer a l'aveugle par clignotements codes, comme cela
a ete necessaire a plusieurs reprises plus tot dans cette formation.

## Procedure de lancement (specifique a cet environnement)
1. Preset CMake sur Debug
2. Ctrl+Shift+P -> "Wokwi: Start Simulator and Wait for Debugger"
3. F5 pour attacher le debugger (configuration deja presente dans
   .vscode/launch.json, generee avec le projet)

## Notions apprises
- Breakpoint (arret sur une ligne) vs watchpoint (arret sur un changement
  de valeur)
- Panneaux Variables/Watch pour inspecter l'etat du programme fige
- HardFault/BusFault/UsageFault/MemManageFault : types de fautes materielles
  Cortex-M, chacune avec une cause typique differente
- Un stack overflow ou une corruption memoire ne plante pas -
  comportement incoherent sans message, a chercher via watch/breakpoints
  plutot qu'en supposant

## Application
Utilise pour rouvrir l'incident non resolu de la phase 19 (chaine
Mutex -> TaskMonitor)

## Limites / ameliorations possibles
Pas de vue SFR (registres de peripheriques nommes) confirmee disponible
dans cet environnement - l'extension Cortex-Debug (Marus25) est mentionnee
par la documentation officielle comme alternative offrant cette vue, non
testee ici.