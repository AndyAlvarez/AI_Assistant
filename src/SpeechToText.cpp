#include "SpeechToText.h"
#include "VoskBackend.h"
#include "WhisperAPIBackend.h"
#include <fstream>
#include <iostream>

SpeechToText* SpeechToText::_instance = nullptr;

SpeechToText::SpeechToText() : _inputStream(nullptr), _isRecording(false), 
                                _sampleRate(16000), _framesPerBuffer(256), _numChannels(1),
                                _backend(nullptr), _currentBackend(BackendType::WHISPER_API) {
    printf("SpeechToText initialized\n");
}

SpeechToText::~SpeechToText() {
    cleanup();
    printf("SpeechToText destroyed\n");
}

SpeechToText* SpeechToText::getInstance() {
    if (_instance == nullptr) {
        _instance = new SpeechToText();
    }
    return _instance;
}

bool SpeechToText::setBackend(BackendType type) {
    // Clean up existing backend
    if (_backend) {
        _backend->cleanup();
        delete _backend;
        _backend = nullptr;
    }
    
    // Create new backend
    switch (type) {
        case BackendType::VOSK_LOCAL:
            _backend = new VoskBackend();
            break;
        case BackendType::WHISPER_API:
            _backend = new WhisperAPIBackend();
            break;
        case BackendType::GOOGLE_SPEECH_API:
            // TODO: Implement Google Speech API backend
            printf("Google Speech API backend not yet implemented\n");
            return false;
        default:
            printf("Unknown backend type\n");
            return false;
    }
    
    _currentBackend = type;
    
    // Initialize the new backend
    if (!_backend->initialize()) {
        printf("Failed to initialize %s backend\n", _backend->getName().c_str());
        delete _backend;
        _backend = nullptr;
        return false;
    }
    
    printf("Switched to %s backend\n", _backend->getName().c_str());
    return true;
}

bool SpeechToText::initializeRecordModule() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio initialization failed: %s\n", Pa_GetErrorText(err));
        return false;
    }
    
    // Open input stream
    err = Pa_OpenDefaultStream(&_inputStream,
                              _numChannels,           // input channels
                              0,                      // output channels
                              paFloat32,              // sample format
                              _sampleRate,            // sample rate
                              _framesPerBuffer,       // frames per buffer
                              nullptr,                // stream callback
                              nullptr);               // user data
    
    if (err != paNoError) {
        printf("Failed to open input stream: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return false;
    }
    
    // Initialize default backend (Whisper API)
    if (!setBackend(BackendType::WHISPER_API)) {
        printf("Failed to initialize default backend\n");
        return false;
    }
    
    printf("Audio recording module initialized successfully\n");
    return true;
}

bool SpeechToText::startRecording() {
    if (_inputStream == nullptr) {
        printf("Recording module not initialized\n");
        return false;
    }
    
    if (_isRecording) {
        printf("Already recording\n");
        return true;
    }
    
    // Clear previous recording
    _recordedAudio.clear();
    
    PaError err = Pa_StartStream(_inputStream);
    if (err != paNoError) {
        printf("Failed to start recording: %s\n", Pa_GetErrorText(err));
        return false;
    }
    
    _isRecording = true;
    printf("Recording started\n");
    
    // Start capturing audio data
    const int maxFrames = _sampleRate * 30; // Max 30 seconds
    _recordedAudio.reserve(maxFrames);
    
    while (_isRecording && _recordedAudio.size() < maxFrames) {
        float buffer[256];
        err = Pa_ReadStream(_inputStream, buffer, _framesPerBuffer);
        if (err == paInputOverflowed) {
            printf("Input overflow\n");
            continue;
        } else if (err != paNoError) {
            printf("Error reading stream: %s\n", Pa_GetErrorText(err));
            break;
        }
        
        // Add audio data to recording
        for (int i = 0; i < _framesPerBuffer; i++) {
            _recordedAudio.push_back(buffer[i]);
        }
        
        // Small delay to prevent busy waiting
        Pa_Sleep(10);
    }
    
    return true;
}

bool SpeechToText::stopRecording() {
    if (!_isRecording) {
        printf("Not currently recording\n");
        return true;
    }
    
    _isRecording = false;
    
    if (_inputStream == nullptr) {
        return true;
    }
    
    PaError err = Pa_StopStream(_inputStream);
    if (err != paNoError) {
        printf("Failed to stop recording: %s\n", Pa_GetErrorText(err));
        return false;
    }
    
    printf("Recording stopped. Captured %zu samples\n", _recordedAudio.size());
    return true;
}

bool SpeechToText::isRecording() const {
    return _isRecording;
}

std::string SpeechToText::getTranscribedText() {
    if (!_backend) {
        printf("No backend initialized\n");
        return "";
    }
    
    if (_recordedAudio.empty()) {
        printf("No audio data to transcribe\n");
        return "";
    }
    
    printf("Transcribing audio with %s...\n", _backend->getName().c_str());
    std::string result = _backend->transcribe(_recordedAudio, _sampleRate);
    printf("Transcription result: %s\n", result.c_str());
    return result;
}

std::string SpeechToText::getLiveTranscription() {
    if (!_backend) {
        return "";
    }
    
    if (_recordedAudio.empty()) {
        return "";
    }
    
    // For live transcription, we'll use a smaller buffer of recent audio
    // This gives us near real-time results
    const size_t liveBufferSize = _sampleRate * 2; // Last 2 seconds
    std::vector<float> liveBuffer;
    
    if (_recordedAudio.size() > liveBufferSize) {
        liveBuffer.assign(_recordedAudio.end() - liveBufferSize, _recordedAudio.end());
    } else {
        liveBuffer = _recordedAudio;
    }
    
    // Get transcription for the recent audio buffer
    std::string result = _backend->transcribe(liveBuffer, _sampleRate);
    return result;
}

void SpeechToText::cleanup() {
    if (_isRecording) {
        stopRecording();
    }
    
    if (_inputStream != nullptr) {
        Pa_CloseStream(_inputStream);
        _inputStream = nullptr;
    }
    
    if (_backend) {
        _backend->cleanup();
        delete _backend;
        _backend = nullptr;
    }
    
    Pa_Terminate();
    printf("Audio recording module cleaned up\n");
}