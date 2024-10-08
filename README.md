> [!NOTE]
> le code fonctionnel mais je doit revoir le code pour aller jusqu'a 6 octaves (72 notes)

# Orchestrion-Xylophone

Ce projet utilise un Arduino Leonardo pour contrôler un xylophone mécanique à l'aide de signaux MIDI.  
Le xylophone est équipé de solenoides pour jouer les notes et de deux servomoteurs pour la gestion du vibrato et du silencieux.
Le contrôleur MIDI permet de jouer des notes et de gérer le volume et le vibrato à l'aide d'un potentiomètre et d'un servomoteur supplémentaire.
Afin d'augmenter le nombre de note possible, nous pouvons utiliser un bouton pour permettre l'ajout d'une octave en jouant les note une octave au dessus et en dessous.

## Schemas branchements
![schema electronique](https://github.com/glloq/Orchestrion-Xylophone/blob/main/schemas.png?raw=true)
Il est possible d'utiliser directement les solenoides pour frapper les plaques/notes mais l'utilisation de maillet permet un meilleur son.

## Fonctionnalités

- Lecture et execution des notes MIDI dans la plage jouable
- Gestion de la velocié de frappe avec PWM
- Gestion du volume et du vibrato/modulation à l'aide d'un servomoteur 
- Fonction mute/unmute pour les notes à l'aide d'un servomoteur 
- Réponse aux messages SysEx pour l'identification du contrôleur

## Options de configuration

Le fichier `Settings.h` contient plusieurs options de configuration pour personnaliser le fonctionnement du contrôleur Arduino Xylophone MIDI. 
Vous pouvez modifier ces options avant de téléverser le code sur votre Arduino.

### Paramètres du xylophone

- `INSTRUMENT_START_NOTE` et `INSTRUMENT_RANGE` : Définissent la plage de notes jouables sur le xylophone.
- `INSTRUMENT_RANGE` : Le nombre de notes sur le xylophone.
- `EXTRA_OCTAVE_SWITCH_PIN` : Le numéro de broche pour le commutateur d'octave supplémentaire.

### Paramètres des servomoteurs

- `SERVO_ANGLE_NOTE_ON` et `SERVO_ANGLE_NOTE_OFF` : Angles des servomoteurs pour jouer et arrêter les notes.
- `SERVO_ANGLE_MUTE_MAX` : Angle maximum pour le servomoteur de mute.
- `SERVO_VOLUME_MIN_ANGLE_VIBRATO` et `SERVO_VOLUME_MAX_ANGLE_VIBRATO` : Limites d'angle pour le servomoteur de volume lors de l'utilisation du vibrato.
- `SERVO_VOLUME_MIN_FREQUENCY_VIBRATO` et `SERVO_VOLUME_MAX_FREQUENCY_VIBRATO` : Plage de fréquences de vibrato pour le servomoteur de volume.

### Paramètres MIDI

- `CHANNEL_XYLO` : Le canal MIDI sur lequel écouter les messages MIDI.
- `ALL_CHANNEL` : Si `true`, le contrôleur écoutera tous les canaux MIDI. Si `false`, il écoutera uniquement le canal défini par `CHANNEL_XYLO`.

Pour modifier ces paramètres, ouvrez le fichier `Settings.h` et ajustez les valeurs en conséquence. Assurez-vous de sauvegarder vos modifications avant de téléverser le code sur votre Arduino.



## Matériel requis
- Alimentation 12V 1A min 
- un cable USB pour l'arduino
- Arduino Leonardo(ou compatible) 
- Xylophone 25 notes (le code est adaptable de 17 a 32) 
- 2 servomoteurs pour ServoMute et ServoVolume
- 25 électroaimants; un pour chaque notes
- 2 MCP23017 : pour l'extension des pins de l'arduino
- 4 ULN2803 : pour le controle des electroaimants, la puce est faite pour ca.
- un port femelle rond DC12V
- un fusible de voiture 12v 2 à 3 ampères ( a adapter a votre besoin! )
- un switch 2 position pour indiquer l'extra octave à jouer

## Bibliothèques requises

- [MidiUSB.H](https://github.com/arduino-libraries/MIDIUSB)
- [Adafruit_MCP23X17.h](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library)
- avr/interrupt.h
- Arduino.h
  
## Installation

1. Clonez ou téléchargez ce dépôt.
2. Ouvrez le fichier .ino dans l'IDE Arduino.
3. Installez la bibliothèque MIDIUSB et adafruit MCP23x17 si vous ne l'avez pas déjà fait.
4. faites les modifications necessaire à votre montage dans settings.h
5. Connectez votre Arduino Leonardo à votre ordinateur via un câble USB.
6. Sélectionnez le port série approprié et le type de carte dans le menu Outils de l'IDE Arduino.
7. Téléversez le code sur votre Arduino Leonardo.
8. Connectez votre Arduino à un hôte MIDI et profitez de votre xylophone mécanique contrôlé par MIDI !

## Licence

Ce projet est sous licence "je partage mon taf gratuirtement si tu veut faire de l'argent dessus demande avant et on partage :D"
