#include "VMConfigDialog.h"
#include "../core/VirtualMachine.h"
#include "../core/KVMManager.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QFileDialog>
#include <QHeaderView>
#include <QSplitter>
#include <QMessageBox>
#include <QApplication>

VMConfigDialog::VMConfigDialog(VirtualMachine *vm, QWidget *parent)
    : QDialog(parent)
    , m_virtualMachine(vm)
    , m_kvmManager(nullptr)
{
    setupUI();
    loadVMSettings();
}

VMConfigDialog::VMConfigDialog(VirtualMachine *vm, KVMManager *kvmManager, QWidget *parent)
    : QDialog(parent)
    , m_virtualMachine(vm)
    , m_kvmManager(kvmManager)
{
    setWindowTitle(tr("Configuración - %1").arg(vm ? vm->getName() : tr("Nueva VM")));
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    resize(900, 700);
    
    setupUI();
    if (vm) {
        loadVMSettings();
    }
}

void VMConfigDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget();
    
    createGeneralTab();
    createSystemTab();
    createDisplayTab();
    createStorageTab();
    createAudioTab();
    createNetworkTab();
    createSerialPortsTab();
    createUSBTab();
    createSharedFoldersTab();
    createUserInterfaceTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    m_applyButton = buttonBox->button(QDialogButtonBox::Apply);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &VMConfigDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &VMConfigDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &VMConfigDialog::saveVMSettings);
    
    mainLayout->addWidget(buttonBox);
}

void VMConfigDialog::createGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);
    
    // Basic information
    QGroupBox *basicGroup = new QGroupBox(tr("🧾 Información Básica"));
    QGridLayout *basicLayout = new QGridLayout(basicGroup);
    
    basicLayout->addWidget(new QLabel(tr("Nombre:")), 0, 0);
    m_nameEdit = new QLineEdit();
    basicLayout->addWidget(m_nameEdit, 0, 1);
    
    basicLayout->addWidget(new QLabel(tr("Tipo de SO:")), 1, 0);
    m_osTypeCombo = new QComboBox();
    m_osTypeCombo->addItems({tr("Linux"), tr("Microsoft Windows"), tr("macOS"), tr("BSD"), tr("Otros")});
    basicLayout->addWidget(m_osTypeCombo, 1, 1);
    
    basicLayout->addWidget(new QLabel(tr("Versión:")), 2, 0);
    m_osVersionCombo = new QComboBox();
    m_osVersionCombo->addItems({
        tr("Ubuntu 22.04 LTS"), tr("Ubuntu 20.04 LTS"), tr("Debian 12"), tr("Debian 11"),
        tr("Fedora 39"), tr("CentOS 9"), tr("Red Hat Enterprise Linux 9"),
        tr("Windows 11"), tr("Windows 10"), tr("Windows Server 2022"),
        tr("macOS Monterey"), tr("macOS Big Sur"), tr("FreeBSD 14"), tr("OpenBSD 7")
    });
    basicLayout->addWidget(m_osVersionCombo, 2, 1);
    
    layout->addWidget(basicGroup);
    
    // Advanced settings
    QGroupBox *advancedGroup = new QGroupBox(tr("🧩 Configuración Avanzada"));
    QGridLayout *advancedLayout = new QGridLayout(advancedGroup);
    
    advancedLayout->addWidget(new QLabel(tr("Carpeta de instantáneas:")), 0, 0);
    QHBoxLayout *snapshotLayout = new QHBoxLayout();
    m_snapshotFolderEdit = new QLineEdit();
    m_browseSnapshotButton = new QPushButton(tr("Examinar..."));
    connect(m_browseSnapshotButton, &QPushButton::clicked, [this]() {
        QString folder = QFileDialog::getExistingDirectory(this, tr("Seleccionar carpeta de instantáneas"));
        if (!folder.isEmpty()) {
            m_snapshotFolderEdit->setText(folder);
        }
    });
    snapshotLayout->addWidget(m_snapshotFolderEdit);
    snapshotLayout->addWidget(m_browseSnapshotButton);
    advancedLayout->addLayout(snapshotLayout, 0, 1);
    
    advancedLayout->addWidget(new QLabel(tr("Portapapeles compartido:")), 1, 0);
    m_clipboardCombo = new QComboBox();
    m_clipboardCombo->addItems({tr("Deshabilitado"), tr("Anfitrión a invitado"), tr("Invitado a anfitrión"), tr("Bidireccional")});
    advancedLayout->addWidget(m_clipboardCombo, 1, 1);
    
    advancedLayout->addWidget(new QLabel(tr("Arrastrar y soltar:")), 2, 0);
    m_dragDropCombo = new QComboBox();
    m_dragDropCombo->addItems({tr("Deshabilitado"), tr("Anfitrión a invitado"), tr("Invitado a anfitrión"), tr("Bidireccional")});
    advancedLayout->addWidget(m_dragDropCombo, 2, 1);
    
    layout->addWidget(advancedGroup);
    
    // Description
    QGroupBox *descGroup = new QGroupBox(tr("📋 Descripción"));
    QVBoxLayout *descLayout = new QVBoxLayout(descGroup);
    
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setMaximumHeight(100);
    m_descriptionEdit->setPlaceholderText(tr("Escriba una descripción para esta máquina virtual..."));
    descLayout->addWidget(m_descriptionEdit);
    
    layout->addWidget(descGroup);
    
    // Encryption (if Extension Pack available)
    QGroupBox *encryptionGroup = new QGroupBox(tr("🔐 Cifrado de Disco"));
    QGridLayout *encryptionLayout = new QGridLayout(encryptionGroup);
    
    m_encryptionCheckBox = new QCheckBox(tr("Habilitar cifrado de disco"));
    encryptionLayout->addWidget(m_encryptionCheckBox, 0, 0, 1, 2);
    
    encryptionLayout->addWidget(new QLabel(tr("Contraseña:")), 1, 0);
    m_encryptionPasswordEdit = new QLineEdit();
    m_encryptionPasswordEdit->setEchoMode(QLineEdit::Password);
    m_encryptionPasswordEdit->setEnabled(false);
    encryptionLayout->addWidget(m_encryptionPasswordEdit, 1, 1);
    
    connect(m_encryptionCheckBox, &QCheckBox::toggled, this, &VMConfigDialog::enableEncryption);
    
    layout->addWidget(encryptionGroup);
    
    layout->addStretch();
    
    m_tabWidget->addTab(m_generalTab, tr("General"));
}

