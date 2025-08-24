#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "SpeechToText.h"

// Global variable to handle graceful shutdown
volatile bool shouldStop = false;

void signalHandler(int signum) {
    printf("\nReceived interrupt signal, stopping recording...\n");
    shouldStop = true;
}

void init() {
    printf("main::init(): Initialized\n");
}

int main(int argc, char *argv[]) {
    init();
    
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signalHandler);
    
    // Get SpeechToText instance
    SpeechToText* stt = SpeechToText::getInstance();
    
    // Initialize the recording module (this will default to Vosk)
    if (!stt->initializeRecordModule()) {
        printf("Failed to initialize recording module\n");
        return 1;
    }
    
    printf("Using OpenAI Whisper API backend for speech recognition\n");
    printf("Starting continuous recording... Press Ctrl+C to stop\n");
    
    // Start recording immediately
    if (stt->startRecording()) {
        printf("Recording started - speak now!\n");
        
        std::string lastTranscription = "";
        int transcriptionCounter = 0;
        
        // Continuous recording loop with live transcription
        while (!shouldStop) {
            // Small delay to prevent busy waiting
            usleep(500000); // 500ms - check transcription every half second
            
            // Check if still recording
            if (!stt->isRecording()) {
                printf("Recording stopped unexpectedly\n");
                break;
            }
            
            // Get live transcription
            std::string currentTranscription = stt->getLiveTranscription();
            
            // Only print if transcription changed or every 5 seconds
            if (currentTranscription != lastTranscription || transcriptionCounter % 10 == 0) {
                if (!currentTranscription.empty()) {
                    printf("Live: %s\n", currentTranscription.c_str());
                } else if (transcriptionCounter % 10 == 0) {
                    printf("Listening... (speak now)\n");
                }
                lastTranscription = currentTranscription;
            }
            
            transcriptionCounter++;
        }
        
        // Stop recording
        stt->stopRecording();
        printf("Recording stopped\n");
        
        // Get final transcription
        std::string text = stt->getTranscribedText();
        printf("Final transcription: %s\n", text.c_str());
    }
    
    // Cleanup
    stt->cleanup();
    
    return 0;
}