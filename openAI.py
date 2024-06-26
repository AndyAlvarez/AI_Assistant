import os
import creds
import textToSpeech
from openai import OpenAI
import tiktoken

client = OpenAI(
    api_key=creds.API_KEY
)


def chat_with_DNA(prompt, returning=False, streaming=False):
    
    print(f"\n Length of prompt: <<{len(prompt)}>>\n\n")

    if streaming == True:

        stream = client.chat.completions.create(
            messages=[
                {
                    "role": "user",
                    "content": prompt,
                }
            ],
            model=creds.GPT_MODEL,
            stream=True
        )
    
        for chunk in stream:

            result = chunk.choices[0].delta.content or ""
            print(f"{creds.ASSISTANT_NAME}: ", result, end="", flush=True)


            if result != "" and creds.CHAT_TYPE != "text":
                textToSpeech.say(result)
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
            model=creds.GPT_MODEL,
        )
        result = response.choices[0].message.content.strip()

        if returning == True:
            print(f"{creds.ASSISTANT_NAME}: ", result)
            if creds.CHAT_TYPE != "text":
                textToSpeech.say(result)
            return result

        else:
            print(f"{creds.ASSISTANT_NAME}: ", result)
            if creds.CHAT_TYPE != "text":
                textToSpeech.say(result)
