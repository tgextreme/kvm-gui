#include "AdvancedVMConfigDialog.h"
#include "../core/VirtualMachine.h"
#include "../core/KVMManager.h"
#include "../core/QemuManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QHeaderView>
#include <QRadioButton>
#include <QButtonGroup>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

AdvancedVMConfigDialog::AdvancedVMConfigDialog(VirtualMachine *vm, KVMManager *kvmManager, QWidget *parent)
    : QDialog(parent)
    , m_vm(vm)
    , m_kvmManager(kvmManager)
    , m_qemuManager(nullptr)
{
    setWindowTitle("Configuración de " + vm->getName());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(800, 600);
    setMinimumSize(700, 500);
    
    setupUI();
    loadVMSettings();
}

void AdvancedVMConfigDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget;
    mainLayout->addWidget(m_tabWidget);
    
    setupGeneralTab();
    setupSystemTab();
    setupStorageTab();
    setupNetworkTab();
    setupAudioTab();
    setupSharedFoldersTab();
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    auto *okButton = new QPushButton("&Aceptar");
    auto *cancelButton = new QPushButton("&Cancelar");
    auto *applyButton = new QPushButton("&Aplicar");
    
    connect(okButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(applyButton, &QPushButton::clicked, [this]() {
        if (validateSettings()) {
            saveVMSettings();
        }
    });
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(applyButton);
    
    mainLayout->addLayout(buttonLayout);
}

void AdvancedVMConfigDialog::setupGeneralTab()
{
    m_generalTab = new QWidget;
    m_tabWidget->addTab(m_generalTab, "General");
    
    auto *layout = new QVBoxLayout(m_generalTab);
    
    // Basic Information
    auto *basicGroup = new QGroupBox("Información Básica");
    auto *basicLayout = new QFormLayout(basicGroup);
    
    m_nameEdit = new QLineEdit;
    connect(m_nameEdit, &QLineEdit::textChanged, this, &AdvancedVMConfigDialog::onNameChanged);
    
    m_descriptionEdit = new QTextEdit;
    m_descriptionEdit->setMaximumHeight(80);
    connect(m_descriptionEdit, &QTextEdit::textChanged, this, &AdvancedVMConfigDialog::onDescriptionChanged);
    
    basicLayout->addRow("&Nombre:", m_nameEdit);
    basicLayout->addRow("&Descripción:", m_descriptionEdit);
    
    // Operating System
    auto *osGroup = new QGroupBox("Sistema Operativo");
    auto *osLayout = new QFormLayout(osGroup);
    
    m_osTypeCombo = new QComboBox;
    m_osTypeCombo->addItems({"Linux", "Windows", "BSD", "MacOS", "Otros"});
    
    m_osVersionCombo = new QComboBox;
    m_osVersionCombo->addItems({"Ubuntu", "Debian", "CentOS", "RHEL", "Windows 10", "Windows 11", "FreeBSD", "OpenBSD"});
    
    osLayout->addRow("&Tipo:", m_osTypeCombo);
    osLayout->addRow("&Versión:", m_osVersionCombo);
    
    layout->addWidget(basicGroup);
    layout->addWidget(osGroup);
    layout->addStretch();
}

void AdvancedVMConfigDialog::setupSystemTab()
{
    m_systemTab = new QWidget;
    m_tabWidget->addTab(m_systemTab, "Sistema");
    
    auto *layout = new QVBoxLayout(m_systemTab);
    
    // Memory Configuration
    auto *memoryGroup = new QGroupBox("Memoria");
    auto *memoryLayout = new QGridLayout(memoryGroup);
    
    memoryLayout->addWidget(new QLabel("Memoria Base:"), 0, 0);
    
    m_memorySlider = new QSlider(Qt::Horizontal);
    m_memorySlider->setRange(512, 32768);
    m_memorySlider->setValue(2048);
    m_memorySlider->setTickPosition(QSlider::TicksBelow);
    m_memorySlider->setTickInterval(1024);
    
    m_memorySpin = new QSpinBox;
    m_memorySpin->setRange(512, 32768);
    m_memorySpin->setSuffix(" MB");
    m_memorySpin->setValue(2048);
    
    connect(m_memorySlider, &QSlider::valueChanged, this, &AdvancedVMConfigDialog::onMemorySliderChanged);
    connect(m_memorySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &AdvancedVMConfigDialog::onMemorySpinChanged);
    
    m_memoryInfoLabel = new QLabel;
    
    // Host memory usage bar
    m_hostMemoryBar = new QProgressBar;
    m_hostMemoryBar->setMaximum(100);
    m_hostMemoryBar->setFormat("Memoria del Host: %p%");
    
    memoryLayout->addWidget(m_memorySlider, 0, 1);
    memoryLayout->addWidget(m_memorySpin, 0, 2);
    memoryLayout->addWidget(m_memoryInfoLabel, 1, 0, 1, 3);
    memoryLayout->addWidget(m_hostMemoryBar, 2, 0, 1, 3);
    
    // Processor Configuration
    auto *cpuGroup = new QGroupBox("Procesador");
    auto *cpuLayout = new QFormLayout(cpuGroup);
    
    m_cpuCountSpin = new QSpinBox;
    m_cpuCountSpin->setRange(1, 32);
    m_cpuCountSpin->setValue(2);
    connect(m_cpuCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &AdvancedVMConfigDialog::onCPUCountChanged);
    
    m_chipsetCombo = new QComboBox;
    m_chipsetCombo->addItems({"PC (i440FX)", "PC (Q35)"});
    
    m_enablePAECheck = new QCheckBox("Habilitar PAE/NX");
    m_enableVTxCheck = new QCheckBox("Habilitar VT-x/AMD-V");
    
    cpuLayout->addRow("&Procesadores:", m_cpuCountSpin);
    cpuLayout->addRow("&Chipset:", m_chipsetCombo);
    cpuLayout->addRow(m_enablePAECheck);
    cpuLayout->addRow(m_enableVTxCheck);
    
    // Boot Order
    auto *bootGroup = new QGroupBox("Orden de Arranque");
    auto *bootLayout = new QHBoxLayout(bootGroup);
    
    m_bootOrderList = new QListWidget;
    m_bootOrderList->addItems({"Disco Duro", "CD/DVD", "Red", "Disquete"});
    
    auto *bootButtonLayout = new QVBoxLayout;
    m_bootUpButton = new QPushButton("↑");
    m_bootDownButton = new QPushButton("↓");
    connect(m_bootUpButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::onBootOrderChanged);
    connect(m_bootDownButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::onBootOrderChanged);
    
    bootButtonLayout->addWidget(m_bootUpButton);
    bootButtonLayout->addWidget(m_bootDownButton);
    bootButtonLayout->addStretch();
    
    bootLayout->addWidget(m_bootOrderList);
    bootLayout->addLayout(bootButtonLayout);
    
    layout->addWidget(memoryGroup);
    layout->addWidget(cpuGroup);
    layout->addWidget(bootGroup);
    layout->addStretch();
    
    updateMemoryInfo();
}

