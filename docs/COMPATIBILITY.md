# KMail Qt5/KF5 Compatibility Work

This document explains what we did to make KMail work with Qt5 and KDE Frameworks 5 instead of requiring the newer Qt6/KF6.

## What We Did

We built a compatibility layer that includes:

- Simple header files that make older implementations work with newer APIs
- Helper code that handles differences between Qt5 and Qt6 collection classes
- Namespace fixes for renamed or moved classes
- Fixes for how string literals are handled in Qt5 vs Qt6

## Compatibility Headers

We created these compatibility headers in the src/compat/ directory:

For collection types that changed between versions:
- qvector_compat.h - Makes QVector work the same way in both Qt5 and Qt6

For KDE PIM components:
- kidentitymanagementcore.h - Handles the KIdentityManagement namespace changes
- kldapcore.h - Maps the newer LDAP interfaces to the older ones
- kmimecore.h - Handles MIME processing differences
- textcustomeditor.h - Provides a simple replacement for text editing components
- transportmanagementwidgetng.h - Makes the mail transport widget work with older code

For utility functions:
- klocalization.h - Adds helper functions for localization that weren't in Qt5
- ldapclientsearch.h - Makes LDAP searches work the same way

## String Handling

Qt6 introduced a new way to write string literals using _L1 for Latin1 strings. For Qt5 compatibility we:

- Changed "string"_L1 to QLatin1String("string")
- Changed "string"_s to u"string"_s in some places
- Used Qt::Literals::StringLiterals namespace where it made sense

## Collection Classes

Qt6 replaced QVector with QList, so we added this simple adapter:

```cpp
// Works with both Qt5 and Qt6
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
template <typename T>
using QVector_compat = QVector<T>;
#else
template <typename T>
using QVector_compat = QList<T>;
#endif
```

This way, code using QVector_compat works in both versions without changes.

## UI Files

For UI files that referenced newer component classes, we:

1. Found which UI files were using newer components
2. Created simple compatibility headers to provide those components
3. Updated the generated UI files to include our compatibility headers

## Namespace Changes

KDE PIM libraries reorganized their namespaces between versions. We created compatibility to handle:

- KIdentityManagementCore → KIdentityManagement
- KLDAPCore → KLDAP
- KMimeCore → KMime

## Plugin System

We updated the plugin loading system to:

- Work with KPluginMetaData using the KF5 API
- Fix how plugin factories are instantiated

## Build System Changes

We modified CMakeLists.txt files to:

- Find and use KF5 modules instead of KF6
- Handle optional dependencies better for Qt5
- Use the Qt5 versions of DBus tools instead of Qt6 versions