void VMConfigDialog::createSystemTab()
{
    m_systemTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_systemTab);
    
    // Motherboard section
    QGroupBox *motherboardGroup = new QGroupBox(tr("⚙️ Placa Base"));
    QGridLayout *motherboardLayout = new QGridLayout(motherboardGroup);
    
    // Memory
    motherboardLayout->addWidget(new QLabel(tr("Memoria base:")), 0, 0);
    QHBoxLayout *memoryLayout = new QHBoxLayout();
    
    m_memorySlider = new QSlider(Qt::Horizontal);
    m_memorySlider->setRange(256, 32768); // 256MB to 32GB
    m_memorySlider->setValue(2048); // Default 2GB
    connect(m_memorySlider, &QSlider::valueChanged, this, &VMConfigDialog::onMemorySliderChanged);
    
    m_memorySpin = new QSpinBox();
    m_memorySpin->setRange(256, 32768);
    m_memorySpin->setSuffix(" MB");
    m_memorySpin->setValue(2048);
    connect(m_memorySpin, QOverload<int>::of(&QSpinBox::valueChanged), m_memorySlider, &QSlider::setValue);
    
    m_memoryLabel = new QLabel(tr("2048 MB"));
    
    memoryLayout->addWidget(m_memorySlider, 3);
    memoryLayout->addWidget(m_memorySpin, 1);
    memoryLayout->addWidget(m_memoryLabel, 1);
    motherboardLayout->addLayout(memoryLayout, 0, 1);
    
    // Boot order
    motherboardLayout->addWidget(new QLabel(tr("Orden de arranque:")), 1, 0);
    QVBoxLayout *bootLayout = new QVBoxLayout();
    
    m_bootOrderList = new QListWidget();
    m_bootOrderList->addItems({tr("Disco Duro"), tr("CD/DVD"), tr("Red"), tr("Disquete")});
    m_bootOrderList->setMaximumHeight(120);
    
    QHBoxLayout *bootButtonLayout = new QHBoxLayout();
    m_moveBootUpButton = new QPushButton(tr("↑"));
    m_moveBootDownButton = new QPushButton(tr("↓"));
    m_moveBootUpButton->setMaximumWidth(30);
    m_moveBootDownButton->setMaximumWidth(30);
    bootButtonLayout->addWidget(m_moveBootUpButton);
    bootButtonLayout->addWidget(m_moveBootDownButton);
    bootButtonLayout->addStretch();
    
    bootLayout->addWidget(m_bootOrderList);
    bootLayout->addLayout(bootButtonLayout);
    motherboardLayout->addLayout(bootLayout, 1, 1);
    
    // Chipset
    motherboardLayout->addWidget(new QLabel(tr("Chipset:")), 2, 0);
    m_chipsetCombo = new QComboBox();
    m_chipsetCombo->addItems({tr("PIIX3"), tr("ICH9"), tr("Q35")});
    motherboardLayout->addWidget(m_chipsetCombo, 2, 1);
    
    // Extended features
    m_utcTimeCheckBox = new QCheckBox(tr("Reloj de hardware en hora UTC"));
    motherboardLayout->addWidget(m_utcTimeCheckBox, 3, 0, 1, 2);
    
    m_efiCheckBox = new QCheckBox(tr("Habilitar EFI (solo SO especiales)"));
    motherboardLayout->addWidget(m_efiCheckBox, 4, 0, 1, 2);
    
    layout->addWidget(motherboardGroup);
    
    // Processor section
    QGroupBox *processorGroup = new QGroupBox(tr("🧠 Procesador"));
    QGridLayout *processorLayout = new QGridLayout(processorGroup);
    
    // CPU count
    processorLayout->addWidget(new QLabel(tr("Procesadores:")), 0, 0);
    QHBoxLayout *cpuLayout = new QHBoxLayout();
    
    m_cpuCountSpin = new QSpinBox();
    m_cpuCountSpin->setRange(1, 32);
    m_cpuCountSpin->setValue(1);
    connect(m_cpuCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &VMConfigDialog::onCPUCountChanged);
    
    m_cpuCountLabel = new QLabel(tr("1 CPU"));
    
    cpuLayout->addWidget(m_cpuCountSpin);
    cpuLayout->addWidget(m_cpuCountLabel);
    cpuLayout->addStretch();
    processorLayout->addLayout(cpuLayout, 0, 1);
    
    // Execution cap
    processorLayout->addWidget(new QLabel(tr("Límite de ejecución:")), 1, 0);
    m_cpuExecutionSlider = new QSlider(Qt::Horizontal);
    m_cpuExecutionSlider->setRange(1, 100);
    m_cpuExecutionSlider->setValue(100);
    processorLayout->addWidget(m_cpuExecutionSlider, 1, 1);
    
    // Extended features
    m_paeCheckBox = new QCheckBox(tr("Habilitar PAE/NX"));
    processorLayout->addWidget(m_paeCheckBox, 2, 0, 1, 2);
    
    m_nestedVirtCheckBox = new QCheckBox(tr("Habilitar virtualización anidada"));
    processorLayout->addWidget(m_nestedVirtCheckBox, 3, 0, 1, 2);
    
    // Paravirtualization
    processorLayout->addWidget(new QLabel(tr("Paravirtualización:")), 4, 0);
    m_paravirtCombo = new QComboBox();
    m_paravirtCombo->addItems({tr("Por defecto"), tr("Legado"), tr("Minimal"), tr("Hyper-V"), tr("KVM")});
    processorLayout->addWidget(m_paravirtCombo, 4, 1);
    
    layout->addWidget(processorGroup);
    
    // Acceleration section
    QGroupBox *accelGroup = new QGroupBox(tr("⚡ Aceleración"));
    QGridLayout *accelLayout = new QGridLayout(accelGroup);
    
    m_vtxCheckBox = new QCheckBox(tr("Habilitar VT-x/AMD-V"));
    m_vtxCheckBox->setChecked(true);
    accelLayout->addWidget(m_vtxCheckBox, 0, 0, 1, 2);
    
    m_nestedPagingCheckBox = new QCheckBox(tr("Habilitar paginación anidada"));
    m_nestedPagingCheckBox->setChecked(true);
    accelLayout->addWidget(m_nestedPagingCheckBox, 1, 0, 1, 2);
    
    layout->addWidget(accelGroup);
    
    layout->addStretch();
    
    m_tabWidget->addTab(m_systemTab, tr("Sistema"));
}

