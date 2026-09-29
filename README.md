# ⌨️ SimpleSwitcher Linux

**SimpleSwitcher Linux** is a lightweight utility designed to quickly correct the keyboard layout of already typed text on Linux systems.

## ⚠️ Important Notice

> [!WARNING]
> * **Root Privileges Required:** The program requires root privileges to run because it reads from and creates input devices.
> * **Keyboard Grab:** If the corresponding mode is enabled, the program will exclusively capture keyboard input. In case of bugs or unexpected behavior, you might temporarily lose keyboard input entirely, or keys may become permanently stuck.
> * **Privilege Dropping:** For security purposes, root privileges are dropped to standard user privileges immediately after all necessary input devices are opened.

---

## 🚀 Getting the Program

### Download Sources
```bash
git clone https://github.com/Aegel5/SimpleSwitcherLinux
```

### Compilation
Run the release build script:
```bash
./build_release.sh
```

---

## ⚙️ Configuration (`SimpleSwitcher.json`)

To generate a new configuration file or update an existing one, run:
```bash
./release/SimpleSwitcher -cfg
```
