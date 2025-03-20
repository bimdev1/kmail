# KMail with AI Features

This is an enhanced version of KMail that integrates AI capabilities for improved email management. The project adds intelligent features while maintaining the robust functionality of the original KMail client.

## New AI Features

- **AI Inbox Categorization**: Automatically categorizes incoming emails using AI
- **AI Reply Drafting**: Generates intelligent reply suggestions
- **AI Follow-Up Reminders**: Smart detection of emails requiring follow-up
- **Task Extraction**: Automatically identifies and extracts tasks from emails
- **Email Summarization**: Provides AI-generated summaries of long emails

## System Requirements

- Fedora 41 or compatible Linux distribution
- KDE Frameworks 5 (KF5) dependencies
- Qt 5.15.2 or higher
- CMake 3.16 or higher
- C++17 compatible compiler

## Dependencies

The following packages are required:

### Qt Dependencies
```bash
sudo dnf install -y qt5-qtwebengine-devel
```

### KDE Framework Dependencies
```bash
sudo dnf install -y \
    kf5-kcrash-devel \
    kf5-kdbusaddons-devel \
    kf5-kguiaddons-devel \
    kf5-kiconthemes-devel \
    kf5-knotifications-devel \
    kf5-knotifyconfig-devel \
    kf5-kcontacts-devel \
    kf5-kcalendarcore-devel \
    kf5-kdoctools-devel
```

### Akonadi Dependencies
```bash
sudo dnf install -y akonadi-devel
```

## Building from Source

1. Clone the repository:
```bash
git clone https://invent.kde.org/pim/kmail.git
cd kmail
```

2. Create and enter the build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..
```

4. Build the project:
```bash
make -j$(nproc)
```

5. Install (optional):
```bash
sudo make install
```

## Project Structure

- `/src/ai/`: Contains all AI-related components
  - `aimainwidgetextension.cpp/.h`: Main AI feature integration
  - `kmmainwidgetaifilter.cpp/.h`: AI-based email filtering
  - `aireplydialog.cpp/.h`: AI reply generation dialog
  - `aifollowupdialog.cpp/.h`: Follow-up reminder dialog
  - `aitaskdialog.cpp/.h`: Task extraction dialog
  - `aisummarydialog.cpp/.h`: Email summarization dialog
  - `korganizerintegration.cpp/.h`: KOrganizer integration
  - `localaiservice.cpp/.h`: Local AI service implementation

## Configuration

The AI features can be configured through:
1. KMail Settings → AI Features
2. Individual dialog settings for each AI feature
3. System-wide KDE PIM settings

## Development Notes

### Build System Changes
- Modified CMakeLists.txt to support KF5 dependencies
- Made Akonadi Search optional to resolve dependency conflicts
- Updated version requirements for KDE PIM components

### Known Issues
- Status Notifier Item integration is currently optional
- Some KF6 dependencies may conflict with KF5 requirements

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

This project is licensed under the GPL v2 or later. See the LICENSE file for details.

## Acknowledgments

- KDE PIM Team
- Codeium AI Team
- All contributors to the original KMail project