void VMConfigDialog::createDisplayTab()
{
    m_displayTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_displayTab);
    
    // Screen section
    QGroupBox *screenGroup = new QGroupBox(tr("💻 Pantalla"));
    QGridLayout *screenLayout = new QGridLayout(screenGroup);
    
    // Video memory
    screenLayout->addWidget(new QLabel(tr("Memoria de vídeo:")), 0, 0);
    QHBoxLayout *videoMemLayout = new QHBoxLayout();
    
    m_videoMemorySlider = new QSlider(Qt::Horizontal);
    m_videoMemorySlider->setRange(8, 256);
    m_videoMemorySlider->setValue(128);
    
    m_videoMemoryLabel = new QLabel(tr("128 MB"));
    connect(m_videoMemorySlider, &QSlider::valueChanged, [this](int value) {
        m_videoMemoryLabel->setText(tr("%1 MB").arg(value));
    });
    
    videoMemLayout->addWidget(m_videoMemorySlider, 3);
    videoMemLayout->addWidget(m_videoMemoryLabel, 1);
    screenLayout->addLayout(videoMemLayout, 0, 1);
    
    // Monitor count
    screenLayout->addWidget(new QLabel(tr("Cantidad de monitores:")), 1, 0);
    m_monitorCountSpin = new QSpinBox();
    m_monitorCountSpin->setRange(1, 8);
    m_monitorCountSpin->setValue(1);
    screenLayout->addWidget(m_monitorCountSpin, 1, 1);
    
    // Scale factor
    screenLayout->addWidget(new QLabel(tr("Factor de escala:")), 2, 0);
    m_scaleFactorSlider = new QSlider(Qt::Horizontal);
    m_scaleFactorSlider->setRange(100, 300);
    m_scaleFactorSlider->setValue(100);
    screenLayout->addWidget(m_scaleFactorSlider, 2, 1);
    
    // Acceleration
    m_3dAccelCheckBox = new QCheckBox(tr("Habilitar aceleración 3D"));
    screenLayout->addWidget(m_3dAccelCheckBox, 3, 0, 1, 2);
    
    m_2dAccelCheckBox = new QCheckBox(tr("Habilitar aceleración 2D"));
    screenLayout->addWidget(m_2dAccelCheckBox, 4, 0, 1, 2);
    
    layout->addWidget(screenGroup);
    
    // Remote display section
    QGroupBox *remoteGroup = new QGroupBox(tr("🖼️ Pantalla Remota"));
    QGridLayout *remoteLayout = new QGridLayout(remoteGroup);
    
    m_enableRemoteCheckBox = new QCheckBox(tr("Habilitar servidor de pantalla remota"));
    remoteLayout->addWidget(m_enableRemoteCheckBox, 0, 0, 1, 2);
    
    remoteLayout->addWidget(new QLabel(tr("Puerto del servidor:")), 1, 0);
    m_remotePortSpin = new QSpinBox();
    m_remotePortSpin->setRange(1024, 65535);
    m_remotePortSpin->setValue(3389);
    m_remotePortSpin->setEnabled(false);
    remoteLayout->addWidget(m_remotePortSpin, 1, 1);
    
    remoteLayout->addWidget(new QLabel(tr("Método de autenticación:")), 2, 0);
    m_authMethodCombo = new QComboBox();
    m_authMethodCombo->addItems({tr("Nulo"), tr("Externo"), tr("Invitado")});
    m_authMethodCombo->setEnabled(false);
    remoteLayout->addWidget(m_authMethodCombo, 2, 1);
    
    connect(m_enableRemoteCheckBox, &QCheckBox::toggled, m_remotePortSpin, &QSpinBox::setEnabled);
    connect(m_enableRemoteCheckBox, &QCheckBox::toggled, m_authMethodCombo, &QComboBox::setEnabled);
    
    layout->addWidget(remoteGroup);
    
    // Recording section
    QGroupBox *recordingGroup = new QGroupBox(tr("📺 Grabación"));
    QGridLayout *recordingLayout = new QGridLayout(recordingGroup);
    
    m_enableRecordingCheckBox = new QCheckBox(tr("Habilitar grabación"));
    recordingLayout->addWidget(m_enableRecordingCheckBox, 0, 0, 1, 2);
    
    recordingLayout->addWidget(new QLabel(tr("Modo de grabación:")), 1, 0);
    m_recordingModeCombo = new QComboBox();
    m_recordingModeCombo->addItems({tr("Vídeo/Audio"), tr("Solo vídeo"), tr("Solo audio")});
    m_recordingModeCombo->setEnabled(false);
    recordingLayout->addWidget(m_recordingModeCombo, 1, 1);
    
    recordingLayout->addWidget(new QLabel(tr("Archivo de destino:")), 2, 0);
    QHBoxLayout *recordingFileLayout = new QHBoxLayout();
    m_recordingFileEdit = new QLineEdit();
    m_recordingFileEdit->setEnabled(false);
    m_browseRecordingButton = new QPushButton(tr("Examinar..."));
    m_browseRecordingButton->setEnabled(false);
    
    recordingFileLayout->addWidget(m_recordingFileEdit);
    recordingFileLayout->addWidget(m_browseRecordingButton);
    recordingLayout->addLayout(recordingFileLayout, 2, 1);
    
    connect(m_enableRecordingCheckBox, &QCheckBox::toggled, m_recordingModeCombo, &QComboBox::setEnabled);
    connect(m_enableRecordingCheckBox, &QCheckBox::toggled, m_recordingFileEdit, &QLineEdit::setEnabled);
    connect(m_enableRecordingCheckBox, &QCheckBox::toggled, m_browseRecordingButton, &QPushButton::setEnabled);
    
    layout->addWidget(recordingGroup);
    
    layout->addStretch();
    
    m_tabWidget->addTab(m_displayTab, tr("Pantalla"));
}

