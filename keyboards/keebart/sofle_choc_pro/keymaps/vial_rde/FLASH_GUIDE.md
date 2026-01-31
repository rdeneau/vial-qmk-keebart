# How to Flash Your Sofle Choc Pro Keyboard

## What You Need

- ✅ Your compiled firmware file: `.build\keebart_sofle_choc_pro_vial_rde.uf2`
- ✅ USB-C cable to connect keyboard to computer
- ✅ Access to both halves of your split keyboard

## Flashing Process (Do This for BOTH Halves)

### Step 1: Locate Your Firmware File

After compilation, find the file at:

```txt
C:\Dev\_github\vial-qmk-keebart\.build\keebart_sofle_choc_pro_vial_rde.uf2
```

You may want to copy it to your Desktop for easy access.

### Step 2: Enter Bootloader Mode

You have **two options** to enter bootloader mode:

#### Option A: Double-Tap Reset (Easiest)

1. **Unplug the keyboard** if connected
2. **Plug in the USB cable** to the keyboard half you want to flash
3. **Press the reset button twice quickly** (within 500ms)
   - The reset button is usually on the bottom or back of the keyboard PCB
4. **The keyboard will appear as a USB drive** named **`RPI-RP2`**

#### Option B: Bootmagic Key (No Reset Button Needed)

1. **Unplug the keyboard**
2. **Hold down the top-left key** (Esc on your layout)
3. **While holding it, plug in the USB cable**
4. **Release the key after 1-2 seconds**
5. **The keyboard will appear as a USB drive** named **`RPI-RP2`**

### Step 3: Flash the Firmware

1. **A new drive appears** in File Explorer called **`RPI-RP2`**
2. **Open the drive** - you'll see some files like `INFO_UF2.TXT`
3. **Copy (or drag) your `.uf2` file** to the `RPI-RP2` drive
4. **Wait a few seconds** - the drive will disappear automatically
5. **The keyboard reboots** with the new firmware!

### Step 4: Flash the Other Half

Since this is a split keyboard, **repeat Steps 2-3 for the other half**:

1. **Unplug the first half**
2. **Plug in the second half**
3. **Enter bootloader mode** (double-tap reset or bootmagic)
4. **Copy the same `.uf2` file** to the `RPI-RP2` drive
5. **Wait for automatic reboot**

### Step 5: Connect Both Halves

1. **Plug in the TRRS cable** between left and right halves
2. **Plug USB cable into your PRIMARY half** (usually left)
3. **Both halves should now work** with the new firmware!

## Verification

After flashing both halves, check that:

- ✅ Both keyboard halves respond to keypresses
- ✅ LEDs light up at 50% brightness
- ✅ Edge columns (first & last) glow **white**
- ✅ Top row (F2-F5, F7-F10) shows **grey**
- ✅ Home row (ASDF/JKL;) is **yellow**
- ✅ O key is **red**
- ✅ NAV/TG(LOWER) thumb key is **orange**
- ✅ RAlt thumb key is **blue/pink**
- ✅ Press NAV key - navigation/numpad keys turn **orange**

## Troubleshooting

### ❌ RPI-RP2 drive doesn't appear

**Solution**: Try the other bootloader entry method, or check the reset button is working

### ❌ "File copy failed" error

**Solution**:

- Make sure you're copying a valid `.uf2` file
- Try copying to Desktop first, then to the drive
- Ensure the file isn't corrupted (should be ~200-500 KB)

### ❌ Keyboard doesn't work after flashing

**Solution**:

- Unplug and replug the USB cable
- Check TRRS cable connection between halves
- Verify you flashed BOTH halves

### ❌ Only one half works

**Solution**:

- Flash the other half - both need the firmware
- Check TRRS cable is properly connected
- Make sure USB is connected to the PRIMARY half

### ❌ LEDs don't show the expected colors

**Solution**:

- The colors are set in the firmware, they should work automatically
- If LEDs are off, try pressing RGB toggle keys (if mapped)
- Check brightness isn't set to 0 in Vial

## Using QMK Toolbox (Alternative Method)

If you prefer a GUI tool:

1. **Open QMK Toolbox**
2. **Select your `.uf2` file** using "Open" button
3. **Set MCU to `RP2040`** (if not auto-detected)
4. **Enter bootloader mode** on keyboard (double-tap reset)
5. **Click "Flash"** button
6. **Repeat for the other half**

## Quick Reference Card

```txt
┌─────────────────────────────────────────┐
│ QUICK FLASH STEPS                       │
├─────────────────────────────────────────┤
│ 1. Unplug keyboard                      │
│ 2. Plug in USB cable                    │
│ 3. Double-tap RESET button              │
│ 4. Copy .uf2 to RPI-RP2 drive           │
│ 5. Wait for auto-reboot                 │
│ 6. Repeat for other half                │
└─────────────────────────────────────────┘
```

## Important Notes

⚠️ **Both halves must be flashed** - each half has its own microcontroller
⚠️ **TRRS cable** - Only connect when both halves are NOT in bootloader mode
⚠️ **Backup** - The old firmware is replaced, but you can always reflash the old one if needed

That's it! Your keyboard should now have custom LED lighting. Enjoy! 🎨✨
