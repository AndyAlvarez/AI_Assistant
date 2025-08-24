#pragma once
#include <stdio.h>
#include <portaudio.h>
#include <vector>
#include <string>

// Forward declarations
class SpeechRecognitionBackend;
class VoskBackend;
class WhisperAPIBackend;

class SpeechToText {
    public:
        enum class BackendType {
            VOSK_LOCAL,
            WHISPER_API,
            GOOGLE_SPEECH_API
        };
        
        static SpeechToText* getInstance();
        bool initializeRecordModule();
        bool setBackend(BackendType type);
        bool startRecording();
        bool stopRecording();
        bool isRecording() const;
        std::string getTranscribedText();
        std::string getLiveTranscription(); 
        void cleanup();

    private:
        static SpeechToText* _instance;
        SpeechToText();
        ~SpeechToText();
        
        // Audio recording members
        PaStream* _inputStream;
        bool _isRecording;
        int _sampleRate;
        int _framesPerBuffer;
        int _numChannels;
        std::vector<float> _recordedAudio;
        
        // Speech recognition backend
        SpeechRecognitionBackend* _backend;
        BackendType _currentBackend;
};