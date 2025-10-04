#include "DiskManagerDialog.h"
#include "../core/KVMManager.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QHeaderView>
#include <QSplitter>
#include <QFormLayout>

DiskManagerDialog::DiskManagerDialog(KVMManager *kvmManager, QWidget *parent)
    : QDialog(parent)
    , m_kvmManager(kvmManager)
    , m_currentTab(HardDisksTab)
{
    setWindowTitle(tr("Administrador de Medios Virtuales"));
    setWindowIcon(QIcon(":/icons/disk-manager.png"));
    setModal(false);
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    setupUI();
    updateDiskList();
}

void DiskManagerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Menu bar
    setupMenuBar();
    mainLayout->addWidget(m_menuBar);
    
    // Tool bar
    setupToolBar();
    mainLayout->addWidget(m_toolBar);
    
    // Tab widget
    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabPosition(QTabWidget::North);
    
    setupHardDiskTab();
    setupOpticalTab();
    setupFloppyTab();
    
    m_tabWidget->addTab(m_hardDiskTab, QIcon(":/icons/harddisk.png"), tr("Discos Duros"));
    m_tabWidget->addTab(m_opticalTab, QIcon(":/icons/optical.png"), tr("Discos Ópticos"));
    m_tabWidget->addTab(m_floppyTab, QIcon(":/icons/floppy.png"), tr("Discos Flexibles"));
    
    mainLayout->addWidget(m_tabWidget);
    
    // Status bar
    m_statusBar = new QStatusBar();
    m_statusBar->showMessage(tr("Listo"));
    mainLayout->addWidget(m_statusBar);
    
    // Connections
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &DiskManagerDialog::onTabChanged);
}

void DiskManagerDialog::setupMenuBar()
{
    m_menuBar = new QMenuBar(this);
    
    // Actions
    m_createAction = new QAction(QIcon(":/icons/disk-create.png"), tr("&Crear..."), this);
    m_createAction->setShortcut(QKeySequence::New);
    m_createAction->setStatusTip(tr("Crear un nuevo disco virtual"));
    connect(m_createAction, &QAction::triggered, this, &DiskManagerDialog::onCreateDisk);
    
    m_copyAction = new QAction(QIcon(":/icons/disk-copy.png"), tr("C&opiar..."), this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setStatusTip(tr("Copiar el disco virtual seleccionado"));
    m_copyAction->setEnabled(false);
    connect(m_copyAction, &QAction::triggered, this, &DiskManagerDialog::onCopyDisk);
    
    m_modifyAction = new QAction(QIcon(":/icons/disk-modify.png"), tr("&Modificar..."), this);
    m_modifyAction->setStatusTip(tr("Modificar las propiedades del disco virtual"));
    m_modifyAction->setEnabled(false);
    connect(m_modifyAction, &QAction::triggered, this, &DiskManagerDialog::onModifyDisk);
    
    m_removeAction = new QAction(QIcon(":/icons/disk-remove.png"), tr("&Eliminar"), this);
    m_removeAction->setShortcut(QKeySequence::Delete);
    m_removeAction->setStatusTip(tr("Eliminar el disco virtual seleccionado"));
    m_removeAction->setEnabled(false);
    connect(m_removeAction, &QAction::triggered, this, &DiskManagerDialog::onRemoveDisk);
    
    m_releaseAction = new QAction(QIcon(":/icons/disk-release.png"), tr("&Liberar..."), this);
    m_releaseAction->setStatusTip(tr("Liberar el disco virtual de todas las máquinas"));
    m_releaseAction->setEnabled(false);
    connect(m_releaseAction, &QAction::triggered, this, &DiskManagerDialog::onReleaseDisk);
    
    m_refreshAction = new QAction(QIcon(":/icons/refresh.png"), tr("A&ctualizar"), this);
    m_refreshAction->setShortcut(QKeySequence::Refresh);
    m_refreshAction->setStatusTip(tr("Actualizar la lista de discos"));
    connect(m_refreshAction, &QAction::triggered, this, &DiskManagerDialog::onRefreshList);
    
    m_propertiesAction = new QAction(QIcon(":/icons/properties.png"), tr("&Propiedades..."), this);
    m_propertiesAction->setStatusTip(tr("Ver las propiedades del disco virtual"));
    m_propertiesAction->setEnabled(false);
    
    // Menus
    QMenu *actionMenu = m_menuBar->addMenu(tr("&Acciones"));
    actionMenu->addAction(m_createAction);
    actionMenu->addAction(m_copyAction);
    actionMenu->addAction(m_modifyAction);
    actionMenu->addSeparator();
    actionMenu->addAction(m_removeAction);
    actionMenu->addAction(m_releaseAction);
    actionMenu->addSeparator();
    actionMenu->addAction(m_refreshAction);
    actionMenu->addSeparator();
    actionMenu->addAction(m_propertiesAction);
}

void DiskManagerDialog::setupToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_toolBar->setIconSize(QSize(32, 32));
    
    m_toolBar->addAction(m_createAction);
    m_toolBar->addAction(m_copyAction);
    m_toolBar->addAction(m_modifyAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_removeAction);
    m_toolBar->addAction(m_releaseAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_refreshAction);
    m_toolBar->addAction(m_propertiesAction);
}

