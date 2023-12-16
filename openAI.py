import os
import creds
import textToSpeech
from openai import OpenAI


client = OpenAI(
    api_key=creds.API_KEY
)

def chat_with_DNA(prompt, speakRate=200, streaming=False):

    ASSISTANT_NAME = "Mr. DNA"
    
    if streaming == True:

        stream = client.chat.completions.create(
            messages=[
                {
                    "role": "user",
                    "content": prompt,
                }
            ],
            model="gpt-3.5-turbo",
            stream=True
        )
    
        for chunk in stream:

            result = chunk.choices[0].delta.content or ""
            print(f"{ASSISTANT_NAME}: ", result, end="", flush=True)


            if result != "":
                textToSpeech.say(result, speakRate)
            else: pass

        print()
    
    else:
        response = client.chat.completions.create(
            messages=[
                {
                    "role": "user",
                    "content": prompt,
                }
            ],
            model="gpt-3.5-turbo",
        )
        result = response.choices[0].message.content.strip()
        print(f"{ASSISTANT_NAME}: ", result)
        textToSpeech.say(result, speakRate)