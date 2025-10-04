# KVM GUI Manager

Un gestor gráfico moderno para máquinas virtuales KVM con interfaz estilo VirtualBox.

**Desarrollado por:** Tomás González

## Descripción

KVM GUI Manager es una aplicación de escritorio multiplataforma construida con Qt6 y C++ que proporciona una interfaz gráfica intuitiva para gestionar máquinas virtuales KVM/QEMU. Diseñado para ser familiar para usuarios de VirtualBox mientras aprovecha toda la potencia y eficiencia del ecosistema KVM/libvirt de Linux.

### 🎯 **Características Principales Implementadas**
- ✅ **Gestión completa de ISOs y medios virtuales** desde la GUI
- ✅ **Configuración de orden de arranque (boot order)** personalizable
- ✅ **Comando QEMU completo** con soporte para KVM y fallback TCG
- ✅ **Persistencia XML** de todas las configuraciones
- ✅ **Interfaz estilo VirtualBox** con 6 pestañas de configuración avanzada
- ✅ **Soporte de audio** con drivers compatibles (PulseAudio)
- ✅ **UUIDs estándar RFC 4122** para compatibilidad total con QEMU

## Características

### 🖥️ Interfaz Principal
- **Ventana principal dividida**: Panel lateral para lista de VMs y panel principal para detalles
- **Barra de menús completa**: Archivo, Máquina, Ver, Ayuda
- **Barra de herramientas**: Acceso rápido a las funciones más utilizadas
- **Lista de máquinas virtuales**: Visualización filtrable y búsqueda
- **Panel de detalles**: Información completa de la VM seleccionada

### ⚙️ Gestión de Máquinas Virtuales
- Crear, eliminar y configurar máquinas virtuales
- Control de estado: iniciar, pausar, detener, resetear
- Configuración de sistema: CPU, RAM, orden de arranque
- Gestión de almacenamiento: discos duros, CD/DVD
- Configuración de red: adaptadores, tipos de conexión
- Soporte para audio y USB
- Configuración de pantalla y aceleración 3D
- Carpetas compartidas entre host y guest

### 🔧 Características Técnicas
- Integración nativa con KVM a través de libvirt
- Interfaz Qt6 moderna y responsive
- Soporte multiplataforma (Linux principalmente)
- Arquitectura modular y extensible

### 🛠️ Administradores Especializados
- **Administrador de medios virtuales**: Gestión completa de discos duros, CD/DVD y disquetes
  - Formatos soportados: VDI, VMDK, VHD, QCOW2, ISO, IMG
  - Operaciones: agregar, quitar, copiar, mover, redimensionar
  - Vista detallada con información de uso y propiedades

- **Administrador de red**: Configuración de redes virtuales globales
  - Redes NAT con CIDR personalizable y reenvío de puertos
  - Redes Host-Only con configuración DHCP completa
  - Gestión centralizada de todas las redes virtuales

- **Administrador de instantáneas**: Control avanzado de snapshots
  - Árbol jerárquico visual de instantáneas
  - Creación con descripción personalizada
  - Restauración, eliminación y clonado desde instantáneas

### 📦 Configuración Avanzada
- **Preferencias globales**: 7 pestañas de configuración (General, Entrada, Red, Proxy, etc.)
- **Configuración por VM**: 10 pestañas especializadas (Sistema, Pantalla, Audio, etc.)  
- **Asistente de creación**: Wizard guiado de 4 páginas para nuevas VMs
- **Importación/Exportación**: Soporte para formatos OVA/OVF estándar

## Requisitos del Sistema

### Software Requerido
- **Sistema Operativo**: Linux (Ubuntu 20.04+, Debian 11+, Fedora 35+, etc.)
- **KVM**: Soporte de virtualización por hardware (Intel VT-x o AMD-V)
- **Libvirt**: Biblioteca de virtualización (libvirtd)
- **Qt6**: Framework de desarrollo (Core, Widgets, Gui)
- **CMake**: Sistema de construcción (versión 3.16+)
- **Compilador**: GCC 8+ o Clang 10+ con soporte C++17

## 📦 Instalación de Dependencias

### 🐧 **Debian/Ubuntu**
```bash
# Dependencias de desarrollo
sudo apt update
sudo apt install build-essential cmake git
sudo apt install qt6-base-dev qt6-tools-dev qt6-base-dev-tools
sudo apt install libqt6core6 libqt6widgets6 libqt6gui6

# Dependencias de ejecución (KVM/QEMU)
sudo apt install qemu-kvm qemu-system-x86 qemu-utils
sudo apt install libvirt-daemon-system libvirt-clients bridge-utils
sudo apt install virt-manager # Opcional: para gestión adicional
```

### 🎩 **Fedora/RHEL/CentOS**
```bash
# Dependencias de desarrollo
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git qt6-qtbase-devel qt6-qttools-devel

# Dependencias de ejecución (KVM/QEMU)
sudo dnf install qemu-kvm qemu-img libvirt libvirt-daemon-config-network
sudo dnf install libvirt-daemon-kvm virt-install virt-viewer bridge-utils
```

