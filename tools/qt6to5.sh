#!/bin/bash

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directories to process
DIRS="src agents"

echo -e "${YELLOW}Converting Qt6 references to Qt5 in the KMail project...${NC}"

# 1. Convert KF6 to KF5
echo -e "\n${YELLOW}Converting KF6 to KF5...${NC}"
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/KF6::/KF5::/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/find_package(KF6 /find_package(KF5 /g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/find_dependency(KF6/find_dependency(KF5/g' 2>/dev/null || true

# 2. Convert KPim6 to KPim5
echo -e "\n${YELLOW}Converting KPim6 to KPim5...${NC}"
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/KPim6::/KPim5::/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/find_package(KPim6/find_package(KPim5/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/find_dependency(KPim6/find_dependency(KPim5/g' 2>/dev/null || true

# 3. Convert from qt_add functions to qt5_add functions
echo -e "\n${YELLOW}Converting qt_add functions to qt5_add functions...${NC}"
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/qt_add_dbus_adaptor/qt5_add_dbus_adaptor/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/qt_add_dbus_interfaces/qt5_add_dbus_interfaces/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/qt_add_resources/qt5_add_resources/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/qt_wrap_ui/qt5_wrap_ui/g' 2>/dev/null || true

# 4. Convert Qt6 to Qt5
echo -e "\n${YELLOW}Converting Qt6 to Qt5...${NC}"
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/find_package(Qt6/find_package(Qt5/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/find_dependency(Qt6/find_dependency(Qt5/g' 2>/dev/null || true
find $DIRS -name "CMakeLists.txt" -o -name "*.cmake" | xargs sed -i 's/Qt6::/Qt5::/g' 2>/dev/null || true

# 5. Find remaining possible Qt6/KF6/KPim6 references
echo -e "\n${YELLOW}Checking for remaining Qt6/KF6/KPim6 references...${NC}"
REMAINING=$(grep -r "Qt6\|KF6\|KPim6" --include="CMakeLists.txt" --include="*.cmake" $DIRS 2>/dev/null || true)

if [ -n "$REMAINING" ]; then
    echo -e "${RED}Found remaining Qt6/KF6/KPim6 references:${NC}"
    echo "$REMAINING"
    echo -e "\n${YELLOW}These might need manual inspection${NC}"
else
    echo -e "${GREEN}No remaining Qt6/KF6/KPim6 references found!${NC}"
fi

echo -e "\n${GREEN}Conversion complete!${NC}" 