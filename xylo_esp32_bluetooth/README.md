# Orchestrion-Xylophone - Version ESP32 Bluetooth (BLE MIDI)

> **Version ESP32 avec réception MIDI via Bluetooth Low Energy (BLE)**

## Description

Cette version du projet Orchestrion-Xylophone utilise un ESP32 pour contrôler un xylophone mécanique via des messages MIDI reçus par Bluetooth (BLE MIDI).

Le xylophone est équipé de solénoïdes pour jouer les notes. Le contrôleur MIDI permet de jouer des notes avec gestion de la vélocité via PWM. Un bouton permet d'étendre la plage jouable d'une octave de chaque côté.

## Différences avec la version Arduino Leonardo

- ✅ **Communication sans fil** via Bluetooth BLE
- ✅ **Pas de câble USB** nécessaire pour le MIDI
- ✅ Compatible avec iOS, macOS, Android et Windows (avec adaptateur BLE MIDI)
- ⚡ **ESP32** au lieu d'Arduino Leonardo
- 📡 Utilise **BLE MIDI** au lieu de MIDIUSB
- 🔧 Configuration PWM via **LEDC** (ESP32)
- 🔌 Pins I2C configurables (SDA=21, SCL=22 par défaut)

## 🔵 Bouton d'appairage et LED de statut (OPTIONNELS)

> ⚠️ **Par défaut, le BLE est activé au démarrage** - Le bouton et la LED sont **optionnels** et peuvent être désactivés.

Cette version peut inclure un système de contrôle de l'appairage BLE via bouton et LED :

### 🔘 Bouton d'appairage (GPIO 0) - Optionnel
- **Appui court** : Active le BLE MIDI (si désactivé)
- **Appui long (3s)** : Désactive le BLE MIDI
- **Par défaut** : Fonctionnalité désactivée (BLE toujours actif)

### 💡 LED de statut (GPIO 2) - Optionnel
- **Éteinte** 🔴 : BLE désactivé
- **Clignotante** 🟡 : BLE activé, en attente de connexion
- **Allumée fixe** 🟢 : BLE connecté à un appareil

### ⚙️ Configuration
Dans `settings.h` :
```cpp
#define BLE_ENABLED_BY_DEFAULT true   // true = BLE activé au démarrage (recommandé)
#define USE_PAIRING_BUTTON false       // false = pas de bouton/LED (par défaut)
#define LONG_PRESS_TIME 3000           // Durée appui long en ms
#define LED_BLINK_INTERVAL 500         // Intervalle clignotement en ms
```

**Modes de fonctionnement :**
- `USE_PAIRING_BUTTON = false` (défaut) : BLE toujours actif, pas de bouton/LED
- `USE_PAIRING_BUTTON = true` : Contrôle par bouton et LED activé

## Fonctionnalités

- Réception MIDI via Bluetooth Low Energy (BLE MIDI)
- **BLE activé automatiquement au démarrage** - prêt à l'emploi
- **Contrôle d'appairage par bouton** avec LED de statut (optionnel)
- Lecture et exécution des notes MIDI dans la plage jouable
- Gestion de la vélocité de frappe avec PWM (LEDC)
- Support du switch octave extra pour étendre la plage jouable
- Gestion automatique de l'extinction des électroaimants après frappe
- Support des Control Change 121 (reset all controllers) et 123 (all notes off)
- Compatible avec applications iOS/macOS (GarageBand, Logic Pro, etc.)
- Compatible avec Android (via apps compatibles BLE MIDI)

## Matériel requis

### Électronique principale
- **ESP32** (DevKit ou similaire) - remplace l'Arduino Leonardo
- Alimentation 12V 1A min
- Câble USB (uniquement pour la programmation et le debug)
- Xylophone 25 notes (le code est adaptable de 17 à 32)
- 25 électroaimants : un pour chaque note
- 2 **MCP23017** : pour l'extension des pins de l'ESP32
- 4 **ULN2803** : pour le contrôle des électroaimants
- Un port femelle rond DC12V
- Un fusible de voiture 12V 2 à 3 ampères (à adapter à votre besoin)
- Un switch 2 positions pour indiquer l'extra octave à jouer

### Câblage spécifique ESP32

| Fonction | Pin ESP32 | Notes |
|----------|-----------|-------|
| I2C SDA | GPIO 21 | Configurable dans settings.h |
| I2C SCL | GPIO 22 | Configurable dans settings.h |
| PWM Électroaimants | GPIO 25 | Utilise LEDC |
| Switch Extra Octave | GPIO 4 | INPUT_PULLUP |
| **Bouton BLE** | **GPIO 0** | **Bouton BOOT intégré / externe** |
| **LED Statut BLE** | **GPIO 2** | **LED intégrée sur la plupart des ESP32** |

> 💡 **Astuce** : Sur la plupart des cartes ESP32 DevKit, le bouton BOOT (GPIO 0) et la LED intégrée (GPIO 2) sont déjà présents ! Aucun câblage supplémentaire n'est nécessaire pour utiliser cette fonctionnalité.

## Bibliothèques requises

Installez via le gestionnaire de bibliothèques Arduino :

1. **ESP32 Board Support** (via Board Manager)
   - URL additionnelle: `https://dl.espressif.com/dl/package_esp32_index.json`

2. **Adafruit_MCP23X17** - Contrôle des MCP23017
   - https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library

3. **Ticker** (inclus avec ESP32)
   - Gestion des timers non-bloquants

