#include "VMCreationWizard.h"
#include "../core/KVMManager.h"

#include <QApplication>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QRandomGenerator>
#include <QSysInfo>
#include <QStorageInfo>

// Constructor principal del wizard
VMCreationWizard::VMCreationWizard(KVMManager *kvmManager, QWidget *parent)
    : QWizard(parent)
    , m_kvmManager(kvmManager)
{
    setWindowTitle(tr("Asistente para Nueva Máquina Virtual"));
    setWindowIcon(QIcon(":/icons/new-vm.png"));
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::HaveHelpButton, true);
    setOption(QWizard::HaveFinishButtonOnEarlyPages, false);
    setMinimumSize(800, 600);
    
    setupWizardPages();
    
    connect(this, &QWizard::currentIdChanged, this, &VMCreationWizard::onCurrentIdChanged);
}

void VMCreationWizard::setupWizardPages()
{
    setPage(WelcomePageId, new VMWelcomePage(this));
    setPage(BasicConfigPageId, new VMBasicConfigPage(this));
    setPage(MemoryPageId, new VMMemoryPage(this));
    setPage(HardDiskPageId, new VMHardDiskPage(this));
    setPage(NetworkPageId, new VMNetworkPage(this));
    setPage(SummaryPageId, new VMSummaryPage(this));
    
    setStartId(WelcomePageId);
}

void VMCreationWizard::onCurrentIdChanged(int id)
{
    // Actualizar el título de cada página
    switch (id) {
        case WelcomePageId:
            setWindowTitle(tr("Asistente para Nueva Máquina Virtual - Bienvenida"));
            break;
        case BasicConfigPageId:
            setWindowTitle(tr("Asistente para Nueva Máquina Virtual - Configuración Básica"));
            break;
        case MemoryPageId:
            setWindowTitle(tr("Asistente para Nueva Máquina Virtual - Memoria"));
            break;
        case HardDiskPageId:
            setWindowTitle(tr("Asistente para Nueva Máquina Virtual - Disco Duro"));
            break;
        case NetworkPageId:
            setWindowTitle(tr("Asistente para Nueva Máquina Virtual - Red"));
            break;
        case SummaryPageId:
            setWindowTitle(tr("Asistente para Nueva Máquina Virtual - Resumen"));
            break;
    }
}

void VMCreationWizard::accept()
{
    if (createVirtualMachine()) {
        QWizard::accept();
    }
}

bool VMCreationWizard::createVirtualMachine()
{
    QString name = field("vmName").toString();
    QString osType = field("osType").toString();
    int memory = field("memory").toInt();
    int diskSize = field("diskSize").toInt();
    
    if (m_kvmManager->createVirtualMachine(name, osType, memory, diskSize)) {
        QMessageBox::information(this, tr("Éxito"), 
            tr("La máquina virtual '%1' ha sido creada correctamente.").arg(name));
        return true;
    } else {
        QMessageBox::critical(this, tr("Error"), 
            tr("No se pudo crear la máquina virtual '%1'.").arg(name));
        return false;
    }
}

// Página de bienvenida
VMWelcomePage::VMWelcomePage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Bienvenido al Asistente de Nueva Máquina Virtual"));
    setSubTitle(tr("Este asistente le ayudará a crear una nueva máquina virtual paso a paso."));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/icons/vm-wizard-watermark.png"));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    
    QLabel *welcomeLabel = new QLabel(tr(
        "<h2>¡Bienvenido al Asistente de Creación de Máquinas Virtuales!</h2>"
        "<p>Este asistente le guiará através de los pasos necesarios para crear "
        "una nueva máquina virtual en su sistema KVM.</p>"
        "<p>Durante este proceso podrá configurar:</p>"
        "<ul>"
        "<li>Nombre y tipo de sistema operativo</li>"
        "<li>Cantidad de memoria RAM</li>"
        "<li>Disco duro virtual</li>"
        "<li>Configuración de red</li>"
        "</ul>"
        "<p>Presione <b>Siguiente</b> para continuar.</p>"));
    welcomeLabel->setWordWrap(true);
    layout->addWidget(welcomeLabel);
    
    layout->addStretch();
}

