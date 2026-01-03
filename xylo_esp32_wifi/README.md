# Orchestrion-Xylophone - Version ESP32 WiFi (AppleMIDI/RTP-MIDI)

> **Version ESP32 avec réception MIDI via WiFi (AppleMIDI/RTP-MIDI)**

## Description

Cette version du projet Orchestrion-Xylophone utilise un ESP32 pour contrôler un xylophone mécanique via des messages MIDI reçus par WiFi en utilisant le protocole AppleMIDI (également appelé RTP-MIDI).

Le xylophone est équipé de solénoïdes pour jouer les notes. Le contrôleur MIDI permet de jouer des notes avec gestion de la vélocité via PWM. Un bouton permet d'étendre la plage jouable d'une octave de chaque côté.

## Différences avec la version Arduino Leonardo

- ✅ **Communication sans fil** via WiFi
- ✅ **Pas de câble USB** nécessaire pour le MIDI
- ✅ Compatible avec iOS, macOS, Windows et Linux
- ✅ **Portée supérieure** au Bluetooth (toute la maison)
- ⚡ **ESP32** au lieu d'Arduino Leonardo
- 📡 Utilise **AppleMIDI** (RTP-MIDI) au lieu de MIDIUSB
- 🔧 Configuration PWM via **LEDC** (ESP32)
- 🔌 Pins I2C configurables (SDA=21, SCL=22 par défaut)

## Fonctionnalités

- Réception MIDI via WiFi (AppleMIDI/RTP-MIDI)
- Lecture et exécution des notes MIDI dans la plage jouable
- Gestion de la vélocité de frappe avec PWM (LEDC)
- Support du switch octave extra pour étendre la plage jouable
- Gestion automatique de l'extinction des électroaimants après frappe
- Support des Control Change 121 (reset all controllers) et 123 (all notes off)
- Compatible avec applications macOS/iOS (GarageBand, Logic Pro, etc.)
- Compatible avec Windows (rtpMIDI)
- Compatible avec Linux (diverses solutions)
- **Connexions multiples possibles** (plusieurs clients simultanés)
- **Portée WiFi étendue** (toute la maison)

## Matériel requis

### Électronique principale
- **ESP32** (DevKit ou similaire) - remplace l'Arduino Leonardo
- Alimentation 12V 1A min
- Câble USB (uniquement pour la programmation et le debug)
- **Routeur WiFi** (réseau local requis)
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

## Bibliothèques requises

Installez via le gestionnaire de bibliothèques Arduino :

1. **ESP32 Board Support** (via Board Manager)
   - URL additionnelle: `https://dl.espressif.com/dl/package_esp32_index.json`

2. **Adafruit_MCP23X17** - Contrôle des MCP23017
   - https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library

3. **Ticker** (inclus avec ESP32)
   - Gestion des timers non-bloquants

4. **AppleMIDI** - Communication MIDI via WiFi (RTP-MIDI)
   - https://github.com/lathoub/Arduino-AppleMIDI-Library
   - Installation: `Sketch → Include Library → Manage Libraries → "AppleMIDI"`

5. **WiFi** (inclus avec ESP32)
   - Gestion de la connexion réseau

## Configuration

### ⚠️ IMPORTANT - Configurer le WiFi

**Avant de téléverser le code**, modifiez `settings.h` :

```cpp
// ⚠️ À MODIFIER OBLIGATOIREMENT
#define WIFI_SSID "VotreSSID"           // Nom de votre réseau WiFi
#define WIFI_PASSWORD "VotreMotDePasse" // Mot de passe WiFi

// Nom de la session AppleMIDI (optionnel)
#define APPLEMIDI_SESSION_NAME "Xylophone-WiFi"
```

### Autres paramètres (settings.h)

```cpp
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
- Accès à un réseau WiFi

### 2. Installation des bibliothèques
```
1. Ouvrir Arduino IDE
2. Aller dans: Sketch → Include Library → Manage Libraries
3. Installer:
   - Adafruit MCP23X17
   - AppleMIDI (chercher "AppleMIDI")
