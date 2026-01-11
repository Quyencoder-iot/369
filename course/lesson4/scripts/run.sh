#!/bin/bash
# ============================================
# RUN SCRIPT - Start Dashboard System
# ============================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "=========================================="
echo "  AUTOSAR Dashboard - Run"
echo "=========================================="

# Configuration
export VSOMEIP_CONFIGURATION="$PROJECT_DIR/course/lesson2/example2/vsomeip.json"

# Paths
SERVICE_DIR="$PROJECT_DIR/course/lesson2/example2/build"
CLIENT_DIR="$PROJECT_DIR/course/lesson3/example2/build"

# Check binaries
if [ ! -f "$SERVICE_DIR/dashboard_service" ]; then
    echo "[ERROR] Service not found. Run build.sh first."
    exit 1
fi

# Cleanup function
cleanup() {
    echo ""
    echo "Stopping services..."
    pkill -f dashboard_service 2>/dev/null || true
    exit 0
}
trap cleanup SIGINT SIGTERM

# Start service
echo "[1] Starting Dashboard Service..."
cd "$SERVICE_DIR"
./dashboard_service ../database.json &
SERVICE_PID=$!
echo "    PID: $SERVICE_PID"

sleep 2

# Start UI
if [ -f "$CLIENT_DIR/qt_dashboard" ]; then
    echo "[2] Starting Qt Dashboard..."
    cd "$CLIENT_DIR"
    ./qt_dashboard
else
    echo "[2] Starting Console Client..."
    cd "$SERVICE_DIR"
    ./dashboard_client
fi

cleanup