void VMWelcomePage::initializePage()
{
    // Esta página no necesita inicialización especial
}

// Página de configuración básica
VMBasicConfigPage::VMBasicConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuración Básica de la Máquina Virtual"));
    setSubTitle(tr("Configure el nombre y tipo de sistema operativo."));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Configuración básica
    QGroupBox *basicGroup = new QGroupBox(tr("Información Básica"));
    QFormLayout *basicForm = new QFormLayout(basicGroup);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Ej: Ubuntu-22.04, Windows-11"));
    basicForm->addRow(tr("&Nombre:"), m_nameEdit);
    
    m_folderEdit = new QLineEdit();
    m_folderEdit->setText(QDir::homePath() + "/.VM");
    m_folderButton = new QPushButton(tr("Examinar..."));
    QHBoxLayout *folderLayout = new QHBoxLayout();
    folderLayout->addWidget(m_folderEdit);
    folderLayout->addWidget(m_folderButton);
    basicForm->addRow(tr("&Carpeta:"), folderLayout);
    
    mainLayout->addWidget(basicGroup);
    
    // Configuración del SO
    QGroupBox *osGroup = new QGroupBox(tr("Tipo de Sistema Operativo"));
    QGridLayout *osLayout = new QGridLayout(osGroup);
    
    m_osTypeCombo = new QComboBox();
    m_osTypeCombo->addItems({
        tr("Linux"), tr("Windows"), tr("macOS"), tr("Otros")
    });
    osLayout->addWidget(new QLabel(tr("Tipo:")), 0, 0);
    osLayout->addWidget(m_osTypeCombo, 0, 1);
    
    m_osVersionCombo = new QComboBox();
    osLayout->addWidget(new QLabel(tr("Versión:")), 1, 0);
    osLayout->addWidget(m_osVersionCombo, 1, 1);
    
    m_osIconLabel = new QLabel();
    m_osIconLabel->setFixedSize(64, 64);
    m_osIconLabel->setAlignment(Qt::AlignCenter);
    m_osIconLabel->setStyleSheet("border: 1px solid gray;");
    osLayout->addWidget(m_osIconLabel, 0, 2, 2, 1);
    
    mainLayout->addWidget(osGroup);
    
    // Descripción
    QGroupBox *descGroup = new QGroupBox(tr("Descripción"));
    QVBoxLayout *descLayout = new QVBoxLayout(descGroup);
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(100);
    m_descriptionEdit->setPlaceholderText(tr("Descripción opcional de la máquina virtual"));
    descLayout->addWidget(m_descriptionEdit);
    mainLayout->addWidget(descGroup);
    
    // Registrar campos
    registerField("vmName*", m_nameEdit);
    registerField("vmFolder", m_folderEdit);
    registerField("osType", m_osTypeCombo, "currentText");
    registerField("osVersion", m_osVersionCombo, "currentText");
    registerField("description", m_descriptionEdit, "plainText");
    
    // Conectar señales
    connect(m_osTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VMBasicConfigPage::onOSTypeChanged);
    connect(m_osVersionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VMBasicConfigPage::onOSVersionChanged);
    connect(m_folderButton, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Seleccionar Carpeta"), m_folderEdit->text());
        if (!dir.isEmpty()) {
            m_folderEdit->setText(dir);
        }
    });
    
    updateOSVersions();
    updateOSIcon();
}

void VMBasicConfigPage::onOSTypeChanged()
{
    updateOSVersions();
    updateOSIcon();
}

void VMBasicConfigPage::onOSVersionChanged()
{
    updateOSIcon();
}

