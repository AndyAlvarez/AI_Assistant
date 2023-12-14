import pyttsx3

def say(text, speakRate_WPM):
    engine = pyttsx3.init()
    engine.setProperty('rate', speakRate_WPM)
    engine.say(text)
    engine.runAndWait()
