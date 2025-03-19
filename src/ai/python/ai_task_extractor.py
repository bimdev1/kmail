#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 KMail AI Integration Team
# SPDX-License-Identifier: GPL-2.0-only

"""
AI Task Extractor

This script analyzes email content and extracts potential tasks or action items.
"""

import sys
import json
import re

# Regular expressions for identifying tasks
TASK_PATTERNS = [
    # Direct requests
    r'(?:please|kindly|pls)\s+([^.!?]+)[.!?]',
    
    # Action items with "need to" or "have to"
    r'(?:need to|have to|must)\s+([^.!?]+)[.!?]',
    
    # Deadlines
    r'(?:by|before|due|deadline)\s+([^.!?]+)[.!?]',
    
    # Assignments
    r'(?:assigned to you|your responsibility|you are responsible for)\s+([^.!?]+)[.!?]',
    
    # Questions requiring action
    r'(?:can you|could you|would you)\s+([^.!?]+)[?]',
    
    # Follow-up requests
    r'(?:follow up on|follow-up on|followup on)\s+([^.!?]+)[.!?]',
    
    # Review requests
    r'(?:review|look at|check)\s+([^.!?]+)[.!?]',
    
    # Meeting scheduling
    r'(?:schedule|set up|arrange)\s+(?:a|the)?\s*(?:meeting|call|conference|discussion)\s+([^.!?]+)[.!?]'
]

# Keywords that indicate a task
TASK_KEYWORDS = [
    'send', 'create', 'prepare', 'develop', 'implement', 'finish', 'complete',
    'review', 'update', 'check', 'verify', 'confirm', 'schedule', 'organize',
    'call', 'email', 'contact', 'follow up', 'followup', 'follow-up', 'submit',
    'share', 'distribute', 'upload', 'download', 'install', 'configure', 'setup'
]

def extract_tasks(email_text):
    """
    Extract tasks from email content.
    
    Args:
        email_text (str): The content of the email to analyze
        
    Returns:
        list: List of extracted tasks
    """
    tasks = []
    
    # Apply regex patterns to find potential tasks
    for pattern in TASK_PATTERNS:
        matches = re.finditer(pattern, email_text, re.IGNORECASE)
        for match in matches:
            task = match.group(1).strip()
            if task and len(task) > 5:  # Avoid very short matches
                tasks.append(task)
    
    # Look for sentences containing task keywords
    sentences = re.split(r'[.!?]', email_text)
    for sentence in sentences:
        sentence = sentence.strip()
        if not sentence:
            continue
            
        for keyword in TASK_KEYWORDS:
            if re.search(r'\b' + re.escape(keyword) + r'\b', sentence, re.IGNORECASE):
                # Check if this sentence is already captured
                if not any(sentence in task for task in tasks):
                    tasks.append(sentence)
                break
    
    # Remove duplicates and similar tasks
    unique_tasks = []
    for task in tasks:
        # Check if this task is a subset of an existing task
        if not any(task in existing_task and task != existing_task for existing_task in unique_tasks):
            # Check if an existing task is a subset of this task
            subset_indices = [i for i, existing_task in enumerate(unique_tasks) 
                             if existing_task in task and existing_task != task]
            
            # Replace subsets with this task
            for i in reversed(subset_indices):
                unique_tasks.pop(i)
                
            unique_tasks.append(task)
    
    return unique_tasks

def main():
    """Main function to process input and return extracted tasks."""
    # Read email content from stdin
    email_text = sys.stdin.read()
    
    # Extract tasks
    tasks = extract_tasks(email_text)
    
    # Output the tasks as JSON
    result = {"tasks": tasks}
    print(json.dumps(result))

if __name__ == "__main__":
    main()
