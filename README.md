# Orchestrion-Xylophone

> **Contrôleur MIDI pour xylophone mécanique** - 3 versions disponibles

Ce projet permet de contrôler un xylophone mécanique (25 notes) via MIDI en utilisant des électroaimants. Disponible en 3 versions : **USB**, **Bluetooth** et **WiFi**.

---

## 🎹 Versions disponibles

### 📁 Version 1 : Arduino Leonardo - USB MIDI *(Câblé)*
**Dossier : [`xylo/`](./xylo/)**

- ✅ Connexion USB MIDI (câble requis)
- ✅ Latence minimale (~1ms)
- ✅ Setup le plus simple
- ✅ Stabilité maximale
- 💻 **Idéal pour** : Studio fixe, latence critique

**Bibliothèques** : MIDIUSB, Adafruit_MCP23X17, Ticker

---

### 📁 Version 2 : ESP32 - Bluetooth BLE MIDI *(Sans fil)*
**Dossier : [`xylo_esp32_bluetooth/`](./xylo_esp32_bluetooth/)**

- 🔵 Connexion **Bluetooth Low Energy**
- ✅ Sans fil (portée 10-15m)
- ✅ **BLE activé par défaut** - Plug & Play
- ⚡ Compatible iOS, macOS, Android, Windows
- 🎛️ Bouton d'appairage et LED de statut **optionnels**
- 🔋 Faible consommation
- 💻 **Idéal pour** : Mobilité courte distance, iOS/Android

**Bibliothèques** : ESP32-BLE-MIDI, Adafruit_MCP23X17, Ticker (ESP32)

**Configuration par défaut :**
```cpp
BLE_ENABLED_BY_DEFAULT = true   // BLE activé au démarrage
USE_PAIRING_BUTTON = false       // Pas de bouton nécessaire
```

---

### 📁 Version 3 : ESP32 - WiFi AppleMIDI *(Sans fil)*
**Dossier : [`xylo_esp32_wifi/`](./xylo_esp32_wifi/)**

- 🟢 Connexion **WiFi** (RTP-MIDI/AppleMIDI)
- ✅ Portée WiFi étendue (toute la maison)
- ✅ Connexions multiples simultanées
- ⚡ Compatible iOS, macOS, Windows, Linux
- ⚙️ Configuration WiFi requise (SSID/Password)
- 💻 **Idéal pour** : Studio avec réseau, longue portée, multi-clients

**Bibliothèques** : AppleMIDI, WiFi (ESP32), Adafruit_MCP23X17, Ticker (ESP32)

**⚠️ Configuration obligatoire dans `settings.h` :**
```cpp
#define WIFI_SSID "VotreSSID"
#define WIFI_PASSWORD "VotreMotDePasse"
```

---

## 🎯 Quelle version choisir ?

| Critère | USB (Leonardo) | Bluetooth (ESP32) | WiFi (ESP32) |
|---------|---------------|-------------------|--------------|
| **Sans fil** | ❌ | ✅ | ✅ |
| **Latence** | ⭐⭐⭐⭐⭐ (~1ms) | ⭐⭐⭐⭐ (~5ms) | ⭐⭐⭐ (~15ms) |
| **Portée** | Câble USB | 10-15m | Portée WiFi |
| **Setup** | Très simple | Simple | Moyen |
| **Config requise** | Aucune | Aucune | WiFi SSID/Pass |
| **Multi-clients** | ❌ | ❌ | ✅ |
| **Prix** | ~15€ | ~8€ | ~8€ |
| **Stabilité** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

**Recommandations :**
- 🎵 **Studio/Performance** → Version USB
- 📱 **Mobile/iOS/Android** → Version Bluetooth
- 🏠 **Réseau existant/Multi-postes** → Version WiFi

---

## 🔧 Matériel commun (toutes versions)

### Électronique
- **Alimentation** : 12V 1A minimum
- **Xylophone** : 25 notes (adaptable 17-32)
- **Électroaimants** : 25 (un par note)
- **MCP23017** : 2 unités (extension I/O)
- **ULN2803** : 4 unités (drivers électroaimants)
- **Fusible** : 12V 2-3A
- **Switch** : 2 positions (octave extra)

