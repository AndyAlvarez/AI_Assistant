import speechToText
import textToSpeech
import openAI

# Variables
speakRate_WPM = 160

# Record Audio
user_audio = speechToText.record()

# Generate Response from ChatGPT
response = openAI.chat_with_DNA(user_audio)
print("\nMr.DNA: ", response)

# Read Text aloud
textToSpeech.say(response, speakRate_WPM)