# Phase 17 — Stockage : CRC, Flash/EEPROM, FatFS

## CRC — STATUT : teste et fonctionnel
Le peripherique CRC materiel de cette puce est simule par Wokwi. Test
implemente : calcul du CRC-32 d'une meme chaine deux fois de suite (avec
reinitialisation entre les deux), verification que le resultat est identique
- preuve d'un calcul deterministe et reproductible, sans avoir besoin de
connaitre a l'avance la valeur CRC exacte attendue.

### Notions apprises (CRC)
- Le CRC materiel dispense de calculer un CRC en logiciel (plus lent, plus
  de code) - on ecrit les octets/mots dans DR, le resultat est disponible
  immediatement en lecture
- CRC_CR_RESET : revient a la valeur d'initialisation avant un nouveau calcul
- Cette famille supporte plusieurs tailles de polynome (32/16/8 bits,
  7 bits non supporte) - ici, taille par defaut (32 bits, CRC-32 standard)

## Flash / EEPROM emulation — STATUT : non teste
Ecriture/effacement de la memoire Flash non confirmes dans le tableau de
fonctionnalites Wokwi (non liste, contrairement au CRC). Traite en
documentation plutot qu'en pratique, pour eviter un risque de session de
diagnostic prolongee sur un point non garanti par l'outil

### Notions a retenir
- La Flash s'efface par page/secteur entiere, jamais par octet individuel -
  ecrire un seul octet demande d'effacer puis reecrire toute la page
- "EEPROM emulation" : technique logicielle qui simule une memoire
  reinscriptible octet par octet en utilisant plusieurs pages Flash en
  rotation, pour repartir l'usure (wear leveling) sur une memoire qui ne
  supporte qu'un nombre limite de cycles effacement/ecriture
- A tester sur materiel reel (Nucleo-H563ZI, phase 11) : HAL_FLASH_Unlock(),
  effacement de page, programmation, verrouillage

## FatFS / carte SD — STATUT : non teste ici, plausible plus tard
Un composant carte SD generique existe sur Wokwi (pilotable en SPI, deja
confirme fonctionnel dans des exemples Arduino/MicroPython), et le SPI
fonctionne deja sur cette carte (phase 09). Mais l'integration specifique
avec le middleware FatFS de STM32Cube n'a jamais ete testee dans cet
environnement, et represente une combinaison plus complexe (sequence
d'initialisation SD via SPI, puis couche FatFS par-dessus) avec un risque
reel de blocage difficile a diagnostiquer. Laisse en theorie ici par choix,
pas par impossibilite averee — a retenter isolement si l'envie est la.

### Configuration a prevoir dans STM32CubeMX (si tente plus tard)
1. Categorie **Middleware and Software Packs > FATFS** : activer, mode
   User-defined ou SD (selon le driver bas niveau utilise)
2. SPI deja configure (comme en phase 09) pour porter la communication
   avec la carte SD
3. Fournir les fonctions de bas niveau (lecture/ecriture de bloc) attendues
   par la couche FatFS, generalement un fichier utilisateur a completer

## Prochaine etape
Phase 18 (FreeRTOS) — retour a un rythme pratique complet, cœur + SysTick
deja simules, bases deja connues via l'experience ESP32.