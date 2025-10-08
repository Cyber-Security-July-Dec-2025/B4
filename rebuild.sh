#!/usr/bin/env bash
set -euo pipefail

# Path vars (change if your project root differs)
PROJECT_DIR="$HOME/B4.1"
BUILD_DIR="$PROJECT_DIR/build"
RUN_A="$PROJECT_DIR/run-A"
RUN_B="$PROJECT_DIR/run-B"

echo "==> Working in project: $PROJECT_DIR"

# 1) Full clean build
echo "==> Cleaning previous build..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "==> Running cmake..."
cmake .. # add -D flags here if needed

echo "==> Building all targets (make -j$(nproc))..."
make -j"$(nproc)"

# 2) Verify binary
BINARY="$BUILD_DIR/safetalk"
KEYGEN_BINARY="$BUILD_DIR/keygen"
if [ ! -x "$BINARY" ] || [ ! -x "$KEYGEN_BINARY" ]; then
  echo "ERROR: A required binary was not found or not executable."
  exit 2
fi
echo "==> Build successful: $BINARY and $KEYGEN_BINARY"

# 3) Generate and organize keys
echo "==> Generating keys for both instances..."
rm -rf "$PROJECT_DIR/keys_temp" # Clean old temp keys
mkdir -p "$PROJECT_DIR/keys_temp/run-A" "$PROJECT_DIR/keys_temp/run-B"

# Create the 'keys' directory before running keygen
mkdir -p keys

# Generate keys for run-A
"$KEYGEN_BINARY"
mv keys/my_private.der "$PROJECT_DIR/keys_temp/run-A/my_private.der"
mv keys/my_public.der "$PROJECT_DIR/keys_temp/run-A/my_public.der"

# Generate keys for run-B
"$KEYGEN_BINARY"
mv keys/my_private.der "$PROJECT_DIR/keys_temp/run-B/my_private.der"
mv keys/my_public.der "$PROJECT_DIR/keys_temp/run-B/my_public.der"
rmdir keys # remove empty directory created by keygen

# --- MODIFIED SECTION ---
# Create peer keys using rsync with user@localhost to simulate network transfer
# echo "==> Transferring public keys using rsync (network mode)..."
# rsync "$PROJECT_DIR/keys_temp/run-B/my_public.der" "$USER@localhost:$PROJECT_DIR/keys_temp/run-A/peer_public.der"
# rsync "$PROJECT_DIR/keys_temp/run-A/my_public.der" "$USER@localhost:$PROJECT_DIR/keys_temp/run-B/peer_public.der"
# --- END MODIFIED SECTION ---


# 4) Prepare run folders
echo "==> Preparing run folders..."
rm -rf "$RUN_A" "$RUN_B"
mkdir -p "$RUN_A/keys" "$RUN_B/keys"

echo "==> Copying generated keys into run folders..."
cp -r "$PROJECT_DIR/keys_temp/run-A/." "$RUN_A/keys/"
cp -r "$PROJECT_DIR/keys_temp/run-B/." "$RUN_B/keys/"
rm -rf "$PROJECT_DIR/keys_temp" # Clean up temp key directory

mkdir -p "$RUN_A/keys/rcvd_key" "$RUN_B/keys/rcvd_key"

# 5) Copy the binary and styles
echo "==> Copying binary to run folders..."
cp "$BINARY" "$RUN_A/"
cp "$BINARY" "$RUN_B/"
chmod +x "$RUN_A/safetalk" "$RUN_B/safetalk"

# Copy styles if present
if [ -d "$PROJECT_DIR/styles" ]; then
  echo "==> Copying styles/ into run folders..."
  cp -r "$PROJECT_DIR/styles" "$RUN_A/" || true
  cp -r "$PROJECT_DIR/styles" "$RUN_B/" || true
fi

# 6) If configs exist in project root prepare sample configs (do not overwrite existing)
if [ -f "$PROJECT_DIR/run-A-config.json" ] && [ ! -f "$RUN_A/config.json" ]; then
  cp "$PROJECT_DIR/run-A-config.json" "$RUN_A/config.json"
fi
if [ -f "$PROJECT_DIR/run-B-config.json" ] && [ ! -f "$RUN_B/config.json" ]; then
  cp "$PROJECT_DIR/run-B-config.json" "$RUN_B/config.json"
fi

# 7) Create start scripts
cat > "$RUN_A/start.sh" <<'SH'
#!/usr/bin/env bash
cd "$(dirname "$0")"
export QT_QPA_PLATFORM=xcb
./safetalk
SH
cat > "$RUN_B/start.sh" <<'SH'
#!/usr/bin/env bash
cd "$(dirname "$0")"
export QT_QPA_PLATFORM=xcb
./safetalk
SH
chmod +x "$RUN_A/start.sh" "$RUN_B/start.sh"

echo "==> Deploy done."
echo "Run the two instances in separate terminals:"
echo "  Terminal 1: cd $RUN_A && ./start.sh"
echo "  Terminal 2: cd $RUN_B && ./start.sh"

