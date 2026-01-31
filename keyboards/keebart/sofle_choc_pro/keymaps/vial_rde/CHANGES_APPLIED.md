# LED Layout Changes - Summary

## ✅ Changes Applied

### 1. Brightness Reduced
- **From**: 50% (128/255)
- **To**: 30% (77/255)
- **File**: `config.h`

### 2. White Color Changed
- **From**: #FFFFFF (255, 255, 255)
- **To**: #EEEEEE (238, 238, 238)
- **File**: `keymap.c`

### 3. Edge Column Configuration Updated
- **Before**: White on ALL edge columns (left col 0 & 5, right col 0 & 5)
- **After**: White ONLY on:
  - Left half: Column 0 (first column)
  - Right half: Column 5 (last column)
- **Removed white from**:
  - Left half column 5 (6th column)
  - Right half column 0 (1st column)

### 4. Home Row Color Changed
- **From**: Yellow #FFEB3B (255, 235, 59)
- **To**: Green #61BB66 (97, 187, 102)
- **Keys affected**: A, S, D, F, J, K, L, ;

## Updated Color Scheme

### BASE Layer Colors:
- **Edge columns** (left col 0, right col 5): #EEEEEE (soft white)
- **Top row** (F2-F5, F7-F10): #999999 (grey)
- **Home row** (ASDF/JKL;): #61BB66 (green) ✨ NEW
- **O key**: #DA4444 (red)
- **NAV thumb**: #EE8833 (orange)
- **RAlt thumb**: #7777DD (blue/pink)
- **All other keys**: OFF (black)

### LOWER Layer:
- Navigation keys turn #EE8833 (orange)
- Numpad keys turn #EE8833 (orange)
- Other keys maintain BASE layer colors

## Compilation & Flashing

Ready to compile! Run in **QMK MSYS**:

```bash
cd /c/Dev/_github/vial-qmk-keebart
qmk compile -kb keebart/sofle_choc_pro -km vial_rde
```

Then flash the `.uf2` file to both keyboard halves following the **FLASH_GUIDE.md** instructions.

## What Changed in Code

### `config.h`
```c
// Before
#define RGB_MATRIX_DEFAULT_VAL 128  // Brightness 50/100

// After
#define RGB_MATRIX_DEFAULT_VAL 77   // Brightness 30/100
```

### `keymap.c`
```c
// Before
#define CLR_WHITE   255, 255, 255  // #ffffff
#define CLR_YELLOW  255, 235, 59   // #ffeb3b

// After
#define CLR_WHITE   238, 238, 238  // #eeeeee
#define CLR_GREEN   97, 187, 102   // #61bb66

// Edge column logic simplified - only leftmost and rightmost columns
// Home row now uses CLR_GREEN instead of CLR_YELLOW
```

All changes are complete and ready to test! 🎨
