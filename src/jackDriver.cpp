#include <iostream>
#include <cstdlib>

int main() {
  system("qjackctl &");
  sleep(2);
  //system("gnome-terminal -e \"./metronome -b 15\" &");
  system("gnome-terminal -e \"./a.out\" &");
  sleep(2);
  system("jack_connect metro:Paganini system:playback_1 &");
}
