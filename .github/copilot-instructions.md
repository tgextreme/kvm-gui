# KVM GUI Manager - Copilot Instructions

This is a C++ Qt6 KVM GUI Manager project similar to VirtualBox with the following completed features:

## Project Structure
- [x] Complete C++ Qt6 project structure with CMakeLists.txt
- [x] Source files: main.cpp, MainWindow, VMListWidget, VMDetailsWidget
- [x] Core classes: KVMManager, VirtualMachine, VMXmlManager
- [x] Model classes: VMListModel
- [x] Resource files: icons.qrc
- [x] Documentation: README.md

## Project Features
- [x] VirtualBox-like interface with menu bar, toolbar, VM list panel, details panel
- [x] Spanish localization for UI elements
- [x] Dark theme applied
- [x] KVM/libvirt integration implemented
- [x] XML-based persistence system for VM configurations in ~/.VM folder
- [x] Modular architecture with clear separation of concerns

## Build Status
- [x] Qt6 dependencies installed and configured
- [x] CMake configuration completed
- [x] Make build completed without errors
- [x] Executable KVMManager created in build/ directory
- [x] XML persistence system fully functional

## Key Components
- **VMXmlManager**: Handles XML persistence for VM configurations
- **KVMManager**: Core management with XML integration
- **VM Configuration**: Complete dialog system with XML saving
- **Automatic folder creation**: ~/.VM folder created on first run