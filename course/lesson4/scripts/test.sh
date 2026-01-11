#!/bin/bash
# ============================================
# TEST SCRIPT - Integration tests
# ============================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "=========================================="
echo "  AUTOSAR Dashboard - Integration Tests"
echo "=========================================="

# Configuration
export VSOMEIP_CONFIGURATION="$PROJECT_DIR/course/lesson2/example2/vsomeip.json"

SERVICE_DIR="$PROJECT_DIR/course/lesson2/example2/build"
PASSED=0
FAILED=0

# Test function
test_case() {
    local name="$1"
    local result="$2"
    
    if [ "$result" -eq 0 ]; then
        echo "[PASS] $name"
        ((PASSED++))
    else
        echo "[FAIL] $name"
        ((FAILED++))
    fi
}

# Check if binaries exist
echo ""
echo "1. Checking binaries..."
test_case "Dashboard Service exists" $([ -f "$SERVICE_DIR/dashboard_service" ] && echo 0 || echo 1)
test_case "Dashboard Client exists" $([ -f "$SERVICE_DIR/dashboard_client" ] && echo 0 || echo 1)

# Check configuration
echo ""
echo "2. Checking configuration..."
test_case "vsomeip.json exists" $([ -f "$PROJECT_DIR/course/lesson2/example2/vsomeip.json" ] && echo 0 || echo 1)
test_case "database.json exists" $([ -f "$PROJECT_DIR/course/lesson2/example2/database.json" ] && echo 0 || echo 1)

# Check JSON validity
echo ""
echo "3. Validating JSON files..."
python3 -c "import json; json.load(open('$PROJECT_DIR/course/lesson2/example2/database.json'))" 2>/dev/null
test_case "database.json valid" $?

python3 -c "import json; json.load(open('$PROJECT_DIR/course/lesson2/example2/vsomeip.json'))" 2>/dev/null
test_case "vsomeip.json valid" $?

# Integration test (if binaries exist)
if [ -f "$SERVICE_DIR/dashboard_service" ]; then
    echo ""
    echo "4. Running integration test..."
    
    cd "$SERVICE_DIR"
    ./dashboard_service ../database.json &
    SERVICE_PID=$!
    sleep 2
    
    if kill -0 $SERVICE_PID 2>/dev/null; then
        test_case "Service starts successfully" 0
        
        # Try to run client briefly
        timeout 3 ./dashboard_client &>/dev/null &
        CLIENT_PID=$!
        sleep 2
        
        if kill -0 $SERVICE_PID 2>/dev/null; then
            test_case "Service stable with client" 0
        else
            test_case "Service stable with client" 1
        fi
        
        kill $CLIENT_PID 2>/dev/null || true
    else
        test_case "Service starts successfully" 1
    fi
    
    kill $SERVICE_PID 2>/dev/null || true
fi

# Summary
echo ""
echo "=========================================="
echo "  Results: $PASSED passed, $FAILED failed"
echo "=========================================="

exit $FAILED
