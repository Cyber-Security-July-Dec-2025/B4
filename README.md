# 🛡️ SafeTalk

**SafeTalk** — a lightweight **end-to-end encrypted chat application** built with **Qt** and **Crypto++**.

> **Crypto:** RSA (key exchange) + AES-GCM (message confidentiality & integrity)  
> **Transport:** TCP sockets (peer-to-peer, LAN)  
> **UI:** Qt GUI  
> **Platform:** Linux (Ubuntu/Debian tested)

---

## ✨ Highlights

- 🔐 **E2E encryption** using RSA + AES-GCM  
- ⚡ **Lightweight Qt-based GUI** for testing  
- 🌍 **Peer-to-peer over LAN** with JSON configuration  
- 🧱 **Built with standard Linux toolchain (CMake, GNU)**  

---

## 📦 Prerequisites

**On Debian/Ubuntu:**

```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools libcrypto++-dev openssh-server
```

> **Note:** For key exchange between two devices, both computers must have the SSH server running.  
> Start it with:
> ```bash
> sudo systemctl start ssh
> ```

---

## 🚀 Build & Run

This guide covers setting up two instances on two separate devices (**Device A** and **Device B**).

### 1️⃣ Clone the Repository (on both devices)

```bash
git clone https://github.com/Cyber-Security-July-Dec-2025/B4.git ~/B4
cd ~/B4
```

---

### 2️⃣ Build the Software (on both devices)

The `rebuild.sh` script compiles the application, generates RSA keys, and prepares run folders.

```bash
chmod +x rebuild.sh
./rebuild.sh
```

If the build succeeds, you will have:

- `build/safetalk` → compiled binary  
- `run-A/` and `run-B/` → preconfigured run folders

---

### 3️⃣ Exchange Public Keys (between devices)

You’ll use the provided `transfer_keys.sh` script.

#### Steps:

1. **Make the transfer script executable**
   ```bash
   chmod +x transfer_keys.sh
   ```

2. **On Device A**
   - Open `transfer_keys.sh`
   - Set:
     ```bash
     INSTANCE_ID="A"
     PEER_USER="<username_on_B>"
     PEER_IP="<ip_of_device_B>"
     ```
   - Run:
     ```bash
     ./transfer_keys.sh
     ```

3. **On Device B**
   - Open `transfer_keys.sh`
   - Set:
     ```bash
     INSTANCE_ID="B"
     PEER_USER="<username_on_A>"
     PEER_IP="<ip_of_device_A>"
     ```
   - Run:
     ```bash
     ./transfer_keys.sh
     ```

4. **On Device A (final run)**
   - Run again:
     ```bash
     ./transfer_keys.sh
     ```

✅ Key exchange is now complete!

---

### 4️⃣ Start SafeTalk (on both devices)

**Device A**
```bash
cd ~/B4/run-A
bash start.sh
```

**Device B**
```bash
cd ~/B4/run-B
bash start.sh
```

Make sure `config.json` in each run folder is updated with the correct IPs and ports.

---

## ⚙️ Example `config.json`

```json
{
  "listen_ip": "127.0.0.1",
  "listen_port": 9001,
  "peer_ip": "127.0.0.1",
  "peer_port": 9002,
  "my_private_key": "keys/my_private.der",
  "my_public_key": "keys/my_public.der",
  "peer_public_key": "keys/peer_public.der",
  "rsa_bits": 2048,
  "aes_key_bytes": 32,
  "aes_iv_bytes": 12
}
```

### 🧩 Notes

- `my_private_key` → Your private RSA key  
- `my_public_key` → Your public RSA key  
- `peer_public_key` → The peer’s public key  
- Use `127.0.0.1` (loopback) for same-machine testing  
- Use your **LAN IPs (e.g., 192.168.x.x)** for cross-device chat  
- Set `listen_ip` to `0.0.0.0` for all interfaces (LAN access)

---

## 🌐 Finding Your LAN IP

```bash
hostname -I
# or
ip addr show
```

Look for an address like **192.168.x.x** or **10.x.x.x**.

---

## 🔧 Troubleshooting

| Issue | Possible Fix |
|-------|---------------|
| **Permission denied when running ./safetalk** | Run `chmod +x build/safetalk` |
| **Running from network share fails** | Move project to native Linux# 🛡️ SafeTalk

**SafeTalk** — a lightweight **end-to-end encrypted chat application** built with **Qt** and **Crypto++**.

> **Crypto:** RSA (key exchange) + AES-GCM (message confidentiality & integrity)  
> **Transport:** TCP sockets (peer-to-peer, LAN)  
> **UI:** Qt GUI  
> **Platform:** Linux (Ubuntu/Debian tested)

---

## ✨ Highlights

- 🔐 **E2E encryption** using RSA + AES-GCM  
- ⚡ **Lightweight Qt-based GUI** for testing  
- 🌍 **Peer-to-peer over LAN** with JSON configuration  
- 🧱 **Built with standard Linux toolchain (CMake, GNU)**  