void DiskManagerDialog::setupHardDiskTab()
{
    m_hardDiskTab = new QWidget();
    
    m_hardDiskSplitter = new QSplitter(Qt::Horizontal, m_hardDiskTab);
    
    // Left side - disk tree
    m_hardDiskTree = new QTreeWidget();
    m_hardDiskTree->setHeaderLabels({tr("Nombre"), tr("Tamaño Virtual"), tr("Tamaño Real"), tr("Ubicación")});
    m_hardDiskTree->setAlternatingRowColors(true);
    m_hardDiskTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_hardDiskTree->setMinimumWidth(400);
    
    // Right side - info panel
    m_hardDiskInfoPanel = new QWidget();
    m_hardDiskInfoPanel->setMinimumWidth(300);
    m_hardDiskInfoPanel->setMaximumWidth(350);
    
    QVBoxLayout *infoPanelLayout = new QVBoxLayout(m_hardDiskInfoPanel);
    
    QGroupBox *infoGroup = new QGroupBox(tr("Información"));
    QFormLayout *infoForm = new QFormLayout(infoGroup);
    
    m_hardDiskInfoLabel = new QLabel(tr("Seleccione un disco para ver su información"));
    m_hardDiskInfoLabel->setWordWrap(true);
    m_hardDiskInfoLabel->setAlignment(Qt::AlignTop);
    infoForm->addRow(m_hardDiskInfoLabel);
    
    infoPanelLayout->addWidget(infoGroup);
    infoPanelLayout->addStretch();
    
    m_hardDiskSplitter->addWidget(m_hardDiskTree);
    m_hardDiskSplitter->addWidget(m_hardDiskInfoPanel);
    m_hardDiskSplitter->setSizes({600, 300});
    
    QHBoxLayout *hardDiskLayout = new QHBoxLayout(m_hardDiskTab);
    hardDiskLayout->addWidget(m_hardDiskSplitter);
    
    // Connections
    connect(m_hardDiskTree, &QTreeWidget::itemSelectionChanged, 
            this, &DiskManagerDialog::onDiskSelectionChanged);
}

void DiskManagerDialog::setupOpticalTab()
{
    m_opticalTab = new QWidget();
    
    QHBoxLayout *opticalLayout = new QHBoxLayout(m_opticalTab);
    
    m_opticalTree = new QTreeWidget();
    m_opticalTree->setHeaderLabels({tr("Nombre"), tr("Tamaño"), tr("Ubicación")});
    m_opticalTree->setAlternatingRowColors(true);
    
    opticalLayout->addWidget(m_opticalTree);
}

void DiskManagerDialog::setupFloppyTab()
{
    m_floppyTab = new QWidget();
    
    QHBoxLayout *floppyLayout = new QHBoxLayout(m_floppyTab);
    
    m_floppyTree = new QTreeWidget();
    m_floppyTree->setHeaderLabels({tr("Nombre"), tr("Tamaño"), tr("Ubicación")});
    m_floppyTree->setAlternatingRowColors(true);
    
    floppyLayout->addWidget(m_floppyTree);
}

