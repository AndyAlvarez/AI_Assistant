#pragma once
#include "SpeechRecognitionBackend.h"
#include <string>

// Forward declaration to avoid including Vosk headers here
struct VoskModel;
struct VoskRecognizer;

class VoskBackend : public SpeechRecognitionBackend {
    public:
        VoskBackend(const std::string& modelPath = "");
        ~VoskBackend();
        
        bool initialize() override;
        std::string transcribe(const std::vector<float>& audioData, int sampleRate) override;
        void cleanup() override;
        std::string getName() const override { return "Vosk (Local)"; }
        
    private:
        std::string _modelPath;
        VoskModel* _model;
        VoskRecognizer* _recognizer;
        bool _initialized;
};