void VMBasicConfigPage::updateOSVersions()
{
    m_osVersionCombo->clear();
    
    QString osType = m_osTypeCombo->currentText();
    
    if (osType == tr("Linux")) {
        m_osVersionCombo->addItems({
            tr("Ubuntu 22.04 LTS"), tr("Ubuntu 20.04 LTS"),
            tr("Debian 12 (Bookworm)"), tr("Debian 11 (Bullseye)"),
            tr("Fedora 39"), tr("CentOS 9 Stream"),
            tr("Red Hat Enterprise Linux 9"),
            tr("openSUSE Leap 15.5"), tr("Arch Linux"),
            tr("Linux Mint 21"), tr("Otro Linux")
        });
    } else if (osType == tr("Windows")) {
        m_osVersionCombo->addItems({
            tr("Windows 11"), tr("Windows 10"),
            tr("Windows Server 2022"), tr("Windows Server 2019"),
            tr("Windows 8.1"), tr("Windows 7"),
            tr("Otro Windows")
        });
    } else if (osType == tr("macOS")) {
        m_osVersionCombo->addItems({
            tr("macOS Sonoma"), tr("macOS Ventura"),
            tr("macOS Monterey"), tr("macOS Big Sur"),
            tr("Otro macOS")
        });
    } else {
        m_osVersionCombo->addItems({
            tr("FreeBSD"), tr("OpenBSD"), tr("NetBSD"),
            tr("Solaris"), tr("DOS"), tr("Otro")
        });
    }
}

void VMBasicConfigPage::updateOSIcon()
{
    QString osType = m_osTypeCombo->currentText();
    QString iconPath;
    
    if (osType == tr("Linux")) {
        iconPath = ":/icons/os-linux.png";
    } else if (osType == tr("Windows")) {
        iconPath = ":/icons/os-windows.png";
    } else if (osType == tr("macOS")) {
        iconPath = ":/icons/os-macos.png";
    } else {
        iconPath = ":/icons/os-other.png";
    }
    
    QPixmap pixmap(iconPath);
    if (pixmap.isNull()) {
        // Si no hay icono, usar un placeholder
        pixmap = QPixmap(64, 64);
        pixmap.fill(Qt::lightGray);
    }
    
    m_osIconLabel->setPixmap(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

bool VMBasicConfigPage::validatePage()
{
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Debe especificar un nombre para la máquina virtual."));
        return false;
    }
    
    // Validar caracteres permitidos en el nombre
    QRegularExpression nameRegex("^[a-zA-Z0-9._-]+$");
    if (!nameRegex.match(name).hasMatch()) {
        QMessageBox::warning(this, tr("Error"), 
            tr("El nombre solo puede contener letras, números, puntos, guiones y guiones bajos."));
        return false;
    }
    
    return true;
}

void VMBasicConfigPage::initializePage()
{
    // Generar un nombre por defecto si está vacío
    if (m_nameEdit->text().isEmpty()) {
        QString defaultName = tr("Nueva-VM-") + 
            QString::number(QDateTime::currentSecsSinceEpoch() % 10000);
        m_nameEdit->setText(defaultName);
    }
}

// Página de configuración de memoria
VMMemoryPage::VMMemoryPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuración de Memoria"));
    setSubTitle(tr("Configure la cantidad de memoria RAM para la máquina virtual."));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Información del sistema
    QGroupBox *hostInfoGroup = new QGroupBox(tr("Memoria del Sistema"));
    QFormLayout *hostForm = new QFormLayout(hostInfoGroup);
    
    // Obtener información de memoria del host
    QStorageInfo storage = QStorageInfo::root();
    qint64 totalMemoryMB = 8192; // Valor por defecto, en un caso real se obtendría del sistema
    
    hostForm->addRow(tr("Memoria total:"), new QLabel(tr("%1 MB").arg(totalMemoryMB)));
    
    m_hostMemoryBar = new QProgressBar();
    m_hostMemoryBar->setRange(0, static_cast<int>(totalMemoryMB));
    m_hostMemoryBar->setValue(static_cast<int>(totalMemoryMB * 0.6)); // Simular 60% usado
    hostForm->addRow(tr("Uso actual:"), m_hostMemoryBar);
    
    mainLayout->addWidget(hostInfoGroup);
    
    // Configuración de memoria para la VM
    QGroupBox *vmMemoryGroup = new QGroupBox(tr("Memoria de la Máquina Virtual"));
    QVBoxLayout *vmMemoryLayout = new QVBoxLayout(vmMemoryGroup);
    
    // Slider y SpinBox sincronizados
    QHBoxLayout *memoryControlLayout = new QHBoxLayout();
    
    m_memorySlider = new QSlider(Qt::Horizontal);
    m_memorySlider->setRange(512, static_cast<int>(totalMemoryMB / 2));
    m_memorySlider->setValue(2048);
    m_memorySlider->setTickPosition(QSlider::TicksBelow);
    m_memorySlider->setTickInterval(1024);
    
    m_memorySpin = new QSpinBox();
    m_memorySpin->setRange(512, static_cast<int>(totalMemoryMB / 2));
    m_memorySpin->setValue(2048);
    m_memorySpin->setSuffix(" MB");
    m_memorySpin->setMinimumWidth(120);
    
    memoryControlLayout->addWidget(m_memorySlider, 1);
    memoryControlLayout->addWidget(m_memorySpin);
    vmMemoryLayout->addLayout(memoryControlLayout);
    
    // Etiquetas informativas
    m_memoryRecommendedLabel = new QLabel(tr("Recomendado: 2048 MB"));
    m_memoryRecommendedLabel->setStyleSheet("color: blue;");
    vmMemoryLayout->addWidget(m_memoryRecommendedLabel);
    
    m_memoryWarningLabel = new QLabel();
    m_memoryWarningLabel->setStyleSheet("color: red;");
    m_memoryWarningLabel->setVisible(false);
    vmMemoryLayout->addWidget(m_memoryWarningLabel);
    
    mainLayout->addWidget(vmMemoryGroup);
    mainLayout->addStretch();
    
    // Registrar campo
    registerField("memory", m_memorySpin);
    
    // Conectar señales
    connect(m_memorySlider, &QSlider::valueChanged, this, &VMMemoryPage::onMemorySliderChanged);
    connect(m_memorySpin, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &VMMemoryPage::onMemorySpinChanged);
    
    updateMemoryInfo();
}

