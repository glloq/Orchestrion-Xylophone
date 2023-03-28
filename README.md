# --- WORK IN PROGRESS / TRAVAIL EN COURS ---

## Amélioration a venir
### plans et fichiers 3D
 -  fichiers .STL pour les supports des barres du xylophone, electroaiamants, silencieux etc et aussi les carte elec MCP et ULN2803
 -  les plans de la boite avec es positions pour les differentes parties
 -  


# Orchestrion-Xylophone

Ce projet utilise un Arduino Leonardo pour contrôler un xylophone mécanique à l'aide de signaux MIDI. Le xylophone est équipé de servomoteurs et d'électroaimants pour jouer les notes. Le contrôleur MIDI permet de jouer des notes et de gérer le volume et le vibrato à l'aide d'un potentiomètre et d'un servomoteur supplémentaire.

## Fonctionnalités

- Lecture et execution des notes MIDI dans la plage jouable du xylophone
- Gestion de la velocié de frappe avec PWM
- Gestion du volume et du vibrato/modulation à l'aide d'un servomoteur 
- Fonction mute/démute pour les notes à l'aide d'un servomoteur 
- Réponse aux messages SysEx pour l'identification du contrôleur

## Options de configuration

Le fichier `Settings.h` contient plusieurs options de configuration pour personnaliser le fonctionnement du contrôleur Arduino Xylophone MIDI. Vous pouvez modifier ces options avant de téléverser le code sur votre Arduino.

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


## Schemas branchements
![schema electronique](https://github.com/glloq/Orchestrion-Xylophone/blob/main/schemas.png?raw=true)

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

- [MIDIUSB](https://github.com/arduino-libraries/MIDIUSB)

## Installation

1. Clonez ou téléchargez ce dépôt.
2. Ouvrez le fichier .ino dans l'IDE Arduino.
3. Installez la bibliothèque MIDIUSB si vous ne l'avez pas déjà fait.
4. Connectez votre Arduino Leonardo à votre ordinateur via un câble USB.
5. Sélectionnez le port série approprié et le type de carte dans le menu Outils de l'IDE Arduino.
6. Téléversez le code sur votre Arduino Leonardo.
7. Connectez votre Arduino à un hôte MIDI et profitez de votre xylophone mécanique contrôlé par MIDI !

## Licence

Ce projet est sous licence "je partage mon taf gratuirtement si tu veut faire de l'argent dessus demande avant et on partage :D"
