# Phase 15 — USB

## Statut
Non simulable, et impossible meme en theorie sur la puce utilisee jusqu'ici
(STM32C031C6) : cette famille d'entree de gamme n'integre pas de peripherique
USB. Cette phase necessite une puce disposant d'un controleur USB device,
comme le STM32H563ZI recommande en phase 11.

## Objectif de la phase
Comprendre le fonctionnement du bus USB cote peripherique (device), et
mettre en oeuvre le STM32 comme peripherique USB CDC (port serie virtuel) -
la classe la plus simple a exploiter depuis un PC, sans driver specifique
a installer.

## Notions fondamentales

- **Host / Device** : l'USB est asymetrique, un hote (generalement un PC)
  pilote toujours la communication, le peripherique (le STM32 ici) repond -
  contrairement au CAN ou a l'I2C multi-maitre potentiel
- **Endpoint** : canal de communication logique entre l'hote et le
  peripherique, identifie par un numero et une direction (IN vers l'hote,
  OUT vers le peripherique) - un peripherique USB expose plusieurs endpoints
  selon les flux qu'il gere
- **Descripteurs** : structures de donnees que le peripherique presente a
  l'hote pour se decrire (fabricant, type de peripherique, endpoints
  disponibles, classe USB...) - l'hote s'appuie dessus pour charger le bon
  driver automatiquement
- **Enumeration** : sequence d'echanges au branchement, ou l'hote decouvre
  le peripherique via ses descripteurs et lui assigne une adresse sur le bus
- **Classes USB standardisees** : permettent a l'hote d'utiliser un driver
  generique sans logiciel specifique a installer :
  - **CDC (Communications Device Class)** : simule un port serie virtuel
    (COM sous Windows, /dev/ttyACM sous Linux) - approche la plus simple,
    reconnu nativement par la plupart des systemes d'exploitation
  - **HID (Human Interface Device)** : simule clavier/souris/manette -
    driver generique lui aussi, utile pour des peripheriques d'entree
    personnalises

## Lien avec ce qu'on a deja fait
La phase 08 (UART) a implemente un terminal serie physique sur les broches
PA2/PA3. Le CDC USB reproduit la meme experience cote logiciel (un port
serie apparait sur le PC), mais transporte les donnees sur le bus USB plutot
que sur une liaison serie filaire classique - le code applicatif (lire/
ecrire des caracteres) reste conceptuellement tres proche.

## Configuration a prevoir dans STM32CubeMX (a faire une fois la carte en main)

1. Categorie **Connectivity > USB** (ou **USB_OTG_FS** selon la puce) :
   activer en mode Device
2. Categorie **Middleware and Software Packs > USB_DEVICE** : Class for
   FS IP = **Communication Device Class (Virtual Port Com)**
3. Verifier les parametres du descripteur (VID/PID par defaut fournis par
   ST pour le developpement, a remplacer par des identifiants propres pour
   un produit final)
4. Generer le code ; l'envoi de donnees se fait via une fonction du type
   `CDC_Transmit_FS()`, la reception via un callback genere dans
   `usbd_cdc_if.c`

## Projet prevu (a faire sur materiel reel)
STM32 reconnu comme port serie virtuel par le PC des le branchement, sans
driver a installer - verification par un terminal serie classique (comme
celui utilise en phase 08), mais cette fois via USB plutot que par le
ST-LINK.

## Prochaine etape
Phase 16 (basse consommation), meme format documentaire prevu.