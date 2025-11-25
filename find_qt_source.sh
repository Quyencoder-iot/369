#!/bin/bash

# Script để tìm Qt source files trên hệ thống
# Usage: ./find_qt_source.sh

echo "=== Qt Source Code Finder ==="
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to find Qt installation
find_qt_installation() {
    echo "Searching for Qt installation..."
    
    # Common Qt installation paths
    QT_PATHS=(
        "$HOME/Qt"
        "/opt/qt"
        "/usr/local/qt"
        "/usr/lib/qt"
        "/usr/share/qt"
        "$(qmake -query QT_INSTALL_PREFIX 2>/dev/null)"
    )
    
    QT_INSTALL=""
    for path in "${QT_PATHS[@]}"; do
        if [ -d "$path" ]; then
            echo -e "${GREEN}Found Qt at: $path${NC}"
            QT_INSTALL="$path"
            break
        fi
    done
    
    if [ -z "$QT_INSTALL" ]; then
        echo -e "${YELLOW}Qt installation not found in common paths${NC}"
        echo "Trying qmake..."
        if command -v qmake &> /dev/null; then
            QT_INSTALL=$(qmake -query QT_INSTALL_PREFIX 2>/dev/null)
            if [ -n "$QT_INSTALL" ]; then
                echo -e "${GREEN}Found Qt via qmake: $QT_INSTALL${NC}"
            fi
        fi
    fi
    
    echo ""
    return 0
}

# Function to find source files
find_source_files() {
    local qt_path=$1
    local component=$2
    
    echo "Searching for $component source files..."
    
    # Possible source locations
    SOURCE_LOCATIONS=(
        "$qt_path/Src/qtbase/src/gui/kernel"
        "$qt_path/qtbase/src/gui/kernel"
        "$qt_path/src/gui/kernel"
        "/usr/src/qt/qtbase/src/gui/kernel"
    )
    
    case $component in
        "QGuiApplication")
            for loc in "${SOURCE_LOCATIONS[@]}"; do
                if [ -f "$loc/qguiapplication.h" ]; then
                    echo -e "${GREEN}Found QGuiApplication header: $loc/qguiapplication.h${NC}"
                    echo -e "${GREEN}Found QGuiApplication source: $loc/qguiapplication.cpp${NC}"
                    return 0
                fi
            done
            ;;
        "QQmlApplicationEngine")
            QML_LOCATIONS=(
                "$qt_path/Src/qtdeclarative/src/qml/qml"
                "$qt_path/qtdeclarative/src/qml/qml"
                "$qt_path/src/qml/qml"
                "/usr/src/qt/qtdeclarative/src/qml/qml"
            )
            for loc in "${QML_LOCATIONS[@]}"; do
                if [ -f "$loc/qqmlapplicationengine.h" ]; then
                    echo -e "${GREEN}Found QQmlApplicationEngine header: $loc/qqmlapplicationengine.h${NC}"
                    echo -e "${GREEN}Found QQmlApplicationEngine source: $loc/qqmlapplicationengine.cpp${NC}"
                    return 0
                fi
            done
            ;;
        "QQmlContext")
            QML_LOCATIONS=(
                "$qt_path/Src/qtdeclarative/src/qml/qml"
                "$qt_path/qtdeclarative/src/qml/qml"
                "$qt_path/src/qml/qml"
                "/usr/src/qt/qtdeclarative/src/qml/qml"
            )
            for loc in "${QML_LOCATIONS[@]}"; do
                if [ -f "$loc/qqmlcontext.h" ]; then
                    echo -e "${GREEN}Found QQmlContext header: $loc/qqmlcontext.h${NC}"
                    echo -e "${GREEN}Found QQmlContext source: $loc/qqmlcontext.cpp${NC}"
                    return 0
                fi
            done
            ;;
    esac
    
    echo -e "${YELLOW}Source files not found locally${NC}"
    echo "You may need to:"
    echo "  1. Install Qt source packages"
    echo "  2. Clone from https://code.qt.io/"
    echo "  3. Download from https://www.qt.io/download-open-source"
    return 1
}

# Function to show online resources
show_online_resources() {
    echo ""
    echo "=== Online Resources ==="
    echo ""
    echo "Qt Source Code (Git):"
    echo "  - QGuiApplication: https://code.qt.io/cgit/qt/qtbase.git/tree/src/gui/kernel/qguiapplication.h"
    echo "  - QQmlApplicationEngine: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/src/qml/qml/qqmlapplicationengine.h"
    echo "  - QQmlContext: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/src/qml/qml/qqmlcontext.h"
    echo ""
    echo "Qt Documentation:"
    echo "  - QGuiApplication: https://doc.qt.io/qt-6/qguiapplication.html"
    echo "  - QQmlApplicationEngine: https://doc.qt.io/qt-6/qqmlapplicationengine.html"
    echo "  - QQmlContext: https://doc.qt.io/qt-6/qqmlcontext.html"
    echo ""
    echo "Browse Source Online:"
    echo "  - https://code.qt.io/cgit/qt/qtbase.git/"
    echo "  - https://code.qt.io/cgit/qt/qtdeclarative.git/"
    echo ""
}

# Main execution
main() {
    find_qt_installation
    
    if [ -n "$QT_INSTALL" ]; then
        echo "=== Searching for Source Files ==="
        echo ""
        find_source_files "$QT_INSTALL" "QGuiApplication"
        echo ""
        find_source_files "$QT_INSTALL" "QQmlApplicationEngine"
        echo ""
        find_source_files "$QT_INSTALL" "QQmlContext"
        echo ""
    fi
    
    show_online_resources
    
    echo "=== Tips ==="
    echo ""
    echo "1. Use Qt Creator to browse source:"
    echo "   - Open Qt source in Qt Creator"
    echo "   - Use F2 to follow symbols"
    echo "   - Use Ctrl+K to locate in file"
    echo ""
    echo "2. Use online browser:"
    echo "   - Visit https://code.qt.io/"
    echo "   - Search for class names"
    echo "   - Browse source directly"
    echo ""
    echo "3. Clone Qt source:"
    echo "   git clone https://code.qt.io/qt/qt5.git"
    echo "   cd qt5"
    echo "   git submodule update --init --recursive"
    echo ""
}

main