void VMMemoryPage::onMemorySliderChanged(int value)
{
    m_memorySpin->setValue(value);
    updateMemoryInfo();
}

void VMMemoryPage::onMemorySpinChanged(int value)
{
    m_memorySlider->setValue(value);
    updateMemoryInfo();
}

void VMMemoryPage::updateMemoryInfo()
{
    int memory = m_memorySpin->value();
    int totalMemory = 8192; // En un caso real se obtendría del sistema
    
    // Actualizar recomendación basada en el OS
    QString osType = field("osType").toString();
    int recommended = 2048;
    
    if (osType == tr("Windows")) {
        recommended = 4096;
    } else if (osType == tr("Linux")) {
        recommended = 2048;
    } else if (osType == tr("macOS")) {
        recommended = 4096;
    }
    
    m_memoryRecommendedLabel->setText(tr("Recomendado para %1: %2 MB").arg(osType).arg(recommended));
    
    // Mostrar advertencias
    if (memory > totalMemory * 0.75) {
        m_memoryWarningLabel->setText(tr("¡Advertencia! Esta cantidad puede afectar el rendimiento del sistema host."));
        m_memoryWarningLabel->setVisible(true);
    } else if (memory < 512) {
        m_memoryWarningLabel->setText(tr("¡Advertencia! Esta cantidad de memoria puede ser insuficiente."));
        m_memoryWarningLabel->setVisible(true);
    } else {
        m_memoryWarningLabel->setVisible(false);
    }
}

void VMMemoryPage::initializePage()
{
    updateMemoryInfo();
}

