# 🛡️ SafeTalk

**SafeTalk** — a lightweight end-to-end encrypted chat application built with **Qt** and **Crypto++**.

- **Crypto:** RSA (key exchange) + AES‑GCM (message confidentiality & integrity)
- **Transport:** TCP sockets (peer‑to‑peer, LAN)
- **UI:** Qt GUI
- **Platform:** Linux (Ubuntu/Debian tested)

---

## ✨ Highlights

- 🔐 E2E encryption using RSA + AES‑GCM
- ⚡ Small, Qt-based GUI for quick testing
- 🌍 Peer-to-peer over LAN; easy JSON-based configuration
- 🐧 Built with standard Linux toolchain (cmake, GNU toolchain)

---

## 📦 Prerequisites

On Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake qtbase5-dev libcrypto++-dev
```

---

## 🚀 Build & Run

Follow these steps — this is a single, reliable flow that builds the project, prepares keys, and runs two instances (A and B).

### 1) Clone the repo

```bash
git clone https://github.com/Cyber-Security-July-Dec-2025/B4.git ~/B4
cd ~/B4
```

### 2) Build (clean, reproducible)

```bash
# make scripts executable and run the rebuild script
chmod +x rebuild.sh
./rebuild.sh 
```

If the build succeeds you should have a binary such as `build/securechat`.

### 3) Prepare keys for each run folder

```bash
# populate run-A keys
cp -r ~/B4/keys/run-A/* ~/B4/run-A/keys/

# populate run-B keys
cp -r ~/B4/keys/run-B/* ~/B4/run-B/keys/
```

> Note: The repository README previously duplicated the `run-A` copy — the commands above copy the correct key sets to each run folder.

### 4) Make sure required files are executable

```bash
chmod +x build/securechat || true
chmod +x run-A/start.sh run-B/start.sh || true
```

### 5) Start the two instances (two terminals)

Open **two terminals**. In Terminal 1:

```bash
cd ~/B4/run-A
bash start.sh
```

In Terminal 2:

```bash
cd ~/B4/run-B
bash start.sh
```

If `start.sh` expects to be run from the run folder (it usually does), `cd` into the folder first so the `./securechat` path resolves correctly.

### 6) Alternative: run the built binary directly

If `start.sh` fails, run the binary and point it at the config in each run folder:

Terminal 1 (A):

```bash
cd ~/B4/run-A
export QT_QPA_PLATFORM=xcb       # GUI helper for some setups
../build/securechat --config config.json
```

Terminal 2 (B):

```bash
cd ~/B4/run-B
export QT_QPA_PLATFORM=xcb
../build/securechat --config config.json
```

Replace `--config config.json` with the correct CLI form if your binary uses a different flag.

---

## ⚙️ Example `config.json`

Each run folder (`run-A`, `run-B`) contains a `config.json` file. Example:
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

**Tips:**
- `my_private_key` → Your private RSA key.  
- `my_public_key` → Your public RSA key.  
- `peer_public_key` → The peer’s public key.  
- Use `127.0.0.1` (loopback) to run both instances on the same machine for testing.
- Use LAN IPs (e.g., `192.168.x.x`) if running across different machines on the same network.
- `listen_ip` is often `0.0.0.0` for listening on all interfaces; set it to `127.0.0.1` for local-only testing.

---

## 🌐 Finding Your LAN IP

On Linux:

```bash
hostname -I
```

or

```bash
ip addr show
```

Look for an address like `192.168.x.x` or `10.x.x.x`.  

---

## 🔧 Troubleshooting (quick)

- **`Permission denied` when running `./securechat`**
  - Ensure the binary is executable: `chmod +x build/securechat` (or `../build/securechat` from run folder).
  - If repo is on a mounted Windows/OneDrive/SMB share, that mount may have `noexec` — move the project to a native Linux filesystem or remount with `exec`.
  - If the `file` command shows a Windows PE file, you built the wrong target (build on Linux).

- **`failed to open config.json`**
  - Ensure `config.json` exists in the run folder and the binary is pointed at it.

- **GUI not showing or crashes**
  - Make sure Qt is installed; set `QT_QPA_PLATFORM=xcb` if needed.

- **Instances don’t connect**
  - Check `peer_ip` / `peer_port` in each `config.json`.
  - Ensure firewall allows the ports: `sudo ufw allow <port>`.

---

## 📸 Demonstration

Below is a sample screenshot of **SafeTalk** in action:

![Chat Example](chat_ex.png)

This image shows two peers exchanging encrypted messages using the Qt-based interface.

---

## 📄 License

MIT License — free to use and modify.

---


