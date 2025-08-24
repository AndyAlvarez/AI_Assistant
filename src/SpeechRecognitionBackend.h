#pragma once
#include <string>
#include <vector>

class SpeechRecognitionBackend {
    public:
        virtual ~SpeechRecognitionBackend() = default;
        virtual bool initialize() = 0;
        virtual std::string transcribe(const std::vector<float>& audioData, int sampleRate) = 0;
        virtual void cleanup() = 0;
        virtual std::string getName() const = 0;
};
