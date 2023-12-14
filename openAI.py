import os
import creds
from openai import OpenAI

client = OpenAI(
    # This is the default and can be omitted
    # api_key=os.environ.get("OPENAI_API_KEY"),
    api_key=creds.API_KEY
)

def chat_with_DNA(prompt):
    response = client.chat.completions.create(
        messages=[
            {
                "role": "user",
                "content": prompt,
            }
        ],
        model="gpt-3.5-turbo",
        # stream = True
    )
    
    return response.choices[0].message.content.strip()
    # for chunk in response:
    #     return chunk.choices[0].delta.content

# if __name__ == "__main__":
    # while True:
    #     user_input = input("You: ")
    #     if user_input.lower() in ["quit", "exit", "bye"]:
    #         break

    #     response = chat_with_DNA(user_input)
    #     print("Mr.DNA: ", response)
