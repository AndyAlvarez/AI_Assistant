#include "WhisperAPIBackend.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>

// Callback function for CURL to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

WhisperAPIBackend::WhisperAPIBackend(const std::string& apiKey) 
    : _apiKey(apiKey), _initialized(false) {
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

WhisperAPIBackend::~WhisperAPIBackend() {
    cleanup();
}

bool WhisperAPIBackend::initialize() {
    if (_apiKey.empty()) {
        printf("WhisperAPIBackend: No API key provided\n");
        printf("To use Whisper API:\n");
        printf("1. Get your API key from: https://platform.openai.com/api-keys\n");
        printf("2. Set it in the code or environment variable\n");
        printf("3. For now, this will show placeholder results\n");
        printf("Setting up in placeholder mode...\n");
        _initialized = true;
        return true;
    }
    
    printf("WhisperAPIBackend: Initialized with API key\n");
    _initialized = true;
    return true;
}

std::string WhisperAPIBackend::transcribe(const std::vector<float>& audioData, int sampleRate) {
    if (!_initialized) {
        printf("WhisperAPIBackend: Not initialized\n");
        return "";
    }
    
    if (audioData.empty()) {
        printf("WhisperAPIBackend: No audio data provided\n");
        return "";
    }
    
    printf("WhisperAPIBackend: Transcribing %zu samples at %d Hz\n", audioData.size(), sampleRate);
    
    // If no API key, return placeholder
    if (_apiKey.empty()) {
        float duration = (float)audioData.size() / sampleRate;
        return "[WHISPER PLACEHOLDER] Recorded " + std::to_string((int)duration) + " seconds of audio. Set your OpenAI API key to get real transcription.";
    }
    
    // Convert float audio to 16-bit PCM for Whisper API
    std::vector<int16_t> pcmData;
    pcmData.reserve(audioData.size());
    
    for (float sample : audioData) {
        // Clamp to [-1, 1] and convert to 16-bit
        sample = std::max(-1.0f, std::min(1.0f, sample));
        pcmData.push_back(static_cast<int16_t>(sample * 32767.0f));
    }
    
    // Save audio to temporary WAV file
    std::string tempFile = "/tmp/whisper_audio.wav";
    if (!saveAudioAsWAV(tempFile, pcmData, sampleRate)) {
        return "[ERROR] Failed to save audio file";
    }
    
    // Send to Whisper API
    std::string result = _sendAudioToWhisper(tempFile, sampleRate);
    
    // Clean up temp file
    std::remove(tempFile.c_str());
    
    return result;
}

void WhisperAPIBackend::cleanup() {
    curl_global_cleanup();
    _initialized = false;
    printf("WhisperAPIBackend: Cleaned up\n");
}

std::string WhisperAPIBackend::_sendAudioToWhisper(const std::string& audioFile, int sampleRate) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "[ERROR] Failed to initialize CURL";
    }
    
    // Read audio file
    std::ifstream file(audioFile, std::ios::binary);
    if (!file.is_open()) {
        curl_easy_cleanup(curl);
        return "[ERROR] Failed to open audio file";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string audioData = buffer.str();
    file.close();
    
    // Prepare multipart form data
    struct curl_slist* headers = nullptr;
    std::string authHeader = "Authorization: Bearer " + _apiKey;
    headers = curl_slist_append(headers, authHeader.c_str());
    
    // Create form data
    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part = curl_mime_addpart(mime);
    
    curl_mime_name(part, "file");
    curl_mime_filedata(part, audioFile.c_str());
    curl_mime_type(part, "audio/wav");
    
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "model");
    curl_mime_data(part, "whisper-1", CURL_ZERO_TERMINATED);
    
    // Set up CURL options
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/audio/transcriptions");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    
    // Clean up
    curl_slist_free_all(headers);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return "[ERROR] CURL request failed: " + std::string(curl_easy_strerror(res));
    }
    
    // Parse response (simplified - you might want to use a JSON parser)
    if (response.find("\"text\":") != std::string::npos) {
        size_t start = response.find("\"text\":") + 8;
        size_t end = response.find("\"", start);
        if (end != std::string::npos) {
            return response.substr(start, end - start);
        }
    }
    
    return "[ERROR] Failed to parse Whisper API response: " + response;
}

bool WhisperAPIBackend::saveAudioAsWAV(const std::string& filename, const std::vector<int16_t>& audioData, int sampleRate) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Write WAV header
    // RIFF header
    file.write("RIFF", 4);
    uint32_t fileSize = 36 + audioData.size() * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&fileSize), 4);
    file.write("WAVE", 4);
    
    // fmt chunk
    file.write("fmt ", 4);
    uint32_t fmtSize = 16;
    file.write(reinterpret_cast<const char*>(&fmtSize), 4);
    uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    uint16_t numChannels = 1;
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    uint32_t sampleRate32 = sampleRate;
    file.write(reinterpret_cast<const char*>(&sampleRate32), 4);
    uint32_t byteRate = sampleRate * numChannels * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    uint16_t blockAlign = numChannels * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    uint16_t bitsPerSample = 16;
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    
    // data chunk
    file.write("data", 4);
    uint32_t dataSize = audioData.size() * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);
    
    // Write audio data
    file.write(reinterpret_cast<const char*>(audioData.data()), dataSize);
    
    file.close();
    return true;
}
