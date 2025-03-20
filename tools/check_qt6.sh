#!/bin/bash

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directories to check
DIRS="src agents"

echo -e "${YELLOW}Checking for Qt6, KF6, and KPim6 references in the codebase...${NC}"

# Check for Qt6 references
echo -e "\n${YELLOW}Checking for Qt6 references...${NC}"
FOUND_QT6=$(grep -r "Qt6" --include="*.h" --include="*.cpp" --include="*.cmake" --include="CMakeLists.txt" $DIRS 2>/dev/null || true)

if [ -n "$FOUND_QT6" ]; then
    echo -e "${RED}Found Qt6 references:${NC}"
    echo "$FOUND_QT6"
else
    echo -e "${GREEN}No Qt6 references found.${NC}"
fi

# Check for KF6 references
echo -e "\n${YELLOW}Checking for KF6 references...${NC}"
FOUND_KF6=$(grep -r "KF6" --include="*.h" --include="*.cpp" --include="*.cmake" --include="CMakeLists.txt" $DIRS 2>/dev/null || true)

if [ -n "$FOUND_KF6" ]; then
    echo -e "${RED}Found KF6 references:${NC}"
    echo "$FOUND_KF6"
else
    echo -e "${GREEN}No KF6 references found.${NC}"
fi

# Check for KPim6 references
echo -e "\n${YELLOW}Checking for KPim6 references...${NC}"
FOUND_KPIM6=$(grep -r "KPim6" --include="*.h" --include="*.cpp" --include="*.cmake" --include="CMakeLists.txt" $DIRS 2>/dev/null || true)

if [ -n "$FOUND_KPIM6" ]; then
    echo -e "${RED}Found KPim6 references:${NC}"
    echo "$FOUND_KPIM6"
else
    echo -e "${GREEN}No KPim6 references found.${NC}"
fi

# Summary
echo -e "\n${YELLOW}Summary:${NC}"
if [ -z "$FOUND_QT6" ] && [ -z "$FOUND_KF6" ] && [ -z "$FOUND_KPIM6" ]; then
    echo -e "${GREEN}No Qt6/KF6/KPim6 references found in the codebase!${NC}"
else
    echo -e "${RED}Found Qt6/KF6/KPim6 references that need to be fixed.${NC}"
    exit 1
fi

exit 0 