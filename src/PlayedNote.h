#include <time.h>
#include "Note.h"

class PlayedNote {
 private:
  Note note;
  struct time_val beginTime;
  struct time_val endTime;

 public:
  PlayedNote(Note note);
  Note getNote();
  struct time_val getBeginTime();
  struct time_val getEndTime();
}
