> [!NOTE]
> le code fonctionnel mais je doit revoir le code pour aller jusqu'a 6 octaves (72 notes)

# Orchestrion-Xylophone

Ce projet utilise un Arduino Leonardo pour contrôler un xylophone mécanique à l'aide de signaux MIDI.
Le xylophone est équipé de solénoïdes pour jouer les notes.
Le contrôleur MIDI permet de jouer des notes avec gestion de la vélocité via PWM.
Afin d'augmenter le nombre de notes possibles, nous pouvons utiliser un bouton pour permettre l'ajout d'une octave en jouant les notes une octave au-dessus et en dessous.

## Schemas branchements
![schema electronique](https://github.com/glloq/Orchestrion-Xylophone/blob/main/schemas.png?raw=true)
Il est possible d'utiliser directement les solenoides pour frapper les plaques/notes mais l'utilisation de maillet permet un meilleur son.

## Fonctionnalités

- Lecture et exécution des notes MIDI dans la plage jouable
- Gestion de la vélocité de frappe avec PWM
- Support du switch octave extra pour étendre la plage jouable
- Gestion automatique de l'extinction des électroaimants après frappe
- Réponse aux messages SysEx pour l'identification du contrôleur
- Support des Control Change 121 (reset all controllers) et 123 (all notes off)

## Options de configuration

Le fichier `Settings.h` contient plusieurs options de configuration pour personnaliser le fonctionnement du contrôleur Arduino Xylophone MIDI. 
Vous pouvez modifier ces options avant de téléverser le code sur votre Arduino.

### Paramètres du xylophone

- `INSTRUMENT_START_NOTE` : Note MIDI de départ (par défaut 65 = Fa)
- `INSTRUMENT_RANGE` : Le nombre de notes sur le xylophone (par défaut 25)
- `EXTRA_OCTAVE_SWITCH_PIN` : Le numéro de broche pour le commutateur d'octave supplémentaire (pin 4)
- `TIME_HIT` : Temps d'activation de l'électroaimant en millisecondes (20ms)
- `MIN_PWM_VALUE` : Valeur PWM minimale pour activer l'électroaimant (100)
- `PWM_PIN` : Pin de sortie pour le PWM de puissance des électroaimants (pin 6)

### Paramètres MIDI

- `CHANNEL_XYLO` : Le canal MIDI sur lequel écouter les messages MIDI.
- `ALL_CHANNEL` : Si `true`, le contrôleur écoutera tous les canaux MIDI. Si `false`, il écoutera uniquement le canal défini par `CHANNEL_XYLO`.

Pour modifier ces paramètres, ouvrez le fichier `Settings.h` et ajustez les valeurs en conséquence. Assurez-vous de sauvegarder vos modifications avant de téléverser le code sur votre Arduino.



## Matériel requis
- Alimentation 12V 1A min
- Un câble USB pour l'Arduino
- Arduino Leonardo (ou compatible)
- Xylophone 25 notes (le code est adaptable de 17 à 32)
- 25 électroaimants : un pour chaque note
- 2 MCP23017 : pour l'extension des pins de l'Arduino
- 4 ULN2803 : pour le contrôle des électroaimants
- Un port femelle rond DC12V
- Un fusible de voiture 12V 2 à 3 ampères (à adapter à votre besoin)
- Un switch 2 positions pour indiquer l'extra octave à jouer

## Bibliothèques requises

- [MIDIUSB](https://github.com/arduino-libraries/MIDIUSB) - Communication MIDI via USB
- [Adafruit_MCP23X17](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library) - Contrôle des MCP23017
- [Ticker](https://github.com/sstaub/Ticker) - Gestion des timers non-bloquants
- avr/interrupt.h - Bibliothèque standard Arduino
- Arduino.h - Bibliothèque standard Arduino
  
## Installation

1. Clonez ou téléchargez ce dépôt.
2. Ouvrez le fichier .ino dans l'IDE Arduino.
3. Installez les bibliothèques requises via le gestionnaire de bibliothèques Arduino :
   - MIDIUSB
   - Adafruit MCP23X17
   - Ticker
4. Faites les modifications nécessaires à votre montage dans `settings.h`
5. Connectez votre Arduino Leonardo à votre ordinateur via un câble USB.
6. Sélectionnez le port série approprié et le type de carte dans le menu Outils de l'IDE Arduino.
7. Téléversez le code sur votre Arduino Leonardo.
8. Connectez votre Arduino à un hôte MIDI et profitez de votre xylophone mécanique contrôlé par MIDI !

## Licence

Ce projet est sous licence "je partage mon taf gratuirtement si tu veut faire de l'argent dessus demande avant et on partage :D"
