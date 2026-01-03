/***********************************************************************************************************
---------------------------------------------------------------------------------------------------------
------------------------    Ochestrion Project  : Xolophone/Glokenspiel      ---------------------------- 
----------------------------------         XYLO.INO         ---------------------------------------------
_________________________________________________________________________________________________________

les differents parametres et reglages du systeme sont dans settings.h


***********************************************************************************************************/

#include "MidiHandler.h"
#include "Xylophone.h"

// les instances pour les objets Xylophone et MidiHandler
Xylophone xylophone;
MidiHandler midiHandler(xylophone);

void setup() {

  Serial.begin(9600);
  
 // while (!Serial) {
 //   delay(10); // Attendre que la connexion série soit établie
 // }
  midiHandler.begin();//definition de tout les pins etc
  Serial.println("Orchestrion : Xylophone MIDI Controller");  
 
  // midiHandler.test(true); // Joue la mélodie spécifiée dans INIT_MELODY
   midiHandler.test(false);  // Joue toutes les notes l'une après l'autre avec 200 ms entre chaque note

}

bool _serialConnected = true;

void loop() {
  // Traitez les messages MIDI entrants
  midiHandler.handleMidiEvent();
  midiHandler.update(); // Mise à jour pour la gestion des électroaimants

// securité pour desactiver les electroaiamnts si coupure serial 
  if (Serial && _serialConnected) {
    // La connexion série est toujours active
  } else if (!Serial && !_serialConnected) {
    // La connexion série est interrompue
    xylophone.reset();
    _serialConnected = false;
  } else if (Serial && !_serialConnected) {
    // La connexion série est rétablie
    _serialConnected = true;
  }

}
