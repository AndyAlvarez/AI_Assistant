GCC=g++
CFLAGS=-Wall -std=c++11 -I/opt/homebrew/include
LIBS=-L/opt/homebrew/lib -lportaudio -lcurl

# Source files
SOURCES=main.cpp SpeechToText.cpp VoskBackend.cpp WhisperAPIBackend.cpp
HEADERS=SpeechToText.h SpeechRecognitionBackend.h VoskBackend.h WhisperAPIBackend.h

all: ai_assistant

ai_assistant: $(SOURCES) $(HEADERS)
	$(GCC) $(CFLAGS) -o ai_assistant $(SOURCES) $(LIBS)

clean:
	rm -f ai_assistant

run: ai_assistant
	./ai_assistant