void DiskManagerDialog::onCreateDisk()
{
    CreateVirtualDiskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString path = dialog.getDiskPath();
        QString format = dialog.getDiskFormat();
        qint64 size = dialog.getDiskSize();
        bool dynamic = dialog.isDynamicAllocation();
        
        // TODO: Implementar creación real del disco
        QMessageBox::information(this, tr("Crear Disco"),
            tr("Se creará el disco:\nRuta: %1\nFormato: %2\nTamaño: %3 GB\nAsignación: %4")
            .arg(path).arg(format).arg(size).arg(dynamic ? tr("Dinámica") : tr("Fija")));
        
        updateDiskList();
    }
}

void DiskManagerDialog::onCopyDisk()
{
    if (m_selectedDisk.isEmpty()) {
        return;
    }
    
    QString newPath = QFileDialog::getSaveFileName(this,
        tr("Copiar Disco Virtual"), QString(),
        tr("Archivos de Disco (*.qcow2 *.img *.vdi *.vmdk)"));
    
    if (!newPath.isEmpty()) {
        // TODO: Implementar copia real del disco
        QMessageBox::information(this, tr("Copiar Disco"),
            tr("Se copiará el disco:\nOrigen: %1\nDestino: %2")
            .arg(m_selectedDisk).arg(newPath));
        updateDiskList();
    }
}

void DiskManagerDialog::onModifyDisk()
{
    if (m_selectedDisk.isEmpty()) {
        return;
    }
    
    // TODO: Implementar modificación del disco (redimensionar, etc.)
    QMessageBox::information(this, tr("Modificar Disco"),
        tr("Funcionalidad de modificación de disco será implementada"));
}

void DiskManagerDialog::onRemoveDisk()
{
    if (m_selectedDisk.isEmpty()) {
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Eliminar Disco"),
        tr("¿Está seguro que desea eliminar el disco virtual?\n%1\n\nEsta acción no se puede deshacer.")
        .arg(m_selectedDisk),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // TODO: Implementar eliminación real del disco
        QMessageBox::information(this, tr("Eliminar Disco"),
            tr("Se eliminará el disco: %1").arg(m_selectedDisk));
        updateDiskList();
    }
}

void DiskManagerDialog::onReleaseDisk()
{
    if (m_selectedDisk.isEmpty()) {
        return;
    }
    
    // TODO: Implementar liberación del disco de todas las VMs
    QMessageBox::information(this, tr("Liberar Disco"),
        tr("Se liberará el disco: %1").arg(m_selectedDisk));
}

void DiskManagerDialog::onRefreshList()
{
    updateDiskList();
    m_statusBar->showMessage(tr("Lista actualizada"), 2000);
}

void DiskManagerDialog::onDiskSelectionChanged()
{
    QTreeWidget *currentTree = nullptr;
    
    switch (m_currentTab) {
        case HardDisksTab:
            currentTree = m_hardDiskTree;
            break;
        case OpticalDisksTab:
            currentTree = m_opticalTree;
            break;
        case FloppyDisksTab:
            currentTree = m_floppyTree;
            break;
    }
    
    if (currentTree) {
        QTreeWidgetItem *item = currentTree->currentItem();
        if (item) {
            m_selectedDisk = item->data(0, Qt::UserRole).toString();
            m_copyAction->setEnabled(true);
            m_modifyAction->setEnabled(true);
            m_removeAction->setEnabled(true);
            m_releaseAction->setEnabled(true);
            m_propertiesAction->setEnabled(true);
            
            updateDiskInfo();
        } else {
            m_selectedDisk.clear();
            m_copyAction->setEnabled(false);
            m_modifyAction->setEnabled(false);
            m_removeAction->setEnabled(false);
            m_releaseAction->setEnabled(false);
            m_propertiesAction->setEnabled(false);
        }
    }
}

void DiskManagerDialog::onTabChanged(int index)
{
    m_currentTab = index;
    m_selectedDisk.clear();
    onDiskSelectionChanged(); // Update button states
    updateDiskList();
}