```

### 3. Configuration WiFi
```
1. Ouvrir settings.h
2. Modifier WIFI_SSID avec le nom de votre réseau
3. Modifier WIFI_PASSWORD avec votre mot de passe
4. Sauvegarder
```

### 4. Configuration de l'IDE
```
1. Outils → Type de carte → ESP32 Arduino → ESP32 Dev Module
2. Outils → Port → Sélectionner le port COM de votre ESP32
3. Outils → Upload Speed → 921600
```

### 5. Téléversement
```
1. Ouvrir xylo_esp32_wifi.ino
2. Cliquer sur Téléverser
3. Ouvrir le Moniteur Série (115200 baud)
4. Noter l'adresse IP affichée (exemple: 192.168.1.100)
```

## Connexion MIDI

### macOS / iOS
1. Le dispositif "Xylophone-WiFi" devrait apparaître automatiquement
2. Dans Audio MIDI Setup (macOS):
   - Ouvrir "Audio MIDI Setup"
   - Fenêtre → Afficher la fenêtre MIDI
   - Le xylophone devrait apparaître
3. Dans GarageBand/Logic Pro:
   - Le dispositif sera disponible comme source MIDI

### Windows
1. Installer **rtpMIDI** (gratuit)
   - https://www.tobias-erichsen.de/software/rtpmidi.html
2. Lancer rtpMIDI
3. Dans la section "Directory":
   - "Xylophone-WiFi" devrait apparaître
   - Cliquer sur "Connect"
4. Le dispositif apparaîtra comme port MIDI dans votre DAW

### Linux
1. Installer **QmidiNet** ou similaire
2. Configurer la connexion RTP-MIDI
3. Se connecter à l'adresse IP de l'ESP32

## Dépannage

### L'ESP32 ne se connecte pas au WiFi
- Vérifier le SSID et mot de passe dans settings.h
- Vérifier que le WiFi est en 2.4GHz (ESP32 ne supporte pas 5GHz)
- Vérifier la force du signal WiFi
- Redémarrer le routeur

### Le dispositif MIDI n'apparaît pas
- Vérifier que l'ESP32 est connecté au WiFi (moniteur série)
- Vérifier l'adresse IP affichée
- S'assurer que le client MIDI est sur le même réseau
- Vérifier le pare-feu (autoriser UDP port 5004-5005)

### Les notes ne jouent pas
- Vérifier les connexions I2C (MCP23017)
- Vérifier les adresses I2C (0x20, 0x21)
- Vérifier le câblage PWM (GPIO 25)
- Activer DEBUG_XYLO dans settings.h

### Latence élevée
- Vérifier la qualité du signal WiFi
- Réduire la distance au routeur
- Éviter les interférences (micro-ondes, etc.)
- Utiliser WiFi 5GHz si possible (nécessite ESP32 compatible)

### Connexion instable
- Vérifier la stabilité du réseau WiFi
- Utiliser une IP statique (configurable dans le code)
- Éviter les réseaux surchargés

## Portée et Performance

### Portée
- **Portée WiFi**: Dépend de votre routeur (20-50m typique)
- **À travers les murs**: Oui (selon matériaux)
- **Toute la maison**: Généralement oui
- **Conseil**: Utiliser un répéteur WiFi si nécessaire

### Latence
- **Latence typique**: 5-15ms (dépend du réseau)
- **Meilleure que BLE**: Non (BLE ~3-5ms)
- **Acceptable pour jeu en direct**: Oui

## Avantages / Inconvénients

### ✅ Avantages
- Portée WiFi étendue (toute la maison)
- Connexions multiples simultanées possibles
- Pas de limite de distance tant que WiFi accessible
- Compatible multi-plateformes
- Plus stable que Bluetooth pour longue durée
- ESP32 plus puissant que Leonardo

### ⚠️ Inconvénients
- Nécessite un réseau WiFi
- Configuration WiFi obligatoire
- Latence légèrement supérieure au Bluetooth
- Consommation énergétique supérieure au BLE
- Dépend de la qualité du réseau WiFi

## Sécurité

- Le protocole AppleMIDI n'est **pas chiffré**
- Utiliser un réseau WiFi privé et sécurisé
- Ne pas exposer le port RTP-MIDI à Internet

## Auteur

Projet Orchestrion-Xylophone - Version ESP32 WiFi

## Licence

Ce projet est sous licence "je partage mon taf gratuitement si tu veux faire de l'argent dessus demande avant et on partage :D"
