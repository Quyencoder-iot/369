#!/bin/bash
# Script build nhanh cho Qt Models Tutorial

echo "🔨 Building Qt Models Tutorial..."

# Tạo build directory nếu chưa có
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure với CMake
echo "📐 Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    echo "Hãy chắc chắn bạn đã cài Qt và set CMAKE_PREFIX_PATH"
    echo "Ví dụ: export CMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64"
    exit 1
fi

# Build
echo "🏗️ Building..."
make -j$(nproc) || cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build successful!"
echo ""
echo "📚 Để chạy các ví dụ:"
echo "   cd build"
echo "   ./qt_models_demo          # Main tutorial app"
echo "   ./list_model_example      # List model demo"
echo "   ./table_model_example     # Table model demo"
echo "   ./tree_model_example      # Tree model demo"
echo "   ./proxy_model_example     # Proxy model demo"
echo "   ./sql_model_example       # SQL models demo"
echo ""
echo "📖 Đọc tài liệu tại:"
echo "   docs/HUONG_DAN_QT_MODELS.md"
echo "   docs/MOI_QUAN_HE_MODELS.md"