# Securite embarquee : modele de menace, protections disponibles (phase 24)

## Statut
Theorique. Certains mecanismes decrits ici sont absents du STM32C031C6
(TrustZone, accelerateur cryptographique materiel), d'autres existent
reellement sur cette puce mais sont trop risques a manipuler en simulation
pour etre testes ici (option de lecture protegee, avec un niveau
irreversible).

## Objectif
Etablir un modele de menace pour un systeme embarque, et faire l'inventaire
precis des protections reellement disponibles sur cette puce, en distinguant
clairement ce qui existe de ce qui n'existe pas.

## Modele de menace : contre quoi se proteger

Quatre categories de risques a considerer separement, chacune avec des
contre-mesures differentes :

1. Extraction du firmware par un acteur ayant un acces physique a la carte
   (via le port de debug SWD) - contre-mesure : option de lecture protegee
2. Execution d'un firmware modifie ou malveillant au demarrage - contre-mesure :
   verification d'integrite avant de faire confiance au code (CRC deja
   pratique en phase 17 et 22, signature cryptographique pour aller plus loin)
3. Installation d'une version anterieure du firmware, contenant une faille
   deja corrigee (attaque par retour en arriere) - contre-mesure :
   anti-rollback, un numero de version qui ne peut que croitre
4. Fuite d'une cle secrete stockee dans le firmware (cle de chiffrement,
   cle d'API, mot de passe) si le firmware est extrait ou lu par retro-ingenierie
   - contre-mesure : stockage materiel dedie, absent de cette puce (voir plus bas)

## Option de lecture protegee (RDP) - reelle sur cette puce, non testee ici

Mecanisme present sur la quasi-totalite des STM32, y compris celui-ci, via
les "option bytes" (des octets de configuration stockes a part de la Flash
applicative, modifiables uniquement par un outil comme STM32CubeProgrammer,
pas par le programme lui-meme au moment de l'execution normale) :
- Niveau 0 : aucune protection, lecture et debug libres - le niveau utilise
  depuis le debut de cette formation
- Niveau 1 : lecture de la Flash bloquee via le port de debug, mais
  reversible - repasser au niveau 0 efface toute la puce (protection contre
  la copie, pas contre la remise a zero par le legitime proprietaire)
- Niveau 2 : verrouillage definitif et irreversible, meme le retour au
  niveau 0 est impossible - le port de debug est desactive pour toujours

Raison de ne pas tester ceci ici : le niveau 2 est irreversible par
definition, et meme le niveau 1 modifie un comportement fondamental
(acces debug) que ce projet utilise en continu depuis la phase 00 -
aucune raison de risquer de perdre cet acces pour une demonstration.
A faire uniquement sur une carte dediee, en connaissance de cause, jamais
sur la carte principale de developpement.

## MPU (Memory Protection Unit) - presence incertaine sur cette puce

Le Cortex-M0+ supporte une MPU en tant qu'option architecturale (jusqu'a
16 regions, meme famille de registres que sur M3/M4/M7). Cependant, la
documentation officielle ST listant les series supportees par la gestion
MPU (AN4838) mentionne explicitement G0, mais pas C0, cette derniere,
positionnee comme la serie la plus reduite en cout, a potentiellement omis
ce bloc pour reduire la surface de silicium, dans la meme logique qui
explique l'absence du DAC (phase 6) et de l'USB (phase 15). A verifier
directement dans le reference manual RM0490 avant de s'appuyer dessus.
Si presente, la MPU sert a isoler des zones memoire (empecher une tache
FreeRTOS de corrompre la pile d'une autre, par exemple), un usage
pertinent avec l'architecture multi-tache deja mise en place en phase 19.

## TrustZone - absent, confirme

Le Cortex-M0+ ne supporte pas TrustZone, extension reservee aux coeurs
Cortex-M23 et Cortex-M33. Aucune ambiguite ici, contrairement a la MPU.
La Nucleo-H563ZI (recommandee en phase 11, Cortex-M33) supporte TrustZone :
isolation materielle entre un monde "securise" (cles, code sensible) et un
monde "non securise" (reste de l'application), avec des violations d'acces
bloquees par le materiel plutot que par la seule discipline du programmeur.

## Chiffrement et stockage de cles - pas d'acceleration materielle ici

Cette puce n'integre pas de bloc AES materiel ni de stockage de cles dedie
(HSM ou equivalent). Un chiffrement logiciel reste possible mais coute des
cycles CPU et surtout n'apporte aucune protection contre l'extraction d'une
cle stockee en clair dans le firmware lui-meme, un attaquant qui lit la
Flash lit aussi la cle. La Nucleo-H563ZI integre des accelerateurs
cryptographiques materiels (AES, hachage, cle privee) justement concus pour
ce cas d'usage.

## Signature de firmware et anti-rollback - meme limite que phases 22/23

Verifier une signature cryptographique (pas seulement un CRC) avant de
sauter vers une application necessiterait le meme bootloader personnalise
deja ecarte en phase 22 pour des raisons de restructuration de build. Le
principe reste le meme que celui deja documente pour le CRC : le bootloader
recalcule et compare avant de faire confiance au code, la difference est
la nature de la verification (signature asymetrique plutot que simple
somme de controle) et donc sa resistance a une falsification volontaire
(un CRC peut etre recalcule par un attaquant apres modification du firmware,
une signature necessite la cle privee, jamais presente sur la carte).
Un compteur de version stocke en Flash, verifie avant d'accepter une mise
a jour et incremente uniquement par une mise a jour valide, constitue
l'anti-rollback, egalement non implemente ici pour la meme raison.

## Notions apprises
- Un modele de menace precede toujours le choix des protections, proteger
  contre tout indistinctement gaspille des ressources sans ameliorer la
  securite reelle
- RDP : trois niveaux, le niveau 2 est irreversible, a ne jamais manipuler
  sans en comprendre pleinement les consequences au prealable
- Un CRC protege contre une corruption accidentelle, pas contre une
  modification volontaire, une signature cryptographique proteges contre
  les deux, au prix d'une complexite et d'un besoin materiel plus eleves
- Une cle stockee en clair dans un firmware sans stockage materiel dedie
  n'est jamais vraiment secrete face a un attaquant ayant acces au firmware

## Limites / ameliorations possibles
Verification directe de la presence ou de l'absence de la MPU dans le
reference manual RM0490, non faite ici. Mise en oeuvre reelle de RDP niveau
1, signature de firmware et anti-rollback a prevoir uniquement sur la
Nucleo-H563ZI (phase 11), qui dispose du materiel adapte (TrustZone,
accelerateurs cryptographiques) pour une implementation complete et
pertinente.