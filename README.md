# Architecture RTOS : pipeline Sensor -> Control -> Monitor (phase 19)

## Objectif
Concevoir une architecture a trois taches avec roles distincts, reliees par
une queue (pipeline de donnees) et un mutex (ressource partagee) - les deux
mecanismes non illustres en phase 18.

## Architecture
TaskSensor (lit ADC PA0) --Queue--> TaskControl (pilote PWM PA6, ecrit dans
une variable partagee) --Mutex--> TaskMonitor (lit la variable partagee,
allume LD4 selon un seuil)

## Configuration
- QueueSensorControl : taille 1, item uint32_t
- MutexShared : protege la variable partagee entre TaskControl et TaskMonitor
- Stack size 256 mots par tache, TOTAL_HEAP_SIZE 8192

## Test
Potentiometre tourne -> LD4 s'allume au-dessus de la moitie de course.

## Notions apprises
- Difference entre queue (transfert de donnees, pipeline) et mutex
  (protection d'une ressource lue/ecrite par plusieurs taches)
- Une variable partagee sans mutex est une source de corruption/incoherence
  des que plus d'une tache y touche
- Conception d'architecture : donner un role et une priorite distincts a
  chaque tache plutot que tout mettre dans une seule boucle

## Limites / ameliorations possibles
Pas de TaskLogger ni TaskWatchdog demontres ici (mentionnes dans le
programme d'origine) - extension possible si besoin plus tard.