void AdvancedVMConfigDialog::setupStorageTab()
{
    m_storageTab = new QWidget;
    m_tabWidget->addTab(m_storageTab, "Almacenamiento");
    
    auto *layout = new QVBoxLayout(m_storageTab);
    
    // Toolbar
    auto *toolbarLayout = new QHBoxLayout;
    
    m_addHardDiskButton = new QPushButton("Añadir Disco Duro");
    m_addOpticalButton = new QPushButton("Añadir Unidad Óptica");
    m_removeStorageButton = new QPushButton("Eliminar");
    m_modifyStorageButton = new QPushButton("Propiedades");
    
    auto *selectISOButton = new QPushButton("Cargar ISO");
    
    m_addHardDiskButton->setIcon(QIcon(":/icons/hdd.png"));
    m_addOpticalButton->setIcon(QIcon(":/icons/cd.png"));
    m_removeStorageButton->setIcon(QIcon(":/icons/remove.png"));
    m_modifyStorageButton->setIcon(QIcon(":/icons/properties.png"));
    selectISOButton->setIcon(QIcon(":/icons/cd.png"));
    
    connect(m_addHardDiskButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::addHardDisk);
    connect(m_addOpticalButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::addOpticalDrive);
    connect(m_removeStorageButton, &QPushButton::clicked, [this]() {
        auto *currentItem = m_storageTree->currentItem();
        if (!currentItem) return;
        
        QString deviceType = currentItem->text(1);
        if (deviceType == "Unidad Óptica") {
            removeOpticalDrive();
        } else {
            removeHardDisk();
        }
    });
    connect(m_modifyStorageButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::modifyHardDisk);
    connect(selectISOButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::selectISO);
    
    toolbarLayout->addWidget(m_addHardDiskButton);
    toolbarLayout->addWidget(m_addOpticalButton);
    toolbarLayout->addWidget(selectISOButton);
    toolbarLayout->addSpacing(10);
    toolbarLayout->addWidget(m_removeStorageButton);
    toolbarLayout->addWidget(m_modifyStorageButton);
    toolbarLayout->addStretch();
    
    // Storage tree and details
    m_storageSplitter = new QSplitter(Qt::Horizontal);
    
    m_storageTree = new QTreeWidget;
    m_storageTree->setHeaderLabels({"Dispositivo", "Tipo", "Tamaño", "Ubicación"});
    m_storageTree->setMinimumWidth(300);
    connect(m_storageTree, &QTreeWidget::itemSelectionChanged, this, &AdvancedVMConfigDialog::onStorageSelectionChanged);
    
    m_storageDetailsPanel = new QWidget;
    m_storageDetailsPanel->setMinimumWidth(250);
    auto *detailsLayout = new QVBoxLayout(m_storageDetailsPanel);
    m_storageDetailsLabel = new QLabel("Seleccione un dispositivo para ver detalles");
    m_storageDetailsLabel->setAlignment(Qt::AlignTop);
    detailsLayout->addWidget(m_storageDetailsLabel);
    detailsLayout->addStretch();
    
    m_storageSplitter->addWidget(m_storageTree);
    m_storageSplitter->addWidget(m_storageDetailsPanel);
    m_storageSplitter->setSizes({400, 300});
    
    layout->addLayout(toolbarLayout);
    layout->addWidget(m_storageSplitter);
    
    updateStorageList();
}

