#!/bin/bash
# ============================================================================
# build_package.sh - VÍ DỤ 2: Script build và đóng gói hoàn chỉnh
# ============================================================================
#
# Quy trình đóng gói:
# 1. Kiểm tra dependencies
# 2. Clean build
# 3. Configure với CMake
# 4. Build
# 5. Run tests (optional)
# 6. Create package (DEB/RPM/TGZ)
# 7. Verify package
#

set -e

# ============================================================================
# CONFIGURATION
# ============================================================================
PROJECT_NAME="autosar_adaptive_dashboard"
PROJECT_VERSION="1.0.0"
BUILD_DIR="build"
PACKAGE_DIR="packages"
BUILD_TYPE="Release"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ============================================================================
# FUNCTIONS
# ============================================================================

print_header() {
    echo -e "${BLUE}"
    echo "============================================================================"
    echo "  $1"
    echo "============================================================================"
    echo -e "${NC}"
}

print_step() {
    echo -e "${GREEN}[STEP]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

check_command() {
    if ! command -v $1 &> /dev/null; then
        print_error "$1 is not installed"
        exit 1
    fi
    echo "  ✓ $1"
}

# ============================================================================
# PARSE ARGUMENTS
# ============================================================================
CLEAN_BUILD=false
BUILD_TESTS=false
CREATE_DEB=false
CREATE_RPM=false
CREATE_TGZ=true
SKIP_CHECKS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --with-tests)
            BUILD_TESTS=true
            shift
            ;;
        --deb)
            CREATE_DEB=true
            shift
            ;;
        --rpm)
            CREATE_RPM=true
            shift
            ;;
        --skip-checks)
            SKIP_CHECKS=true
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --clean       Clean build directory before building"
            echo "  --with-tests  Build and run tests"
            echo "  --deb         Create DEB package (requires dpkg)"
            echo "  --rpm         Create RPM package (requires rpmbuild)"
            echo "  --skip-checks Skip dependency checks"
            echo "  --debug       Build in Debug mode"
            echo "  --help        Show this help"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# ============================================================================
# MAIN
# ============================================================================

print_header "AUTOSAR Adaptive Dashboard - Build & Package"

echo "Configuration:"
echo "  Project: $PROJECT_NAME"
echo "  Version: $PROJECT_VERSION"
echo "  Build type: $BUILD_TYPE"
echo "  Clean build: $CLEAN_BUILD"
echo "  Build tests: $BUILD_TESTS"
echo ""

# ----------------------------------------------------------------------------
# Step 1: Check dependencies
# ----------------------------------------------------------------------------
if [ "$SKIP_CHECKS" = false ]; then
    print_step "Checking dependencies..."
    
    check_command cmake
    check_command g++
    check_command make
    
    # Check vsomeip
    if pkg-config --exists vsomeip3 2>/dev/null; then
        VSOMEIP_VERSION=$(pkg-config --modversion vsomeip3)
        echo "  ✓ vsomeip3 ($VSOMEIP_VERSION)"
    elif [ -f /usr/local/lib/libvsomeip3.so ]; then
        echo "  ✓ vsomeip3 (manual installation)"
    else
        print_error "vsomeip3 not found"
        exit 1
    fi
    
    # Check Boost
    if dpkg -l | grep -q libboost-dev 2>/dev/null; then
        echo "  ✓ Boost"
    elif [ -f /usr/include/boost/version.hpp ]; then
        echo "  ✓ Boost"
    else
        print_warning "Boost not found via dpkg, may be installed manually"
    fi
    
    # Check Qt5
    if command -v qmake &> /dev/null; then
        QT_VERSION=$(qmake -version | grep "Qt version" | awk '{print $4}')
        echo "  ✓ Qt5 ($QT_VERSION)"
    else
        print_warning "Qt5 not found - Qt client will not be built"
    fi
    
    echo ""
fi

# ----------------------------------------------------------------------------
# Step 2: Clean build (optional)
# ----------------------------------------------------------------------------
if [ "$CLEAN_BUILD" = true ]; then
    print_step "Cleaning build directory..."
    rm -rf $BUILD_DIR
    echo "  Done"
    echo ""
fi

# ----------------------------------------------------------------------------
# Step 3: Create build directory
# ----------------------------------------------------------------------------
print_step "Creating build directory..."
mkdir -p $BUILD_DIR
mkdir -p $PACKAGE_DIR
echo "  Done"
echo ""

