# Qt Installation Guide

## Check if Qt is Already Installed

### Quick Check
```bash
qmake6 --version
# OR
qmake --version
```

If you see a version number, Qt is already installed!

### Check Homebrew Installation
```bash
brew list qt@6
# OR
brew list qt
```

## Do You Need to Install Qt?

**Yes, if:**
- You want to build the C++ tests
- You want to run the GUI application
- You want to use Qt Creator

**No, if:**
- You only want to run Python tests (they don't need Qt)
- You're just verifying core logic

## Installation Options

### Option 1: Install Qt via Homebrew (Recommended for macOS)

```bash
# Install Qt6
brew install qt@6

# Add to PATH (add to ~/.zshrc or ~/.bash_profile)
echo 'export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# Verify installation
qmake6 --version
```

### Option 2: Install Qt Creator (Includes Qt)

1. **Download Qt Creator:**
   - Visit: https://www.qt.io/download
   - Download Qt Creator for macOS
   - Install the .dmg file

2. **Qt Creator includes:**
   - Qt Creator IDE
   - Qt libraries
   - CMake support
   - Everything you need!

### Option 3: Install Qt via Qt Online Installer

1. Visit: https://www.qt.io/download
2. Download Qt Online Installer
3. Run installer
4. Select:
   - Qt 6.9.x (or latest)
   - Qt Creator
   - CMake
   - macOS components

## After Installation

### Verify Qt Installation
```bash
qmake6 --version
# Should show: QMake version 3.1, Using Qt version 6.x.x
```

### Build the Project
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
cmake ..
make
```

## What You Can Do Without Qt

✅ **Python tests** - Work without Qt
✅ **Core logic verification** - Python scripts test everything
✅ **Code review** - All code is there and documented

## What Requires Qt

❌ **C++ unit tests** - Need Qt for UI tests
❌ **GUI application** - Needs Qt Widgets
❌ **Qt Creator** - Needs Qt installed

## Recommendation

**If you just need to verify tests work:**
- Use Python tests: `./run_python_tests.sh`
- No Qt installation needed!

**If you need to build and run everything:**
- Install Qt via Homebrew: `brew install qt@6`
- Or install Qt Creator (includes Qt)

## Quick Install Command

```bash
# Install Qt6 via Homebrew (one command)
brew install qt@6
```

Then add to your shell profile:
```bash
echo 'export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

