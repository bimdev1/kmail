# Changelog

## [0.1.0] - 2025-03-19

### Added
- Integrated AI features into KMail
  - Email categorization
  - Reply drafting
  - Follow-up reminders
  - Task extraction
  - Email summarization
- Created new AI-specific UI components
- Added Python backend for AI processing
- Implemented D-Bus communication with KOrganizer

### Changed
- Modified CMakeLists.txt for KF5 compatibility
  - Made Akonadi Search optional
  - Updated version requirements
  - Removed KF5StatusNotifierItem requirement
- Updated build system to support AI features
- Integrated with DeepSeek API for enhanced AI capabilities

### Dependencies
- Added Qt5 WebEngine support
- Added KF5 framework dependencies
- Added Akonadi development packages

### Fixed
- Resolved conflicts between KF5 and KF6 packages
- Fixed build system issues with missing dependencies
- Addressed package version conflicts

### Development
- Created comprehensive documentation
- Added detailed README files
- Implemented error handling for AI features
- Added configuration options for AI services

## [Unreleased] - 2025-03-19

### Changed
- Switched from KF6 to KF5 dependencies for better compatibility
- Updated build system configuration for KF5
- Made KF5StatusNotifierItem optional
- Resolved dependency conflicts between Akonadi versions

### Added
- Comprehensive build documentation in BUILD_LOG.md
- Development guide in DEVELOPMENT.md
- Project handoff documentation in HANDOFF.md
- Detailed AI feature documentation
- Build system troubleshooting guide

### Fixed
- Dependency conflicts between KF5 and KF6 packages
- Akonadi version mismatch issues
- Build system configuration for KDE PIM components

### Dependencies
- Removed conflicting akonadi-devel package
- Added KF5 Akonadi development packages
- Added KDE PIM framework dependencies
- Updated Qt5 dependencies

### Documentation
- Added detailed build logs
- Created comprehensive development guide
- Added project handoff documentation
- Updated installation instructions
- Added troubleshooting guides

### Planned
- Enhanced AI model integration
- Improved task extraction accuracy
- Better email categorization algorithms
- More sophisticated reply generation
