#!/usr/bin/env bash
set -euo pipefail

# --- Configuration ---
# Set this to "A" on your first device, and "B" on your second device.
INSTANCE_ID="A"

# Set the peer's user and IP address.
# Make sure to change these values on each machine to point to the other one.
PEER_USER="rahul"
PEER_IP="10.48.102.50"
# ---------------------

# --- Script Logic ---
# Do not edit below this line

# Path vars
if [ "$INSTANCE_ID" = "A" ]; then
    RUN_DIR="$HOME/B4.1/run-A"
    PEER_RUN_DIR="$HOME/B4/run-B"
else
    RUN_DIR="$HOME/B4/run-B"
    PEER_RUN_DIR="$HOME/B4.1/run-A"
fi

MY_PUBLIC_KEY="$RUN_DIR/keys/my_public.der"
RCVD_KEY_DIR="$RUN_DIR/keys/rcvd_key"
PEER_RCVD_KEY_DIR="$PEER_RUN_DIR/keys/rcvd_key"

echo "### SCRIPT FOR INSTANCE ${INSTANCE_ID} ###"

# 1. Send our public key to the peer
echo "==> Sending public key to ${PEER_USER}@${PEER_IP}..."
rsync -avz "$MY_PUBLIC_KEY" "${PEER_USER}@${PEER_IP}:${PEER_RCVD_KEY_DIR}/"
echo "==> Key sent successfully."
echo ""

# 2. Check for a received key and place it correctly
echo "==> Checking for received key in ${RCVD_KEY_DIR}..."
if [ -f "${RCVD_KEY_DIR}/my_public.der" ]; then
    echo "--> Found received key. Moving and renaming it to peer_public.der..."
    mv "${RCVD_KEY_DIR}/my_public.der" "${RUN_DIR}/keys/peer_public.der"
    echo "--> Peer key successfully placed."
else
    echo "--> No key received from peer yet."
    echo "--> After the peer runs this script, re-run this script on this machine to finalize."
fi

echo "==> Process complete for Instance ${INSTANCE_ID}."