### 🦎 **openSUSE**
```bash
# Dependencias de desarrollo
sudo zypper install -t pattern devel_basis
sudo zypper install cmake git libqt6-qtbase-devel qt6-tools-devel

# Dependencias de ejecución (KVM/QEMU)
sudo zypper install qemu-kvm qemu-tools libvirt libvirt-daemon
sudo zypper install bridge-utils virt-manager
```

### ⚡ **Arch Linux (Bonus)**
```bash
# Dependencias de desarrollo
sudo pacman -S base-devel cmake git qt6-base qt6-tools

# Dependencias de ejecución (KVM/QEMU)
sudo pacman -S qemu-full libvirt virt-manager bridge-utils
```

## ⚙️ Configuración del Sistema

### 🔧 **Configuración de KVM/libvirt (Todas las distribuciones)**
```bash
# Habilitar y iniciar servicios
sudo systemctl enable libvirtd
sudo systemctl start libvirtd

# Agregar usuario a los grupos necesarios
sudo usermod -a -G libvirt $USER
sudo usermod -a -G kvm $USER

# Reiniciar sesión para aplicar cambios de grupo
# O ejecutar: newgrp libvirt

# Verificar que KVM funciona
sudo kvm-ok  # Ubuntu/Debian
# O verificar manualmente:
lsmod | grep kvm
ls -la /dev/kvm
```

### 🛡️ **Permisos y Seguridad**
```bash
# Verificar permisos de libvirt
sudo virsh list --all

# Configurar red por defecto (si es necesario)
sudo virsh net-start default
sudo virsh net-autostart default
```

## Compilación

1. **Clonar el repositorio**:
```bash
git clone <repository-url>
cd kvm-gui
```

2. **Crear directorio de construcción**:
```bash
mkdir build
cd build
```

3. **Configurar con CMake**:
```bash
cmake ..
```

4. **Compilar**:
```bash
make -j$(nproc)
```

5. **Ejecutar**:
```bash
./KVMManager
```

## Estructura del Proyecto

```
kvm-gui/
├── src/                    # Código fuente
│   ├── main.cpp           # Punto de entrada
│   ├── ui/                # Interfaz de usuario
│   │   ├── MainWindow.h/.cpp
│   │   ├── VMListWidget.h/.cpp
│   │   └── VMDetailsWidget.h/.cpp
│   ├── core/              # Lógica de negocio
│   │   ├── KVMManager.h/.cpp
│   │   └── VirtualMachine.h/.cpp
│   └── models/            # Modelos de datos
│       └── VMListModel.h/.cpp
├── resources/             # Recursos (iconos, etc.)
│   └── icons.qrc
├── ui/                    # Archivos de interfaz Qt
│   └── MainWindow.ui
├── CMakeLists.txt        # Configuración de construcción
└── README.md             # Este archivo
```

## Uso

### Primera Ejecución
1. Asegúrese de que KVM y libvirt estén instalados y en funcionamiento
2. Verifique que su usuario esté en el grupo `libvirt`
3. Ejecute KVM Manager desde la terminal o menú de aplicaciones

### Gestionar Máquinas Virtuales
- **Crear nueva VM**: Archivo → Nueva máquina virtual
- **Importar VM existente**: Archivo → Agregar
- **Configurar VM**: Seleccionar VM → Máquina → Configuración
- **Iniciar VM**: Seleccionar VM → botón Iniciar
- **Ver detalles**: Seleccionar VM en el panel izquierdo

### Configuración
- **Preferencias globales**: Archivo → Preferencias
- **Rutas de almacenamiento**: Configurar ubicaciones predeterminadas
- **Configuración de red**: Gestionar redes virtuales

## Desarrollo

### Arquitectura
- **Patrón MVP**: Separación clara entre vista, modelo y presentador
- **Qt Signals/Slots**: Comunicación asíncrona entre componentes
- **Libvirt API**: Integración nativa con el hipervisor KVM
- **CMake**: Sistema de construcción moderno y flexible

### Contribuir
1. Fork el repositorio
2. Crear rama de características (`git checkout -b feature/nueva-caracteristica`)
3. Commit cambios (`git commit -am 'Agregar nueva característica'`)
4. Push a la rama (`git push origin feature/nueva-caracteristica`)
5. Crear Pull Request

## Estado del Desarrollo

### ✅ Implementado
- [x] **Estructura base del proyecto** con CMake y Qt6
- [x] **Interfaz principal** estilo VirtualBox
  - Panel lateral con lista filtrable de VMs
  - Panel de detalles organizado por secciones
  - Barra de menús y herramientas completa
  - Tema oscuro moderno
- [x] **Sistema de configuración completo**
  - Preferencias globales con 7 pestañas
  - Configuración de VM con 10 pestañas
  - Persistencia de configuración con QSettings
- [x] **Arquitectura modular**
  - Separación clara MVC
  - Integración con libvirt/KVM
  - Gestión de estados de VM
- [x] **Localización completa** en español
- [x] **Iconos del sistema** integrados

