#ifndef JACK_BY_THE_NOTES
#define JACK_BY_THE_NOTES
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <jack/jack.h>
#include <jack/transport.h>
//#include <queue>
#include <deque>

#include "PlayedNote.h"

class JackByTheNotes {
 private:
  static const double PI = 3.14159265359;
  static const double MaxAmp = 0.5;

  jack_client_t * client;
  jack_port_t * outputPort;
  unsigned long sampleRate;
  int bpm;
  jack_nframes_t waveLength;
  jack_default_audio_sample_t *wave;
  long offset;
  jack_transport_state_t transport_state;
  time_t seconds;
  double *amp;
  jack_default_audio_sample_t scale, scale2;
  int newNote;
  
  // std::priority_queue<Note> currentNotes;
  std::deque<PlayedNote> currentNotes;
  void removeNote();
  void addNote();
  
 public:
  JackByTheNotes();

  void playNote(Note note);
  void checkNote();
};
#endif