# ----------------------------------------------------------------------------
# Step 4: Configure with CMake
# ----------------------------------------------------------------------------
print_step "Configuring with CMake..."
cd $BUILD_DIR

CMAKE_ARGS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DCMAKE_INSTALL_PREFIX=/usr/local"
)

if [ "$BUILD_TESTS" = true ]; then
    CMAKE_ARGS+=("-DBUILD_TESTS=ON")
fi

cmake .. "${CMAKE_ARGS[@]}"

echo "  Done"
echo ""
cd ..

# ----------------------------------------------------------------------------
# Step 5: Build
# ----------------------------------------------------------------------------
print_step "Building project..."
cd $BUILD_DIR
make -j$(nproc)
echo "  Done"
echo ""
cd ..

# ----------------------------------------------------------------------------
# Step 6: Run tests (optional)
# ----------------------------------------------------------------------------
if [ "$BUILD_TESTS" = true ]; then
    print_step "Running tests..."
    cd $BUILD_DIR
    ctest --output-on-failure
    echo "  Done"
    echo ""
    cd ..
fi

# ----------------------------------------------------------------------------
# Step 7: Create packages
# ----------------------------------------------------------------------------
print_step "Creating packages..."

cd $BUILD_DIR

# Create TGZ package
if [ "$CREATE_TGZ" = true ]; then
    echo "  Creating TGZ package..."
    
    PACKAGE_NAME="${PROJECT_NAME}-${PROJECT_VERSION}-linux-$(uname -m)"
    mkdir -p $PACKAGE_NAME/bin
    mkdir -p $PACKAGE_NAME/etc
    mkdir -p $PACKAGE_NAME/doc
    
    # Copy binaries
    cp bin/* $PACKAGE_NAME/bin/ 2>/dev/null || true
    
    # Copy configs
    cp bin/*.json $PACKAGE_NAME/etc/ 2>/dev/null || true
    
    # Copy docs
    cp ../README.md $PACKAGE_NAME/doc/ 2>/dev/null || true
    
    # Create run script
    cat > $PACKAGE_NAME/run_demo.sh << 'SCRIPT'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export VSOMEIP_CONFIGURATION=$SCRIPT_DIR/etc/vsomeip.json
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

echo "Starting Dashboard Service..."
$SCRIPT_DIR/bin/dashboard_service &
SERVICE_PID=$!

sleep 2

echo "Starting Dashboard Client..."
$SCRIPT_DIR/bin/dashboard_client_qt &
CLIENT_PID=$!

trap "kill $CLIENT_PID $SERVICE_PID 2>/dev/null" EXIT
wait
SCRIPT
    chmod +x $PACKAGE_NAME/run_demo.sh
    
    # Create tarball
    tar -czf ../$PACKAGE_DIR/$PACKAGE_NAME.tar.gz $PACKAGE_NAME
    rm -rf $PACKAGE_NAME
    
    print_success "Created: $PACKAGE_DIR/$PACKAGE_NAME.tar.gz"
fi

# Create DEB package
if [ "$CREATE_DEB" = true ]; then
    echo "  Creating DEB package..."
    cpack -G DEB
    mv *.deb ../$PACKAGE_DIR/ 2>/dev/null || true
    print_success "Created DEB package in $PACKAGE_DIR/"
fi

# Create RPM package
if [ "$CREATE_RPM" = true ]; then
    echo "  Creating RPM package..."
    cpack -G RPM
    mv *.rpm ../$PACKAGE_DIR/ 2>/dev/null || true
    print_success "Created RPM package in $PACKAGE_DIR/"
fi

cd ..

# ----------------------------------------------------------------------------
# Step 8: Summary
# ----------------------------------------------------------------------------
echo ""
print_header "Build Summary"

echo "Build artifacts:"
echo "  - $BUILD_DIR/bin/ (executables)"
echo ""
echo "Packages created:"
ls -la $PACKAGE_DIR/ 2>/dev/null || echo "  (none)"
echo ""

print_success "Build complete!"
echo ""
echo "To run the demo:"
echo "  cd $BUILD_DIR/bin"
echo "  export VSOMEIP_CONFIGURATION=\$(pwd)/vsomeip.json"
echo "  ./dashboard_service &"
echo "  ./dashboard_client_qt"
