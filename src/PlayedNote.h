#ifndef PLAYED_NOTE
#define PLAYED_NOTE

#include <stddef.h>
#include <sys/time.h>
#include "Note.h"

class PlayedNote {
 private:
  Note note;
  struct timeval beginTime;
  struct timeval endTime;

 public:
  PlayedNote(const Note& note);
  Note getNote();
  struct timeval getBeginTime();
  struct timeval getEndTime();
};
#endif
