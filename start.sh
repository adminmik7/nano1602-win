#!/usr/bin/env bash
# start.sh — One-command setup & launch for nano1602
# Checks/installs Python3 + pip if missing, then runs sender.py (auto-installs deps)
#
# Usage:
#   chmod +x start.sh && ./start.sh          # auto-detect port
#   chmod +x start.sh && ./start.sh setup    # install deps only, don't run
#   ./start.sh /dev/ttyUSB0                   # specify port
#
# Supported distros: Debian/Ubuntu, Fedora, Arch, Alpine

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SENDER="${SCRIPT_DIR}/sender.py"

# ─── Colors ──────────────────────────────────────────────
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

log_info()  { echo -e "${GREEN}[✓]${NC} $1"; }
log_warn()  { echo -e "${YELLOW}[!]${NC} $1"; }
log_error() { echo -e "${RED}[✗]${NC} $1"; }

# ─── Check sender.py exists ──────────────────────────────
if [ ! -f "$SENDER" ]; then
    log_error "sender.py not found at: $SENDER"
    log_error "Make sure sender.py is in the same directory as start.sh"
    exit 1
fi

# ─── Detect + install Python 3 ───────────────────────────
if ! command -v python3 &>/dev/null; then
    log_info "Python3 not found, installing..."
    if command -v apt-get &>/dev/null; then
        sudo apt-get update -qq && sudo apt-get install -y -qq python3 python3-pip
    elif command -v dnf &>/dev/null; then
        sudo dnf install -y -q python3 python3-pip
    elif command -v yum &>/dev/null; then
        sudo yum install -y -q python3 python3-pip
    elif command -v pacman &>/dev/null; then
        sudo pacman -S --noconfirm python python-pip
    elif command -v apk &>/dev/null; then
        sudo apk add --quiet python3 py3-pip
    elif command -v zypper &>/dev/null; then
        sudo zypper install -y -q python3 python3-pip
    else
        log_error "Unsupported package manager. Install Python3 manually:"
        log_error "  https://www.python.org/downloads/"
        exit 1
    fi
    log_info "Python3 installed: $(python3 --version 2>&1)"
else
    PY_MIN=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')" 2>/dev/null)
    log_info "Python3 already available: $PY_MIN"
fi

# ─── Ensure pip3 works ───────────────────────────────────
if ! python3 -m pip --version &>/dev/null; then
    log_warn "pip not found, installing..."
    if command -v apt-get &>/dev/null; then
        sudo apt-get install -y -qq python3-pip
    else
        curl -sS https://bootstrap.pypa.io/get-pip.py | python3
    fi
    log_info "pip3 ready: $(python3 -m pip --version 2>&1)"
fi

# ─── Setup-only mode ─────────────────────────────────────
if [ "$1" = "setup" ]; then
    echo ""
    log_info "Running dependency install (pyserial + psutil)..."
    if [ -f "${SCRIPT_DIR}/requirements.txt" ]; then
        python3 -m pip install --quiet -r "${SCRIPT_DIR}/requirements.txt"
    fi
    python3 -m pip install --quiet psutil 2>/dev/null || log_warn "psutil skipped (will use /proc fallback)"
    echo ""
    log_info "All dependencies installed. Run './start.sh' to start the monitor."
    exit 0
fi

# ─── Run sender.py ───────────────────────────────────────
echo ""
echo "════════════════════════════════════════════════"
echo "  nano1602 — PC Monitor  Arduino Nano + LCD1602"
echo "════════════════════════════════════════════════"
echo ""

if [ -n "$1" ]; then
    python3 "$SENDER" "$1"
else
    python3 "$SENDER"
fi