4. **ESP32-BLE-MIDI** - Communication MIDI via Bluetooth
   - https://github.com/lathoub/Arduino-BLE-MIDI
   - Installation: `Sketch → Include Library → Manage Libraries → "BLE-MIDI"`

## Configuration

### settings.h

Paramètres principaux à vérifier/modifier :

```cpp
// Nom du dispositif Bluetooth (visible lors de la connexion)
#define BLE_DEVICE_NAME "Xylophone-BLE"

// Pins I2C (modifiables si nécessaire)
const int I2C_SDA = 21;
const int I2C_SCL = 22;

// Pin PWM pour électroaimants
const int PWM_PIN = 25;

// Configuration MIDI
#define ALL_CHANNEL true  // Écoute tous les canaux
#define CHANNEL_XYLO 1    // Canal si ALL_CHANNEL = false
```

## Installation

### 1. Prérequis
- Arduino IDE 1.8.x ou 2.x
- Support ESP32 installé

### 2. Installation des bibliothèques
```
1. Ouvrir Arduino IDE
2. Aller dans: Sketch → Include Library → Manage Libraries
3. Installer:
   - Adafruit MCP23X17
   - ESP32-BLE-MIDI (chercher "BLE-MIDI")
```

### 3. Configuration de l'IDE
```
1. Outils → Type de carte → ESP32 Arduino → ESP32 Dev Module
2. Outils → Port → Sélectionner le port COM de votre ESP32
3. Outils → Upload Speed → 921600
```

### 4. Téléversement
```
1. Ouvrir xylo_esp32_bluetooth.ino
2. Modifier settings.h si nécessaire
3. Cliquer sur Téléverser
4. Ouvrir le Moniteur Série (115200 baud)
```

## 🎮 Utilisation

### Mode par défaut (sans bouton)
1. **Alimenter l'ESP32**
2. Le BLE MIDI s'active automatiquement
3. Le dispositif "Xylophone-BLE" est **immédiatement visible**
4. Connecter depuis votre appareil MIDI
5. **C'est prêt !** 🎵

### Mode avec bouton (optionnel - si `USE_PAIRING_BUTTON = true`)

#### Première utilisation
1. Alimenter l'ESP32
2. Si `BLE_ENABLED_BY_DEFAULT = false`, appuyer sur le bouton BOOT
3. La LED commence à **clignoter** 🟡 - BLE activé
4. Le dispositif "Xylophone-BLE" est visible

#### Connexion à un appareil
1. Scanner les dispositifs BLE MIDI disponibles
2. Connecter "Xylophone-BLE"
3. La LED passe en mode **fixe** 🟢 - Connecté !

#### Désactivation du BLE (économie d'énergie)
1. **Maintenir appuyé** le bouton BOOT pendant **3 secondes**
2. La LED s'**éteint** 🔴 - BLE désactivé
3. Économie d'énergie maximale

#### États de la LED

| État LED | Signification | Action |
|----------|--------------|--------|
| 🔴 Éteinte | BLE désactivé | Appui court pour activer |
| 🟡 Clignotante | BLE activé, en attente | Connecter un appareil BLE |
| 🟢 Fixe | BLE connecté | Prêt à jouer ! |

## Connexion MIDI

### iOS / macOS
1. Ouvrir une application compatible BLE MIDI (GarageBand, Logic Pro, etc.)
2. Dans les paramètres MIDI, chercher "Xylophone-BLE"
3. Connecter le dispositif
4. Jouer des notes !

### Android
1. Installer une app compatible BLE MIDI (MIDI BLE Connect, etc.)
2. Scanner les dispositifs BLE
3. Connecter "Xylophone-BLE"
4. Utiliser avec votre app MIDI préférée

### Windows
1. Installer un driver BLE MIDI (MIDIberry ou similar)
2. Scanner et connecter "Xylophone-BLE"
3. Le dispositif apparaîtra comme port MIDI
4. Utiliser avec votre DAW

## Dépannage

### Le dispositif BLE n'apparaît pas
- Vérifier que le code est bien téléversé
- Vérifier le moniteur série (115200 baud)
- Redémarrer l'ESP32
- S'assurer que le Bluetooth est activé sur le client

### Les notes ne jouent pas
- Vérifier les connexions I2C (MCP23017)
- Vérifier les adresses I2C (0x20, 0x21)
- Vérifier le câblage PWM (GPIO 25)
- Activer DEBUG_XYLO dans settings.h

### Connexion instable
- Réduire la distance entre ESP32 et client
- Éviter les interférences WiFi (désactiver WiFi sur ESP32)
- Vérifier l'alimentation de l'ESP32

## Portée Bluetooth

- **Portée typique**: 10-15 mètres en champ libre
- **Obstacles**: Murs et obstacles métalliques réduisent la portée
- **Conseil**: Garder une ligne de vue dégagée si possible

## Avantages / Inconvénients

### ✅ Avantages
- Sans fil - pas de câble USB nécessaire
- Compatible multi-plateformes (iOS, Android, macOS, Windows)
- Faible latence
- Consommation énergétique réduite (BLE)
- ESP32 plus puissant que Leonardo

### ⚠️ Inconvénients
- Portée limitée (10-15m)
- Nécessite un client compatible BLE MIDI
- Connexion peut être moins stable que WiFi
- Une seule connexion BLE MIDI à la fois

## Auteur

Projet Orchestrion-Xylophone - Version ESP32 Bluetooth

## Licence

Ce projet est sous licence "je partage mon taf gratuitement si tu veux faire de l'argent dessus demande avant et on partage :D"
