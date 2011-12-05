#include "JackByTheNotes.h"
#include <iostream>

JackByTheNotes::JackByTheNotes() {
  const char * client_name = "metro";
  if ((client = jack_client_new(client_name)) == 0) {
    fprintf(stderr, "jack server unavailable\n");
    return;
  }
  jack_set_process_callback(client, ::processAll, this);
  outputPort = jack_port_register(client, "Paganini", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  sampleRate = jack_get_sample_rate(client);
  bpm = 60;
  waveLength = 60 * sampleRate / bpm;
  wave = (jack_default_audio_sample_t *) malloc (waveLength * sizeof(jack_default_audio_sample_t));
  for (int i = 0; i < (int)waveLength; i++) {
    wave[i] = 0;
  }

  offset = 0;
}

int processAll(jack_nframes_t nframes, void * arg) {
  JackByTheNotes * jackByTheNotes = static_cast<JackByTheNotes *>(arg);
  jackByTheNotes->play(nframes);
}

void JackByTheNotes::play(jack_nframes_t nframes) {
  jack_default_audio_sample_t * buffer = (jack_default_audio_sample_t *)jack_port_get_buffer(outputPort, nframes);
  jack_nframes_t framesLeft = nframes;

  checkNote();
  while (waveLength - offset < framesLeft) {
    memcpy(buffer + (nframes - framesLeft), wave + offset, sizeof(jack_default_audio_sample_t) * (waveLength - offset));
    framesLeft -= waveLength - offset;
    offset = 0;
}
  if (framesLeft > 0) {
    memcpy(buffer + (nframes - framesLeft), wave + offset, sizeof(jack_default_audio_sample_t) * framesLeft);
    offset += framesLeft;
  }
}

void JackByTheNotes::addNote() {
  Note note = currentNotes.back().getNote();
  jack_nframes_t toneLength = waveLength;//sampleRate * getDuration() / 1000;
  jack_nframes_t attackLength = toneLength * note.getAttackPercent() / 100;
  jack_nframes_t decayLength = toneLength * note.getDecayPercent() / 100;
  jack_default_audio_sample_t scale = 2 * PI * note.getFrequency() / sampleRate;
  if ((toneLength > waveLength) || (attackLength + decayLength > (int)toneLength)) {
    std::cout << toneLength << " " << waveLength << " " << attackLength << " " << decayLength << std::endl;
    std::cout << note.getDecayPercent() << std::endl;
    fprintf(stderr, "invalid note information\n");
    return;
  }
  amp = (double *) malloc (toneLength * sizeof(double));

  for (int i = 0; i < attackLength; i++) {
    amp[i] = MaxAmp * i / ((double)attackLength);
  }

  for (int i = attackLength; i < (int)toneLength - decayLength; i++) {
    amp[i] = MaxAmp;
  }

  for (int i = (int)toneLength - decayLength; i < (int)toneLength; i++) {
    amp[i] = (-1) * MaxAmp * (i - (double)toneLength) / ((double)decayLength);
  }

  for (int i = 0; i < (int)toneLength; i++) {
    wave[i] += amp[i] * sin(scale * i);
  }

}

void JackByTheNotes::removeNote() {
  Note note = currentNotes.front().getNote();
  jack_nframes_t toneLength = waveLength;
  jack_nframes_t attackLength = toneLength * note.getAttackPercent() / 100;
  unsigned long decayLength = toneLength * note.getDecayPercent() / 100;
  jack_default_audio_sample_t scale = 2 * PI * note.getFrequency() / sampleRate;

  amp = (double *) malloc (toneLength * sizeof(double));

  for (int i = 0; i < attackLength; i++) {
    amp[i] = MaxAmp * i / ((double)attackLength);
  }

  for (int i = attackLength; i < (int)toneLength - decayLength; i++) {
    amp[i] = MaxAmp;
  }

  for (int i = (int)toneLength - decayLength; i < (int)toneLength; i++) {
    amp[i] = (-1) * MaxAmp * (i - (double)toneLength) / ((double)decayLength);
  }

  for (int i = 0; i < (int)toneLength; i++) {
    wave[i] -= amp[i] * sin(scale * i);
  }

}

void JackByTheNotes::playNote(Note note) {
  PlayedNote playedNote(note);
  currentNotes.push_back(playedNote);
  addNote();
}

void JackByTheNotes::checkNote() {
  timeval currentTime;
  gettimeofday(&currentTime, NULL);
  while ((!currentNotes.empty())) {
    timeval noteTime = currentNotes.front().getEndTime();
    if (timercmp(&currentTime, &noteTime, >=)) {
      std::cout<<"remove old"<<std::endl;
      removeNote();
      currentNotes.pop_front();
    } else {
      break;
    }
  }
}

void JackByTheNotes::activate() {
  if (jack_activate(client)) {
    fprintf(stderr, "Activate failure\n");
  }
  while (1) {
    sleep(1);
  }
}

void JackByTheNotes::connect() {
  pthread_create(&messenger, 0, &startConnection, this);
}

void * JackByTheNotes::startConnection(void * instance) {
  reinterpret_cast<JackByTheNotes *>(instance)->activate();
}

int main() {
  JackByTheNotes * notesJack = new JackByTheNotes();
  notesJack->connect();
  while (true) {
    int frequency = 440;
    for (int i = 0; i < 4; i++) {
      Note * note = new Note(frequency, 2, 1800);
      std::cout<<frequency<<std::endl;
      notesJack->playNote(*note);
      sleep(2);
      frequency *= 1.414;
    }
  }
}
