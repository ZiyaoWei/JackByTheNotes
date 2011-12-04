#include "PlayedNote.h"

PlayedNote::PlayedNote(const Note& note):note(note) {
  gettimeofday(&beginTime, NULL);
  endTime.tv_usec = beginTime.tv_usec + this->note.getDuration() * 1000;
  endTime.tv_sec = beginTime.tv_sec + endTime.tv_usec / 1000000;
  endTime.tv_usec %= 1000000;
}

Note PlayedNote::getNote() {
  return note;
}

struct timeval PlayedNote::getBeginTime() {
  return beginTime;
}

struct timeval PlayedNote::getEndTime() {
  return endTime;
}
