# Orchestrion Xylophone - ESP32 Version

Version ESP32 du contrôleur MIDI pour xylophone mécanique avec support Bluetooth Low Energy (BLE) et WiFi MIDI.

## 📋 Table des Matières

- [Caractéristiques](#caractéristiques)
- [Matériel Requis](#matériel-requis)
- [Bibliothèques Arduino](#bibliothèques-arduino)
- [Configuration](#configuration)
- [Installation](#installation)
- [Connexion MIDI](#connexion-midi)
- [Broches GPIO ESP32](#broches-gpio-esp32)
- [Différences vs Version Arduino Leonardo](#différences-vs-version-arduino-leonardo)
- [Dépannage](#dépannage)

---

## ✨ Caractéristiques

### Nouvelles Fonctionnalités ESP32

- ✅ **MIDI via Bluetooth Low Energy (BLE)** - Connexion sans fil depuis n'importe quel appareil compatible
- ✅ **MIDI via WiFi (Apple MIDI/RTP-MIDI)** - Support réseau pour DAWs et contrôleurs
- ✅ **Performance améliorée** - Processeur 240 MHz dual-core
- ✅ **Plus de mémoire** - 520 KB SRAM vs 2.5 KB sur Leonardo
- ✅ **Watchdog ESP32** - Redémarrage automatique en cas de blocage
- ✅ **Monitoring de connexion** - LED d'état de connexion MIDI
- ✅ **WiFi OTA** - Possibilité de mise à jour sans fil (à implémenter)

### Fonctionnalités Conservées

- ✅ Protection contre les race conditions
- ✅ Validation des limites de tableaux
- ✅ Gestion des erreurs robuste
- ✅ Debouncing du switch octave
- ✅ Limitation du courant des électroaimants
- ✅ Timeout de sécurité des électroaimants
- ✅ Documentation Doxygen complète

---

## 🛠️ Matériel Requis

### Microcontrôleur
- **ESP32-DevKitC** ou équivalent (ESP32-WROOM-32)
- Processeur: Dual-core 240 MHz
- Flash: 4 MB minimum
- RAM: 520 KB

### Composants Électroniques

| Composant | Quantité | Notes |
|-----------|----------|-------|
| ESP32 DevKit | 1 | Avec Bluetooth et WiFi |
| MCP23017 | 2 | Expandeurs I2C 16-bits |
| Électroaimants/Solénoïdes | 25 | 12V, adaptés pour xylophone |
| ULN2803 | 4 | Drivers Darlington pour solénoïdes |
| Servo moteur | 0 | Retirés dans cette version |
| Alimentation 12V | 1 | 2A minimum recommandé |
| Alimentation 5V | 1 | Pour ESP32 (via USB ou régulateur) |
| Condensateurs | Divers | Filtrage alimentation |
| Résistances pull-up | 2 | 4.7kΩ pour I2C (optionnel) |
| Switch | 1 | Mode octave supplémentaire |

### Câblage

- Câbles Dupont femelle-femelle
- Breadboard ou PCB personnalisé
- Connecteurs appropriés

---

## 📚 Bibliothèques Arduino

### Installation via Arduino IDE

Allez dans **Outils → Gérer les bibliothèques** et installez:

#### Obligatoires

1. **Adafruit MCP23017** (par Adafruit)
   ```
   Version: 2.3.2 ou supérieure
   ```

2. **Ticker-esp32** (ou Ticker inclus avec ESP32)
   ```
   Version: Compatible avec votre version ESP32
   ```

#### Pour BLE MIDI (par défaut)

3. **ESP32-BLE-MIDI** (par lathoub)
   ```bash
   Version: 0.3.2 ou supérieure
   https://github.com/lathoub/Arduino-BLE-MIDI
   ```

#### Pour WiFi MIDI (optionnel)

4. **AppleMIDI** (par lathoub)
   ```bash
   Version: 3.2.0 ou supérieure
   https://github.com/lathoub/Arduino-AppleMIDI-Library
   ```

### Installation de la Plateforme ESP32

Dans **Fichier → Préférences**, ajoutez cette URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Puis dans **Outils → Type de carte → Gestionnaire de cartes**:
- Recherchez "esp32"
- Installez **"esp32 by Espressif Systems"**
- Version recommandée: 2.0.11 ou supérieure

---

## ⚙️ Configuration

### 1. Éditer `settings.h`

#### Choisir le Transport MIDI

```cpp
// Dans settings.h, ligne ~22
#define MIDI_USE_BLE true      // Bluetooth (défaut)
#define MIDI_USE_WIFI false    // WiFi MIDI
#define MIDI_USE_SERIAL false  // MIDI série (non implémenté)
```

#### Configuration BLE

```cpp
#define BLE_DEVICE_NAME "Xylophone-ESP32"
#define BLE_MANUFACTURER "Orchestrion"
```

#### Configuration WiFi MIDI

```cpp
#define WIFI_SSID "VotreSSID"
#define WIFI_PASSWORD "VotreMotDePasse"
#define WIFI_MIDI_SESSION_NAME "Xylophone-ESP32"
#define WIFI_MIDI_PORT 5004
```

### 2. Configuration des Pins

#### Pins I2C (modifiables)

```cpp
#define I2C_SDA_PIN 21  // GPIO 21 (défaut ESP32)
#define I2C_SCL_PIN 22  // GPIO 22 (défaut ESP32)
```

#### Pins de Contrôle

```cpp
const byte EXTRA_OCTAVE_SWITCH_PIN = 4;   // Switch mode octave
const int PWM_PIN = 25;                    // PWM pour électroaimants
```

### 3. Configuration PWM ESP32

```cpp
#define PWM_CHANNEL 0      // Canal PWM (0-15)
#define PWM_FREQUENCY 5000 // Fréquence en Hz
#define PWM_RESOLUTION 8   // Résolution (8-bit = 0-255)
```

### 4. Paramètres du Xylophone

```cpp
const byte INSTRUMENT_START_NOTE = 65;  // Note MIDI de départ
const byte INSTRUMENT_RANGE = 25;       // Nombre de touches
#define MAX_SIMULTANEOUS_MAGNETS 8      // Limite électroaimants
```

---

## 🚀 Installation

### 1. Préparer l'Arduino IDE

1. Installer Arduino IDE 2.x ou supérieur
2. Installer ESP32 board support (voir ci-dessus)
3. Installer toutes les bibliothèques requises

### 2. Configuration de la Carte

Dans Arduino IDE:
- **Outils → Type de carte**: "ESP32 Dev Module"
- **Outils → Upload Speed**: 921600
- **Outils → CPU Frequency**: 240 MHz
- **Outils → Flash Frequency**: 80 MHz
- **Outils → Flash Size**: "4MB (32Mb)"
- **Outils → Partition Scheme**: "Default 4MB with spiffs"
- **Outils → PSRAM**: "Disabled"
- **Outils → Port**: Sélectionner le port USB de l'ESP32

### 3. Téléverser le Code

1. Ouvrir `xylo_esp32.ino`
2. Vérifier/compiler (Ctrl+R ou ✓)
3. Téléverser (Ctrl+U ou →)

### 4. Moniteur Série

- Ouvrir le moniteur série (Ctrl+Shift+M)
- Régler à **115200 baud**
- Vous devriez voir:
  ```
  =========================================
    Orchestrion: Xylophone MIDI Controller
             ESP32 Version
  =========================================

  Free heap: 295096 bytes
  I2C initialized: SDA=21, SCL=22

  Initializing system...
  Start Xylophone init
  End Xylophone init
  Xylophone initialized successfully

  Initializing BLE MIDI...
  BLE MIDI Device Name: Xylophone-ESP32
  Waiting for BLE MIDI connection...

  ✓ System initialized successfully!
  ```

---

## 📱 Connexion MIDI

### Via Bluetooth (BLE MIDI)

#### iOS (iPhone/iPad)

1. Aller dans **Réglages → Bluetooth**
2. Chercher "Xylophone-ESP32"
3. Appuyer pour jumeler
4. Ouvrir votre app MIDI (GarageBand, AUM, etc.)
5. Aller dans les paramètres MIDI
6. Activer "Xylophone-ESP32" comme source MIDI

#### macOS

1. Ouvrir **Configuration MIDI Audio** (dans Applications → Utilitaires)
2. Fenêtre → Afficher la fenêtre Bluetooth MIDI
3. Cliquer sur "Xylophone-ESP32" pour connecter
4. Le périphérique apparaîtra dans votre DAW

#### Windows

1. Installer [BLE MIDI Driver for Windows](https://www.tobias-erichsen.de/software/loopmidi.html)
2. Ouvrir le gestionnaire Bluetooth
3. Jumeler avec "Xylophone-ESP32"
4. Le périphérique MIDI apparaîtra dans le gestionnaire de périphériques

#### Android

1. Utiliser une app compatible BLE MIDI (MIDI BLE Connect, USB MIDI Driver)
2. Scanner les périphériques Bluetooth
3. Connecter à "Xylophone-ESP32"

### Via WiFi (Apple MIDI/RTP-MIDI)

#### Configuration Préalable

1. Modifier `settings.h`:
   ```cpp
   #define MIDI_USE_BLE false
   #define MIDI_USE_WIFI true
   #define WIFI_SSID "VotreReseau"
   #define WIFI_PASSWORD "VotreMotDePasse"
   ```
2. Téléverser le code
3. Noter l'adresse IP affichée dans le moniteur série

#### macOS

1. Ouvrir **Configuration MIDI Audio**
2. Fenêtre → Afficher le répertoire réseau MIDI
3. "Xylophone-ESP32" devrait apparaître
4. Cliquer "Connecter"

#### Windows

1. Installer [rtpMIDI by Tobias Erichsen](https://www.tobias-erichsen.de/software/rtpmidi.html)
2. Lancer rtpMIDI
3. Ajouter "Xylophone-ESP32" depuis la liste
4. Connecter

#### iOS

Apps compatibles:
- **midimittr** (gratuit)
- **MIDI Network Bridge** (payant)

---

## 🔌 Broches GPIO ESP32

### Connexions I2C

| Signal | ESP32 Pin | MCP23017 |
|--------|-----------|----------|
| SDA | GPIO 21 | Pin 13 (SDA) |
| SCL | GPIO 22 | Pin 12 (SCL) |
| GND | GND | Pin 10 (VSS) |
| 3.3V | 3.3V | Pin 18 (VDD) |

### MCP23017 #1 (Adresse 0x20)

- **A0, A1, A2**: Tous à GND
- **RESET**: 3.3V via résistance 10kΩ
- **Pins GPA0-GPA7, GPB0-GPB4**: Électroaimants 1-13

### MCP23017 #2 (Adresse 0x21)

- **A0**: 3.3V, **A1, A2**: GND
- **RESET**: 3.3V via résistance 10kΩ
- **Pins GPA0-GPB3**: Électroaimants 14-25

### Autres Connexions

| Signal | ESP32 Pin | Destination |
|--------|-----------|-------------|
| PWM Électroaimants | GPIO 25 | ULN2803 inputs |
| Switch Octave | GPIO 4 | Commutateur (avec pull-up) |

### Broches à Éviter

Sur ESP32, évitez ces pins pour un fonctionnement stable:
- **GPIO 0**: Mode boot, éviter
- **GPIO 2**: LED interne sur certains modules
- **GPIO 6-11**: Connectés à la flash SPI
- **GPIO 12**: Strapping pin (boot mode)
- **GPIO 15**: Strapping pin (JTAG)

---

## 🆚 Différences vs Version Arduino Leonardo

### Matériel

| Caractéristique | Arduino Leonardo | ESP32 |
|----------------|------------------|-------|
| Processeur | ATmega32U4 @ 16 MHz | Xtensa LX6 dual-core @ 240 MHz |
| Flash | 32 KB | 4 MB |
| SRAM | 2.5 KB | 520 KB |
| MIDI | USB natif | BLE / WiFi |
| Pins PWM | 7 | 16 canaux |
| Prix | ~25€ | ~5-10€ |

### Logiciel

#### Changements Principaux

1. **MIDI Transport**
   - Leonardo: `MIDIUSB.h` (USB)
   - ESP32: `BLEMidi.h` ou `AppleMIDI.h`

2. **PWM**
   - Leonardo: `analogWrite(pin, value)`
   - ESP32: `ledcWrite(channel, value)` avec configuration préalable

3. **Watchdog**
   - Leonardo: `<avr/wdt.h>`, `wdt_enable()`, `wdt_reset()`
   - ESP32: `<esp_task_wdt.h>`, `esp_task_wdt_init()`, `esp_task_wdt_reset()`

4. **Interruptions**
   - Les deux supportent `noInterrupts()` / `interrupts()`
   - ESP32 a aussi `portENTER_CRITICAL()` / `portEXIT_CRITICAL()`

5. **I2C**
   - ESP32 nécessite: `Wire.begin(SDA_PIN, SCL_PIN)`
   - Leonardo: `Wire.begin()` (pins fixes)

### Avantages ESP32

✅ Sans fil (BLE/WiFi)
✅ Plus rapide et plus de mémoire
✅ Moins cher
✅ Dual-core (tâches parallèles possibles)
✅ Support FreeRTOS

### Avantages Leonardo

✅ USB MIDI natif (reconnaissance instantanée)
✅ Pas de configuration WiFi nécessaire
✅ Consommation moindre
✅ Plus simple à déboguer

---

## 🔧 Dépannage

### Problèmes de Compilation

#### Erreur: "BLEMidi.h: No such file"

**Solution**: Installer la bibliothèque ESP32-BLE-MIDI
```
Outils → Gérer les bibliothèques → Rechercher "ESP32 BLE MIDI"
```

#### Erreur: "ledcSetup was not declared"

**Solution**: Vérifier que la carte sélectionnée est bien "ESP32 Dev Module"

### Problèmes de Connexion

#### BLE ne se connecte pas

1. Vérifier que Bluetooth est activé sur votre appareil
2. Vérifier qu'aucun autre appareil n'est déjà connecté au xylophone
3. Réinitialiser l'ESP32 (bouton EN)
4. Vérifier les logs dans le moniteur série
5. Essayer de désappairer et réapparier

#### WiFi ne se connecte pas

1. Vérifier SSID et mot de passe dans `settings.h`
2. Vérifier que le réseau est en 2.4 GHz (ESP32 ne supporte pas 5 GHz)
3. Vérifier la force du signal WiFi
4. Réduire la distance au routeur

### Problèmes Matériels

#### MCP23017 non détecté

```
ERROR: MCP1 not responding
```

**Solutions**:
1. Vérifier les connexions I2C (SDA, SCL, GND, VCC)
2. Vérifier les adresses I2C avec un scanner:
   ```cpp
   Wire.beginTransmission(0x20);
   byte error = Wire.endTransmission();
   ```
3. Ajouter des résistances pull-up 4.7kΩ sur SDA et SCL
4. Vérifier l'alimentation 3.3V du MCP23017

#### Électroaimants ne s'activent pas

1. Vérifier le signal PWM avec un oscilloscope/multimètre
2. Vérifier l'alimentation 12V des électroaimants
3. Vérifier les ULN2803
4. Tester manuellement:
   ```cpp
   ledcWrite(PWM_CHANNEL, 255);  // PWM maximum
   _mcpLowNotes.digitalWrite(0, HIGH);  // Activer pin 0
   ```

### Problèmes de Performance

#### Watchdog timeout / Redémarrages

**Solutions**:
1. Augmenter le timeout du watchdog dans `settings.h`:
   ```cpp
   #define WATCHDOG_TIMEOUT_SECONDS 5
   ```
2. Désactiver temporairement le debug pour réduire la charge:
   ```cpp
   #define DEBUG_XYLO false
   #define DEBUG_HANDLER false
   ```

#### Latence MIDI

- **BLE**: Latence typique 20-50ms (acceptable pour instruments acoustiques)
- **WiFi**: Latence typique 5-20ms
- Réduire la charge CPU en désactivant les Serial.print() en production

---

## 📊 Moniteur Série

### Messages Normaux

```
✓ System initialized successfully!
MIDI Transport: Bluetooth Low Energy
Device name: Xylophone-ESP32

✓ Ready to receive MIDI!

✓ MIDI device connected!
MIDI noteOn: ch=0 note=60 vel=100
playNote: note=60, mcpPin=0, activeCount=1
stopNote: note=60, mcpPin=0, activeCount=0
```

### Messages d'Erreur

```
ERROR: MCP1 not responding
CRITICAL ERROR: Init Failed
```

---

## 🎵 Test de Fonctionnement

### 1. Test Automatique au Démarrage

Par défaut, `xylo_esp32.ino` joue toutes les notes séquentiellement:

```cpp
midiHandler.test(false);  // Joue notes 65-89
```

Vous devriez entendre chaque note du xylophone.

### 2. Test Manuel via BLE

Depuis une app MIDI:
1. Connecter le xylophone
2. Envoyer Note On MIDI 60-84 (Do-Do)
3. Observer les électroaimants s'activer
4. Vérifier dans le moniteur série

### 3. Test du Switch Octave

1. Activer le switch (GPIO 4 → GND)
2. Envoyer Note MIDI 53 (devrait jouer note 65)
3. Le moniteur série devrait afficher:
   ```
   Extra octave mode: ENABLED
   ```

---

## 📈 Prochaines Améliorations

- [ ] Support Serial MIDI (UART)
- [ ] Interface web de configuration (WebSocket)
- [ ] OTA (Over-The-Air) updates
- [ ] Enregistrement des performances sur SD
- [ ] Mode stand-alone avec séquenceur intégré
- [ ] Support MIDI 2.0
- [ ] Multi-transport simultané (BLE + WiFi)

---

## 📝 Licence

Voir LICENSE dans le répertoire parent.

## 👥 Contribution

Les contributions sont les bienvenues ! Voir CONTRIBUTING.md

---

## 📞 Support

Pour questions et support:
- Ouvrir un issue sur GitHub
- Consulter la documentation ESP32 officielle
- Forum Arduino ESP32

---

**Version**: 2.0.0-esp32
**Date**: Novembre 2025
**Auteur**: Orchestrion Project
