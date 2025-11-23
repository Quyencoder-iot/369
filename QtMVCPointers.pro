# Qt Project File (.pro) cho qmake
# Dùng cho những ai thích qmake hơn cmake

TEMPLATE = subdirs

# Các sub-projects
SUBDIRS = \
    simple_model_example \
    custom_delegate_example \
    pointer_patterns

# Config chung
CONFIG += c++17
QT += core widgets

# Simple Model Example
simple_model_example.subdir = .
simple_model_example.target = simple_model_example
simple_model_example.sources = simple_model_example.cpp

# Custom Delegate Example  
custom_delegate_example.subdir = .
custom_delegate_example.target = custom_delegate_example
custom_delegate_example.sources = custom_delegate_example.cpp

# Pointer Patterns
pointer_patterns.subdir = .
pointer_patterns.target = pointer_patterns
pointer_patterns.sources = pointer_patterns.cpp

message("🚀 HƯỚNG DẪN BUILD VỚI QMAKE:")
message("   qmake QtMVCPointers.pro")
message("   make")
message("")
message("📝 Hoặc build từng file riêng:")
message("   qmake -project simple_model_example.cpp")
message("   qmake")
message("   make")