void VMConfigDialog::createStorageTab()
{
    m_storageTab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(m_storageTab);
    
    // Storage tree
    QVBoxLayout *treeLayout = new QVBoxLayout();
    
    QLabel *storageLabel = new QLabel(tr("Árbol de almacenamiento:"));
    treeLayout->addWidget(storageLabel);
    
    m_storageTree = new QTreeWidget();
    m_storageTree->setHeaderLabels({tr("Nombre"), tr("Tamaño"), tr("Tipo")});
    m_storageTree->setRootIsDecorated(true);
    
    // Add sample storage tree
    QTreeWidgetItem *controller = new QTreeWidgetItem({tr("Controlador SATA"), "", "Controlador"});
    controller->addChild(new QTreeWidgetItem({tr("Disco duro principal"), "25.0 GB", "Disco Duro"}));
    controller->addChild(new QTreeWidgetItem({tr("CD/DVD"), "Vacío", "Óptico"}));
    m_storageTree->addTopLevelItem(controller);
    m_storageTree->expandAll();
    
    treeLayout->addWidget(m_storageTree);
    
    // Storage buttons
    QHBoxLayout *storageButtonLayout = new QHBoxLayout();
    m_addControllerButton = new QPushButton(tr("Agregar Controlador"));
    m_removeControllerButton = new QPushButton(tr("Eliminar Controlador"));
    m_addAttachmentButton = new QPushButton(tr("Agregar Disco"));
    m_removeAttachmentButton = new QPushButton(tr("Eliminar"));
    
    connect(m_addControllerButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, tr("Almacenamiento"), tr("Función en desarrollo"));
    });
    
    storageButtonLayout->addWidget(m_addControllerButton);
    storageButtonLayout->addWidget(m_removeControllerButton);
    storageButtonLayout->addWidget(m_addAttachmentButton);
    storageButtonLayout->addWidget(m_removeAttachmentButton);
    storageButtonLayout->addStretch();
    
    treeLayout->addLayout(storageButtonLayout);
    
    layout->addLayout(treeLayout, 1);
    
    // Attachment details
    m_attachmentDetails = new QGroupBox(tr("Propiedades"));
    QGridLayout *detailsLayout = new QGridLayout(m_attachmentDetails);
    
    detailsLayout->addWidget(new QLabel(tr("Archivo de disco:")), 0, 0);
    QHBoxLayout *diskFileLayout = new QHBoxLayout();
    m_diskFileEdit = new QLineEdit();
    m_browseDiskButton = new QPushButton(tr("Examinar..."));
    diskFileLayout->addWidget(m_diskFileEdit);
    diskFileLayout->addWidget(m_browseDiskButton);
    detailsLayout->addLayout(diskFileLayout, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Tipo de disco:")), 1, 0);
    m_diskTypeCombo = new QComboBox();
    m_diskTypeCombo->addItems({tr("Dinámico"), tr("Fijo"), tr("Diferenciación")});
    detailsLayout->addWidget(m_diskTypeCombo, 1, 1);
    
    m_solidStateCheckBox = new QCheckBox(tr("Estado sólido"));
    detailsLayout->addWidget(m_solidStateCheckBox, 2, 0, 1, 2);
    
    m_hostCacheCheckBox = new QCheckBox(tr("Usar caché del host"));
    m_hostCacheCheckBox->setChecked(true);
    detailsLayout->addWidget(m_hostCacheCheckBox, 3, 0, 1, 2);
    
    layout->addWidget(m_attachmentDetails, 1);
    
    m_tabWidget->addTab(m_storageTab, tr("Almacenamiento"));
}