void AdvancedVMConfigDialog::setupNetworkTab()
{
    m_networkTab = new QWidget;
    m_tabWidget->addTab(m_networkTab, "Red");
    
    auto *layout = new QVBoxLayout(m_networkTab);
    
    // Network adapters table
    m_networkTable = new QTableWidget(0, 4);
    m_networkTable->setHorizontalHeaderLabels({"Adaptador", "Habilitado", "Modo", "Dirección MAC"});
    m_networkTable->horizontalHeader()->setStretchLastSection(true);
    m_networkTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Add/Remove buttons
    auto *networkButtonLayout = new QHBoxLayout;
    m_addNetworkButton = new QPushButton("Añadir Adaptador");
    m_removeNetworkButton = new QPushButton("Eliminar Adaptador");
    
    connect(m_addNetworkButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::addNetworkAdapter);
    connect(m_removeNetworkButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::removeNetworkAdapter);
    
    networkButtonLayout->addWidget(m_addNetworkButton);
    networkButtonLayout->addWidget(m_removeNetworkButton);
    networkButtonLayout->addStretch();
    
    // Network adapter details
    m_networkDetailsGroup = new QGroupBox("Detalles del Adaptador");
    auto *detailsLayout = new QFormLayout(m_networkDetailsGroup);
    
    m_networkModeCombo = new QComboBox;
    m_networkModeCombo->addItems({"NAT", "Bridge", "Host-only", "Internal"});
    
    m_networkAdapterCombo = new QComboBox;
    m_networkAdapterCombo->addItems({"Intel PRO/1000 MT Desktop", "AMD PCNet-PCI II", "Realtek RTL8139"});
    
    m_macAddressEdit = new QLineEdit;
    m_macAddressEdit->setInputMask("HH:HH:HH:HH:HH:HH");
    
    m_generateMACButton = new QPushButton("Generar");
    connect(m_generateMACButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::generateMACAddress);
    
    m_cableConnectedCheck = new QCheckBox("Cable Conectado");
    m_cableConnectedCheck->setChecked(true);
    
    auto *macLayout = new QHBoxLayout;
    macLayout->addWidget(m_macAddressEdit);
    macLayout->addWidget(m_generateMACButton);
    
    detailsLayout->addRow("&Modo de Red:", m_networkModeCombo);
    detailsLayout->addRow("&Tipo de Adaptador:", m_networkAdapterCombo);
    detailsLayout->addRow("&Dirección MAC:", macLayout);
    detailsLayout->addRow(m_cableConnectedCheck);
    
    layout->addWidget(m_networkTable);
    layout->addLayout(networkButtonLayout);
    layout->addWidget(m_networkDetailsGroup);
    layout->addStretch();
    
    updateNetworkList();
}

void AdvancedVMConfigDialog::setupAudioTab()
{
    m_audioTab = new QWidget;
    m_tabWidget->addTab(m_audioTab, "Audio");
    
    auto *layout = new QVBoxLayout(m_audioTab);
    
    auto *audioGroup = new QGroupBox("Configuración de Audio");
    auto *audioLayout = new QFormLayout(audioGroup);
    
    m_enableAudioCheck = new QCheckBox("Habilitar Audio");
    connect(m_enableAudioCheck, &QCheckBox::toggled, this, &AdvancedVMConfigDialog::onAudioEnabledChanged);
    
    m_audioControllerCombo = new QComboBox;
    m_audioControllerCombo->addItems({"Intel HD Audio", "AC97", "SB16"});
    connect(m_audioControllerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &AdvancedVMConfigDialog::onAudioControllerChanged);
    
    m_audioDriverCombo = new QComboBox;
    m_audioDriverCombo->addItems({"PulseAudio", "ALSA", "OSS"});
    
    audioLayout->addRow(m_enableAudioCheck);
    audioLayout->addRow("&Controlador:", m_audioControllerCombo);
    audioLayout->addRow("&Driver de Host:", m_audioDriverCombo);
    
    layout->addWidget(audioGroup);
    layout->addStretch();
}

void AdvancedVMConfigDialog::setupSharedFoldersTab()
{
    m_sharedFoldersTab = new QWidget;
    m_tabWidget->addTab(m_sharedFoldersTab, "Carpetas Compartidas");
    
    auto *layout = new QVBoxLayout(m_sharedFoldersTab);
    
    // Shared folders table
    m_sharedFoldersTable = new QTableWidget(0, 4);
    m_sharedFoldersTable->setHorizontalHeaderLabels({"Nombre", "Ruta del Host", "Solo Lectura", "Auto-montar"});
    m_sharedFoldersTable->horizontalHeader()->setStretchLastSection(true);
    m_sharedFoldersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Buttons
    auto *folderButtonLayout = new QHBoxLayout;
    m_addFolderButton = new QPushButton("Añadir Carpeta");
    m_removeFolderButton = new QPushButton("Eliminar Carpeta");
    m_modifyFolderButton = new QPushButton("Modificar Carpeta");
    
    m_addFolderButton->setIcon(QIcon(":/icons/folder_add.png"));
    m_removeFolderButton->setIcon(QIcon(":/icons/folder_remove.png"));
    m_modifyFolderButton->setIcon(QIcon(":/icons/folder_edit.png"));
    
    connect(m_addFolderButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::addSharedFolder);
    connect(m_removeFolderButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::removeSharedFolder);
    connect(m_modifyFolderButton, &QPushButton::clicked, this, &AdvancedVMConfigDialog::modifySharedFolder);
    
    folderButtonLayout->addWidget(m_addFolderButton);
    folderButtonLayout->addWidget(m_removeFolderButton);
    folderButtonLayout->addWidget(m_modifyFolderButton);
    folderButtonLayout->addStretch();
    
    layout->addWidget(m_sharedFoldersTable);
    layout->addLayout(folderButtonLayout);
    
    updateSharedFoldersList();
}

