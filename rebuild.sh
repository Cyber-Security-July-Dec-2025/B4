#!/usr/bin/env bash
set -euo pipefail

# Path vars (change if your project root differs)
PROJECT_DIR="$HOME/B4"
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
cmake ..           # add -D flags here if needed

echo "==> Building (make -j$(nproc))..."
make -j"$(nproc)"

# 2) Verify binary
BINARY="$BUILD_DIR/securechat"
if [ ! -x "$BINARY" ]; then
  echo "ERROR: built binary not found or not executable: $BINARY"
  exit 2
fi
echo "==> Build successful: $BINARY"

# 3) Prepare run folders
echo "==> Preparing run folders..."
rm -rf "$RUN_A" "$RUN_B"
mkdir -p "$RUN_A/keys" "$RUN_B/keys"

# NOTE: you said you already have .der keys in run-A and run-B;
# if you do, this next block will NOT overwrite them. It copies if a top-level keys/ exists.
# if [ -d "$PROJECT_DIR/keys" ]; then
#   echo "==> Copying top-level keys/ into run folders (only if present)"
#   cp -r "$PROJECT_DIR/keys/." "$RUN_A/keys/" || true
#   cp -r "$PROJECT_DIR/keys/." "$RUN_B/keys/" || true
# fi

# 4) Copy the binary and styles
echo "==> Copying binary to run folders..."
cp "$BINARY" "$RUN_A/"
cp "$BINARY" "$RUN_B/"
chmod +x "$RUN_A/securechat" "$RUN_B/securechat"

# Copy styles if present
if [ -d "$PROJECT_DIR/styles" ]; then
  echo "==> Copying styles/ into run folders..."
  cp -r "$PROJECT_DIR/styles" "$RUN_A/" || true
  cp -r "$PROJECT_DIR/styles" "$RUN_B/" || true
fi

# 5) If configs exist in project root prepare sample configs (do not overwrite existing)
if [ -f "$PROJECT_DIR/run-A-config.json" ] && [ ! -f "$RUN_A/config.json" ]; then
  cp "$PROJECT_DIR/run-A-config.json" "$RUN_A/config.json"
fi
if [ -f "$PROJECT_DIR/run-B-config.json" ] && [ ! -f "$RUN_B/config.json" ]; then
  cp "$PROJECT_DIR/run-B-config.json" "$RUN_B/config.json"
fi

# 6) Create start scripts
cat > "$RUN_A/start.sh" <<'SH'
#!/usr/bin/env bash
cd "$(dirname "$0")"
export QT_QPA_PLATFORM=xcb
./securechat
SH
cat > "$RUN_B/start.sh" <<'SH'
#!/usr/bin/env bash
cd "$(dirname "$0")"
export QT_QPA_PLATFORM=xcb
./securechat
SH
chmod +x "$RUN_A/start.sh" "$RUN_B/start.sh"

echo "==> Deploy done."
echo "Run the two instances in separate terminals:"
echo "  Terminal 1: cd $RUN_A && ./start.sh"
echo "  Terminal 2: cd $RUN_B && ./start.sh"
