# 🔨 Hướng Dẫn Build Qt Models Examples

## ✅ Tương Thích

Code này **hoàn toàn tương thích** với:
- ✅ **Qt 5.x** (Qt 5.6 trở lên)
- ✅ **Qt 6.x** (tất cả versions)

CMakeLists.txt sẽ tự động phát hiện và sử dụng Qt version có sẵn trên hệ thống của bạn.

---

## 📋 Yêu Cầu

### Tối Thiểu:
- **CMake**: 3.5 trở lên
- **Qt**: 5.6 trở lên (hoặc Qt 6.x)
- **C++ Compiler**: Hỗ trợ C++11
- **Qt Modules**: Core, Widgets, Sql

### Cài Đặt Qt

#### Ubuntu/Debian:
```bash
# Qt5
sudo apt-get install qt5-default qtbase5-dev libqt5sql5-sqlite

# Hoặc Qt6
sudo apt-get install qt6-base-dev libqt6sql6-sqlite
```

#### Fedora/RHEL:
```bash
# Qt5
sudo dnf install qt5-qtbase-devel qt5-qtbase-gui

# Hoặc Qt6
sudo dnf install qt6-qtbase-devel
```

#### macOS (với Homebrew):
```bash
# Qt5
brew install qt@5

# Hoặc Qt6
brew install qt@6
```

#### Windows:
- Download Qt từ: https://www.qt.io/download
- Chọn Qt 5.x hoặc Qt 6.x
- Install với Qt Creator

---

## 🚀 Build với Qt5

### Option 1: Tự Động (CMake tìm Qt5)

```bash
cd examples
mkdir build
cd build
cmake ..
make
```

### Option 2: Chỉ Định Qt5 Path

```bash
cd examples
mkdir build
cd build

# Linux/macOS
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/5.x.x/gcc_64
make

# Windows (PowerShell)
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"
cmake --build . --config Release
```

### Option 3: Dùng Qt5 khi có cả Qt5 và Qt6

```bash
cd examples
mkdir build
cd build

# Force Qt5
cmake .. -DQT_VERSION_MAJOR=5
make
```

---

## 🚀 Build với Qt6

### Tự Động:

```bash
cd examples
mkdir build
cd build

# CMake sẽ ưu tiên Qt6 nếu có
cmake ..
make
```

### Chỉ Định Qt6:

```bash
cd examples
mkdir build
cd build

# Linux/macOS
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
make

# Windows
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64"
cmake --build . --config Release
```

---

## 🎯 Chạy Examples

Sau khi build thành công:

```bash
# Từ thư mục build/
./example_01_list_model
./example_02_table_model
./example_03_tree_model
./example_04_proxy_model
./example_05_qsql_models
./example_06_complete

# Trên Windows:
example_01_list_model.exe
example_02_table_model.exe
# ... etc
```

---

## 🔍 Kiểm Tra Qt Version

Để biết CMake đang dùng Qt nào:

```bash
cmake .. 2>&1 | grep "Qt version"

# Output sẽ hiện:
# -- Qt version: 5.15.2
# hoặc
# -- Qt version: 6.5.0
```

---

## 📝 Build Examples Riêng Lẻ

Nếu chỉ muốn build 1 example:

```bash
cd examples
mkdir build && cd build
cmake ..

# Build chỉ 1 target
make example_01_list_model

# Hoặc trên Windows
cmake --build . --target example_01_list_model
```

---

## 🐛 Troubleshooting

### Lỗi: "Qt5 (or Qt6) could not be found"

**Giải pháp:**
```bash
# Tìm Qt trên hệ thống
# Linux:
find /usr -name "Qt5Config.cmake" 2>/dev/null
find /opt -name "Qt5Config.cmake" 2>/dev/null

# macOS:
find /usr/local -name "Qt5Config.cmake" 2>/dev/null

# Sau đó set path:
export CMAKE_PREFIX_PATH=/path/to/Qt/5.x.x/gcc_64
cmake ..
```

### Lỗi: "Could not find a package configuration file Qt5Sql"

**Nguyên nhân:** Thiếu Qt Sql module

**Giải pháp:**
```bash
# Ubuntu/Debian
sudo apt-get install libqt5sql5 libqt5sql5-sqlite

# Fedora
sudo dnf install qt5-qtbase-devel

# macOS
brew reinstall qt@5
```

