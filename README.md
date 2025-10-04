# KVM Manager

Un gestor gráfico moderno para máquinas virtuales KVM, inspirado en VirtualBox.

## Descripción

KVM Manager es una aplicación de escritorio construida con Qt6 y C++ que proporciona una interfaz gráfica intuitiva para gestionar máquinas virtuales KVM. Está diseñado para ser familiar para usuarios de VirtualBox mientras aprovecha la potencia y eficiencia de KVM.

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

### Dependencias de Desarrollo
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev libvirt-dev

# Fedora
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qttools-devel libvirt-devel

# Arch Linux
sudo pacman -S base-devel cmake qt6-base qt6-tools libvirt
```

### Dependencias de Ejecución
```bash
# Instalar KVM y libvirt
sudo apt install qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils

# Habilitar y iniciar servicios
sudo systemctl enable libvirtd
sudo systemctl start libvirtd

# Agregar usuario al grupo libvirt
sudo usermod -a -G libvirt $USER
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

## Licencia

Este proyecto está licenciado bajo la Licencia MIT. Ver el archivo `LICENSE` para más detalles.

## Contacto

Para preguntas, sugerencias o reportes de errores, por favor abra un issue en el repositorio de GitHub.

---

**Nota**: Este es un proyecto en desarrollo temprano. Algunas características pueden no estar completamente implementadas. Se recomienda usar en entornos de desarrollo/prueba solamente.# kvm-gui
