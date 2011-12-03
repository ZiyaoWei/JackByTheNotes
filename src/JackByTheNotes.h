#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <jack/jack.h>
#include <jack/transport.h>
#include <dequeue>

#include "Note.h"

class JackByTheNotes {
 private:
  std::dequeue<Note> currentNotes;
}