bool VMMemoryPage::validatePage()
{
    int memory = m_memorySpin->value();
    if (memory < 512) {
        QMessageBox::warning(this, tr("Error"), 
            tr("La cantidad mínima de memoria es 512 MB."));
        return false;
    }
    return true;
}

// Página de configuración de disco duro
VMHardDiskPage::VMHardDiskPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuración de Disco Duro Virtual"));
    setSubTitle(tr("Configure el disco duro virtual para la máquina virtual."));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Opciones de disco
    QGroupBox *diskOptionsGroup = new QGroupBox(tr("Opciones de Disco Duro"));
    QVBoxLayout *diskOptionsLayout = new QVBoxLayout(diskOptionsGroup);
    
    m_createNewDiskRadio = new QRadioButton(tr("Crear un disco duro virtual nuevo"));
    m_useExistingDiskRadio = new QRadioButton(tr("Usar un archivo de disco duro existente"));
    m_noDiskRadio = new QRadioButton(tr("No agregar un disco duro virtual"));
    
    m_createNewDiskRadio->setChecked(true);
    
    diskOptionsLayout->addWidget(m_createNewDiskRadio);
    diskOptionsLayout->addWidget(m_useExistingDiskRadio);
    diskOptionsLayout->addWidget(m_noDiskRadio);
    
    mainLayout->addWidget(diskOptionsGroup);
    
    // Configuración de nuevo disco
    m_newDiskGroup = new QGroupBox(tr("Configuración de Nuevo Disco"));
    QFormLayout *newDiskForm = new QFormLayout(m_newDiskGroup);
    
    // Ubicación del archivo
    m_diskLocationEdit = new QLineEdit();
    m_diskLocationButton = new QPushButton(tr("Examinar..."));
    QHBoxLayout *locationLayout = new QHBoxLayout();
    locationLayout->addWidget(m_diskLocationEdit);
    locationLayout->addWidget(m_diskLocationButton);
    newDiskForm->addRow(tr("Ubicación:"), locationLayout);
    
    // Tamaño del disco
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    m_diskSizeSlider = new QSlider(Qt::Horizontal);
    m_diskSizeSlider->setRange(1, 500);
    m_diskSizeSlider->setValue(25);
    m_diskSizeSpin = new QSpinBox();
    m_diskSizeSpin->setRange(1, 2048);
    m_diskSizeSpin->setValue(25);
    m_diskSizeSpin->setSuffix(" GB");
    sizeLayout->addWidget(m_diskSizeSlider, 1);
    sizeLayout->addWidget(m_diskSizeSpin);
    newDiskForm->addRow(tr("Tamaño:"), sizeLayout);
    
    // Tipo de disco
    m_diskTypeCombo = new QComboBox();
    m_diskTypeCombo->addItems({
        tr("Dinámicamente asignado"),
        tr("Tamaño fijo")
    });
    newDiskForm->addRow(tr("Tipo:"), m_diskTypeCombo);
    
    m_preallocateCheck = new QCheckBox(tr("Pre-asignar espacio completo"));
    newDiskForm->addRow("", m_preallocateCheck);
    
    mainLayout->addWidget(m_newDiskGroup);
    
    // Configuración de disco existente
    m_existingDiskGroup = new QGroupBox(tr("Seleccionar Disco Existente"));
    QFormLayout *existingDiskForm = new QFormLayout(m_existingDiskGroup);
    
    m_existingDiskEdit = new QLineEdit();
    m_existingDiskButton = new QPushButton(tr("Examinar..."));
    QHBoxLayout *existingLayout = new QHBoxLayout();
    existingLayout->addWidget(m_existingDiskEdit);
    existingLayout->addWidget(m_existingDiskButton);
    existingDiskForm->addRow(tr("Archivo:"), existingLayout);
    
    mainLayout->addWidget(m_existingDiskGroup);
    mainLayout->addStretch();
    
    // Registrar campos
    registerField("createNewDisk", m_createNewDiskRadio);
    registerField("useExistingDisk", m_useExistingDiskRadio);
    registerField("noDisk", m_noDiskRadio);
    registerField("diskLocation", m_diskLocationEdit);
    registerField("diskSize", m_diskSizeSpin);
    registerField("diskType", m_diskTypeCombo, "currentText");
    registerField("preallocateDisk", m_preallocateCheck);
    registerField("existingDiskPath", m_existingDiskEdit);
    
    // Conectar señales
    connect(m_createNewDiskRadio, &QRadioButton::toggled, this, &VMHardDiskPage::onDiskOptionChanged);
    connect(m_useExistingDiskRadio, &QRadioButton::toggled, this, &VMHardDiskPage::onDiskOptionChanged);
    connect(m_noDiskRadio, &QRadioButton::toggled, this, &VMHardDiskPage::onDiskOptionChanged);
    
    connect(m_diskSizeSlider, &QSlider::valueChanged, this, &VMHardDiskPage::onDiskSizeChanged);
    connect(m_diskSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &VMHardDiskPage::onDiskSizeChanged);
    connect(m_diskTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VMHardDiskPage::onDiskTypeChanged);
    
    connect(m_diskLocationButton, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Nuevo Archivo de Disco Virtual"), 
            m_diskLocationEdit->text(), tr("Archivos de Disco Virtual (*.qcow2 *.img *.vdi)"));
        if (!fileName.isEmpty()) {
            m_diskLocationEdit->setText(fileName);
        }
    });
    
    connect(m_existingDiskButton, &QPushButton::clicked, this, &VMHardDiskPage::onBrowseExistingDisk);
    
    updateDiskControls();
}

