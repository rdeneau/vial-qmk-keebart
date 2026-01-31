# Building the Firmware - vial_rde Keymap

## Quick Guide

Your custom LED-enabled firmware is ready to compile! Here are several methods to build it:

## Option 1: Using GitHub Actions (Easiest - No Local Setup Required)

1. **Commit and push your changes** to GitHub:
   ```powershell
   cd C:\Dev\_github\vial-qmk-keebart
   git add keyboards/keebart/sofle_choc_pro/keymaps/vial_rde/
   git commit -m "Add custom LED lighting to vial_rde keymap"
   git push
   ```

2. **Go to the Actions tab** in your GitHub repository

3. **The CI workflow will automatically build** all keymaps including `vial_rde`

4. **Download the .uf2 file** from the build artifacts once the workflow completes

## Option 2: Using Docker (Recommended for Local Builds)

### Prerequisites
- Docker Desktop must be running

### Steps

1. **Start Docker Desktop**

2. **Run the build command**:
   ```powershell
   cd C:\Dev\_github\vial-qmk-keebart
   docker run --rm -v "${PWD}:/qmk_firmware" ghcr.io/qmk/qmk_cli qmk compile -kb keebart/sofle_choc_pro -km vial_rde
   ```

3. **Find your .uf2 file** in the `.build` directory:
   ```
   .build/keebart_sofle_choc_pro_vial_rde.uf2
   ```

## Option 3: Using WSL with QMK Setup (One-time Setup Required)

### Initial Setup

1. **Start your Ubuntu WSL instance**:
   ```powershell
   wsl -d Ubuntu-22.04
   ```

2. **Inside WSL, navigate to the repository**:
   ```bash
   cd /mnt/c/Dev/_github/vial-qmk-keebart
   ```

3. **Install QMK CLI and dependencies**:
   ```bash
   # Install prerequisites
   sudo apt update
   sudo apt install -y git python3-pip

   # Install QMK
   python3 -m pip install --user qmk

   # Add QMK to PATH (add this to your ~/.bashrc for permanence)
   export PATH="$HOME/.local/bin:$PATH"

   # Setup QMK (this installs arm-none-eabi-gcc and other tools)
   qmk setup -H /mnt/c/Dev/_github/vial-qmk-keebart -y
   ```

### Building

Once setup is complete, compile with:

```bash
cd /mnt/c/Dev/_github/vial-qmk-keebart
qmk compile -kb keebart/sofle_choc_pro -km vial_rde
```

Or using make:

```bash
make keebart/sofle_choc_pro:vial_rde
```

The `.uf2` file will be in the `.build` directory.

## Option 4: Using MSYS2 (Native Windows Build)

### Initial Setup

1. **Install MSYS2** from https://www.msys2.org/

2. **Open MSYS2 MinGW 64-bit terminal**

3. **Install QMK and dependencies**:
   ```bash
   pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-python-pip
   python -m pip install qmk
   qmk setup -H /c/Dev/_github/vial-qmk-keebart -y
   ```

### Building

```bash
cd /c/Dev/_github/vial-qmk-keebart
qmk compile -kb keebart/sofle_choc_pro -km vial_rde
```

## Flashing the Firmware

Once you have the `.uf2` file:

1. **Locate the file**: 
   - `.build/keebart_sofle_choc_pro_vial_rde.uf2`

2. **Enter bootloader mode** on your keyboard:
   - Press the reset button twice quickly, OR
   - Hold the bootmagic key (usually top-left key) while plugging in the keyboard

3. **The keyboard will appear as a USB drive** named `RPI-RP2`

4. **Copy the .uf2 file** to the RPI-RP2 drive

5. **The keyboard will automatically reboot** with the new firmware

6. **Repeat for the other half** of your split keyboard

## Verifying the Build

After flashing, verify:

- ✅ LEDs are at 50% brightness by default
- ✅ Edge columns (first and last) glow white
- ✅ Top row (F2-F5, F7-F10) shows grey
- ✅ Home row (ASDF/JKL;) is yellow
- ✅ O key is red
- ✅ NAV thumb key is orange
- ✅ RAlt thumb key is blue/pink
- ✅ Toggle LOWER layer - nav/numpad keys turn orange

## Troubleshooting

### Docker: "cannot find the file specified"
- **Solution**: Start Docker Desktop and wait for it to fully initialize

### WSL: "command not found"
- **Solution**: Run the setup steps in Option 3 to install required tools

### Build errors
- **Check**: Ensure all submodules are initialized:
  ```bash
  git submodule update --init --recursive
  ```

### "No space left on device" during build
- **Solution**: Clean the build directory:
  ```bash
  qmk clean
  # or
  make clean
  ```

## Current Files Modified

The following files contain your custom LED configuration:

1. `keyboards/keebart/sofle_choc_pro/keymaps/vial_rde/keymap.c`
   - RGB Matrix indicators implementation
   - LED color mapping for BASE and LOWER layers

2. `keyboards/keebart/sofle_choc_pro/keymaps/vial_rde/config.h`
   - Default brightness: 128/255 (50%)
   - RGB Matrix mode configuration

All changes are ready to compile!

## Quick Build Reference

| Method | Command | Setup Time | Build Time |
|--------|---------|------------|------------|
| GitHub Actions | Push to GitHub | 0 min | ~5-10 min |
| Docker | `docker run ...` | 0 min* | ~3-5 min |
| WSL | `qmk compile ...` | ~10 min | ~2-3 min |
| MSYS2 | `qmk compile ...` | ~15 min | ~2-3 min |

*Assuming Docker Desktop is already running

## Recommended Method

For the fastest result right now: **Option 2 (Docker)** - just start Docker Desktop and run one command!

For future builds: **Option 3 (WSL)** - fast builds after initial setup.
