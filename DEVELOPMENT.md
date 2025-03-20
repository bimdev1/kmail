# KMail AI Development Guide

This document outlines the development process, challenges encountered, and solutions implemented while adding AI features to KMail.

## Getting Started

### Initial Setup
1. Clone the repository:
```bash
git clone https://invent.kde.org/pim/kmail.git
cd kmail
```

2. Create a new branch for AI features:
```bash
git checkout -b feature/ai-integration
```

### Build Environment Setup

#### Dependencies Resolution Process
We encountered several dependency challenges. Here's how to resolve them:

1. First, install Qt5 WebEngine:
```bash
sudo dnf install -y qt5-qtwebengine-devel
```

2. Install KDE Framework 5 dependencies:
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

3. Install Akonadi:
```bash
sudo dnf install -y akonadi-devel
```

## Common Build Issues and Solutions

### 1. KF5/KF6 Conflicts
**Problem**: Conflicts between KF5 and KF6 packages
**Solution**: 
- Modified CMakeLists.txt to use KF5 exclusively
- Removed KF6 dependencies
- Updated version requirements
- Created helper scripts in the tools directory for converting KF6 references to KF5
- Replaced Qt::Literals::StringLiterals with QLatin1String throughout the codebase

### 2. Akonadi Search Dependency
**Problem**: Required Akonadi Search package not available
**Solution**:
- Made Akonadi Search optional in CMakeLists.txt
- Added conditional compilation for search features
```cmake
option(FORCE_DISABLE_AKONADI_SEARCH "Disable features and API that require akonadi-search" ON)
```

### 3. Status Notifier Integration
**Problem**: KF5StatusNotifierItem not available
**Solution**:
- Made it an optional dependency
- Added graceful fallback when not available

### 4. Missing Configuration Headers
**Problem**: Missing configuration headers like config-kmail.h and kmail-version.h
**Solution**:
- Added proper CMake configuration for header generation
- Created standard templates for these headers
- Made sure headers are properly included in the build system

### 5. Qt5/Qt6 String Compatibility
**Problem**: Qt6 string literals (_L1) not compatible with Qt5
**Solution**:
- Replaced all occurrences of _L1 with QLatin1String
- Removed usage of Qt::Literals::StringLiterals namespace
- Updated signal/slot connections to use Qt5 syntax
- Converted DBus adaptor generation to use qt5_add_dbus_adaptor

## Architecture Design

### AI Service Integration
1. **Local AI Service**
   - Handles communication with AI models
   - Manages API keys and rate limiting
   - Implements caching for performance

2. **UI Integration**
   - Extension of KMMainWidget
   - Non-intrusive AI feature buttons
   - Modal dialogs for AI interactions

3. **Data Flow**
```
Email → AIMainWidgetExtension → LocalAIService → DeepSeek API
                                      ↓
                              Cache & Local Storage
```

### Key Components

1. **AIMainWidgetExtension**
   - Entry point for AI features
   - Manages UI integration
   - Handles user interactions

2. **LocalAIService**
   - AI processing logic
   - API communication
   - Result caching

3. **AI Dialogs**
   - Task extraction
   - Reply generation
   - Email summarization

## Testing Strategy

1. **Unit Tests**
   - AI service mocking
   - Response parsing
   - Cache validation

2. **Integration Tests**
   - Email processing workflow
   - UI interaction tests
   - API communication tests

## Performance Considerations

1. **Caching**
   - Cache AI responses
   - Store frequently used templates
   - Maintain user preferences

2. **Async Processing**
   - Non-blocking UI operations
   - Background API calls
   - Progress indicators

## Error Handling

1. **API Errors**
   - Rate limiting handling
   - Retry mechanism
   - User feedback

2. **Network Issues**
   - Offline mode support
   - Request queuing
   - Auto-retry logic

## Future Development

### Planned Features
1. Enhanced AI Models
   - Better context understanding
   - Multi-language support
   - Custom model training

2. UI Improvements
   - Customizable AI features
   - Better integration with KMail
   - More user control

3. Performance Optimizations
   - Improved caching
   - Better resource usage
   - Faster response times

## Contributing

### Development Workflow
1. Create feature branch from the main branch
2. Implement changes following KDE coding style
3. Format your code using the provided formatting tools:
   ```bash
   # Run from project root
   cmake --build build --target format-fix
   ```
4. Add unit tests for new functionality
5. Update documentation for API changes
6. Submit pull request with a clear description of changes

### Code Style
- Follow KDE coding style
- Use consistent naming conventions
- Format code with clang-format using the project's style file
- Add comprehensive comments for complex logic
- Respect existing architectural patterns

### Commit Messages
- Use clear, descriptive commit messages
- Follow the format: `[component]: Brief description`
- Include detailed explanation in the commit body when needed
- Reference issues/tickets when applicable

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Check KDE Framework versions
   - Verify Qt dependencies
   - Review CMake output
   - Ensure all symlinks to generated header files are correct

2. **Runtime Errors**
   - Check API connectivity
   - Verify cache permissions
   - Review log files
   - Check for Qt5/Qt6 compatibility issues in plugins

### Debug Tips
1. Enable debug logging
2. Use Qt Creator for debugging
3. Monitor API responses
4. Use the compile_commands.json file with clangd for better IDE integration

## Resources

### Documentation
- [KDE Developer Guide](https://develop.kde.org)
- [Qt Documentation](https://doc.qt.io)
- [DeepSeek API Docs](https://deepseek.ai/docs)

### Tools
- CMake 3.16+
- Qt Creator
- KDevelop
- Git
- clang-format 19+

## Version Control

### Branch Strategy
- main: stable releases
- develop: development branch
- feature/*: new features
- fix/*: bug fixes

### Commit Guidelines
- Use conventional commits
- Include detailed descriptions
- Reference issues/tickets

## Code Quality Tools

KMail uses several code quality tools to maintain high standards in the codebase:

### Linting and Formatting

#### Code Style and Formatting
The project follows KDE coding style guidelines with clang-format. Configuration is provided in `.clang-format`.

To manually format your code:
```bash
# Run CMake to generate format target
mkdir -p build && cd build
cmake ..

# Format all code
make format-fix

# Check format issues without fixing
make format
```

#### Static Analysis

Several static analyzers are configured:

1. **Clang-Tidy**: C++ static analyzer with modern checks
   - Configuration: `.clang-tidy`
   - Run: `clang-tidy -p build src/file.cpp`

2. **Cppcheck**: Comprehensive C++ static analyzer
   - Configuration: `.cppcheck`
   - Run: `cppcheck --project=build/compile_commands.json`

3. **Flake8**: Python code linter for AI Python components
   - Configuration: `.flake8`
   - Run: `flake8 src/ai/python`

#### Integrated Linting Tool

A comprehensive linting script is available:

```bash
# Make sure the script is executable
chmod +x tools/lint.sh

# Run all linters
./tools/lint.sh
```

#### CI Integration

The project includes GitLab CI configuration `.gitlab-ci-lint.yml` to run linters automatically on every commit.

All these tools help maintain code quality and consistency across the codebase.