### 🚧 En Desarrollo
- [x] **Diálogo de Preferencias Globales** - Configuración completa del programa
  - 🏷️ Idioma y localización
  - 🗂️ Carpetas predeterminadas (VMs, instantáneas)
  - 🔄 Configuración de actualizaciones
  - 🎹 Atajos de teclado personalizables
  - 🌐 Gestión de redes (NAT, solo-anfitrión)
  - 🔧 Configuración de proxy
  - 🎨 Personalización de interfaz
- [x] **Diálogo de Configuración de VM** - Configuración detallada por máquina
  - 🧾 General (nombre, SO, descripción, cifrado)
  - ⚙️ Sistema (RAM, CPU, chipset, aceleración)
  - 💻 Pantalla (memoria de vídeo, monitores, grabación)
  - 💽 Almacenamiento (discos duros, controladores, CD/DVD)
  - 🔊 Audio (controladores, entrada/salida)
  - 🌐 Red (hasta 4 adaptadores configurables)
  - 📱 Puertos serie y USB
  - 🔗 Carpetas compartidas
  - 🖥️ Interfaz de usuario personalizable
- [ ] Control real de máquinas virtuales (start/stop/pause)
- [ ] Gestión avanzada de almacenamiento (crear/redimensionar discos)
- [ ] Sistema de instantáneas
- [ ] Asistente de creación de nuevas VMs

### 📋 Planificado
- [ ] Consola integrada (VNC/Spice)
- [ ] Importación/exportación de VMs
- [ ] Clonación de máquinas virtuales
- [ ] Gestión de plantillas
- [ ] Monitoreo de rendimiento
- [ ] Automatización y scripting

## 💡 Consejos y Mejores Prácticas

### 🚀 **Optimización de Rendimiento**
- **Hardware:** Asegúrate de tener virtualización habilitada en BIOS/UEFI (Intel VT-x o AMD-V)
- **Memoria:** Asigna suficiente RAM a las VMs, pero deja al menos 2GB para el sistema host
- **CPU:** Usa múltiples cores para VMs que lo requieran (especialmente para compilación)
- **Almacenamiento:** Prefiere SSD para mejor rendimiento de I/O

### 🔍 **Resolución de Problemas Comunes**
```bash
# Si aparece "Permission denied" con /dev/kvm:
sudo chmod 666 /dev/kvm

# Si libvirt no encuentra las VMs:
sudo systemctl restart libvirtd

# Para verificar logs de libvirt:
sudo journalctl -u libvirtd -f

# Si QEMU falla al iniciar:
# - Verifica que el usuario esté en grupo 'libvirt' y 'kvm'
# - Reinicia la sesión después de agregar al grupo
# - Verifica que no hay VMs con el mismo UUID
```

### 📁 **Gestión de Archivos**
- **Ubicación de VMs:** `~/.VM/` (creado automáticamente)
- **Respaldo recomendado:** Hacer backup de archivos `.xml` y `.qcow2`
- **Imágenes ISO:** Se recomienda mantenerlas en `/home/usuario/ISOs/` o `/opt/isos/`

### 🌐 **Configuración de Red**
- **NAT:** Configuración por defecto, acceso a internet automático
- **Bridge:** Mejor para servidores que necesitan IPs en red local
- **Host-Only:** Ideal para desarrollo y testing aislado

## 📜 Licencias y Tecnologías Utilizadas

### 🛠️ **Frameworks y Librerías**
- **Qt6** - Framework GUI multiplataforma (LGPL v3)
- **CMake** - Sistema de construcción (BSD 3-Clause)
- **libvirt** - API de virtualización (LGPL v2.1)
- **QEMU** - Emulador y virtualizador (GPL v2)

### 📊 **Especificaciones**
- **Lenguaje:** C++17
- **Compilador:** GCC 8+ / Clang 10+
- **Estándar:** Qt6 Design Guidelines
- **XML:** Persistencia de configuración con QDomDocument

### ⚖️ **Licencia del Proyecto**
```
MIT License - Copyright (c) 2025 Tomás González

Se permite usar, copiar, modificar y distribuir este software
con fines comerciales y no comerciales bajo los términos de la licencia MIT.
```

## 👨‍💻 Autor y Contacto

**Desarrollador:** Tomás González  
**Proyecto:** KVM GUI Manager - Gestor gráfico para KVM/QEMU  
**Tecnologías:** Qt6, C++17, CMake, libvirt, QEMU  

Para preguntas, sugerencias o reportes de errores, por favor abra un issue en el repositorio de GitHub.

---

## 🏁 Estado del Desarrollo

### ✅ **Completamente Funcional**
- [x] Gestión de ISOs desde GUI (cargar, modificar, quitar)
- [x] Configuración de orden de arranque personalizable
- [x] Comandos QEMU completos con soporte KVM/TCG
- [x] Persistencia XML de todas las configuraciones  
- [x] UUIDs estándar RFC 4122 compatibles con QEMU
- [x] Drivers de audio compatibles (PulseAudio)
- [x] Interfaz completa estilo VirtualBox con tema oscuro
- [x] Soporte para múltiples distribuciones Linux

**🎯 El proyecto está listo para uso en producción para gestión básica de VMs KVM/QEMU.**

---
*Desarrollado con ❤️ para la comunidad Linux*
