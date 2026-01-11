#!/bin/bash
# ============================================
# PACKAGE SCRIPT - Create distributable package
# ============================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

VERSION="1.0.0"
PACKAGE_NAME="autosar-dashboard-$VERSION"

echo "=========================================="
echo "  Packaging AUTOSAR Dashboard v$VERSION"
echo "=========================================="

# Create package directory
PACKAGE_DIR="/tmp/$PACKAGE_NAME"
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR"/{bin,etc,doc}

# Copy binaries (if they exist)
SERVICE_BIN="$PROJECT_DIR/course/lesson2/example2/build/dashboard_service"
CLIENT_BIN="$PROJECT_DIR/course/lesson2/example2/build/dashboard_client"
QT_BIN="$PROJECT_DIR/course/lesson3/example2/build/qt_dashboard"

if [ -f "$SERVICE_BIN" ]; then
    cp "$SERVICE_BIN" "$PACKAGE_DIR/bin/"
    echo "[OK] Copied dashboard_service"
fi

if [ -f "$CLIENT_BIN" ]; then
    cp "$CLIENT_BIN" "$PACKAGE_DIR/bin/"
    echo "[OK] Copied dashboard_client"
fi

if [ -f "$QT_BIN" ]; then
    cp "$QT_BIN" "$PACKAGE_DIR/bin/"
    echo "[OK] Copied qt_dashboard"
fi

# Copy configuration
cp "$PROJECT_DIR/course/lesson2/example2/database.json" "$PACKAGE_DIR/etc/"
cp "$PROJECT_DIR/course/lesson2/example2/vsomeip.json" "$PACKAGE_DIR/etc/"
echo "[OK] Copied configuration files"

# Copy documentation
cp "$PROJECT_DIR/README.md" "$PACKAGE_DIR/doc/"
echo "[OK] Copied documentation"

# Create startup script
cat > "$PACKAGE_DIR/bin/start.sh" << 'STARTUP'
#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export VSOMEIP_CONFIGURATION="$DIR/../etc/vsomeip.json"

cleanup() {
    pkill -f dashboard_service 2>/dev/null
    exit 0
}
trap cleanup SIGINT SIGTERM

"$DIR/dashboard_service" "$DIR/../etc/database.json" &
sleep 2

if [ -f "$DIR/qt_dashboard" ]; then
    "$DIR/qt_dashboard"
else
    "$DIR/dashboard_client"
fi

cleanup
STARTUP
chmod +x "$PACKAGE_DIR/bin/start.sh"
echo "[OK] Created startup script"

# Create tarball
cd /tmp
tar -czvf "$PACKAGE_NAME.tar.gz" "$PACKAGE_NAME"

# Move to project
mv "$PACKAGE_NAME.tar.gz" "$PROJECT_DIR/"

# Cleanup
rm -rf "$PACKAGE_DIR"

echo ""
echo "=========================================="
echo "  Package: $PROJECT_DIR/$PACKAGE_NAME.tar.gz"
echo "=========================================="