void VMConfigDialog::createAudioTab()
{
    m_audioTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_audioTab);
    
    QGroupBox *audioGroup = new QGroupBox(tr("🔊 Audio"));
    QGridLayout *audioLayout = new QGridLayout(audioGroup);
    
    m_enableAudioCheckBox = new QCheckBox(tr("Habilitar Audio"));
    m_enableAudioCheckBox->setChecked(true);
    audioLayout->addWidget(m_enableAudioCheckBox, 0, 0, 1, 2);
    
    audioLayout->addWidget(new QLabel(tr("Controlador de audio del host:")), 1, 0);
    m_hostAudioDriverCombo = new QComboBox();
    m_hostAudioDriverCombo->addItems({tr("PulseAudio"), tr("ALSA"), tr("OSS")});
    audioLayout->addWidget(m_hostAudioDriverCombo, 1, 1);
    
    audioLayout->addWidget(new QLabel(tr("Controlador de audio:")), 2, 0);
    m_audioControllerCombo = new QComboBox();
    m_audioControllerCombo->addItems({tr("Intel HD Audio"), tr("ICH AC97"), tr("SoundBlaster 16")});
    audioLayout->addWidget(m_audioControllerCombo, 2, 1);
    
    m_enableInputCheckBox = new QCheckBox(tr("Habilitar entrada de audio"));
    m_enableInputCheckBox->setChecked(true);
    audioLayout->addWidget(m_enableInputCheckBox, 3, 0, 1, 2);
    
    m_enableOutputCheckBox = new QCheckBox(tr("Habilitar salida de audio"));
    m_enableOutputCheckBox->setChecked(true);
    audioLayout->addWidget(m_enableOutputCheckBox, 4, 0, 1, 2);
    
    m_testAudioButton = new QPushButton(tr("Probar Audio"));
    connect(m_testAudioButton, &QPushButton::clicked, this, &VMConfigDialog::testAudioDevice);
    audioLayout->addWidget(m_testAudioButton, 5, 1);
    
    layout->addWidget(audioGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_audioTab, tr("Audio"));
}

