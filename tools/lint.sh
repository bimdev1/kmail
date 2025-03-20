#!/bin/bash

# Exit on error
set -e

# Directories to check
DIRECTORIES="src agents"
PYTHON_DIRECTORIES="src/ai/python"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Number of CPU cores for parallel runs
CORES=$(nproc)

echo -e "${YELLOW}Running linters on KMail codebase...${NC}"

# Create directory for the lint results
mkdir -p build/lint-results

# Run clang-format check
echo -e "\n${YELLOW}Running clang-format...${NC}"
find $DIRECTORIES -name "*.cpp" -o -name "*.h" | xargs -P $CORES -I{} bash -c \
    "clang-format --style=file {} | diff -u {} - > build/lint-results/{}.format.diff 2>&1 || true"

# Check if there are any diff files with content
if [ -n "$(find build/lint-results -name "*.format.diff" -not -empty)" ]; then
    echo -e "${RED}❌ clang-format found formatting issues:${NC}"
    find build/lint-results -name "*.format.diff" -not -empty | xargs ls -la
    echo -e "Run 'find $DIRECTORIES -name \"*.cpp\" -o -name \"*.h\" | xargs clang-format -i' to fix formatting issues."
else
    echo -e "${GREEN}✓ clang-format check passed${NC}"
    find build/lint-results -name "*.format.diff" -delete
fi

# Run manual clang-tidy check without compile_commands.json if it doesn't exist
echo -e "\n${YELLOW}Running clang-tidy...${NC}"
if [ -f build/compile_commands.json ]; then
    # Run with compile_commands.json
    find $DIRECTORIES -name "*.cpp" | grep -v "autotests" | xargs -P $CORES -I{} bash -c \
        "clang-tidy -p build {} > build/lint-results/{}.tidy.log 2>&1 || true"
else
    echo -e "${YELLOW}Warning: compile_commands.json not found, performing basic syntax check only${NC}"
    # Just check for basic syntax errors without compile database
    find $DIRECTORIES -name "*.cpp" | grep -v "autotests" | xargs -P $CORES -I{} bash -c \
        "clang-tidy {} --checks='-*,clang-diagnostic-*' > build/lint-results/{}.tidy.log 2>&1 || true"
fi

# Check if there are any non-empty log files
if [ -n "$(find build/lint-results -name "*.tidy.log" -not -empty)" ]; then
    echo -e "${RED}❌ clang-tidy found issues:${NC}"
    find build/lint-results -name "*.tidy.log" -not -empty -exec cat {} \;
else
    echo -e "${GREEN}✓ clang-tidy check passed${NC}"
    find build/lint-results -name "*.tidy.log" -delete
fi

# Run cppcheck with basic settings if compile_commands.json doesn't exist
echo -e "\n${YELLOW}Running cppcheck...${NC}"
if [ -f build/compile_commands.json ]; then
    cppcheck --project=build/compile_commands.json \
        --file-filter="*/$DIRECTORIES/*" \
        $(cat .cppcheck) \
        --output-file=build/lint-results/cppcheck.log 2>&1 || true
else
    echo -e "${YELLOW}Warning: compile_commands.json not found, running cppcheck with basic settings${NC}"
    find $DIRECTORIES -name "*.cpp" -o -name "*.h" | grep -v "autotests" | \
    cppcheck --std=c++17 --language=c++ --enable=warning,performance \
        --suppress=missingIncludeSystem \
        --inline-suppr \
        --file-list=- \
        --output-file=build/lint-results/cppcheck.log 2>&1 || true
fi

if [ -s build/lint-results/cppcheck.log ]; then
    echo -e "${RED}❌ cppcheck found issues:${NC}"
    cat build/lint-results/cppcheck.log
else
    echo -e "${GREEN}✓ cppcheck check passed${NC}"
    rm -f build/lint-results/cppcheck.log
fi

# Run flake8 on Python code
if [ -n "$PYTHON_DIRECTORIES" ] && [ -d "$PYTHON_DIRECTORIES" ]; then
    echo -e "\n${YELLOW}Running flake8 on Python code...${NC}"
    flake8 $PYTHON_DIRECTORIES --config=.flake8 > build/lint-results/flake8.log 2>&1 || true
    
    if [ -s build/lint-results/flake8.log ]; then
        echo -e "${RED}❌ flake8 found issues:${NC}"
        cat build/lint-results/flake8.log
    else
        echo -e "${GREEN}✓ flake8 check passed${NC}"
        rm -f build/lint-results/flake8.log
    fi
else
    echo -e "${YELLOW}Warning: Python directories not found, skipping Python linting${NC}"
fi

# Check if there are any remaining issues
if [ -n "$(find build/lint-results -not -empty)" ]; then
    echo -e "\n${RED}❌ Linting found issues. Please fix them.${NC}"
    exit 1
else
    echo -e "\n${GREEN}✓ All linting checks passed!${NC}"
    rmdir build/lint-results
    exit 0
fi 