void VMHardDiskPage::onDiskOptionChanged()
{
    updateDiskControls();
}

void VMHardDiskPage::onDiskSizeChanged()
{
    m_diskSizeSlider->setValue(m_diskSizeSpin->value());
    m_diskSizeSpin->setValue(m_diskSizeSlider->value());
}

void VMHardDiskPage::onDiskTypeChanged()
{
    // Actualizar controles basado en el tipo de disco seleccionado
    bool isDynamic = (m_diskTypeCombo->currentIndex() == 0);
    m_preallocateCheck->setEnabled(!isDynamic);
    if (isDynamic) {
        m_preallocateCheck->setChecked(false);
    }
}

void VMHardDiskPage::onBrowseExistingDisk()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Seleccionar Disco Virtual Existente"),
        QString(), tr("Archivos de Disco Virtual (*.qcow2 *.img *.vdi *.vmdk)"));
    if (!fileName.isEmpty()) {
        m_existingDiskEdit->setText(fileName);
    }
}

void VMHardDiskPage::updateDiskControls()
{
    m_newDiskGroup->setEnabled(m_createNewDiskRadio->isChecked());
    m_existingDiskGroup->setEnabled(m_useExistingDiskRadio->isChecked());
}

void VMHardDiskPage::initializePage()
{
    // Generar nombre por defecto para el disco basado en el nombre de la VM
    updateDefaultDiskName();
    updateDiskControls();
}

void VMHardDiskPage::updateDefaultDiskName()
{
    QString vmName = field("vmName").toString();
    QString vmFolder = field("vmFolder").toString();
    
    if (!vmName.isEmpty()) {
        // Limpiar nombre para uso en archivo
        QString cleanName = vmName;
        cleanName.replace(QRegularExpression("[^a-zA-Z0-9._-]"), "-");
        
        // Usar el nombre de la VM como nombre base para el disco
        QString diskPath = vmFolder + "/" + cleanName + "/" + cleanName + "_disco.qcow2";
        m_diskLocationEdit->setText(diskPath);
    }
}

