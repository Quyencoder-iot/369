#!/bin/bash
# Test script to verify Sway nested installation

echo "============================================"
echo "  Sway Nested Mode - Installation Test"
echo "============================================"
echo ""

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
PASSED=0
FAILED=0

# Function to test if command exists
test_command() {
    local cmd=$1
    local name=$2
    
    if command -v $cmd &> /dev/null; then
        echo -e "${GREEN}✓${NC} $name: $(command -v $cmd)"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $name: NOT FOUND"
        ((FAILED++))
        return 1
    fi
}

# Function to test if file exists
test_file() {
    local file=$1
    local name=$2
    
    if [ -f "$file" ]; then
        echo -e "${GREEN}✓${NC} $name: $file"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $name: $file NOT FOUND"
        ((FAILED++))
        return 1
    fi
}

echo "1. Testing Core Components:"
echo "-------------------------------------------"
test_command sway "Sway"
test_command swayidle "SwayIdle"
test_command swaylock "SwayLock"
test_command swaybg "SwayBG"
test_command waybar "Waybar"
echo ""

echo "2. Testing Utilities:"
echo "-------------------------------------------"
test_command foot "Foot Terminal"
test_command wmenu "Wmenu Launcher"
test_command wl-copy "Wl-Clipboard"
test_command grim "Grim Screenshot"
test_command slurp "Slurp Selection"
test_command mako "Mako Notifier"
echo ""

echo "3. Testing Nested Mode Tools:"
echo "-------------------------------------------"
test_command weston "Weston"
test_command cage "Cage"
test_command Xwayland "XWayland"
echo ""

echo "4. Testing Configuration Files:"
echo "-------------------------------------------"
CONFIG_DIR="/workspace/sway-config"
test_file "$CONFIG_DIR/config" "Sway Config"
test_file "$CONFIG_DIR/launch-sway-nested-weston.sh" "Weston Launch Script"
test_file "$CONFIG_DIR/launch-sway-nested-cage.sh" "Cage Launch Script"
test_file "$CONFIG_DIR/launch-sway-x11.sh" "X11 Launch Script"
test_file "$CONFIG_DIR/launch-sway-headless.sh" "Headless Launch Script"
test_file "$CONFIG_DIR/install-config.sh" "Install Script"
echo ""

echo "5. Testing Script Permissions:"
echo "-------------------------------------------"
for script in "$CONFIG_DIR"/*.sh; do
    if [ -x "$script" ]; then
        echo -e "${GREEN}✓${NC} Executable: $(basename $script)"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} Not executable: $(basename $script)"
        ((FAILED++))
    fi
done
echo ""

echo "6. Validating Sway Configuration:"
echo "-------------------------------------------"
export XDG_RUNTIME_DIR=/tmp/runtime-$(id -u)
mkdir -p $XDG_RUNTIME_DIR 2>/dev/null
chmod 700 $XDG_RUNTIME_DIR 2>/dev/null

if sway -C -c "$CONFIG_DIR/config" 2>&1 | grep -q "The config"; then
    echo -e "${GREEN}✓${NC} Sway config validation passed"
    ((PASSED++))
else
    # Try to run validation
    OUTPUT=$(sway -C -c "$CONFIG_DIR/config" 2>&1)
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓${NC} Sway config validation passed"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} Sway config validation failed:"
        echo "$OUTPUT"
        ((FAILED++))
    fi
fi
echo ""

echo "7. Checking Sway Version:"
echo "-------------------------------------------"
SWAY_VERSION=$(sway --version 2>&1)
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} $SWAY_VERSION"
    ((PASSED++))
else
    echo -e "${RED}✗${NC} Could not get Sway version"
    ((FAILED++))
fi
echo ""

echo "8. Checking Weston Version:"
echo "-------------------------------------------"
WESTON_VERSION=$(weston --version 2>&1)
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} $WESTON_VERSION"
    ((PASSED++))
else
    echo -e "${RED}✗${NC} Could not get Weston version"
    ((FAILED++))
fi
echo ""

echo "============================================"
echo "  Test Results"
echo "============================================"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    echo ""
    echo "Installation is complete and ready to use!"
    echo ""
    echo "Next steps:"
    echo "1. Install config: cd /workspace/sway-config && ./install-config.sh"
    echo "2. Run nested Sway using one of the launch scripts:"
    echo "   - ./launch-sway-nested-weston.sh (Wayland)"
    echo "   - ./launch-sway-nested-cage.sh (Wayland)"
    echo "   - ./launch-sway-x11.sh (X11/Wayland)"
    echo "   - ./launch-sway-headless.sh (Virtual display)"
    exit 0
else
    echo -e "${YELLOW}⚠ Some tests failed.${NC}"
    echo "Please review the errors above."
    exit 1
fi
