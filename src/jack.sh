gcc `pkg-config --cflags --libs jack` JackByTheNotes.cpp PlayedNote.cpp Note.cpp -lstdc++ -Wno-deprecated-declarations
g++ jackDriver.cpp
sudo ./a.out