void DiskManagerDialog::updateDiskList()
{
    // Clear all trees
    m_hardDiskTree->clear();
    m_opticalTree->clear();
    m_floppyTree->clear();
    
    // Buscar archivos de disco en carpetas comunes
    QStringList searchPaths = {
        QDir::homePath() + "/.VM",
        QDir::homePath() + "/VirtualBox VMs",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Virtual Machines"
    };
    
    QStringList hardDiskFilters = {"*.qcow2", "*.img", "*.vdi", "*.vmdk"};
    QStringList opticalFilters = {"*.iso", "*.dmg"};
    QStringList floppyFilters = {"*.img", "*.ima"};
    
    for (const QString &path : searchPaths) {
        QDir dir(path);
        if (!dir.exists()) continue;
        
        // Hard disks
        QFileInfoList hardDisks = dir.entryInfoList(hardDiskFilters, QDir::Files);
        for (const QFileInfo &info : hardDisks) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_hardDiskTree);
            item->setText(0, info.baseName());
            item->setText(1, QString("%1 GB").arg(info.size() / (1024*1024*1024)));
            item->setText(2, QString("%1 MB").arg(info.size() / (1024*1024)));
            item->setText(3, info.absoluteFilePath());
            item->setData(0, Qt::UserRole, info.absoluteFilePath());
            item->setIcon(0, QIcon(":/icons/harddisk.png"));
        }
        
        // Optical disks
        QFileInfoList opticalDisks = dir.entryInfoList(opticalFilters, QDir::Files);
        for (const QFileInfo &info : opticalDisks) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_opticalTree);
            item->setText(0, info.baseName());
            item->setText(1, QString("%1 MB").arg(info.size() / (1024*1024)));
            item->setText(2, info.absoluteFilePath());
            item->setData(0, Qt::UserRole, info.absoluteFilePath());
            item->setIcon(0, QIcon(":/icons/optical.png"));
        }
        
        // Floppy disks
        QFileInfoList floppyDisks = dir.entryInfoList(floppyFilters, QDir::Files);
        for (const QFileInfo &info : floppyDisks) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_floppyTree);
            item->setText(0, info.baseName());
            item->setText(1, QString("%1 KB").arg(info.size() / 1024));
            item->setText(2, info.absoluteFilePath());
            item->setData(0, Qt::UserRole, info.absoluteFilePath());
            item->setIcon(0, QIcon(":/icons/floppy.png"));
        }
    }
    
    // Resize columns
    for (int i = 0; i < m_hardDiskTree->columnCount(); ++i) {
        m_hardDiskTree->resizeColumnToContents(i);
    }
}

void DiskManagerDialog::updateDiskInfo()
{
    if (m_selectedDisk.isEmpty() || m_currentTab != HardDisksTab) {
        m_hardDiskInfoLabel->setText(tr("Seleccione un disco para ver su información"));
        return;
    }
    
    QFileInfo info(m_selectedDisk);
    if (!info.exists()) {
        m_hardDiskInfoLabel->setText(tr("El archivo seleccionado no existe"));
        return;
    }
    
    QString infoText = QString(
        "<b>Archivo:</b><br>%1<br><br>"
        "<b>Tamaño:</b><br>%2 MB (%3 bytes)<br><br>"
        "<b>Formato:</b><br>%4<br><br>"
        "<b>Modificado:</b><br>%5<br><br>"
        "<b>En uso por:</b><br>Ninguna VM")
        .arg(info.fileName())
        .arg(info.size() / (1024*1024))
        .arg(info.size())
        .arg(info.suffix().toUpper())
        .arg(info.lastModified().toString());
    
    m_hardDiskInfoLabel->setText(infoText);
}

// CreateVirtualDiskDialog implementation
CreateVirtualDiskDialog::CreateVirtualDiskDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Crear Disco Virtual"));
    setWindowIcon(QIcon(":/icons/disk-create.png"));
    setModal(true);
    setFixedSize(500, 350);
    
    setupUI();
}

void CreateVirtualDiskDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // File path
    QGroupBox *pathGroup = new QGroupBox(tr("Ubicación del Archivo"));
    QHBoxLayout *pathLayout = new QHBoxLayout(pathGroup);
    
    m_pathEdit = new QLineEdit();
    m_pathEdit->setPlaceholderText(tr("Ruta del nuevo disco virtual"));
    m_browseButton = new QPushButton(tr("Examinar..."));
    
    pathLayout->addWidget(m_pathEdit);
    pathLayout->addWidget(m_browseButton);
    mainLayout->addWidget(pathGroup);
    
    // Format and size
    QGroupBox *formatGroup = new QGroupBox(tr("Formato y Tamaño"));
    QFormLayout *formatLayout = new QFormLayout(formatGroup);
    
    m_formatCombo = new QComboBox();
    m_formatCombo->addItems({
        tr("QCOW2 (Recomendado)"),
        tr("VDI (VirtualBox)"), 
        tr("VMDK (VMware)"),
        tr("IMG (Raw)")
    });
    formatLayout->addRow(tr("Formato:"), m_formatCombo);
    
    // Size input
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    m_sizeEdit = new QLineEdit("25");
    m_sizeEdit->setMaximumWidth(100);
    m_sizeUnitCombo = new QComboBox();
    m_sizeUnitCombo->addItems({tr("MB"), tr("GB"), tr("TB")});
    m_sizeUnitCombo->setCurrentIndex(1); // GB by default
    sizeLayout->addWidget(m_sizeEdit);
    sizeLayout->addWidget(m_sizeUnitCombo);
    sizeLayout->addStretch();
    formatLayout->addRow(tr("Tamaño:"), sizeLayout);
    
    // Allocation type
    m_allocationCombo = new QComboBox();
    m_allocationCombo->addItems({
        tr("Dinámicamente asignado"),
        tr("Tamaño fijo")
    });
    formatLayout->addRow(tr("Asignación:"), m_allocationCombo);
    
    mainLayout->addWidget(formatGroup);
    
    // Estimated size
    m_estimatedSizeLabel = new QLabel(tr("Tamaño estimado: 25 GB"));
    m_estimatedSizeLabel->setStyleSheet("color: blue; font-weight: bold;");
    mainLayout->addWidget(m_estimatedSizeLabel);
    
    mainLayout->addStretch();
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *createButton = new QPushButton(tr("Crear"));
    QPushButton *cancelButton = new QPushButton(tr("Cancelar"));
    
    createButton->setDefault(true);
    buttonLayout->addStretch();
    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // Connections
    connect(m_browseButton, &QPushButton::clicked, this, &CreateVirtualDiskDialog::onBrowsePath);
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &CreateVirtualDiskDialog::onFormatChanged);
    connect(m_sizeEdit, &QLineEdit::textChanged, this, &CreateVirtualDiskDialog::onSizeChanged);
    connect(m_sizeUnitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CreateVirtualDiskDialog::onSizeChanged);
    connect(createButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // Initial setup
    QString defaultPath = QDir::homePath() + "/.VM/nuevo-disco.qcow2";
    m_pathEdit->setText(defaultPath);
    onSizeChanged();
}

