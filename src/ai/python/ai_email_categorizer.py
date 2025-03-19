#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 KMail AI Integration Team
# SPDX-License-Identifier: GPL-2.0-only

"""
AI Email Categorizer

This script analyzes email content and categorizes it based on urgency and context.
It uses a simple NLP approach to determine if an email is urgent, requires follow-up,
or is low priority.
"""

import sys
import json
import re
from collections import Counter

# Define keywords for each category
URGENT_KEYWORDS = [
    'urgent', 'immediately', 'asap', 'emergency', 'critical', 'important',
    'deadline', 'today', 'now', 'priority', 'urgent matter', 'time-sensitive'
]

FOLLOW_UP_KEYWORDS = [
    'follow up', 'followup', 'follow-up', 'get back to', 'respond', 'reply',
    'waiting for', 'waiting on', 'pending', 'action required', 'please advise',
    'let me know', 'update me', 'confirm', 'confirmation'
]

LOW_PRIORITY_KEYWORDS = [
    'fyi', 'for your information', 'newsletter', 'update', 'no rush',
    'whenever you have time', 'not urgent', 'low priority', 'just letting you know',
    'no action required', 'no response needed'
]

def count_keyword_matches(text, keywords):
    """Count how many keywords from the list appear in the text."""
    text = text.lower()
    count = 0
    for keyword in keywords:
        if keyword.lower() in text:
            count += 1
    return count

def categorize_email(email_text):
    """
    Categorize an email based on its content.
    
    Args:
        email_text (str): The content of the email to categorize
        
    Returns:
        int: Category code (0 = Uncategorized, 1 = Urgent, 2 = Follow-Up, 3 = Low Priority)
    """
    # Count keyword matches for each category
    urgent_count = count_keyword_matches(email_text, URGENT_KEYWORDS)
    follow_up_count = count_keyword_matches(email_text, FOLLOW_UP_KEYWORDS)
    low_priority_count = count_keyword_matches(email_text, LOW_PRIORITY_KEYWORDS)
    
    # Simple decision logic
    if urgent_count > follow_up_count and urgent_count > low_priority_count:
        return 1  # Urgent
    elif follow_up_count > urgent_count and follow_up_count > low_priority_count:
        return 2  # Follow-Up
    elif low_priority_count > urgent_count and low_priority_count > follow_up_count:
        return 3  # Low Priority
    else:
        # Check if there's a tie between urgent and follow-up
        if urgent_count > 0 and urgent_count == follow_up_count:
            return 1  # Prioritize urgent
        # Default case
        return 0  # Uncategorized

def main():
    """Main function to process input and return categorization."""
    # Read email content from stdin
    email_text = sys.stdin.read()
    
    # Categorize the email
    category = categorize_email(email_text)
    
    # Output the category as JSON
    result = {"category": category}
    print(json.dumps(result))

if __name__ == "__main__":
    main()
