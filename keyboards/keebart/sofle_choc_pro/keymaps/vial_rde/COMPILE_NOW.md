# Compile Instructions

## The RGB_MATRIX_MAXIMUM_BRIGHTNESS error has been fixed!

The redefined constant has been removed from `config.h`.

## To compile using QMK MSYS:

1. **Open QMK MSYS** (search for "QMK MSYS" in Start menu)

2. **Navigate to the repository**:
   ```bash
   cd /c/Dev/_github/vial-qmk-keebart
   ```

3. **Compile the firmware**:
   ```bash
   qmk compile -kb keebart/sofle_choc_pro -km vial_rde
   ```

4. **Find the .uf2 file**:
   ```bash
   ls -lh .build/keebart_sofle_choc_pro_vial_rde.uf2
   ```

The file will be at: `.build\keebart_sofle_choc_pro_vial_rde.uf2`

## What was fixed:

- ❌ Before: `RGB_MATRIX_MAXIMUM_BRIGHTNESS` was defined in both main config.h and keymap config.h (redefinition error)
- ✅ After: Removed from keymap config.h, only kept in main config.h
- ✅ Set brightness to 128/255 (50%) as requested

The firmware is now ready to compile successfully!
