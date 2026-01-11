#!/bin/bash
#
# run_demo.sh - Script chạy demo cơ bản
#
# Khởi động service và client trong các terminal riêng

echo "==================================="
echo "  AUTOSAR Dashboard - Demo"
echo "==================================="

BUILD_DIR="./build"
CONFIG_FILE="./vsomeip.json"

# Kiểm tra build
if [ ! -d "$BUILD_DIR" ]; then
    echo "ERROR: Build directory not found. Run build.sh first."
    exit 1
fi

# Export configuration
export VSOMEIP_CONFIGURATION=$(pwd)/vsomeip.json
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

echo ""
echo "Configuration:"
echo "  VSOMEIP_CONFIGURATION=$VSOMEIP_CONFIGURATION"
echo "  LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
echo ""

# Chạy service
echo "Starting Dashboard Service..."
echo "(Press Ctrl+C to stop)"
echo ""

cd $BUILD_DIR
./dashboard_service_basic &
SERVICE_PID=$!

sleep 2

# Chạy client
echo ""
echo "Starting Dashboard Client..."
./dashboard_client_basic &
CLIENT_PID=$!

# Wait và cleanup
cleanup() {
    echo ""
    echo "Stopping demo..."
    kill $CLIENT_PID 2>/dev/null
    kill $SERVICE_PID 2>/dev/null
    exit 0
}

trap cleanup SIGINT SIGTERM

# Wait
wait $CLIENT_PID
wait $SERVICE_PID