void CreateVirtualDiskDialog::onBrowsePath()
{
    QString currentPath = m_pathEdit->text();
    if (currentPath.isEmpty()) {
        currentPath = QDir::homePath() + "/.VM/";
    }
    
    QString path = QFileDialog::getSaveFileName(this, tr("Guardar Disco Virtual"), 
        currentPath, tr("Archivos de Disco (*.qcow2 *.img *.vdi *.vmdk)"));
    
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void CreateVirtualDiskDialog::onFormatChanged()
{
    // Update file extension based on format
    QString currentPath = m_pathEdit->text();
    QFileInfo info(currentPath);
    QString baseName = info.completeBaseName();
    QString dir = info.absolutePath();
    
    QString extension;
    switch (m_formatCombo->currentIndex()) {
        case 0: extension = "qcow2"; break;
        case 1: extension = "vdi"; break;
        case 2: extension = "vmdk"; break;
        case 3: extension = "img"; break;
    }
    
    QString newPath = dir + "/" + baseName + "." + extension;
    m_pathEdit->setText(newPath);
}

void CreateVirtualDiskDialog::onSizeChanged()
{
    updateEstimatedSize();
}

void CreateVirtualDiskDialog::updateEstimatedSize()
{
    bool ok;
    double size = m_sizeEdit->text().toDouble(&ok);
    if (!ok || size <= 0) {
        m_estimatedSizeLabel->setText(tr("Tamaño inválido"));
        return;
    }
    
    // Convert to bytes
    qint64 sizeInBytes = static_cast<qint64>(size);
    int unit = m_sizeUnitCombo->currentIndex();
    
    switch (unit) {
        case 0: // MB
            sizeInBytes *= 1024 * 1024;
            break;
        case 1: // GB  
            sizeInBytes *= 1024 * 1024 * 1024;
            break;
        case 2: // TB
            sizeInBytes *= 1024LL * 1024 * 1024 * 1024;
            break;
    }
    
    QString sizeText;
    if (sizeInBytes >= 1024LL * 1024 * 1024 * 1024) {
        sizeText = QString("%1 TB").arg(sizeInBytes / (1024.0 * 1024 * 1024 * 1024), 0, 'f', 1);
    } else if (sizeInBytes >= 1024 * 1024 * 1024) {
        sizeText = QString("%1 GB").arg(sizeInBytes / (1024.0 * 1024 * 1024), 0, 'f', 1);
    } else {
        sizeText = QString("%1 MB").arg(sizeInBytes / (1024.0 * 1024), 0, 'f', 1);
    }
    
    m_estimatedSizeLabel->setText(tr("Tamaño estimado: %1").arg(sizeText));
}

QString CreateVirtualDiskDialog::getDiskPath() const
{
    return m_pathEdit->text();
}

QString CreateVirtualDiskDialog::getDiskFormat() const
{
    QStringList formats = {"qcow2", "vdi", "vmdk", "img"};
    return formats.value(m_formatCombo->currentIndex(), "qcow2");
}

qint64 CreateVirtualDiskDialog::getDiskSize() const
{
    bool ok;
    double size = m_sizeEdit->text().toDouble(&ok);
    if (!ok) return 0;
    
    qint64 sizeInMB = static_cast<qint64>(size);
    int unit = m_sizeUnitCombo->currentIndex();
    
    switch (unit) {
        case 0: // MB
            return sizeInMB;
        case 1: // GB
            return sizeInMB * 1024;
        case 2: // TB
            return sizeInMB * 1024 * 1024;
    }
    
    return sizeInMB;
}

bool CreateVirtualDiskDialog::isDynamicAllocation() const
{
    return m_allocationCombo->currentIndex() == 0;
}

// DiskPropertiesDialog implementation
DiskPropertiesDialog::DiskPropertiesDialog(const QString &diskPath, QWidget *parent)
    : QDialog(parent)
    , m_diskPath(diskPath)
{
    setWindowTitle(tr("Propiedades del Disco Virtual"));
    setWindowIcon(QIcon(":/icons/properties.png"));
    setModal(true);
    setFixedSize(400, 300);
    
    setupUI();
    loadDiskInfo();
}

void DiskPropertiesDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    
    m_pathLabel = new QLabel();
    m_formatLabel = new QLabel();
    m_sizeLabel = new QLabel();
    m_actualSizeLabel = new QLabel();
    m_usageLabel = new QLabel();
    m_attachmentsLabel = new QLabel();
    
    formLayout->addRow(tr("Ubicación:"), m_pathLabel);
    formLayout->addRow(tr("Formato:"), m_formatLabel);
    formLayout->addRow(tr("Tamaño virtual:"), m_sizeLabel);
    formLayout->addRow(tr("Tamaño real:"), m_actualSizeLabel);
    formLayout->addRow(tr("Uso:"), m_usageLabel);
    formLayout->addRow(tr("Adjunto a:"), m_attachmentsLabel);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    
    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *closeButton = new QPushButton(tr("Cerrar"));
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
    
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void DiskPropertiesDialog::loadDiskInfo()
{
    QFileInfo info(m_diskPath);
    
    if (!info.exists()) {
        m_pathLabel->setText(tr("Archivo no encontrado"));
        return;
    }
    
    m_pathLabel->setText(info.absoluteFilePath());
    m_formatLabel->setText(info.suffix().toUpper());
    m_actualSizeLabel->setText(QString("%1 MB").arg(info.size() / (1024*1024)));
    m_sizeLabel->setText(tr("Información no disponible")); // Requiere análisis del formato
    m_usageLabel->setText(tr("No determinado"));
    m_attachmentsLabel->setText(tr("Ninguna"));
}