cmake_minimum_required(VERSION 3.16)

project(KVMManager
    VERSION 1.0.0
    DESCRIPTION "KVM GUI Manager - A VirtualBox-like interface for KVM"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Gui
)

# Set Qt6 policies
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# Enable Qt6 MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(
    src
    src/ui
    src/core
    src/models
)

# Source files
set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/VMListWidget.cpp
    src/ui/VMDetailsWidget.cpp
    src/core/KVMManager.cpp
    src/core/VirtualMachine.cpp
    src/models/VMListModel.cpp
)

# Header files
set(HEADERS
    src/ui/MainWindow.h
    src/ui/VMListWidget.h
    src/ui/VMDetailsWidget.h
    src/core/KVMManager.h
    src/core/VirtualMachine.h
    src/models/VMListModel.h
)

# UI files (none - we create UI programmatically)
set(UI_FILES
)

# Resource files
set(RESOURCES
    resources/icons.qrc
)

# Create the executable
add_executable(KVMManager
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# Link Qt6 libraries
target_link_libraries(KVMManager
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# Set additional compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(KVMManager PRIVATE -Wall -Wextra -pedantic)
endif()

# Install rules
install(TARGETS KVMManager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)