bool VMHardDiskPage::validatePage()
{
    if (m_createNewDiskRadio->isChecked()) {
        if (m_diskLocationEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Error"), tr("Debe especificar la ubicación del nuevo disco."));
            return false;
        }
        if (m_diskSizeSpin->value() < 1) {
            QMessageBox::warning(this, tr("Error"), tr("El tamaño del disco debe ser al menos 1 GB."));
            return false;
        }
    } else if (m_useExistingDiskRadio->isChecked()) {
        if (m_existingDiskEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Error"), tr("Debe seleccionar un archivo de disco existente."));
            return false;
        }
        if (!QFile::exists(m_existingDiskEdit->text())) {
            QMessageBox::warning(this, tr("Error"), tr("El archivo de disco seleccionado no existe."));
            return false;
        }
    }
    return true;
}

// Página de configuración de red
VMNetworkPage::VMNetworkPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuración de Red"));
    setSubTitle(tr("Configure las opciones de red para la máquina virtual."));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Habilitar red
    m_networkEnabledCheck = new QCheckBox(tr("Habilitar adaptador de red"));
    m_networkEnabledCheck->setChecked(true);
    mainLayout->addWidget(m_networkEnabledCheck);
    
    // Configuración de red
    QGroupBox *networkGroup = new QGroupBox(tr("Configuración del Adaptador de Red"));
    QFormLayout *networkForm = new QFormLayout(networkGroup);
    
    // Modo de red
    m_networkModeCombo = new QComboBox();
    m_networkModeCombo->addItems({
        tr("NAT"),
        tr("Bridged Adapter"), 
        tr("Internal Network"),
        tr("Host-only Adapter"),
        tr("Generic Driver"),
        tr("No conectado")
    });
    networkForm->addRow(tr("Conectado a:"), m_networkModeCombo);
    
    // Tipo de adaptador
    m_networkAdapterCombo = new QComboBox();
    m_networkAdapterCombo->addItems({
        tr("Intel PRO/1000 MT Desktop (82540EM)"),
        tr("Intel PRO/1000 T Server (82543GC)"),
        tr("Intel PRO/1000 MT Server (82545EM)"),
        tr("Paravirtualized Network (virtio-net)"),
        tr("AMD PCNet PCI II (Am79C970A)"),
        tr("AMD PCNet FAST III (Am79C973)")
    });
    m_networkAdapterCombo->setCurrentIndex(3); // virtio-net por defecto
    networkForm->addRow(tr("Tipo de Adaptador:"), m_networkAdapterCombo);
    
    // Dirección MAC
    QHBoxLayout *macLayout = new QHBoxLayout();
    m_macAddressEdit = new QLineEdit();
    m_generateMacButton = new QPushButton(tr("Generar"));
    macLayout->addWidget(m_macAddressEdit);
    macLayout->addWidget(m_generateMacButton);
    networkForm->addRow(tr("Dirección MAC:"), macLayout);
    
    // Cable conectado
    m_cableConnectedCheck = new QCheckBox(tr("Cable conectado"));
    m_cableConnectedCheck->setChecked(true);
    networkForm->addRow("", m_cableConnectedCheck);
    
    mainLayout->addWidget(networkGroup);
    mainLayout->addStretch();
    
    // Registrar campos
    registerField("networkEnabled", m_networkEnabledCheck);
    registerField("networkMode", m_networkModeCombo, "currentText");
    registerField("networkAdapter", m_networkAdapterCombo, "currentText");
    registerField("macAddress", m_macAddressEdit);
    registerField("cableConnected", m_cableConnectedCheck);
    
    // Conectar señales
    connect(m_networkEnabledCheck, &QCheckBox::toggled, this, &VMNetworkPage::onNetworkEnabledChanged);
    connect(m_networkModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VMNetworkPage::onNetworkModeChanged);
    connect(m_generateMacButton, &QPushButton::clicked, this, &VMNetworkPage::generateMacAddress);
    
    generateMacAddress();
    updateNetworkControls();
}

void VMNetworkPage::onNetworkEnabledChanged()
{
    updateNetworkControls();
}