void AdvancedVMConfigDialog::loadVMSettings()
{
    if (!m_vm) return;
    
    // General
    m_nameEdit->setText(m_vm->getName());
    m_descriptionEdit->setPlainText(m_vm->getDescription());
    
    // System
    m_memorySpin->setValue(m_vm->getMemoryMB());
    m_memorySlider->setValue(m_vm->getMemoryMB());
    m_cpuCountSpin->setValue(m_vm->getCPUCount());
    
    // Boot Order
    QStringList bootOrder = m_vm->getBootOrder();
    if (!bootOrder.isEmpty()) {
        m_bootOrderList->clear();
        m_bootOrderList->addItems(bootOrder);
    }
    
    // Load additional settings from XML or use defaults
    updateMemoryInfo();
    updateStorageList();
    updateNetworkList();
    updateSharedFoldersList();
}

void AdvancedVMConfigDialog::saveVMSettings()
{
    if (!m_vm) return;
    
    // General
    m_vm->setName(m_nameEdit->text());
    m_vm->setDescription(m_descriptionEdit->toPlainText());
    
    // System
    m_vm->setMemoryMB(m_memorySpin->value());
    m_vm->setCPUCount(m_cpuCountSpin->value());
    
    // Boot Order - Get current order from list
    QStringList bootOrder;
    for (int i = 0; i < m_bootOrderList->count(); ++i) {
        bootOrder.append(m_bootOrderList->item(i)->text());
    }
    m_vm->setBootOrder(bootOrder);
    
    // Save VM configuration
    if (m_kvmManager) {
        m_kvmManager->saveVMConfiguration(m_vm);
    }
}

bool AdvancedVMConfigDialog::validateSettings()
{
    // Validate VM name
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error de Validación", 
                           "El nombre de la máquina virtual no puede estar vacío.");
        m_tabWidget->setCurrentWidget(m_generalTab);
        m_nameEdit->setFocus();
        return false;
    }
    
    // Validate memory
    int memory = m_memorySpin->value();
    if (memory < 512) {
        QMessageBox::warning(this, "Error de Validación",
                           "La memoria asignada debe ser al menos 512 MB.");
        m_tabWidget->setCurrentWidget(m_systemTab);
        m_memorySpin->setFocus();
        return false;
    }
    
    return true;
}

void AdvancedVMConfigDialog::accept()
{
    if (validateSettings()) {
        saveVMSettings();
        QDialog::accept();
    }
}

// Slots implementation

void AdvancedVMConfigDialog::onNameChanged()
{
    // Update window title when name changes
    setWindowTitle("Configuración de " + m_nameEdit->text());
}

void AdvancedVMConfigDialog::onDescriptionChanged()
{
    // Real-time validation or other actions if needed
}

void AdvancedVMConfigDialog::onMemorySliderChanged(int value)
{
    m_memorySpin->setValue(value);
    updateMemoryInfo();
}

void AdvancedVMConfigDialog::onMemorySpinChanged(int value)
{
    m_memorySlider->setValue(value);
    updateMemoryInfo();
}

void AdvancedVMConfigDialog::onCPUCountChanged(int value)
{
    Q_UNUSED(value)
    // Update CPU-related info if needed
}

void AdvancedVMConfigDialog::onBootOrderChanged()
{
    auto *button = qobject_cast<QPushButton*>(sender());
    if (!button || !m_bootOrderList->currentItem()) return;
    
    int currentRow = m_bootOrderList->currentRow();
    
    if (button == m_bootUpButton && currentRow > 0) {
        auto *item = m_bootOrderList->takeItem(currentRow);
        m_bootOrderList->insertItem(currentRow - 1, item);
        m_bootOrderList->setCurrentRow(currentRow - 1);
    } else if (button == m_bootDownButton && currentRow < m_bootOrderList->count() - 1) {
        auto *item = m_bootOrderList->takeItem(currentRow);
        m_bootOrderList->insertItem(currentRow + 1, item);
        m_bootOrderList->setCurrentRow(currentRow + 1);
    }
}

void AdvancedVMConfigDialog::addHardDisk()
{
    AddHardDiskDialog dialog(nullptr, this);
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.createNewDisk()) {
            // Create new disk using QemuManager
            QString diskPath = dialog.getDiskPath();
            qint64 diskSize = dialog.getDiskSize();
            QString format = dialog.getDiskFormat();
            
            if (m_qemuManager && m_qemuManager->createDisk(diskPath, format, diskSize)) {
                updateStorageList();
            } else {
                QMessageBox::critical(this, "Error",
                                    "No se pudo crear el disco duro.");
            }
        } else {
            // Use existing disk
            updateStorageList();
        }
    }
}

void AdvancedVMConfigDialog::removeHardDisk()
{
    auto *currentItem = m_storageTree->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "Información",
                               "Seleccione un dispositivo para eliminar.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirmar Eliminación",
                                   "¿Está seguro de que desea eliminar este dispositivo?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        delete currentItem;
        updateStorageList();
    }
}

void AdvancedVMConfigDialog::modifyHardDisk()
{
    auto *currentItem = m_storageTree->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "Información",
                               "Seleccione un dispositivo para modificar.");
        return;
    }
    
    QString deviceType = currentItem->text(1);
    
    if (deviceType == "Unidad Óptica") {
        // Show optical drive properties dialog
        AddOpticalDriveDialog dialog(this);
        dialog.setWindowTitle("Propiedades de Unidad Óptica");
        
        // Pre-fill current ISO if exists
        QString currentISO = m_vm->getCDROMImage();
        dialog.setISOPath(currentISO);
        dialog.setConnected(true); // Always connected for now
        
        if (dialog.exec() == QDialog::Accepted) {
            QString isoPath = dialog.getISOPath();
            m_vm->setCDROMImage(isoPath);
            updateStorageList();
            
            if (!isoPath.isEmpty()) {
                QMessageBox::information(this, "ISO Cambiado",
                                       QString("Nueva imagen ISO: %1").arg(isoPath));
            } else {
                QMessageBox::information(this, "Unidad Vacía",
                                       "La unidad óptica ahora está vacía.");
            }
        }
    } else {
        // Hard disk properties - to be implemented
        QMessageBox::information(this, "Información",
                               "Propiedades de disco duro en desarrollo.");
    }
}

