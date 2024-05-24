import pyttsx3
import creds

def say(text):
    engine = pyttsx3.init()
    engine.setProperty('rate', creds.SPEAK_RATE_WPM)
    engine.say(text) 
    engine.runAndWait()
