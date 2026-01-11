#!/bin/bash
# ============================================
# RUN DEMO - Chạy hệ thống Dashboard
# ============================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COURSE_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "=========================================="
echo "  AUTOSAR ADAPTIVE DASHBOARD - DEMO"
echo "=========================================="

# Set vsomeip configuration
export VSOMEIP_CONFIGURATION="$COURSE_DIR/course/lesson2/example2/vsomeip.json"
export VSOMEIP_APPLICATION_NAME="dashboard_demo"

# Check if service exists
SERVICE_BIN="$COURSE_DIR/course/lesson2/example2/build/dashboard_service"
CLIENT_BIN="$COURSE_DIR/course/lesson3/example2/build/qt_dashboard"
CONSOLE_CLIENT="$COURSE_DIR/course/lesson2/example2/build/dashboard_client"

if [ ! -f "$SERVICE_BIN" ]; then
    echo "[ERROR] Dashboard Service not found at: $SERVICE_BIN"
    echo "Please run build_all.sh first or install vsomeip."
    exit 1
fi

# Start Dashboard Service in background
echo ""
echo "[1/2] Starting Dashboard Service..."
cd "$COURSE_DIR/course/lesson2/example2/build"
./dashboard_service ../database.json &
SERVICE_PID=$!
echo "[OK] Service started (PID: $SERVICE_PID)"

# Wait for service to initialize
sleep 2

# Start Qt Dashboard or Console Client
echo ""
echo "[2/2] Starting Client..."
if [ -f "$CLIENT_BIN" ]; then
    echo "Starting Qt Dashboard..."
    cd "$COURSE_DIR/course/lesson3/example2/build"
    ./qt_dashboard
elif [ -f "$CONSOLE_CLIENT" ]; then
    echo "Starting Console Client..."
    cd "$COURSE_DIR/course/lesson2/example2/build"
    ./dashboard_client
else
    echo "[ERROR] No client found. Please build the project first."
fi

# Cleanup
echo ""
echo "Stopping service..."
kill $SERVICE_PID 2>/dev/null || true
echo "[OK] Demo finished"
