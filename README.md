# Phase 14 — Ethernet

## Statut
Non simulable, et impossible meme en theorie sur la puce utilisee jusqu'ici
(STM32C031C6) : cette famille d'entree de gamme n'integre pas de controleur
Ethernet. Cette phase necessite une puce disposant du peripherique MAC
Ethernet, comme le STM32H563ZI recommande en phase 11.

## Objectif de la phase
Comprendre la pile reseau depuis la couche materielle jusqu'a la couche
applicative, et mettre en oeuvre une communication IP (TCP ou UDP) depuis
un STM32.

## Notions fondamentales

### Couche materielle
- **MAC (Media Access Control)** : bloc integre au microcontroleur, gere
  la mise en trame Ethernet et l'acces au support
- **PHY (Physical layer)** : circuit externe (ou integre selon les cartes)
  qui convertit les signaux numeriques du MAC en signaux electriques sur le
  cable, et inversement
- **RMII / MII** : interfaces standardisees entre le MAC (dans le
  microcontroleur) et le PHY (souvent un circuit externe) - RMII utilise
  moins de broches que MII, au prix d'une frequence d'horloge plus elevee
  sur ces broches
- **Trame Ethernet** : adresse MAC destination, adresse MAC source,
  type/longueur, donnees utiles, controle d'erreur (CRC)
- **Adresse MAC** : identifiant unique 48 bits de l'interface reseau,
  distinct de l'adresse IP (couche differente)

### Couches reseau et transport
- **ARP** : traduit une adresse IP en adresse MAC sur le reseau local,
  etape necessaire avant tout envoi
- **IPv4** : adressage et routage entre reseaux
- **TCP** : transport fiable, avec connexion, accuse de reception,
  retransmission en cas de perte
- **UDP** : transport simple, sans connexion ni garantie de livraison,
  plus leger et plus rapide que TCP
- **DHCP** : attribution automatique d'une adresse IP par un serveur du
  reseau, evite une configuration manuelle
- **DNS** : traduit un nom de domaine en adresse IP

### Pile logicielle
- **LwIP (Lightweight IP)** : pile TCP/IP legere, tres utilisee dans
  l'ecosysteme STM32, integrable comme middleware via STM32CubeMX
- **Sockets** : interface de programmation standard pour ouvrir des
  connexions TCP/UDP, une fois LwIP configure

## Configuration a prevoir dans STM32CubeMX (a faire une fois la carte en main)

1. Categorie **Connectivity > ETH** : Mode RMII (le plus courant sur les
   cartes Nucleo equipees), activer
2. Verifier l'assignation des broches RMII proposees par CubeMX (elles sont
   generalement figees par le brochage du PHY sur la carte)
3. Categorie **Middleware and Software Packs > LWIP** : activer, choisir
   DHCP ou adresse IP statique selon le besoin
4. Generer le code ; LwIP fonctionne generalement avec un appel periodique
   (`MX_LWIP_Process()`) a integrer dans la boucle principale ou une tache
   dediee si un RTOS est utilise

## Projets prevus (a faire sur materiel reel)
1. STM32 vers PC : serveur TCP simple sur le STM32, verification depuis un
   PC sur le meme reseau
2. Extension applicative : connexion a un protocole de plus haut niveau
   (MQTT par exemple) une fois la base TCP/UDP validee, en lien avec
   l'experience acquise sur ESP32

## Prochaine etape
Phase 15 (USB), meme format documentaire prevu.