#pragma once
#include "SpeechRecognitionBackend.h"
#include <string>

class WhisperAPIBackend : public SpeechRecognitionBackend {
    public:
        WhisperAPIBackend(const std::string& apiKey = "");
        ~WhisperAPIBackend();
        
        bool initialize() override;
        std::string transcribe(const std::vector<float>& audioData, int sampleRate) override;
        void cleanup() override;
        std::string getName() const override { return "OpenAI Whisper API"; }
        
        void setAPIKey(const std::string& apiKey) { _apiKey = apiKey; }
        
    private:
        std::string _apiKey;
        bool _initialized;
        std::string _sendAudioToWhisper(const std::string& audioFile, int sampleRate);
        bool saveAudioAsWAV(const std::string& filename, const std::vector<int16_t>& audioData, int sampleRate);
};