void VMNetworkPage::onNetworkModeChanged()
{
    updateNetworkControls();
}

void VMNetworkPage::updateNetworkControls()
{
    bool enabled = m_networkEnabledCheck->isChecked();
    
    m_networkModeCombo->setEnabled(enabled);
    m_networkAdapterCombo->setEnabled(enabled);
    m_macAddressEdit->setEnabled(enabled);
    m_generateMacButton->setEnabled(enabled);
    m_cableConnectedCheck->setEnabled(enabled);
}

void VMNetworkPage::generateMacAddress()
{
    // Generar dirección MAC válida para VM (02:xx:xx:xx:xx:xx)
    QString mac = "02";
    for (int i = 0; i < 5; ++i) {
        int byte = QRandomGenerator::global()->bounded(256);
        mac += QString(":%1").arg(byte, 2, 16, QChar('0')).toUpper();
    }
    m_macAddressEdit->setText(mac);
}

void VMNetworkPage::initializePage()
{
    updateNetworkControls();
}

// Página de resumen
VMSummaryPage::VMSummaryPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Resumen de Configuración"));
    setSubTitle(tr("Revise la configuración antes de crear la máquina virtual."));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QLabel *summaryLabel = new QLabel(tr("Se creará la máquina virtual con la siguiente configuración:"));
    mainLayout->addWidget(summaryLabel);
    
    m_summaryText = new QTextEdit();
    m_summaryText->setReadOnly(true);
    mainLayout->addWidget(m_summaryText);
    
    m_startAfterCreationCheck = new QCheckBox(tr("Iniciar máquina virtual después de crearla"));
    mainLayout->addWidget(m_startAfterCreationCheck);
    
    // Registrar campo
    registerField("startAfterCreation", m_startAfterCreationCheck);
}

void VMSummaryPage::initializePage()
{
    updateSummary();
}

void VMSummaryPage::updateSummary()
{
    QString summary = "<h3>" + tr("Configuración de la Máquina Virtual") + "</h3>";
    
    summary += "<table border='1' cellpadding='5' cellspacing='0'>";
    summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Nombre")).arg(field("vmName").toString());
    summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Carpeta")).arg(field("vmFolder").toString());
    summary += QString("<tr><td><b>%1</b></td><td>%2 - %3</td></tr>").arg(tr("Sistema Operativo"))
        .arg(field("osType").toString()).arg(field("osVersion").toString());
    summary += QString("<tr><td><b>%1</b></td><td>%2 MB</td></tr>").arg(tr("Memoria RAM")).arg(field("memory").toInt());
    
    // Información del disco
    if (field("createNewDisk").toBool()) {
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Disco Duro")).arg(tr("Crear nuevo disco"));
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Ubicación del Disco")).arg(field("diskLocation").toString());
        summary += QString("<tr><td><b>%1</b></td><td>%2 GB (%3)</td></tr>").arg(tr("Tamaño del Disco"))
            .arg(field("diskSize").toInt()).arg(field("diskType").toString());
    } else if (field("useExistingDisk").toBool()) {
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Disco Duro")).arg(tr("Usar disco existente"));
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Archivo de Disco")).arg(field("existingDiskPath").toString());
    } else {
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Disco Duro")).arg(tr("Sin disco duro"));
    }
    
    // Información de red
    if (field("networkEnabled").toBool()) {
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Red")).arg(tr("Habilitada"));
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Modo de Red")).arg(field("networkMode").toString());
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Adaptador")).arg(field("networkAdapter").toString());
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("MAC")).arg(field("macAddress").toString());
    } else {
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Red")).arg(tr("Deshabilitada"));
    }
    
    if (!field("description").toString().trimmed().isEmpty()) {
        summary += QString("<tr><td><b>%1</b></td><td>%2</td></tr>").arg(tr("Descripción")).arg(field("description").toString());
    }
    
    summary += "</table>";
    
    m_summaryText->setHtml(summary);
}