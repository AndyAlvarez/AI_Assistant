import speech_recognition as sr
import creds
import textToSpeech
import openAI


def handle_input(text, conversation_history):

    conversation_history += f"{text}\n"
    
    if len(conversation_history) > creds.MAX_PROMPT_LENGTH:
        excess_length = len(conversation_history) - creds.MAX_PROMPT_LENGTH
        
        trim_index = conversation_history.find('\n', excess_length)
        
        if trim_index != -1:
            conversation_history = conversation_history[trim_index + 1:]
        else:
            conversation_history = conversation_history[-creds.MAX_PROMPT_LENGTH:]

    response = openAI.chat_with_DNA(conversation_history, returning=True)
    
    conversation_history += f"{response}\n"

    #print(f"\n\nConvo History: <<{conversation_history}>>")

    return conversation_history


def record():

    isSaid = False

    conversation_history = ""

    r = sr.Recognizer()

    while isSaid == False:
        
        try:
            with sr.Microphone() as source:
                r.adjust_for_ambient_noise(source, duration=0.5)
                print(f"{creds.ASSISTANT_NAME} is Listening!...")
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
                # openAI.chat_with_DNA(text)
                conversation_history += handle_input(text, conversation_history)

        except Exception as e:
            print(e)
            print("[Reverting to Listening to Wake Word]")
            listenForWakeWord()

def text_chat():

    isSaid = False
    conversation_history = ""

    print(f"{creds.GREETING}")

    while isSaid == False:
                
        print("\n\n")
        print("You:")
        writtenText = input(f"Say something to {creds.ASSISTANT_NAME}: ")
        print("\n\n")

        if writtenText != "exit":
            conversation_history += handle_input(writtenText, conversation_history)
        else:
            isSaid = True
    

def listenForWakeWord(chat_type="text"):

    if creds.CHAT_TYPE == "text":
        text_chat()
    else:
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
                        textToSpeech.say(f"{creds.GREETING}", creds.SPEAK_RATE_WPM) 
                        print(f"\n[Activated: Wake Word '{wakeWordCheckText}']")
                        record()

            except Exception as e:
                print(e)
            