### Contrôleurs (selon version)
- **Version USB** : Arduino Leonardo
- **Version Bluetooth** : ESP32 DevKit
- **Version WiFi** : ESP32 DevKit

### Câblage
![Schéma électronique](https://github.com/glloq/Orchestrion-Xylophone/blob/main/schemas.png?raw=true)

---

## 📚 Fonctionnalités communes

- ✅ Réception MIDI (notes, vélocité, CC)
- ✅ 25 notes jouables (extensible)
- ✅ Gestion vélocité via PWM
- ✅ Switch octave extra (±1 octave)
- ✅ Extinction auto des électroaimants
- ✅ Control Change 121/123 (reset/all notes off)
- ✅ Configuration flexible (settings.h)

---

## 🚀 Installation rapide

### 1️⃣ Choisir votre version
Consultez le tableau ci-dessus pour choisir la version adaptée à vos besoins.

### 2️⃣ Suivre le README spécifique
Chaque dossier contient un README détaillé :
- [`xylo/README.md`](./xylo/) - Version USB
- [`xylo_esp32_bluetooth/README.md`](./xylo_esp32_bluetooth/) - Version Bluetooth
- [`xylo_esp32_wifi/README.md`](./xylo_esp32_wifi/) - Version WiFi

### 3️⃣ Étapes générales
```
1. Cloner le dépôt
2. Ouvrir le dossier de votre version
3. Installer les bibliothèques Arduino
4. Modifier settings.h (si nécessaire)
5. Téléverser sur votre carte
6. Connecter et jouer ! 🎵
```

---

## 📖 Documentation

### Paramètres configurables (settings.h)

```cpp
// Notes MIDI
INSTRUMENT_START_NOTE = 65    // Note de départ (Fa)
INSTRUMENT_RANGE = 25         // Nombre de notes

// Électroaimants
TIME_HIT = 20                 // Durée frappe (ms)
MIN_PWM_VALUE = 100           // PWM minimum

// MIDI
ALL_CHANNEL = true            // Écoute tous canaux
CHANNEL_XYLO = 1              // Canal spécifique si false

// Pins (varient selon version)
EXTRA_OCTAVE_SWITCH_PIN = 4   // Switch octave
```

### Pins MCP23017 (électroaimants)

25 sorties distribuées sur 2 MCP23017 (adresses I2C 0x20 et 0x21).

---

## 🆘 Support

### Problèmes courants

**Les notes ne jouent pas :**
- Vérifier les connexions I2C (MCP23017)
- Vérifier les adresses I2C (0x20, 0x21)
- Activer DEBUG_XYLO dans settings.h

**Version Bluetooth : Dispositif invisible :**
- Vérifier que BLE_ENABLED_BY_DEFAULT = true
- Redémarrer l'ESP32
- Scanner à nouveau depuis l'appareil

**Version WiFi : Connexion échoue :**
- Vérifier SSID et mot de passe dans settings.h
- S'assurer du WiFi 2.4GHz (ESP32 incompatible 5GHz)
- Vérifier le pare-feu (port UDP 5004-5005)

---

## 🔄 Historique des versions

- ✅ **v1.0** - Arduino Leonardo USB MIDI
- ✅ **v2.0** - ESP32 Bluetooth BLE (avec bouton optionnel)
- ✅ **v3.0** - ESP32 WiFi AppleMIDI

---

## 📝 Notes

> **Note** : Le code est fonctionnel. Extension future prévue : support 6 octaves (72 notes).

---

## 📄 Licence

Ce projet est sous licence **"je partage mon taf gratuitement si tu veux faire de l'argent dessus demande avant et on partage :D"**

---

## 🙏 Crédits

Projet Orchestrion-Xylophone
- Hardware : Arduino Leonardo / ESP32
- MIDI : MIDIUSB / BLE-MIDI / AppleMIDI
- I/O Expansion : Adafruit MCP23017
