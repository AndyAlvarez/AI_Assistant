import speech_recognition as sr
import creds
import textToSpeech
import openAI


def handle_input(text, conversation_history):
    conversation_history += f"{text}\n"

    response = openAI.chat_with_DNA(conversation_history, creds.SPEAK_RATE_WPM, returning=True)

    conversation_history += f"{response}\n"

    return conversation_history

def record():

    isSaid = False

    conversation_history = ""

    r = sr.Recognizer()

    while isSaid == False:
        
        try:
            with sr.Microphone() as source:
                r.adjust_for_ambient_noise(source, duration=0.5)
                print("Mr.DNA is Listening!...")
                audio = r.listen(source, timeout=10)
                print("[Done! Speech Recorded]")
        
            text = r.recognize_google(audio)
            print("[Mic Check]: ", text)
            for word in creds.STOP_WORDS:
                if word == text.lower():
                    textToSpeech.say("See you later!", creds.SPEAK_RATE_WPM)
                    listenForWakeWord() 
                    isSaid = True 
            print("\nYou: " + text)
            if isSaid != True:
                # openAI.chat_with_DNA(text, creds.SPEAK_RATE_WPM)
                conversation_history += handle_input(text, conversation_history)

        except Exception as e:
            print(e)
            print("[Reverting to Listening to Wake Word]")
            listenForWakeWord()


def listenForWakeWord():

    r = sr.Recognizer()
    print("[Listening for Wake Word]")
    
    while True:

        with sr.Microphone() as source:
            print("...")
            audio = r.listen(source, phrase_time_limit=6, timeout=6)
            
        try:
            wakeWordCheckText = r.recognize_google(audio)
            print("[Checker:] ", wakeWordCheckText)
            for word in creds.WAKE_WORDS:
                if word in wakeWordCheckText:
                    textToSpeech.say("Hello Explorer!", creds.SPEAK_RATE_WPM)
                    print(f"\n[Activated: Wake Word '{wakeWordCheckText}']")
                    record()

        except Exception as e:
            print(e)
            