void VMConfigDialog::createNetworkTab()
{
    m_networkTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_networkTab);
    
    m_networkAdapters = new QTabWidget();
    
    // Create 4 network adapter tabs
    for (int i = 1; i <= 4; ++i) {
        QWidget *adapterTab = new QWidget();
        QVBoxLayout *adapterLayout = new QVBoxLayout(adapterTab);
        
        QGroupBox *adapterGroup = new QGroupBox(tr("Adaptador %1").arg(i));
        QGridLayout *adapterGridLayout = new QGridLayout(adapterGroup);
        
        QCheckBox *enableAdapter = new QCheckBox(tr("Habilitar adaptador de red"));
        if (i == 1) enableAdapter->setChecked(true); // First adapter enabled by default
        adapterGridLayout->addWidget(enableAdapter, 0, 0, 1, 2);
        
        adapterGridLayout->addWidget(new QLabel(tr("Conectado a:")), 1, 0);
        QComboBox *attachmentCombo = new QComboBox();
        attachmentCombo->addItems({
            tr("NAT"), tr("Adaptador puente"), tr("Red interna"), 
            tr("Solo anfitrión"), tr("Controlador genérico"), tr("Red NAT")
        });
        adapterGridLayout->addWidget(attachmentCombo, 1, 1);
        
        adapterGridLayout->addWidget(new QLabel(tr("Nombre:")), 2, 0);
        QComboBox *nameCombo = new QComboBox();
        nameCombo->setEditable(true);
        nameCombo->addItems({tr("eth0"), tr("enp0s3"), tr("wlan0")});
        adapterGridLayout->addWidget(nameCombo, 2, 1);
        
        // Advanced section
        QGroupBox *advancedGroup = new QGroupBox(tr("Avanzado"));
        QGridLayout *advancedLayout = new QGridLayout(advancedGroup);
        
        advancedLayout->addWidget(new QLabel(tr("Tipo de adaptador:")), 0, 0);
        QComboBox *adapterTypeCombo = new QComboBox();
        adapterTypeCombo->addItems({
            tr("Intel PRO/1000 MT Desktop"), tr("Intel PRO/1000 T Server"),
            tr("PCnet-PCI II"), tr("PCnet-FAST III"), tr("Paravirtualizado")
        });
        advancedLayout->addWidget(adapterTypeCombo, 0, 1);
        
        advancedLayout->addWidget(new QLabel(tr("Modo promiscuo:")), 1, 0);
        QComboBox *promiscuousCombo = new QComboBox();
        promiscuousCombo->addItems({tr("Denegar"), tr("Permitir VMs"), tr("Permitir todo")});
        advancedLayout->addWidget(promiscuousCombo, 1, 1);
        
        advancedLayout->addWidget(new QLabel(tr("Dirección MAC:")), 2, 0);
        QLineEdit *macEdit = new QLineEdit();
        macEdit->setText("08:00:27:12:34:56");
        advancedLayout->addWidget(macEdit, 2, 1);
        
        QCheckBox *cableConnected = new QCheckBox(tr("Cable conectado"));
        cableConnected->setChecked(true);
        advancedLayout->addWidget(cableConnected, 3, 0, 1, 2);
        
        adapterLayout->addWidget(adapterGroup);
        adapterLayout->addWidget(advancedGroup);
        adapterLayout->addStretch();
        
        m_networkAdapters->addTab(adapterTab, tr("Adaptador %1").arg(i));
    }
    
    layout->addWidget(m_networkAdapters);
    
    m_tabWidget->addTab(m_networkTab, tr("Red"));
}

void VMConfigDialog::createSerialPortsTab()
{
    m_serialPortsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_serialPortsTab);
    
    m_serialPorts = new QTabWidget();
    
    // Create 4 serial port tabs
    for (int i = 1; i <= 4; ++i) {
        QWidget *portTab = new QWidget();
        QVBoxLayout *portLayout = new QVBoxLayout(portTab);
        
        QGroupBox *portGroup = new QGroupBox(tr("Puerto serie %1").arg(i));
        QGridLayout *portGridLayout = new QGridLayout(portGroup);
        
        QCheckBox *enablePort = new QCheckBox(tr("Habilitar puerto serie"));
        portGridLayout->addWidget(enablePort, 0, 0, 1, 2);
        
        portGridLayout->addWidget(new QLabel(tr("Número de puerto:")), 1, 0);
        QComboBox *portNumberCombo = new QComboBox();
        portNumberCombo->addItems({tr("COM1"), tr("COM2"), tr("COM3"), tr("COM4")});
        portNumberCombo->setCurrentIndex(i - 1);
        portGridLayout->addWidget(portNumberCombo, 1, 1);
        
        portGridLayout->addWidget(new QLabel(tr("Modo:")), 2, 0);
        QComboBox *modeCombo = new QComboBox();
        modeCombo->addItems({tr("Desconectado"), tr("Puerto del host"), tr("Archivo host"), tr("Tubería host")});
        portGridLayout->addWidget(modeCombo, 2, 1);
        
        portGridLayout->addWidget(new QLabel(tr("Ruta/Dirección:")), 3, 0);
        QLineEdit *pathEdit = new QLineEdit();
        portGridLayout->addWidget(pathEdit, 3, 1);
        
        portLayout->addWidget(portGroup);
        portLayout->addStretch();
        
        m_serialPorts->addTab(portTab, tr("Puerto %1").arg(i));
    }
    
    layout->addWidget(m_serialPorts);
    
    m_tabWidget->addTab(m_serialPortsTab, tr("Puertos Serie"));
}

void VMConfigDialog::createUSBTab()
{
    m_usbTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_usbTab);
    
    // USB Controller
    QGroupBox *controllerGroup = new QGroupBox(tr("Controlador USB"));
    QVBoxLayout *controllerLayout = new QVBoxLayout(controllerGroup);
    
    m_usbControllerCombo = new QComboBox();
    m_usbControllerCombo->addItems({tr("Deshabilitado"), tr("USB 1.1 (OHCI)"), tr("USB 2.0 (EHCI)"), tr("USB 3.0 (xHCI)")});
    m_usbControllerCombo->setCurrentIndex(3); // USB 3.0 by default
    controllerLayout->addWidget(m_usbControllerCombo);
    
    layout->addWidget(controllerGroup);
    
    // USB Filters
    QGroupBox *filtersGroup = new QGroupBox(tr("Filtros de dispositivos USB"));
    QVBoxLayout *filtersLayout = new QVBoxLayout(filtersGroup);
    
    m_usbFiltersList = new QListWidget();
    m_usbFiltersList->addItem(tr("Ejemplo: Dispositivo de almacenamiento USB"));
    filtersLayout->addWidget(m_usbFiltersList);
    
    QHBoxLayout *filterButtonLayout = new QHBoxLayout();
    m_addUSBFilterButton = new QPushButton(tr("Agregar filtro"));
    m_removeUSBFilterButton = new QPushButton(tr("Eliminar filtro"));
    m_editUSBFilterButton = new QPushButton(tr("Editar filtro"));
    
    filterButtonLayout->addWidget(m_addUSBFilterButton);
    filterButtonLayout->addWidget(m_removeUSBFilterButton);
    filterButtonLayout->addWidget(m_editUSBFilterButton);
    filterButtonLayout->addStretch();
    
    filtersLayout->addLayout(filterButtonLayout);
    
    layout->addWidget(filtersGroup);
    
    layout->addStretch();
    
    m_tabWidget->addTab(m_usbTab, tr("USB"));
}

