# Qt Creator Build Guide

## ✅ What's Installed

- **Qt 6.9.3** - Already installed via Homebrew ✓
- **Qt Creator** - Just installed ✓

## 🚀 Quick Start: Build with Qt Creator

### Step 1: Open Qt Creator
```bash
# Open from terminal
open -a "Qt Creator"

# Or find it in Applications folder
```

### Step 2: Open the Project
1. In Qt Creator: **File → Open File or Project**
2. Navigate to: `/Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/CMakeLists.txt`
3. Click **Open**

### Step 3: Configure Project
1. Qt Creator will ask you to configure the project
2. Select a **Kit** (should auto-detect Qt 6.9.3)
3. Click **Configure Project**

### Step 4: Build
1. Click **Build → Build Project** (or press `Cmd+B`)
2. Wait for compilation to complete
3. Check the **Compile Output** panel for any errors

### Step 5: Run Tests
After building, run:
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

## 📋 What Gets Built

After successful build, you'll have:
- `build/gridlock_ui` - Main GUI application
- `build/test_*` - All test executables
- `build/test_route_planner_googletest` - GoogleTest suite
- `build/test_agent_googletest` - Agent tests
- `build/test_metrics_googletest` - Metrics tests
- `build/test_simulation_controller_googletest` - Controller tests
- `build/test_factory_pattern_googletest` - Factory tests

## 🎯 Alternative: Build from Qt Creator Terminal

1. In Qt Creator, go to **Tools → Terminal**
2. Navigate to project:
   ```bash
   cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
   ```
3. Run:
   ```bash
   cmake ..
   make -j4
   ```

## ⚠️ Troubleshooting

### If Qt Creator doesn't find Qt:
1. **Tools → Options → Kits**
2. Click **Add** to add a new kit
3. Set Qt version to: `/opt/homebrew/lib/qt6`
4. Set compiler to: **Apple Clang**

### If build fails:
- Check **Compile Output** panel for errors
- Make sure Qt 6.9.3 is selected in the kit
- Try cleaning: **Build → Clean All**, then rebuild

## ✅ Verification

After building, verify:
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
ls -la build/test_* | head -5
```

You should see test executables like:
- `test_city`
- `test_grid_view`
- `test_main_window`
- `test_metrics_panel`
- etc.

## 🎉 Next Steps

1. **Build the project** using Qt Creator (steps above)
2. **Run all tests**: `./run_all_tests.sh`
3. **Run the GUI**: `./build/gridlock_ui`
4. **Generate coverage**: `./run_coverage.sh` (after tests pass)

---

**You're all set!** Qt and Qt Creator are installed. Just open the project in Qt Creator and build! 🚀

