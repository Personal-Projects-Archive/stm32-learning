# FreeRTOS : deux taches concurrentes, ISR -> Queue -> Task (phase 18)

## Objectif
Integrer FreeRTOS cote STM32Cube (les bases - taches, scheduler, priorites,
queues - sont deja connues via l'experience ESP32) : creation de taches,
priorites, et le pont ISR -> queue -> tache reveillee.

## Cible
STM32C031C6 (Nucleo-64), simule sur Wokwi. Bouton externe sur PC13 (meme
montage que phase 02).

## Middleware et interface
FreeRTOS integre via le pack **X-CUBE-FREERTOS** (obligatoire sur la famille
C0, pas de middleware FreeRTOS natif sur cette famille contrairement a
d'autres series STM32). Interface **CMSIS-RTOS2** (imposee par ce pack) :
API differente de la FreeRTOS "native" utilisee sur ESP-IDF (`osThreadNew`,
`osMessageQueuePut`, `osDelay` au lieu de `xTaskCreate`, `xQueueSend`,
`vTaskDelay`) - les concepts restent identiques, pas les noms de fonctions.

## Configuration
- **TaskButton** (fonction generee : `StartDefaultTask`, priorite High) :
  bloquee sur `osMessageQueueGet`, reveillee par la queue, bascule LD4
- **TaskBlink** (`StartTaskBlink`, priorite Normal) : boucle independante,
  `osDelay(500)`
- **QueueButton** : pont entre l'interruption EXTI13 et TaskButton, item
  type `uint32_t`
- Stack size portee a 256 mots par tache (128 par defaut insuffisant,
  corruption memoire silencieuse observee avec la valeur par defaut)
- Base de temps HAL deplacee du SysTick vers **TIM17** (recommandation
  standard des lors qu'un RTOS est utilise, evite un conflit entre le tick
  FreeRTOS et la base de temps HAL)
- Anti-rebond logiciel sur l'ISR (horodatage HAL_GetTick, fenetre 200ms)

## Structure
- `Core/Src/app_freertos.c` : corps des taches (genere par CubeMX, USER CODE
  remplies)
- `Core/Src/main.c` : config EXTI13 (USER CODE BEGIN 2)
- `Core/Src/stm32c0xx_it.c` : `osMessageQueuePut` + anti-rebond dans le
  handler EXTI4_15_IRQn (zone USER CODE 1 uniquement - la zone 0 contient
  le code genere pour le bouton embarque de la carte, laissee intacte)

## Test
PA6 clignote en continu (TaskBlink, tache independante). LD4 bascule a
chaque appui sur le bouton (TaskButton, reveillee via la queue depuis
l'interruption).

## Difficultes rencontrees et solutions
- Fonction generee nommee StartDefaultTask malgre le renommage de la tache en TaskButton dans CubeMX
- Code duplique par erreur dans deux zones USER CODE du meme handler (EXTI4_15_IRQn 0 et 1) -> deux appels concurrents a osMessageQueuePut
- Rebond observe sur le bouton simule -> anti-rebond logiciel ajoute (meme technique qu'en phase 02)
- TaskBlink ne demarrait jamais (PA6) : cause reelle identifiee via l'ecran FreeRTOS Heap Usage de CubeMX -> configTOTAL_HEAP_SIZE insuffisant (0 octet disponible affiche), la seconde creation de tache echouait (osThreadNew renvoie NULL sans planter). Corrige en augmentant TOTAL_HEAP_SIZE a 8192 dans Config parameters.
- Ajout d'une verification explicite (handle == NULL) apres les creations de tache, pour detecter ce genre d'echec immediatement au lieu d'un comportement partiel difficile a diagnostiquer

## Notions apprises
- X-CUBE-FREERTOS vs middleware FreeRTOS natif : depend de la famille STM32
- CMSIS-RTOS2 vs API FreeRTOS native : memes concepts, noms de fonctions
  differents
- CMSIS-RTOS2 gere automatiquement le contexte ISR (pas de variante *FromISR
  a ecrire manuellement, contrairement a la FreeRTOS API native)
- Toujours verifier le nom reel de la fonction generee (CubeMX peut garder
  un nom du type `StartDefaultTask` meme apres renommage de la tache)
- Le corps des taches vit dans app_freertos.c, pas main.c
- Necessite de changer la base de temps HAL (SysTick -> timer dedie) des
  qu'un RTOS est utilise
- Une pile de tache trop petite corrompt silencieusement la memoire plutot
  que de planter, symptome difficile a distinguer d'un autre bug

## Limites / ameliorations possibles
Deux taches seulement, pas de mutex/semaphore demontres ici (aucune
ressource partagee entre les deux) - a illustrer en phase 19 (architecture
RTOS multi-taches).