void AdvancedVMConfigDialog::addOpticalDrive()
{
    AddOpticalDriveDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString isoPath = dialog.getISOPath();
        
        if (!isoPath.isEmpty() && m_vm) {
            // Set the CDROM image in the VM
            m_vm->setCDROMImage(isoPath);
            
            QMessageBox::information(this, "Unidad Óptica Añadida",
                                   QString("Unidad óptica añadida con imagen: %1").arg(isoPath));
        } else if (m_vm) {
            // Empty optical drive
            m_vm->setCDROMImage("");
            
            QMessageBox::information(this, "Unidad Óptica Añadida",
                                   "Unidad óptica vacía añadida correctamente.");
        }
        
        updateStorageList();
    }
}

void AdvancedVMConfigDialog::removeOpticalDrive()
{
    auto *currentItem = m_storageTree->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "Información",
                               "Seleccione un dispositivo para eliminar.");
        return;
    }
    
    // Check if it's an optical drive
    QString deviceType = currentItem->text(1);
    if (deviceType != "Unidad Óptica") {
        QMessageBox::information(this, "Información",
                               "Seleccione una unidad óptica para eliminar.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirmar Eliminación",
                                   "¿Está seguro de que desea eliminar esta unidad óptica?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes && m_vm) {
        // Remove CDROM image from VM
        m_vm->setCDROMImage("");
        updateStorageList();
        
        QMessageBox::information(this, "Información",
                               "Unidad óptica eliminada correctamente.");
    }
}

void AdvancedVMConfigDialog::selectISO()
{
    QString isoPath = QFileDialog::getOpenFileName(this,
        "Seleccionar archivo ISO", 
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        "Archivos ISO (*.iso);;Todos los archivos (*)");
    
    if (!isoPath.isEmpty() && m_vm) {
        // Set the ISO in the optical drive
        m_vm->setCDROMImage(isoPath);
        updateStorageList();
        
        QMessageBox::information(this, "ISO Cargado",
                               QString("Archivo ISO cargado: %1").arg(isoPath));
    }
}

void AdvancedVMConfigDialog::onStorageSelectionChanged()
{
    auto *currentItem = m_storageTree->currentItem();
    if (currentItem) {
        QString details = QString("Tipo: %1\nTamaño: %2\nUbicación: %3")
                          .arg(currentItem->text(1))
                          .arg(currentItem->text(2))
                          .arg(currentItem->text(3));
        
        m_storageDetailsLabel->setText(details);
    } else {
        m_storageDetailsLabel->setText("Seleccione un dispositivo para ver detalles");
    }
}

void AdvancedVMConfigDialog::addNetworkAdapter()
{
    int row = m_networkTable->rowCount();
    m_networkTable->insertRow(row);
    
    auto *nameItem = new QTableWidgetItem(QString("Adaptador %1").arg(row + 1));
    auto *enabledCheck = new QCheckBox;
    enabledCheck->setChecked(true);
    auto *modeItem = new QTableWidgetItem("NAT");
    auto *macItem = new QTableWidgetItem("52:54:00:12:34:56");
    
    m_networkTable->setItem(row, 0, nameItem);
    m_networkTable->setCellWidget(row, 1, enabledCheck);
    m_networkTable->setItem(row, 2, modeItem);
    m_networkTable->setItem(row, 3, macItem);
}

void AdvancedVMConfigDialog::removeNetworkAdapter()
{
    int currentRow = m_networkTable->currentRow();
    if (currentRow >= 0) {
        m_networkTable->removeRow(currentRow);
    }
}

void AdvancedVMConfigDialog::onNetworkAdapterChanged()
{
    // Update network adapter settings
}

void AdvancedVMConfigDialog::generateMACAddress()
{
    // Generate random MAC address
    QStringList macParts;
    macParts << "52:54:00"; // QEMU prefix
    
    for (int i = 0; i < 3; ++i) {
        int value = QRandomGenerator::global()->bounded(256);
        macParts << QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
    }
    
    m_macAddressEdit->setText(macParts.join(":"));
}

void AdvancedVMConfigDialog::onAudioEnabledChanged()
{
    bool enabled = m_enableAudioCheck->isChecked();
    m_audioControllerCombo->setEnabled(enabled);
    m_audioDriverCombo->setEnabled(enabled);
}

void AdvancedVMConfigDialog::onAudioControllerChanged()
{
    // Update audio configuration
}

void AdvancedVMConfigDialog::addSharedFolder()
{
    AddSharedFolderDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int row = m_sharedFoldersTable->rowCount();
        m_sharedFoldersTable->insertRow(row);
        
        m_sharedFoldersTable->setItem(row, 0, new QTableWidgetItem(dialog.getFolderName()));
        m_sharedFoldersTable->setItem(row, 1, new QTableWidgetItem(dialog.getFolderPath()));
        m_sharedFoldersTable->setItem(row, 2, new QTableWidgetItem(dialog.isReadOnly() ? "Sí" : "No"));
        m_sharedFoldersTable->setItem(row, 3, new QTableWidgetItem(dialog.isAutoMount() ? "Sí" : "No"));
    }
}

