#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 KMail AI Integration Team
# SPDX-License-Identifier: GPL-2.0-only

"""
AI Reply Generator

This script generates appropriate replies to emails based on their content
and the user's previous communication style.
"""

import sys
import json
import re
import random

# Templates for different types of replies
FORMAL_TEMPLATES = [
    "Thank you for your email. {response}",
    "I appreciate your message. {response}",
    "Thank you for reaching out. {response}",
    "I have received your email. {response}",
    "Thank you for your inquiry. {response}"
]

INFORMAL_TEMPLATES = [
    "Thanks for your email! {response}",
    "Got your message. {response}",
    "Thanks for reaching out! {response}",
    "Hey there! {response}",
    "Good to hear from you! {response}"
]

# Response patterns based on email content
RESPONSE_PATTERNS = [
    # Meeting requests
    (r'meet|meeting|appointment|schedule|calendar', [
        "I would be happy to meet. How does {day} at {time} work for you?",
        "I'm available to meet on {day} at {time}. Would that work for you?",
        "Let's schedule a meeting. I'm free on {day} at {time}.",
        "I can make time for a meeting on {day} at {time}."
    ]),
    
    # Information requests
    (r'information|details|specs|documentation|explain', [
        "I'll gather the information you requested and get back to you soon.",
        "I'll send you the details you're looking for shortly.",
        "I'll prepare the requested information and share it with you.",
        "I'll look into this and provide you with the information you need."
    ]),
    
    # Thank you emails
    (r'thank you|thanks|appreciate', [
        "You're welcome! It was my pleasure.",
        "I'm glad I could help.",
        "Happy to be of assistance.",
        "No problem at all."
    ]),
    
    # Questions
    (r'\?|how|what|when|where|why|who', [
        "Regarding your question, {custom_response}",
        "To answer your question, {custom_response}",
        "About your inquiry, {custom_response}",
        "In response to your question, {custom_response}"
    ]),
    
    # Default responses
    (r'.*', [
        "I'll look into this and get back to you soon.",
        "I'll review this and respond in more detail shortly.",
        "Thank you for this information. I'll follow up as needed.",
        "I've received this and will take appropriate action."
    ])
]

# Days and times for meeting suggestions
DAYS = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday"]
TIMES = ["10:00 AM", "11:00 AM", "1:00 PM", "2:00 PM", "3:00 PM", "4:00 PM"]

def analyze_style(user_history):
    """
    Analyze the user's writing style from previous emails.
    
    Args:
        user_history (list): List of previous emails sent by the user
        
    Returns:
        str: 'formal' or 'informal'
    """
    # Simple analysis based on greeting and punctuation
    formal_indicators = 0
    informal_indicators = 0
    
    for email in user_history:
        # Check for formal greetings
        if re.search(r'Dear|Sincerely|Regards|Respectfully', email):
            formal_indicators += 1
        
        # Check for informal language
        if re.search(r'Hey|Hi there|Thanks!|Cheers|:)|!{2,}', email):
            informal_indicators += 1
        
        # Check for contractions (informal)
        if re.search(r"I'm|I'll|don't|can't|won't|you're", email):
            informal_indicators += 1
    
    return 'formal' if formal_indicators >= informal_indicators else 'informal'

def generate_reply(email_content, user_history):
    """
    Generate a reply based on email content and user's style.
    
    Args:
        email_content (str): The content of the email to reply to
        user_history (list): List of previous emails sent by the user
        
    Returns:
        str: Generated reply
    """
    # Analyze user's style
    style = analyze_style(user_history)
    
    # Select template based on style
    templates = FORMAL_TEMPLATES if style == 'formal' else INFORMAL_TEMPLATES
    template = random.choice(templates)
    
    # Find appropriate response pattern
    response = ""
    for pattern, responses in RESPONSE_PATTERNS:
        if re.search(pattern, email_content, re.IGNORECASE):
            response_template = random.choice(responses)
            
            # Fill in placeholders
            if '{day}' in response_template and '{time}' in response_template:
                day = random.choice(DAYS)
                time = random.choice(TIMES)
                response = response_template.format(day=day, time=time)
            elif '{custom_response}' in response_template:
                # For questions, we'd need more sophisticated NLP
                # For now, use a generic response
                response = response_template.format(custom_response="I'll need to look into this further.")
            else:
                response = response_template
            break
    
    # If no pattern matched, use default
    if not response:
        response = "I'll get back to you on this soon."
    
    # Format the final reply using the selected template
    reply = template.format(response=response)
    
    # Add signature based on style
    if style == 'formal':
        reply += "\n\nBest regards,"
    else:
        reply += "\n\nThanks,"
    
    return reply

def main():
    """Main function to process input and return generated reply."""
    # Read input as JSON
    input_data = json.loads(sys.stdin.read())
    
    email_content = input_data.get('email_content', '')
    user_history = input_data.get('user_history', [])
    
    # Generate reply
    reply = generate_reply(email_content, user_history)
    
    # Output the reply as JSON
    result = {"reply": reply}
    print(json.dumps(result))

if __name__ == "__main__":
    main()
