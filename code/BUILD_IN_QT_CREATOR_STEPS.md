# Step-by-Step: Build in Qt Creator

## You've opened Qt Creator - Now Build!

Since you've already opened Qt Creator, follow these steps:

### Step 1: Open the Project (if not already open)

1. In Qt Creator: **File → Open File or Project**
2. Navigate to: `/Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/CMakeLists.txt`
3. Click **Open**

### Step 2: Configure Project

1. Qt Creator will show a "Configure Project" screen
2. **Select a Kit:**
   - Look for a kit with Qt 6.9.3
   - If you see multiple kits, choose one with Qt 6.9.3
   - If no kit appears, click "Manage Kits" and add one
3. Click **Configure Project** button (bottom right)

### Step 3: Build the Project

**Option A: Using Menu**
1. Click **Build → Build Project** (or press `Cmd+B` on Mac)
2. Wait for compilation to complete
3. Check the **Compile Output** panel at the bottom for progress

**Option B: Using Build Button**
1. Look for the green hammer icon (🔨) in the left sidebar
2. Click it to build
3. Or use the build button in the bottom toolbar

### Step 4: Check Build Success

1. Look at the **Compile Output** panel
2. You should see: `Build succeeded` or similar
3. If there are errors, they'll be shown in red

### Step 5: Run Tests

After successful build, go back to terminal:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ls -la test_*
```

You should now see test executables including:
- `test_creational_patterns`
- `test_city`
- `test_grid_view`
- etc.

Then run:
```bash
./test_creational_patterns
```

## Visual Guide

### Qt Creator Layout

```
┌─────────────────────────────────────────┐
│  File  Edit  Build  Debug  ...         │
├──────────┬──────────────────────────────┤
│          │                              │
│ Projects │   Main Editor Area           │
│ (Sidebar)│   (Shows CMakeLists.txt)     │
│          │                              │
│  🔨 Build│                              │
│  ▶ Run   │                              │
│  🐛 Debug│                              │
│          │                              │
│          │                              │
├──────────┴──────────────────────────────┤
│ Compile Output                          │
│ [Shows build progress/errors]           │
└─────────────────────────────────────────┘
```

### What to Look For

1. **Left Sidebar**: Should show your project structure
2. **Bottom Panel**: "Compile Output" tab shows build progress
3. **Build Button**: Green hammer icon or "Build" menu item

## Troubleshooting

### "No kit found" or "No suitable kit"
- Click "Manage Kits" (bottom of configure screen)
- Add a kit with Qt 6.9.3
- Qt should be at: `/opt/homebrew/lib/qt6`

### Build fails with errors
- Check the **Compile Output** panel
- Common issues:
  - Missing Qt modules → Check CMakeLists.txt includes
  - Missing files → Check all pattern files exist
  - CMake errors → Try "Build → Clean All" then rebuild

### Build succeeds but no test executables
- Check build directory location
- Qt Creator might build to a different directory
- Look in: `code/build/` or check Qt Creator's build settings

## Quick Checklist

- [ ] Project opened in Qt Creator
- [ ] Kit selected (Qt 6.9.3)
- [ ] Project configured
- [ ] Build started (Cmd+B or Build menu)
- [ ] Build completed successfully
- [ ] Test executables exist in `build/` directory
- [ ] Tests run successfully

## After Building

Once built, you can:
- Run individual tests: `./test_creational_patterns`
- Run all tests: `cd code && ./run_all_tests.sh`
- Run the app: `./gridlock_ui`

Good luck! 🚀

