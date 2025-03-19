#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 KMail AI Integration Team
# SPDX-License-Identifier: GPL-2.0-only

"""
AI Email Summarizer

This script generates concise summaries of email content.
"""

import sys
import json
import re
from collections import Counter

def preprocess_email(email_text):
    """
    Preprocess email text by removing headers, signatures, and quoted replies.
    
    Args:
        email_text (str): The raw email text
        
    Returns:
        str: Preprocessed email text
    """
    # Remove email headers (if present)
    if re.search(r'^From:', email_text, re.MULTILINE):
        email_text = re.sub(r'^.*?(?=\n\n)', '', email_text, flags=re.DOTALL)
    
    # Remove email signatures
    signature_patterns = [
        r'--\s*\n.*',  # Standard signature separator
        r'Best regards,.*',
        r'Regards,.*',
        r'Sincerely,.*',
        r'Thanks,.*',
        r'Thank you,.*',
        r'Cheers,.*'
    ]
    
    for pattern in signature_patterns:
        email_text = re.sub(pattern, '', email_text, flags=re.DOTALL | re.IGNORECASE)
    
    # Remove quoted replies
    email_text = re.sub(r'On.*wrote:.*', '', email_text, flags=re.DOTALL)
    email_text = re.sub(r'>\s.*', '', email_text, flags=re.MULTILINE)
    
    # Remove extra whitespace
    email_text = re.sub(r'\s+', ' ', email_text).strip()
    
    return email_text

def extract_key_sentences(text, max_sentences=5):
    """
    Extract the most important sentences from the text.
    
    Args:
        text (str): The text to summarize
        max_sentences (int): Maximum number of sentences to include
        
    Returns:
        list: List of key sentences
    """
    # Split into sentences
    sentences = re.split(r'(?<=[.!?])\s+', text)
    
    if not sentences:
        return []
    
    # If there are few sentences, return all of them
    if len(sentences) <= max_sentences:
        return sentences
    
    # Extract words and calculate word frequencies
    words = re.findall(r'\w+', text.lower())
    word_freq = Counter(words)
    
    # Remove common stop words
    stop_words = {'the', 'a', 'an', 'and', 'or', 'but', 'is', 'are', 'was', 'were', 
                 'in', 'on', 'at', 'to', 'for', 'with', 'by', 'about', 'like', 
                 'from', 'of', 'as', 'this', 'that', 'these', 'those', 'it', 'its'}
    
    for word in stop_words:
        if word in word_freq:
            del word_freq[word]
    
    # Score sentences based on word frequency
    sentence_scores = []
    for sentence in sentences:
        score = 0
        words_in_sentence = re.findall(r'\w+', sentence.lower())
        for word in words_in_sentence:
            if word in word_freq:
                score += word_freq[word]
        
        # Normalize by sentence length (with a minimum to avoid division by zero)
        denominator = max(len(words_in_sentence), 1)
        score = score / denominator
        
        # Bonus for sentences at the beginning or end
        if sentences.index(sentence) < len(sentences) // 3:
            score *= 1.25
        elif sentences.index(sentence) > 2 * len(sentences) // 3:
            score *= 1.1
            
        sentence_scores.append((sentence, score))
    
    # Sort by score and take top sentences
    top_sentences = sorted(sentence_scores, key=lambda x: x[1], reverse=True)[:max_sentences]
    
    # Sort by original order
    ordered_sentences = sorted(top_sentences, key=lambda x: sentences.index(x[0]))
    
    return [sentence for sentence, _ in ordered_sentences]

def summarize_email(email_text, max_length=150):
    """
    Generate a concise summary of an email.
    
    Args:
        email_text (str): The content of the email to summarize
        max_length (int): Maximum length of the summary in words
        
    Returns:
        str: Email summary
    """
    # Preprocess the email
    processed_text = preprocess_email(email_text)
    
    # If the processed text is already short, return it
    if len(processed_text.split()) <= max_length:
        return processed_text
    
    # Extract key sentences
    key_sentences = extract_key_sentences(processed_text)
    
    # Join sentences into a summary
    summary = ' '.join(key_sentences)
    
    # Truncate if still too long
    words = summary.split()
    if len(words) > max_length:
        summary = ' '.join(words[:max_length]) + '...'
    
    return summary

def main():
    """Main function to process input and return email summary."""
    # Read email content from stdin
    email_text = sys.stdin.read()
    
    # Generate summary
    summary = summarize_email(email_text)
    
    # Output the summary as JSON
    result = {"summary": summary}
    print(json.dumps(result))

if __name__ == "__main__":
    main()
