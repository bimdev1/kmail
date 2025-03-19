# KMail AI Module

This module adds AI-powered features to KMail, enhancing the email experience with smart categorization, reply drafting, task extraction, and follow-up reminders.

## Features

### AI Inbox Categorization
Automatically sorts incoming emails into priority levels:
- Urgent
- Follow-Up
- Low Priority
- Uncategorized

### AI Reply Drafting
Suggests responses in the user's writing style, analyzing previous emails to match tone and formality.

### AI Follow-Up Reminders
Notifies when an email needs a response and suggests appropriate follow-up dates.

### Task Extraction from Emails
Detects and extracts actionable items from emails, allowing them to be added to KOrganizer.

### Seamless UI Integration
Adds AI-powered buttons for generating replies, summarizing emails, categorizing emails, and more.

## Architecture

The AI module consists of:

1. **C++ Interface Layer** - Integrates with KMail's UI and provides access to AI services
   - `aiserviceinterface.h` - Base interface for AI services
   - `localaiservice.cpp/h` - Implementation using local Python scripts
   - `aimanager.cpp/h` - Manages AI services and configuration
   - `aiconfigwidget.cpp/h` - UI for configuring AI features

2. **UI Components** - Widgets for displaying AI-generated content
   - `aicategoryindicatorwidget.cpp/h` - Displays email categories
   - `aireplydraftwidget.cpp/h` - Displays and manages AI-generated replies

3. **Python Backend** - Provides the actual AI functionality
   - `ai_email_categorizer.py` - Categorizes emails
   - `ai_reply_generator.py` - Generates email replies
   - `ai_task_extractor.py` - Extracts tasks from emails
   - `ai_followup_detector.py` - Detects if emails need follow-up
   - `ai_email_summarizer.py` - Summarizes email content

## Configuration

AI features can be enabled or disabled in the KMail settings dialog. Users can also configure:
- Which AI features to enable
- How aggressively to categorize emails
- Whether to automatically suggest replies
- How to handle extracted tasks

## Dependencies

The Python scripts require:
- Python 3.6+
- Standard Python libraries (re, json, datetime, etc.)

## Future Enhancements

Planned enhancements include:
- Integration with more advanced AI models
- Cloud-based AI services option
- Learning from user corrections
- More sophisticated email analysis
