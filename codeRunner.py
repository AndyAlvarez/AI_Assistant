import speechToText
import openAI

user_audio = speechToText.record()
response = openAI.chat_with_DNA(user_audio)
print("\nMr.DNA: ", response)
