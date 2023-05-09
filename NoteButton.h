class NoteButton {
  public:
    NoteButton(byte row, byte col, byte note) {
      this -> row = row;
      this -> col = col;
      this -> note = note;
      this -> noteVelocity = 127;
    }
    byte row;
    byte col;
    byte note;
    byte lastPlayedNote;
    byte noteVelocity;
    boolean lastState = 1;
    boolean newState = 1;

    void playNote(byte sensorMode, byte sensorVal, byte sensorOctaveShift) {
      byte noteVelocity = sensorMode == 0 ? sensorVal : 127;
      byte noteToSend = note;
//      if (sensorMode == 3) {
//        noteToSend = note + 12 * sensorOctaveShift;
//      }
//      //      if (newState) {
//      if (sensorMode == 3) {
//        lastPlayedNote = noteToSend;
//      }
      sendNote();
      //      } else {
      //        if (sensorMode == 3) {
      //          lastPlayedNote = 0;
      //        }
      //        if (lastPlayedNote) {
      //          midiEventPacket_t noteOff = {0x08, 0x80, lastPlayedNote, noteVelocity};
      //        }
      //        cancelNote();
      //      }
    }

    void sendNote() {
      midiEventPacket_t noteOn = {0x09, 0x90, lastPlayedNote, noteVelocity};
      MidiUSB.sendMIDI(noteOn);
      MidiUSB.flush();
    }

    void cancelNote() {
      midiEventPacket_t noteOff = {0x08, 0x80, lastPlayedNote, noteVelocity};
      MidiUSB.sendMIDI(noteOff);
      MidiUSB.flush();
    }

    void changeButtonNote(byte note) {
      this -> note = note;

      // Used for octave changes mainly. Rearticulates the note if it is changed while playing.
      if (!lastState) {
        cancelNote();
        sendNote();
      }
    }
};
