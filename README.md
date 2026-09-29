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
## 🛠️ System Settings

The configuration file contains the following parameters:

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `keyboard_device` | String | The keyboard device path to read from (e.g., `"/dev/input/by-id/my_keyboard"`). Leave it as an empty string `""` for auto-detection. *Note: Multimedia keyboards might not be detected automatically and must be specified manually.* |
| `mouse_device` | String | The mouse device path to read from. Leave it as an empty string `""` for auto-detection. |
| `keyboard_exclusive_mode` | Boolean | **(Not recommended)** When enabled, all keyboard input is fully intercepted and proxied into a new virtual keyboard. This mode is strictly required if you need to filter input and remove hotkey press events (like `CapsLock`) from the OS queue. If you use neutral hotkeys (like `Break`) or manage to disable hotkeys at the Desktop Manager (DM) level, keep this mode disabled. |

## 🚌 Shell D-Bus Settings

> [!NOTE]
> Currently, **only KDE is supported natively** via D-Bus. For any other Desktop Manager (DM), you must provide the `fallback_*` values.

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `user_id` | Integer | The ID of the current user (run `id -u` to find it). It is used to locate the D-Bus socket. Default is `1000`. |
| `fallback_layouts_list` | Array | A list of keyboard layouts to switch between, for example: `["us", "ru"]`. This list is ignored if layout data is successfully retrieved via D-Bus. |
| `fallback_emulate_switch` | String | The hotkey combo sent to the system if the layout cannot be switched via D-Bus, for example: `"Alt+Shift"`. |

## 🔄 Switching Settings

> [!TIP]
> For detailed implementation details and advanced options, please refer directly to the [`setting.h`](setting.h) source file.

* `hk_last_word` (String/Key): Hotkey to convert the **last typed word**.
* `hk_several_words` (String/Key): Hotkey to convert the **last few words**.
* `hk_all_text` (String/Key): Hotkey to convert the **entire text buffer**.
* `words_separate_mode` (Integer/Enum): Defines the mode for **word separation** (e.g., how spaces, punctuation, or special characters split words).
* `treat_as_letter` (String/Array): A set of additional symbols or characters that should be **treated as letters** rather than word separators.

## 🎮 Running and Management

### Systemd Service
* `./recreate_and_start.sh`: Creates and **starts the systemd service**. You must run this after every rebuild, as the service binds to the file's `sha256` hash.
* `sudo systemctl restart SimpleSwitcher`: **Restarts the service** (useful after modifying configuration files).
* `sudo journalctl -u SimpleSwitcher -f`: Follows and **views logs in real-time**.
* `./delete_service`: **Stops and removes** the systemd service from the system.

### IPC Control
SimpleSwitcher creates a message queue at `/SimpleSwitcher_queue` to listen for external commands. 

Supported commands:
* `'c'` (1 byte): **Clears the internal text buffer**.


