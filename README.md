# Phase 12 — Watchdogs (IWDG / WWDG)

## Statut
Non simulable. Le tableau de fonctionnalites officiel de Wokwi pour cette
carte indique IWDG et WWDG comme non implementes. Cette phase reste
theorique jusqu'a l'acquisition d'une carte physique (voir phase 11 pour
la recommandation materielle : Nucleo-H563ZI).

## Objectif de la phase
Comprendre et mettre en oeuvre les deux mecanismes de surveillance logicielle
disponibles sur STM32 : le watchdog independant (IWDG) et le watchdog a
fenetre (WWDG). Les deux servent a detecter un blocage logiciel et a
provoquer une reinitialisation automatique du systeme si le programme
principal cesse de repondre.

## IWDG (Independent Watchdog)
- Cadence sur un oscillateur independant du reste du systeme (LSI, quelques
  dizaines de kHz), ce qui le rend actif meme si l'horloge principale
  tombe en panne
- Compteur descendant : demarre a une valeur de reload, decremente en
  continu ; s'il atteint zero, reinitialisation immediate du systeme
- Rafraichissement ("petting the dog") : le programme doit ecrire
  regulierement une valeur cle dans un registre pour recharger le compteur
  avant qu'il n'atteigne zero
- Usage typique : filet de securite generique, insensible aux problemes
  d'horloge principale

## WWDG (Window Watchdog)
- Cadence sur l'horloge principale du systeme (contrairement a l'IWDG)
- Ajoute une notion de fenetre : le rafraichissement doit intervenir ni
  trop tot, ni trop tard - une fenetre de temps precise, pas juste une
  limite haute
- Rafraichir trop tot est aussi une faute que de rafraichir trop tard :
  ca detecte des dysfonctionnements plus subtils qu'une simple boucle qui
  se bloque (ex. une tache qui boucle anormalement vite)
- Peut declencher une interruption juste avant la reinitialisation (EWI,
  Early Wakeup Interrupt), utile pour journaliser l'incident avant le reset

## Configuration a prevoir dans STM32CubeMX (a faire une fois la carte en main)

1. Categorie **System Core > IWDG** (ou **WWDG**) : Activated
2. Pour l'IWDG : regler le Prescaler et la valeur de Reload pour obtenir le
   delai de timeout souhaite (le calcul depend de la frequence LSI reelle,
   a verifier dans le datasheet de la puce utilisee)
3. Pour le WWDG : regler le Prescaler, la valeur Window et la valeur Counter
   pour definir la fenetre de rafraichissement valide
4. Generer le code, puis appeler la fonction de rafraichissement HAL
   correspondante (`HAL_IWDG_Refresh()` / `HAL_WWDG_Refresh()`) a un endroit
   du programme execute regulierement et de maniere fiable

## Notions a retenir en attendant la mise en oeuvre reelle
- L'IWDG survit a une panne de l'horloge principale, le WWDG non
- Le WWDG detecte a la fois "trop tard" ET "trop tot", l'IWDG seulement
  "trop tard"
- Ne jamais rafraichir un watchdog depuis une interruption qui continuerait
  a s'executer meme si la boucle principale est bloquee : ca annulerait
  l'interet du mecanisme
- Strategie de projet suggeree (a implementer sur materiel reel) : systeme
  qui detecte une tache bloquee et recupere automatiquement, avec le
  watchdog comme filet de securite final

## Prochaine etape
Phase 13 (CAN), meme format documentaire prevu.