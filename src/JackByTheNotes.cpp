#include "JackByTheNotes.h"

JackByTheNotes::JackByTheNotes() {
  const char * client_name = "metro";
  if ((client = jack_client_new(client_name)) == 0) {
    fprintf(stderr, "jack server unavailable\n");
    return;
  }
  //jack_set_process_callback(client, process, 0);
  outputPort = jack_port_register(client, "Paganini", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  sampleRate = jack_get_sample_rate(client);
  bpm = 60;
  waveLength = 60 * sampleRate / bpm;
  wave = (jack_default_audio_sample_t *) malloc (waveLength * sizeof(jack_default_audio_sample_t));
  for (int i = 0; i < (int)waveLength; i++) {
    wave[i] = 0;
  }

}

void JackByTheNotes::addNote() {
  Note note = currentNotes.front().getNote();
  jack_nframes_t toneLength = sampleRate * note.getDuration() / 1000;
  jack_nframes_t attackLength = toneLength * note.getAttackPercent() / 100;
  unsigned long decayLength = toneLength * note.getDecayPercent() / 100;
  jack_default_audio_sample_t scale = 2 * PI * note.getFrequency() / sampleRate;
  if ((toneLength >= waveLength) || (attackLength + decayLength > (int)toneLength)) {
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
  jack_nframes_t toneLength = sampleRate * note.getDuration() / 1000;
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
}

void JackByTheNotes::checkNote() {
  timeval currentTime;
  gettimeofday(&currentTime, NULL);
  while ((!currentNotes.empty())) {
    timeval noteTime = currentNotes.front().getEndTime();
    if (timercmp(&currentTime, &noteTime, >=)) {
      //removeNote();
    } else {
      break;
    }
  }
}

int main() {
}