void AdvancedVMConfigDialog::removeSharedFolder()
{
    int currentRow = m_sharedFoldersTable->currentRow();
    if (currentRow >= 0) {
        m_sharedFoldersTable->removeRow(currentRow);
    }
}

void AdvancedVMConfigDialog::modifySharedFolder()
{
    int currentRow = m_sharedFoldersTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "Información",
                               "Seleccione una carpeta compartida para modificar.");
        return;
    }
    
    // Get current values
    QString name = m_sharedFoldersTable->item(currentRow, 0)->text();
    QString path = m_sharedFoldersTable->item(currentRow, 1)->text();
    bool readOnly = m_sharedFoldersTable->item(currentRow, 2)->text() == "Sí";
    bool autoMount = m_sharedFoldersTable->item(currentRow, 3)->text() == "Sí";
    
    AddSharedFolderDialog dialog(this);
    // Set current values in dialog (would need to add setters)
    
    if (dialog.exec() == QDialog::Accepted) {
        m_sharedFoldersTable->setItem(currentRow, 0, new QTableWidgetItem(dialog.getFolderName()));
        m_sharedFoldersTable->setItem(currentRow, 1, new QTableWidgetItem(dialog.getFolderPath()));
        m_sharedFoldersTable->setItem(currentRow, 2, new QTableWidgetItem(dialog.isReadOnly() ? "Sí" : "No"));
        m_sharedFoldersTable->setItem(currentRow, 3, new QTableWidgetItem(dialog.isAutoMount() ? "Sí" : "No"));
    }
}

void AdvancedVMConfigDialog::selectFolderPath()
{
    QString folderPath = QFileDialog::getExistingDirectory(this,
        "Seleccionar carpeta",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    
    if (!folderPath.isEmpty()) {
        // Set folder path in current dialog
    }
}

void AdvancedVMConfigDialog::updateMemoryInfo()
{
    int vmMemory = m_memorySpin->value();
    
    // Get system memory info (simplified)
    int totalMemory = 8192; // Would get from system
    int usedMemory = vmMemory;
    int percentage = (usedMemory * 100) / totalMemory;
    
    m_memoryInfoLabel->setText(QString("Memoria asignada: %1 MB").arg(vmMemory));
    m_hostMemoryBar->setValue(percentage);
}

void AdvancedVMConfigDialog::updateStorageList()
{
    m_storageTree->clear();
    
    if (!m_vm) return;
    
    // Add existing disks from VM configuration
    auto diskPaths = m_vm->getHardDisks();
    for (const QString &diskPath : diskPaths) {
        auto *item = new QTreeWidgetItem(m_storageTree);
        item->setText(0, QFileInfo(diskPath).baseName());
        item->setText(1, "Disco Duro");
        item->setText(2, "Variable"); // Would calculate actual size
        item->setText(3, diskPath);
        item->setIcon(0, QIcon(":/icons/hdd.png"));
    }
    
    // Add CDROM/Optical drive
    QString cdromImage = m_vm->getCDROMImage();
    auto *cdromItem = new QTreeWidgetItem(m_storageTree);
    if (!cdromImage.isEmpty()) {
        cdromItem->setText(0, QFileInfo(cdromImage).baseName());
        cdromItem->setText(1, "Unidad Óptica");
        cdromItem->setText(2, "ISO");
        cdromItem->setText(3, cdromImage);
    } else {
        cdromItem->setText(0, "Unidad CD/DVD");
        cdromItem->setText(1, "Unidad Óptica");
        cdromItem->setText(2, "Vacía");
        cdromItem->setText(3, "");
    }
    cdromItem->setIcon(0, QIcon(":/icons/cd.png"));
    
    // Expand all items
    m_storageTree->expandAll();
    m_storageTree->resizeColumnToContents(0);
}

void AdvancedVMConfigDialog::updateNetworkList()
{
    // Clear and populate with VM's network adapters
    m_networkTable->setRowCount(0);
    
    // Add at least one adapter
    addNetworkAdapter();
}

void AdvancedVMConfigDialog::updateSharedFoldersList()
{
    // Clear and populate with VM's shared folders
    m_sharedFoldersTable->setRowCount(0);
    
    // Would load from VM configuration
}

// AddHardDiskDialog Implementation

AddHardDiskDialog::AddHardDiskDialog(QemuManager *qemuManager, QWidget *parent)
    : QDialog(parent)
    , m_qemuManager(qemuManager)
{
    setWindowTitle("Añadir Disco Duro Virtual");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(500, 400);
    
    setupUI();
}

void AddHardDiskDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Create new or use existing
    auto *typeGroup = new QGroupBox("Tipo de Disco");
    auto *typeLayout = new QVBoxLayout(typeGroup);
    
    m_createNewRadio = new QRadioButton("Crear nuevo disco duro virtual");
    m_useExistingRadio = new QRadioButton("Usar archivo de disco existente");
    m_createNewRadio->setChecked(true);
    
    connect(m_createNewRadio, &QRadioButton::toggled, this, &AddHardDiskDialog::onCreateNewToggled);
    connect(m_useExistingRadio, &QRadioButton::toggled, this, &AddHardDiskDialog::onUseExistingToggled);
    
    typeLayout->addWidget(m_createNewRadio);
    typeLayout->addWidget(m_useExistingRadio);
    
    // Create new group
    m_createNewGroup = new QGroupBox("Nuevo Disco Virtual");
    auto *newLayout = new QFormLayout(m_createNewGroup);
    
    auto *pathLayout = new QHBoxLayout;
    m_newPathEdit = new QLineEdit;
    m_browseNewButton = new QPushButton("...");
    connect(m_browseNewButton, &QPushButton::clicked, this, &AddHardDiskDialog::onBrowseNewPath);
    
    pathLayout->addWidget(m_newPathEdit);
    pathLayout->addWidget(m_browseNewButton);
    
    m_formatCombo = new QComboBox;
    m_formatCombo->addItems({"qcow2", "raw", "vmdk", "vdi"});
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AddHardDiskDialog::onFormatChanged);
    
    auto *sizeLayout = new QHBoxLayout;
    m_sizeSpin = new QSpinBox;
    m_sizeSpin->setRange(1, 9999);
    m_sizeSpin->setValue(20);
    connect(m_sizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddHardDiskDialog::onSizeChanged);
    
    m_sizeUnitCombo = new QComboBox;
    m_sizeUnitCombo->addItems({"GB", "MB", "TB"});
    
    sizeLayout->addWidget(m_sizeSpin);
    sizeLayout->addWidget(m_sizeUnitCombo);
    
    m_allocationCombo = new QComboBox;
    m_allocationCombo->addItems({"Asignación Dinámica", "Tamaño Fijo"});
    
    m_estimatedSizeLabel = new QLabel;
    
    newLayout->addRow("&Ubicación:", pathLayout);
    newLayout->addRow("&Formato:", m_formatCombo);
    newLayout->addRow("&Tamaño:", sizeLayout);
    newLayout->addRow("&Asignación:", m_allocationCombo);
    newLayout->addRow("Tamaño estimado:", m_estimatedSizeLabel);
    
    // Use existing group
    m_useExistingGroup = new QGroupBox("Disco Existente");
    auto *existingLayout = new QFormLayout(m_useExistingGroup);
    
    auto *existingPathLayout = new QHBoxLayout;
    m_existingPathEdit = new QLineEdit;
    m_browseExistingButton = new QPushButton("...");
    connect(m_browseExistingButton, &QPushButton::clicked, this, &AddHardDiskDialog::onBrowseExistingPath);
    
    existingPathLayout->addWidget(m_existingPathEdit);
    existingPathLayout->addWidget(m_browseExistingButton);
    
    existingLayout->addRow("&Archivo:", existingPathLayout);
    
    m_useExistingGroup->setEnabled(false);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    auto *okButton = new QPushButton("&Aceptar");
    auto *cancelButton = new QPushButton("&Cancelar");
    
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addWidget(typeGroup);
    mainLayout->addWidget(m_createNewGroup);
    mainLayout->addWidget(m_useExistingGroup);
    mainLayout->addLayout(buttonLayout);
    
    updateEstimatedSize();
}