void VMConfigDialog::createSharedFoldersTab()
{
    m_sharedFoldersTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_sharedFoldersTab);
    
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    
    // Left side - folders list
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    
    QLabel *foldersLabel = new QLabel(tr("Carpetas compartidas:"));
    leftLayout->addWidget(foldersLabel);
    
    m_sharedFoldersList = new QListWidget();
    leftLayout->addWidget(m_sharedFoldersList);
    
    QHBoxLayout *foldersButtonLayout = new QHBoxLayout();
    m_addFolderButton = new QPushButton(tr("Agregar"));
    m_removeFolderButton = new QPushButton(tr("Eliminar"));
    m_editFolderButton = new QPushButton(tr("Editar"));
    
    connect(m_addFolderButton, &QPushButton::clicked, this, &VMConfigDialog::addSharedFolder);
    connect(m_removeFolderButton, &QPushButton::clicked, this, &VMConfigDialog::removeSharedFolder);
    
    foldersButtonLayout->addWidget(m_addFolderButton);
    foldersButtonLayout->addWidget(m_removeFolderButton);
    foldersButtonLayout->addWidget(m_editFolderButton);
    foldersButtonLayout->addStretch();
    
    leftLayout->addLayout(foldersButtonLayout);
    
    splitter->addWidget(leftWidget);
    
    // Right side - folder properties
    QGroupBox *propertiesGroup = new QGroupBox(tr("Propiedades de la carpeta"));
    QGridLayout *propertiesLayout = new QGridLayout(propertiesGroup);
    
    propertiesLayout->addWidget(new QLabel(tr("Nombre de la carpeta:")), 0, 0);
    m_folderNameEdit = new QLineEdit();
    propertiesLayout->addWidget(m_folderNameEdit, 0, 1);
    
    propertiesLayout->addWidget(new QLabel(tr("Ruta de la carpeta:")), 1, 0);
    QHBoxLayout *pathLayout = new QHBoxLayout();
    m_folderPathEdit = new QLineEdit();
    m_browseFolderButton = new QPushButton(tr("Examinar..."));
    connect(m_browseFolderButton, &QPushButton::clicked, this, &VMConfigDialog::selectSharedFolderPath);
    pathLayout->addWidget(m_folderPathEdit);
    pathLayout->addWidget(m_browseFolderButton);
    propertiesLayout->addLayout(pathLayout, 1, 1);
    
    m_folderReadOnlyCheckBox = new QCheckBox(tr("Solo lectura"));
    propertiesLayout->addWidget(m_folderReadOnlyCheckBox, 2, 0, 1, 2);
    
    m_folderAutoMountCheckBox = new QCheckBox(tr("Montar automáticamente"));
    m_folderAutoMountCheckBox->setChecked(true);
    propertiesLayout->addWidget(m_folderAutoMountCheckBox, 3, 0, 1, 2);
    
    m_folderPermanentCheckBox = new QCheckBox(tr("Hacer permanente"));
    m_folderPermanentCheckBox->setChecked(true);
    propertiesLayout->addWidget(m_folderPermanentCheckBox, 4, 0, 1, 2);
    
    splitter->addWidget(propertiesGroup);
    
    layout->addWidget(splitter);
    
    m_tabWidget->addTab(m_sharedFoldersTab, tr("Carpetas Compartidas"));
}

void VMConfigDialog::createUserInterfaceTab()
{
    m_userInterfaceTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_userInterfaceTab);
    
    QGroupBox *interfaceGroup = new QGroupBox(tr("Elementos de la interfaz"));
    QVBoxLayout *interfaceLayout = new QVBoxLayout(interfaceGroup);
    
    m_showMenuBarCheckBox = new QCheckBox(tr("Mostrar barra de menú"));
    m_showMenuBarCheckBox->setChecked(true);
    interfaceLayout->addWidget(m_showMenuBarCheckBox);
    
    m_showStatusBarCheckBox = new QCheckBox(tr("Mostrar barra de estado"));
    m_showStatusBarCheckBox->setChecked(true);
    interfaceLayout->addWidget(m_showStatusBarCheckBox);
    
    m_showMiniToolbarCheckBox = new QCheckBox(tr("Mostrar mini barra de herramientas"));
    interfaceLayout->addWidget(m_showMiniToolbarCheckBox);
    
    QHBoxLayout *miniToolbarLayout = new QHBoxLayout();
    miniToolbarLayout->addWidget(new QLabel(tr("Posición mini barra:")));
    m_miniToolbarPositionCombo = new QComboBox();
    m_miniToolbarPositionCombo->addItems({tr("Superior"), tr("Inferior")});
    m_miniToolbarPositionCombo->setEnabled(false);
    miniToolbarLayout->addWidget(m_miniToolbarPositionCombo);
    miniToolbarLayout->addStretch();
    
    connect(m_showMiniToolbarCheckBox, &QCheckBox::toggled, m_miniToolbarPositionCombo, &QComboBox::setEnabled);
    
    interfaceLayout->addLayout(miniToolbarLayout);
    
    layout->addWidget(interfaceGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_userInterfaceTab, tr("Interfaz de Usuario"));
}

