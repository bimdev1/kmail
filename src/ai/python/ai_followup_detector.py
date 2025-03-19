#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 KMail AI Integration Team
# SPDX-License-Identifier: GPL-2.0-only

"""
AI Follow-Up Detector

This script analyzes email content to determine if it needs a follow-up
and suggests an appropriate follow-up date.
"""

import sys
import json
import re
import datetime

# Patterns that indicate a follow-up is needed
FOLLOWUP_PATTERNS = [
    # Direct requests for response
    r'(?:please respond|get back to me|let me know|waiting for your response|awaiting your reply)',
    
    # Questions requiring answers
    r'\?(?:[^?]*\?){2,}',  # Multiple questions
    r'(?:what|when|where|why|how|who|which)(?:[^?]*)\?',  # WH-questions
    
    # Time-sensitive language
    r'(?:urgent|asap|as soon as possible|immediately|promptly|quickly)',
    
    # Deadlines
    r'(?:by|before|due|deadline|not later than)\s+(?:monday|tuesday|wednesday|thursday|friday|saturday|sunday|\d{1,2}(?:st|nd|rd|th)?(?:\s+of)?(?:\s+\w+)?)',
    
    # Action requests
    r'(?:need you to|would like you to|please|kindly|request you to)\s+(?:\w+)',
    
    # Meeting requests
    r'(?:schedule|set up|arrange|plan)\s+(?:a|the)?\s*(?:meeting|call|discussion|conversation)',
    
    # Follow-up indicators
    r'(?:follow up|follow-up|followup|check back|touch base)'
]

# Timeframes for follow-up suggestions
FOLLOWUP_TIMEFRAMES = [
    # Pattern, days to add for follow-up
    (r'(?:urgent|asap|as soon as possible|immediately|promptly|quickly)', 1),
    (r'(?:tomorrow)', 1),
    (r'(?:this week|by the end of the week)', 3),
    (r'(?:next week)', 5),
    (r'(?:monday|mon)', lambda: days_until('monday')),
    (r'(?:tuesday|tue)', lambda: days_until('tuesday')),
    (r'(?:wednesday|wed)', lambda: days_until('wednesday')),
    (r'(?:thursday|thu)', lambda: days_until('thursday')),
    (r'(?:friday|fri)', lambda: days_until('friday')),
]

def days_until(weekday):
    """Calculate days until the next occurrence of a weekday."""
    weekdays = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday']
    target_day = weekdays.index(weekday.lower())
    today = datetime.datetime.now().weekday()  # 0 = Monday, 6 = Sunday
    
    # Calculate days until next occurrence
    days = (target_day - today) % 7
    if days == 0:  # If today is the target day, go to next week
        days = 7
    
    return days

def needs_followup(email_text):
    """
    Determine if an email needs a follow-up.
    
    Args:
        email_text (str): The content of the email to analyze
        
    Returns:
        bool: True if follow-up is needed, False otherwise
    """
    for pattern in FOLLOWUP_PATTERNS:
        if re.search(pattern, email_text, re.IGNORECASE):
            return True
    
    return False

def suggest_followup_date(email_text):
    """
    Suggest a follow-up date based on email content.
    
    Args:
        email_text (str): The content of the email to analyze
        
    Returns:
        str: Suggested follow-up date in YYYY-MM-DD format
    """
    today = datetime.datetime.now()
    days_to_add = 2  # Default: 2 days
    
    # Check for specific timeframes
    for pattern, timeframe in FOLLOWUP_TIMEFRAMES:
        if re.search(pattern, email_text, re.IGNORECASE):
            if callable(timeframe):
                days_to_add = timeframe()
            else:
                days_to_add = timeframe
            break
    
    # Calculate follow-up date
    followup_date = today + datetime.timedelta(days=days_to_add)
    
    # Format as YYYY-MM-DD
    return followup_date.strftime('%Y-%m-%d')

def main():
    """Main function to process input and return follow-up information."""
    # Read email content from stdin
    email_text = sys.stdin.read()
    
    # Check if follow-up is needed
    followup_needed = needs_followup(email_text)
    
    # Suggest follow-up date if needed
    followup_date = suggest_followup_date(email_text) if followup_needed else ""
    
    # Output the result as JSON
    result = {
        "needs_followup": followup_needed,
        "followup_date": followup_date
    }
    print(json.dumps(result))

if __name__ == "__main__":
    main()
