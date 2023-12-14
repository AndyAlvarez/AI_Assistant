import speech_recognition as sr

def record():
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print("Say Something...!")
        audio = r.listen(source)
        print("done!")

    try:
        text = r.recognize_google(audio)
        print("\nYou: " + text)

    except Exception as e:
        print(e)

    return text