void VMConfigDialog::loadVMSettings()
{
    if (!m_virtualMachine) return;
    
    // Load General settings
    m_nameEdit->setText(m_virtualMachine->getName());
    m_descriptionEdit->setPlainText(m_virtualMachine->getDescription());
    
    // Load System settings
    m_memorySpin->setValue(m_virtualMachine->getMemoryMB());
    m_memorySlider->setValue(m_virtualMachine->getMemoryMB());
    updateMemoryLabel(m_virtualMachine->getMemoryMB());
    
    m_cpuCountSpin->setValue(m_virtualMachine->getCPUCount());
    updateCPULabel(m_virtualMachine->getCPUCount());
    
    // Load other settings...
    // TODO: Implement loading of other VM settings
}

void VMConfigDialog::saveVMSettings()
{
    if (!m_virtualMachine) return;
    
    // Save General settings
    m_virtualMachine->setName(m_nameEdit->text());
    m_virtualMachine->setDescription(m_descriptionEdit->toPlainText());
    
    // Save System settings
    m_virtualMachine->setMemoryMB(m_memorySpin->value());
    m_virtualMachine->setCPUCount(m_cpuCountSpin->value());
    
    // Save General settings
    m_virtualMachine->setDescription(m_descriptionEdit->toPlainText());
    
    // Save to XML if KVMManager is available
    if (m_kvmManager) {
        if (m_kvmManager->saveVMConfiguration(m_virtualMachine)) {
            QMessageBox::information(this, tr("Configuración guardada"), 
                               tr("La configuración de la máquina virtual se ha guardado correctamente."));
        } else {
            QMessageBox::warning(this, tr("Error al guardar"), 
                               tr("No se pudo guardar la configuración de la máquina virtual."));
        }
    } else {
        QMessageBox::information(this, tr("Configuración actualizada"), 
                               tr("La configuración se ha actualizado en memoria (sin persistencia)."));
    }
}

void VMConfigDialog::onMemorySliderChanged(int value)
{
    m_memorySpin->setValue(value);
    updateMemoryLabel(value);
}

void VMConfigDialog::onCPUCountChanged(int value)
{
    updateCPULabel(value);
}

void VMConfigDialog::updateMemoryLabel(int memoryMB)
{
    m_memoryLabel->setText(tr("%1 MB").arg(memoryMB));
}

void VMConfigDialog::updateCPULabel(int cpuCount)
{
    m_cpuCountLabel->setText(tr("%1 CPU%2").arg(cpuCount).arg(cpuCount > 1 ? "s" : ""));
}

void VMConfigDialog::addHardDisk()
{
    QMessageBox::information(this, tr("Almacenamiento"), tr("Función en desarrollo"));
}

void VMConfigDialog::removeHardDisk()
{
    QMessageBox::information(this, tr("Almacenamiento"), tr("Función en desarrollo"));
}

void VMConfigDialog::addCDDVD()
{
    QMessageBox::information(this, tr("Almacenamiento"), tr("Función en desarrollo"));
}

void VMConfigDialog::selectBootISO()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Seleccionar imagen ISO"), "", tr("Imágenes ISO (*.iso)"));
    if (!fileName.isEmpty()) {
        // TODO: Set boot ISO
    }
}

void VMConfigDialog::addNetworkAdapter()
{
    QMessageBox::information(this, tr("Red"), tr("Función en desarrollo"));
}

void VMConfigDialog::removeNetworkAdapter()
{
    QMessageBox::information(this, tr("Red"), tr("Función en desarrollo"));
}

void VMConfigDialog::addSharedFolder()
{
    // TODO: Implement shared folder addition
    QMessageBox::information(this, tr("Carpetas Compartidas"), tr("Función en desarrollo"));
}

void VMConfigDialog::removeSharedFolder()
{
    QListWidgetItem *current = m_sharedFoldersList->currentItem();
    if (current) {
        delete current;
    }
}

void VMConfigDialog::selectSharedFolderPath()
{
    QString folder = QFileDialog::getExistingDirectory(this, tr("Seleccionar carpeta para compartir"));
    if (!folder.isEmpty()) {
        m_folderPathEdit->setText(folder);
    }
}

void VMConfigDialog::enableEncryption(bool enabled)
{
    m_encryptionPasswordEdit->setEnabled(enabled);
}

void VMConfigDialog::testAudioDevice()
{
    QMessageBox::information(this, tr("Prueba de Audio"), tr("Reproduciendo sonido de prueba..."));
}

void VMConfigDialog::accept()
{
    saveVMSettings();
    QDialog::accept();
}

// Note: saveVMSettings should be implemented to save the VM configuration
// For now, it's a placeholder that will update the VM object in memory