### Lỗi Build trên Windows

**Giải pháp:**
```bash
# Đảm bảo dùng đúng compiler
# Nếu cài Qt với MSVC, dùng:
cmake .. -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"

# Nếu cài Qt với MinGW, dùng:
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/mingw81_64"
```

### Library không tìm thấy khi chạy (Linux)

**Giải pháp:**
```bash
# Set library path
export LD_LIBRARY_PATH=/path/to/Qt/5.x.x/gcc_64/lib:$LD_LIBRARY_PATH

# Hoặc thêm vào ~/.bashrc
echo 'export LD_LIBRARY_PATH=/path/to/Qt/5.x.x/gcc_64/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

### Library không tìm thấy khi chạy (macOS)

**Giải pháp:**
```bash
export DYLD_LIBRARY_PATH=/path/to/Qt/5.x.x/clang_64/lib:$DYLD_LIBRARY_PATH
```

---

## 🎨 Build với Qt Creator

### Cách 1: Mở Project

1. Mở Qt Creator
2. File → Open File or Project
3. Chọn `examples/CMakeLists.txt`
4. Select Kit (Qt 5.x hoặc Qt 6.x)
5. Configure Project
6. Build (Ctrl+B)
7. Run (Ctrl+R)

### Cách 2: Tạo .pro File (Cho Qt5 truyền thống)

Nếu muốn dùng qmake thay vì CMake:

```bash
cd examples
qmake -project
# Sau đó edit file .pro và thêm:
# QT += widgets sql
# CONFIG += c++11

qmake
make
```

---

## 📊 Thông Tin Build

Sau khi chạy cmake, bạn sẽ thấy:

```
============================================
Building Qt Models Examples
Qt version: 5.15.2
Qt major version: 5
Build type: Debug
============================================
```

---

## 🔄 Clean Build

Nếu gặp vấn đề, thử clean build:

```bash
cd examples
rm -rf build
mkdir build
cd build
cmake ..
make
```

---

## 📦 Install (Optional)

Để cài đặt examples vào hệ thống:

```bash
cd examples/build
sudo make install

# Mặc định cài vào /usr/local/bin
# Có thể đổi bằng:
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/qt-examples
make
sudo make install
```

---

## 🧪 Test Build

Test nhanh xem build có OK không:

```bash
cd examples/build
./example_01_list_model &
sleep 2
killall example_01_list_model
echo "Build OK!"
```

---

## 💡 Tips

### 1. Build Nhanh với Ninja (nếu có)

```bash
cmake .. -G Ninja
ninja
```

### 2. Build Release (Faster)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Linux/macOS
```

### 3. Verbose Build (Debug)

```bash
make VERBOSE=1
```

### 4. Parallel Build

```bash
# Linux/macOS
make -j$(nproc)

# Windows
cmake --build . --parallel 8
```

---

## 📚 Khác Biệt Qt5 vs Qt6

Code này tương thích với cả 2, nhưng lưu ý:

| Feature | Qt5 | Qt6 | Notes |
|---------|-----|-----|-------|
| QVector | ✅ | ⚠️ | Qt6 khuyến khích dùng QList |
| QRegularExpression | ✅ (5.0+) | ✅ | OK |
| QOverload | ✅ (5.7+) | ✅ | Code dùng static_cast (tương thích Qt5 cũ) |
| QStringList | ✅ | ✅ | OK |
| Signal/Slot | ✅ | ✅ | Dùng new syntax (Qt5 5.0+) |

---

## ✅ Checklist Build Thành Công

- [ ] CMake found Qt (5 hoặc 6)
- [ ] All 6 targets compiled
- [ ] No linker errors
- [ ] Programs run without crashing
- [ ] Can see GUI windows
- [ ] Database examples work (SQLite)

---

## 🆘 Vẫn Gặp Vấn Đề?

1. Kiểm tra Qt version: `qmake --version`
2. Kiểm tra CMake version: `cmake --version`
3. Đọc error message kỹ
4. Google error message + "Qt5" hoặc "Qt6"
5. Check Qt documentation: https://doc.qt.io

---

**Happy Building! 🎉**