---

## 📦 Prerequisites

**On Debian/Ubuntu:**

```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools libcrypto++-dev openssh-server
```

> **Note:** For key exchange between two devices, both computers must have the SSH server running.  
> Start it with:
> ```bash
> sudo systemctl start ssh
> ```

---

## 🚀 Build & Run

This guide covers setting up two instances on two separate devices (**Device A** and **Device B**).

### 1️⃣ Clone the Repository (on both devices)

```bash
git clone https://github.com/Cyber-Security-July-Dec-2025/B4.git ~/B4
cd ~/B4
```

---

### 2️⃣ Build the Software (on both devices)

The `rebuild.sh` script compiles the application, generates RSA keys, and prepares run folders.

```bash
chmod +x rebuild.sh
./rebuild.sh
```

If the build succeeds, you will have:

- `build/safetalk` → compiled binary  
- `run-A/` and `run-B/` → preconfigured run folders

---

### 3️⃣ Exchange Public Keys (between devices)

You’ll use the provided `transfer_keys.sh` script.

#### Steps:

1. **Make the transfer script executable**
   ```bash
   chmod +x transfer_keys.sh
   ```

2. **On Device A**
   - Open `transfer_keys.sh`
   - Set:
     ```bash
     INSTANCE_ID="A"
     PEER_USER="<username_on_B>"
     PEER_IP="<ip_of_device_B>"
     ```
   - Run:
     ```bash
     ./transfer_keys.sh
     ```

3. **On Device B**
   - Open `transfer_keys.sh`
   - Set:
     ```bash
     INSTANCE_ID="B"
     PEER_USER="<username_on_A>"
     PEER_IP="<ip_of_device_A>"
     ```
   - Run:
     ```bash
     ./transfer_keys.sh
     ```

4. **On Device A (final run)**
   - Run again:
     ```bash
     ./transfer_keys.sh
     ```

✅ Key exchange is now complete!

---

### 4️⃣ Start SafeTalk (on both devices)

**Device A**
```bash
cd ~/B4/run-A
bash start.sh
```

**Device B**
```bash
cd ~/B4/run-B
bash start.sh
```

Make sure `config.json` in each run folder is updated with the correct IPs and ports.

---

## ⚙️ Example `config.json`

```json
{
  "listen_ip": "127.0.0.1",
  "listen_port": 9001,
  "peer_ip": "127.0.0.1",
  "peer_port": 9002,
  "my_private_key": "keys/my_private.der",
  "my_public_key": "keys/my_public.der",
  "peer_public_key": "keys/peer_public.der",
  "rsa_bits": 2048,
  "aes_key_bytes": 32,
  "aes_iv_bytes": 12
}
```

### 🧩 Notes

- `my_private_key` → Your private RSA key  
- `my_public_key` → Your public RSA key  
- `peer_public_key` → The peer’s public key  
- Use `127.0.0.1` (loopback) for same-machine testing  
- Use your **LAN IPs (e.g., 192.168.x.x)** for cross-device chat  
- Set `listen_ip` to `0.0.0.0` for all interfaces (LAN access)

---

## 🌐 Finding Your LAN IP

```bash
hostname -I
# or
ip addr show
```

Look for an address like **192.168.x.x** or **10.x.x.x**.

---

## 🔧 Troubleshooting

| Issue | Possible Fix |
|-------|---------------|
| **Permission denied when running ./safetalk** | Run `chmod +x build/safetalk` |
| **Running from network share fails** | Move project to native Linux filesystem |
| **Missing config.json** | Ensure file exists in `run-A` / `run-B` |
| **Qt GUI not showing** | Verify Qt installation; try `export QT_QPA_PLATFORM=xcb` |
| **Instances not connecting** | Check `peer_ip`/`peer_port` and firewall (`sudo ufw allow <port>`) |

---

## 📸 Demonstration

Below is a sample screenshot of SafeTalk in action:

> Two peers exchanging encrypted messages using the Qt-based interface.

---

## 📄 License

**MIT License** — free to use, modify, and distribute.

---

### 🧠 Credits

Developed as part of the **Cyber Security — July–Dec 2025** project.

---

**Enjoy secure chatting with SafeTalk!** 🛡️💬 filesystem |
| **Missing config.json** | Ensure file exists in `run-A` / `run-B` |
| **Qt GUI not showing** | Verify Qt installation; try `export QT_QPA_PLATFORM=xcb` |
| **Instances not connecting** | Check `peer_ip`/`peer_port` and firewall (`sudo ufw allow <port>`) |

---

## 📸 Demonstration

Below is a sample screenshot of SafeTalk in action:

> Two peers exchanging encrypted messages using the Qt-based interface.

---

## 📄 License

**MIT License** — free to use, modify, and distribute.

---

### 🧠 Credits

Developed as part of the **Cyber Security — July–Dec 2025** project.

---

**Enjoy secure chatting with SafeTalk!** 🛡️💬
