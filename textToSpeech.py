#import pyttsx3
#import creds
#import objc

#def say(text):
    #engine = pyttsx3.init()
    #engine.setProperty('rate', creds.SPEAK_RATE_WPM)
    #engine.say(text) 
    #engine.runAndWait()

from gtts import gTTS
from pydub import AudioSegment
from pydub.playback import play
from pydub.effects import speedup
import creds
import io

def say(text):
    tts = gTTS(text=text, lang='en', slow=False)
    with io.BytesIO() as f:
        tts.write_to_fp(f)
        f.seek(0)
        audio = AudioSegment.from_file(f, format="mp3")
        # Adjust playback speed without changing the pitch
        if creds.SPEAK_RATE_WPM != 1.0:
            audio = speedup(audio, playback_speed=creds.SPEAK_RATE_WPM)
        play(audio)