QString AddHardDiskDialog::getDiskPath() const
{
    return m_createNewRadio->isChecked() ? m_newPathEdit->text() : m_existingPathEdit->text();
}

QString AddHardDiskDialog::getDiskFormat() const
{
    return m_formatCombo->currentText();
}

qint64 AddHardDiskDialog::getDiskSize() const
{
    qint64 size = m_sizeSpin->value();
    QString unit = m_sizeUnitCombo->currentText();
    
    if (unit == "MB") {
        return size;
    } else if (unit == "GB") {
        return size * 1024;
    } else if (unit == "TB") {
        return size * 1024 * 1024;
    }
    
    return size * 1024; // Default GB
}

bool AddHardDiskDialog::isDynamicAllocation() const
{
    return m_allocationCombo->currentIndex() == 0;
}

bool AddHardDiskDialog::createNewDisk() const
{
    return m_createNewRadio->isChecked();
}

void AddHardDiskDialog::onCreateNewToggled(bool checked)
{
    m_createNewGroup->setEnabled(checked);
}

void AddHardDiskDialog::onUseExistingToggled(bool checked)
{
    m_useExistingGroup->setEnabled(checked);
}

void AddHardDiskDialog::onBrowseNewPath()
{
    QString path = QFileDialog::getSaveFileName(this,
        "Ubicación del nuevo disco virtual",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        "Archivos QEMU (*.qcow2 *.raw *.vmdk *.vdi)");
    
    if (!path.isEmpty()) {
        m_newPathEdit->setText(path);
    }
}

void AddHardDiskDialog::onBrowseExistingPath()
{
    QString path = QFileDialog::getOpenFileName(this,
        "Seleccionar archivo de disco",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        "Archivos de disco (*.qcow2 *.raw *.vmdk *.vdi)");
    
    if (!path.isEmpty()) {
        m_existingPathEdit->setText(path);
    }
}

void AddHardDiskDialog::onFormatChanged()
{
    updateEstimatedSize();
}

void AddHardDiskDialog::onSizeChanged()
{
    updateEstimatedSize();
}

void AddHardDiskDialog::updateEstimatedSize()
{
    qint64 size = getDiskSize();
    bool dynamic = isDynamicAllocation();
    
    QString sizeText;
    if (dynamic) {
        sizeText = QString("Inicial: ~1 MB, Máximo: %1 MB").arg(size);
    } else {
        sizeText = QString("%1 MB").arg(size);
    }
    
    m_estimatedSizeLabel->setText(sizeText);
}

bool AddHardDiskDialog::validateInput()
{
    if (m_createNewRadio->isChecked()) {
        return !m_newPathEdit->text().trimmed().isEmpty();
    } else {
        return !m_existingPathEdit->text().trimmed().isEmpty() && 
               QFile::exists(m_existingPathEdit->text());
    }
}

// AddOpticalDriveDialog Implementation

AddOpticalDriveDialog::AddOpticalDriveDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Añadir Unidad Óptica");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(400, 200);
    
    setupUI();
}

void AddOpticalDriveDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Type selection
    auto *typeGroup = new QGroupBox("Tipo de Medio");
    auto *typeLayout = new QVBoxLayout(typeGroup);
    
    m_emptyRadio = new QRadioButton("Unidad vacía");
    m_isoRadio = new QRadioButton("Archivo de imagen ISO");
    m_emptyRadio->setChecked(true);
    
    connect(m_emptyRadio, &QRadioButton::toggled, this, &AddOpticalDriveDialog::onEmptyToggled);
    connect(m_isoRadio, &QRadioButton::toggled, this, &AddOpticalDriveDialog::onISOToggled);
    
    typeLayout->addWidget(m_emptyRadio);
    typeLayout->addWidget(m_isoRadio);
    
    // ISO selection
    m_isoGroup = new QGroupBox("Archivo ISO");
    auto *isoLayout = new QHBoxLayout(m_isoGroup);
    
    m_isoPathEdit = new QLineEdit;
    m_browseISOButton = new QPushButton("...");
    connect(m_browseISOButton, &QPushButton::clicked, this, &AddOpticalDriveDialog::onBrowseISO);
    
    isoLayout->addWidget(m_isoPathEdit);
    isoLayout->addWidget(m_browseISOButton);
    
    m_isoGroup->setEnabled(false);
    
    m_connectedCheck = new QCheckBox("Conectado al arranque");
    m_connectedCheck->setChecked(true);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    auto *okButton = new QPushButton("&Aceptar");
    auto *cancelButton = new QPushButton("&Cancelar");
    
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addWidget(typeGroup);
    mainLayout->addWidget(m_isoGroup);
    mainLayout->addWidget(m_connectedCheck);
    mainLayout->addLayout(buttonLayout);
}

QString AddOpticalDriveDialog::getISOPath() const
{
    return m_isoRadio->isChecked() ? m_isoPathEdit->text() : QString();
}

bool AddOpticalDriveDialog::isConnected() const
{
    return m_connectedCheck->isChecked();
}

void AddOpticalDriveDialog::setISOPath(const QString &path)
{
    if (path.isEmpty()) {
        m_emptyRadio->setChecked(true);
        m_isoGroup->setEnabled(false);
    } else {
        m_isoRadio->setChecked(true);
        m_isoPathEdit->setText(path);
        m_isoGroup->setEnabled(true);
    }
}

void AddOpticalDriveDialog::setConnected(bool connected)
{
    m_connectedCheck->setChecked(connected);
}

void AddOpticalDriveDialog::onBrowseISO()
{
    QString path = QFileDialog::getOpenFileName(this,
        "Seleccionar archivo ISO",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        "Archivos ISO (*.iso)");
    
    if (!path.isEmpty()) {
        m_isoPathEdit->setText(path);
    }
}

void AddOpticalDriveDialog::onEmptyToggled(bool checked)
{
    Q_UNUSED(checked)
    m_isoGroup->setEnabled(false);
}

void AddOpticalDriveDialog::onISOToggled(bool checked)
{
    m_isoGroup->setEnabled(checked);
}

// AddSharedFolderDialog Implementation

AddSharedFolderDialog::AddSharedFolderDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Añadir Carpeta Compartida");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(400, 200);
    
    setupUI();
}

void AddSharedFolderDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    auto *formLayout = new QFormLayout;
    
    m_nameEdit = new QLineEdit;
    connect(m_nameEdit, &QLineEdit::textChanged, this, &AddSharedFolderDialog::onNameChanged);
    
    auto *pathLayout = new QHBoxLayout;
    m_pathEdit = new QLineEdit;
    m_browseButton = new QPushButton("...");
    connect(m_browseButton, &QPushButton::clicked, this, &AddSharedFolderDialog::onBrowsePath);
    
    pathLayout->addWidget(m_pathEdit);
    pathLayout->addWidget(m_browseButton);
    
    m_readOnlyCheck = new QCheckBox("Solo lectura");
    m_autoMountCheck = new QCheckBox("Montar automáticamente");
    m_autoMountCheck->setChecked(true);
    
    formLayout->addRow("&Nombre:", m_nameEdit);
    formLayout->addRow("&Ruta:", pathLayout);
    formLayout->addRow(m_readOnlyCheck);
    formLayout->addRow(m_autoMountCheck);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    auto *okButton = new QPushButton("&Aceptar");
    auto *cancelButton = new QPushButton("&Cancelar");
    
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
}

QString AddSharedFolderDialog::getFolderName() const
{
    return m_nameEdit->text();
}

QString AddSharedFolderDialog::getFolderPath() const
{
    return m_pathEdit->text();
}

bool AddSharedFolderDialog::isReadOnly() const
{
    return m_readOnlyCheck->isChecked();
}

bool AddSharedFolderDialog::isAutoMount() const
{
    return m_autoMountCheck->isChecked();
}

void AddSharedFolderDialog::onBrowsePath()
{
    QString path = QFileDialog::getExistingDirectory(this,
        "Seleccionar carpeta",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
        
        // Auto-generate name from folder
        if (m_nameEdit->text().isEmpty()) {
            QDir dir(path);
            m_nameEdit->setText(dir.dirName());
        }
    }
}

void AddSharedFolderDialog::onNameChanged()
{
    // Validate name in real-time
}

bool AddSharedFolderDialog::validateInput()
{
    return !m_nameEdit->text().trimmed().isEmpty() && 
           !m_pathEdit->text().trimmed().isEmpty() &&
           QDir(m_pathEdit->text()).exists();
}