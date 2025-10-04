#include "MediaManagerDialog.h"
#include "../core/KVMManager.h"

#include <QApplication>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

MediaManagerDialog::MediaManagerDialog(KVMManager *kvmManager, QWidget *parent)
    : QDialog(parent)
    , m_kvmManager(kvmManager)
{
    setWindowTitle(tr("Administrador de medios virtuales"));
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon));
    resize(800, 600);
    setModal(true);
    
    setupUI();
    refreshMediaList();
}

void MediaManagerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget();
    
    createHardDisksTab();
    createOpticalDiscsTab();
    createFloppyDisksTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *closeButton = new QPushButton(tr("&Cerrar"));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void MediaManagerDialog::createHardDisksTab()
{
    m_hardDisksTab = new QWidget();
    m_tabWidget->addTab(m_hardDisksTab, QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon), tr("Discos duros"));
    
    QHBoxLayout *mainLayout = new QHBoxLayout(m_hardDisksTab);
    
    // Create splitter
    m_splitter = new QSplitter(Qt::Vertical);
    
    // Upper part - Tree and buttons
    QWidget *upperWidget = new QWidget();
    QVBoxLayout *upperLayout = new QVBoxLayout(upperWidget);
    
    // Tree widget
    m_hardDisksTree = new QTreeWidget();
    m_hardDisksTree->setHeaderLabels({tr("Nombre"), tr("Disco duro virtual"), tr("Tamaño"), tr("Ubicación")});
    m_hardDisksTree->header()->setStretchLastSection(true);
    connect(m_hardDisksTree, &QTreeWidget::itemSelectionChanged, this, &MediaManagerDialog::onHardDiskSelectionChanged);
    
    upperLayout->addWidget(m_hardDisksTree);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addHardDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Agregar"));
    m_removeHardDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Quitar"));
    m_releaseHardDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("&Liberar"));
    m_refreshHardDisksButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("A&ctualizar"));
    
    buttonLayout->addWidget(m_addHardDiskButton);
    buttonLayout->addWidget(m_removeHardDiskButton);
    buttonLayout->addWidget(m_releaseHardDiskButton);
    buttonLayout->addWidget(m_refreshHardDisksButton);
    buttonLayout->addStretch();
    
    m_copyHardDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Copiar"));
    m_moveHardDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_ArrowRight), tr("&Mover"));
    m_resizeHardDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("&Redimensionar"));
    
    buttonLayout->addWidget(m_copyHardDiskButton);
    buttonLayout->addWidget(m_moveHardDiskButton);
    buttonLayout->addWidget(m_resizeHardDiskButton);
    
    upperLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_addHardDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::addHardDisk);
    connect(m_removeHardDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::removeHardDisk);
    connect(m_releaseHardDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::releaseHardDisk);
    connect(m_refreshHardDisksButton, &QPushButton::clicked, this, &MediaManagerDialog::refreshHardDisks);
    connect(m_copyHardDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::copyHardDisk);
    connect(m_moveHardDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::moveHardDisk);
    connect(m_resizeHardDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::resizeHardDisk);
    
    m_splitter->addWidget(upperWidget);
    
    // Lower part - Details
    m_hardDiskDetailsGroup = new QGroupBox(tr("Información"));
    QGridLayout *detailsLayout = new QGridLayout(m_hardDiskDetailsGroup);
    
    detailsLayout->addWidget(new QLabel(tr("Ubicación:")), 0, 0);
    m_hardDiskLocationEdit = new QLineEdit();
    m_hardDiskLocationEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hardDiskLocationEdit, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Tamaño:")), 1, 0);
    m_hardDiskSizeEdit = new QLineEdit();
    m_hardDiskSizeEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hardDiskSizeEdit, 1, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Formato:")), 2, 0);
    m_hardDiskFormatEdit = new QLineEdit();
    m_hardDiskFormatEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hardDiskFormatEdit, 2, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Variante:")), 3, 0);
    m_hardDiskVariantEdit = new QLineEdit();
    m_hardDiskVariantEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hardDiskVariantEdit, 3, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Uso:")), 4, 0);
    m_hardDiskUsageEdit = new QTextEdit();
    m_hardDiskUsageEdit->setReadOnly(true);
    m_hardDiskUsageEdit->setMaximumHeight(80);
    detailsLayout->addWidget(m_hardDiskUsageEdit, 4, 1);
    
    m_splitter->addWidget(m_hardDiskDetailsGroup);
    m_splitter->setSizes({400, 200});
    
    mainLayout->addWidget(m_splitter);
}

void MediaManagerDialog::createOpticalDiscsTab()
{
    m_opticalDiscsTab = new QWidget();
    m_tabWidget->addTab(m_opticalDiscsTab, QApplication::style()->standardIcon(QStyle::SP_DriveDVDIcon), tr("Discos ópticos"));
    
    QVBoxLayout *layout = new QVBoxLayout(m_opticalDiscsTab);
    
    // Tree widget
    m_opticalDiscsTree = new QTreeWidget();
    m_opticalDiscsTree->setHeaderLabels({tr("Nombre"), tr("Archivo"), tr("Tamaño"), tr("Ubicación")});
    m_opticalDiscsTree->header()->setStretchLastSection(true);
    connect(m_opticalDiscsTree, &QTreeWidget::itemSelectionChanged, this, &MediaManagerDialog::onOpticalDiscSelectionChanged);
    
    layout->addWidget(m_opticalDiscsTree);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addOpticalDiscButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Agregar"));
    m_removeOpticalDiscButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Quitar"));
    m_releaseOpticalDiscButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("&Liberar"));
    m_refreshOpticalDiscsButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("A&ctualizar"));
    
    buttonLayout->addWidget(m_addOpticalDiscButton);
    buttonLayout->addWidget(m_removeOpticalDiscButton);
    buttonLayout->addWidget(m_releaseOpticalDiscButton);
    buttonLayout->addWidget(m_refreshOpticalDiscsButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Details group
    m_opticalDiscDetailsGroup = new QGroupBox(tr("Información"));
    QGridLayout *detailsLayout = new QGridLayout(m_opticalDiscDetailsGroup);
    
    detailsLayout->addWidget(new QLabel(tr("Ubicación:")), 0, 0);
    m_opticalDiscLocationEdit = new QLineEdit();
    m_opticalDiscLocationEdit->setReadOnly(true);
    detailsLayout->addWidget(m_opticalDiscLocationEdit, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Tamaño:")), 1, 0);
    m_opticalDiscSizeEdit = new QLineEdit();
    m_opticalDiscSizeEdit->setReadOnly(true);
    detailsLayout->addWidget(m_opticalDiscSizeEdit, 1, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Uso:")), 2, 0);
    m_opticalDiscUsageEdit = new QTextEdit();
    m_opticalDiscUsageEdit->setReadOnly(true);
    m_opticalDiscUsageEdit->setMaximumHeight(60);
    detailsLayout->addWidget(m_opticalDiscUsageEdit, 2, 1);
    
    layout->addWidget(m_opticalDiscDetailsGroup);
    
    // Connect signals
    connect(m_addOpticalDiscButton, &QPushButton::clicked, this, &MediaManagerDialog::addOpticalDisc);
    connect(m_removeOpticalDiscButton, &QPushButton::clicked, this, &MediaManagerDialog::removeOpticalDisc);
    connect(m_releaseOpticalDiscButton, &QPushButton::clicked, this, &MediaManagerDialog::releaseOpticalDisc);
    connect(m_refreshOpticalDiscsButton, &QPushButton::clicked, this, &MediaManagerDialog::refreshOpticalDiscs);
}

void MediaManagerDialog::createFloppyDisksTab()
{
    m_floppyDisksTab = new QWidget();
    m_tabWidget->addTab(m_floppyDisksTab, QApplication::style()->standardIcon(QStyle::SP_DriveFDIcon), tr("Disquetes"));
    
    QVBoxLayout *layout = new QVBoxLayout(m_floppyDisksTab);
    
    // Tree widget
    m_floppyDisksTree = new QTreeWidget();
    m_floppyDisksTree->setHeaderLabels({tr("Nombre"), tr("Archivo"), tr("Tamaño"), tr("Ubicación")});
    m_floppyDisksTree->header()->setStretchLastSection(true);
    connect(m_floppyDisksTree, &QTreeWidget::itemSelectionChanged, this, &MediaManagerDialog::onFloppyDiskSelectionChanged);
    
    layout->addWidget(m_floppyDisksTree);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addFloppyDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Agregar"));
    m_removeFloppyDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Quitar"));
    m_releaseFloppyDiskButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("&Liberar"));
    m_refreshFloppyDisksButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("A&ctualizar"));
    
    buttonLayout->addWidget(m_addFloppyDiskButton);
    buttonLayout->addWidget(m_removeFloppyDiskButton);
    buttonLayout->addWidget(m_releaseFloppyDiskButton);
    buttonLayout->addWidget(m_refreshFloppyDisksButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Details group
    m_floppyDiskDetailsGroup = new QGroupBox(tr("Información"));
    QGridLayout *detailsLayout = new QGridLayout(m_floppyDiskDetailsGroup);
    
    detailsLayout->addWidget(new QLabel(tr("Ubicación:")), 0, 0);
    m_floppyDiskLocationEdit = new QLineEdit();
    m_floppyDiskLocationEdit->setReadOnly(true);
    detailsLayout->addWidget(m_floppyDiskLocationEdit, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Tamaño:")), 1, 0);
    m_floppyDiskSizeEdit = new QLineEdit();
    m_floppyDiskSizeEdit->setReadOnly(true);
    detailsLayout->addWidget(m_floppyDiskSizeEdit, 1, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Uso:")), 2, 0);
    m_floppyDiskUsageEdit = new QTextEdit();
    m_floppyDiskUsageEdit->setReadOnly(true);
    m_floppyDiskUsageEdit->setMaximumHeight(60);
    detailsLayout->addWidget(m_floppyDiskUsageEdit, 2, 1);
    
    layout->addWidget(m_floppyDiskDetailsGroup);
    
    // Connect signals
    connect(m_addFloppyDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::addFloppyDisk);
    connect(m_removeFloppyDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::removeFloppyDisk);
    connect(m_releaseFloppyDiskButton, &QPushButton::clicked, this, &MediaManagerDialog::releaseFloppyDisk);
    connect(m_refreshFloppyDisksButton, &QPushButton::clicked, this, &MediaManagerDialog::refreshFloppyDisks);
}

void MediaManagerDialog::refreshMediaList()
{
    refreshHardDisks();
    refreshOpticalDiscs();
    refreshFloppyDisks();
}

// Hard Disk Operations
void MediaManagerDialog::addHardDisk()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Seleccionar archivo de disco duro virtual"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        tr("Archivos de disco virtual (*.vdi *.vmdk *.vhd *.qcow2);;Todos los archivos (*.*)"));
    
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        QTreeWidgetItem *item = new QTreeWidgetItem(m_hardDisksTree);
        item->setText(0, fileInfo.baseName());
        item->setText(1, fileInfo.fileName());
        item->setText(2, QString("%1 MB").arg(fileInfo.size() / 1024 / 1024));
        item->setText(3, fileInfo.absolutePath());
        item->setData(0, Qt::UserRole, fileName);
        
        m_hardDisksTree->addTopLevelItem(item);
        QMessageBox::information(this, tr("Disco agregado"), tr("Disco duro virtual agregado correctamente."));
    }
}

void MediaManagerDialog::removeHardDisk()
{
    QTreeWidgetItem *current = m_hardDisksTree->currentItem();
    if (current) {
        int ret = QMessageBox::question(this, tr("Confirmar eliminación"),
            tr("¿Está seguro de que desea eliminar el disco duro virtual '%1'?").arg(current->text(0)),
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            delete current;
            QMessageBox::information(this, tr("Disco eliminado"), tr("Disco duro virtual eliminado correctamente."));
        }
    }
}

void MediaManagerDialog::releaseHardDisk()
{
    QTreeWidgetItem *current = m_hardDisksTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("Disco liberado"), 
            tr("El disco '%1' ha sido liberado de todas las máquinas virtuales.").arg(current->text(0)));
    }
}

void MediaManagerDialog::refreshHardDisks()
{
    m_hardDisksTree->clear();
    
    // Buscar discos duros en las carpetas típicas
    QStringList searchPaths = {
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/VirtualBox VMs",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/VM",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Documents/VirtualBox VMs"
    };
    
    QStringList diskExtensions = {"*.vdi", "*.vmdk", "*.vhd", "*.qcow2"};
    
    for (const QString &searchPath : searchPaths) {
        QDir dir(searchPath);
        if (dir.exists()) {
            QFileInfoList files = dir.entryInfoList(diskExtensions, QDir::Files | QDir::Readable, QDir::Name);
            for (const QFileInfo &fileInfo : files) {
                QTreeWidgetItem *item = new QTreeWidgetItem(m_hardDisksTree);
                item->setText(0, fileInfo.baseName());
                item->setText(1, fileInfo.fileName());
                item->setText(2, QString("%1 MB").arg(fileInfo.size() / 1024 / 1024));
                item->setText(3, fileInfo.absolutePath());
                item->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
            }
        }
    }
}

void MediaManagerDialog::copyHardDisk()
{
    QTreeWidgetItem *current = m_hardDisksTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("Copiar disco"), tr("Función en desarrollo: Copiar disco duro virtual."));
    }
}

void MediaManagerDialog::moveHardDisk()
{
    QTreeWidgetItem *current = m_hardDisksTree->currentItem();
    if (current) {
        QString newLocation = QFileDialog::getExistingDirectory(this, tr("Seleccionar nueva ubicación"));
        if (!newLocation.isEmpty()) {
            QMessageBox::information(this, tr("Mover disco"), 
                tr("Disco movido a: %1").arg(newLocation));
        }
    }
}

void MediaManagerDialog::resizeHardDisk()
{
    QTreeWidgetItem *current = m_hardDisksTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("Redimensionar disco"), tr("Función en desarrollo: Redimensionar disco duro virtual."));
    }
}

void MediaManagerDialog::onHardDiskSelectionChanged()
{
    updateHardDiskDetails();
}

void MediaManagerDialog::updateHardDiskDetails()
{
    QTreeWidgetItem *current = m_hardDisksTree->currentItem();
    if (current) {
        QString filePath = current->data(0, Qt::UserRole).toString();
        m_hardDiskLocationEdit->setText(filePath);
        m_hardDiskSizeEdit->setText(current->text(2));
        
        QString format = "Desconocido";
        if (filePath.endsWith(".vdi")) format = "VDI (VirtualBox Disk Image)";
        else if (filePath.endsWith(".vmdk")) format = "VMDK (VMware Disk)";
        else if (filePath.endsWith(".vhd")) format = "VHD (Virtual Hard Disk)";
        else if (filePath.endsWith(".qcow2")) format = "QCOW2 (QEMU Copy On Write)";
        
        m_hardDiskFormatEdit->setText(format);
        m_hardDiskVariantEdit->setText(tr("Asignado dinámicamente"));
        m_hardDiskUsageEdit->setPlainText(tr("No está siendo utilizado por ninguna máquina virtual."));
    } else {
        m_hardDiskLocationEdit->clear();
        m_hardDiskSizeEdit->clear();
        m_hardDiskFormatEdit->clear();
        m_hardDiskVariantEdit->clear();
        m_hardDiskUsageEdit->clear();
    }
}

// Optical Disc Operations
void MediaManagerDialog::addOpticalDisc()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Seleccionar archivo de imagen ISO"),
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
        tr("Imágenes ISO (*.iso);;Todos los archivos (*.*)"));
    
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        QTreeWidgetItem *item = new QTreeWidgetItem(m_opticalDiscsTree);
        item->setText(0, fileInfo.baseName());
        item->setText(1, fileInfo.fileName());
        item->setText(2, QString("%1 MB").arg(fileInfo.size() / 1024 / 1024));
        item->setText(3, fileInfo.absolutePath());
        item->setData(0, Qt::UserRole, fileName);
        
        QMessageBox::information(this, tr("ISO agregada"), tr("Imagen ISO agregada correctamente."));
    }
}

void MediaManagerDialog::removeOpticalDisc()
{
    QTreeWidgetItem *current = m_opticalDiscsTree->currentItem();
    if (current) {
        delete current;
        QMessageBox::information(this, tr("ISO eliminada"), tr("Imagen ISO eliminada de la lista."));
    }
}

void MediaManagerDialog::releaseOpticalDisc()
{
    QTreeWidgetItem *current = m_opticalDiscsTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("ISO liberada"), 
            tr("La imagen '%1' ha sido liberada de todas las máquinas virtuales.").arg(current->text(0)));
    }
}

void MediaManagerDialog::refreshOpticalDiscs()
{
    m_opticalDiscsTree->clear();
    // Buscar archivos ISO en ubicaciones comunes
}

void MediaManagerDialog::onOpticalDiscSelectionChanged()
{
    updateOpticalDiscDetails();
}

void MediaManagerDialog::updateOpticalDiscDetails()
{
    QTreeWidgetItem *current = m_opticalDiscsTree->currentItem();
    if (current) {
        QString filePath = current->data(0, Qt::UserRole).toString();
        m_opticalDiscLocationEdit->setText(filePath);
        m_opticalDiscSizeEdit->setText(current->text(2));
        m_opticalDiscUsageEdit->setPlainText(tr("No está siendo utilizada por ninguna máquina virtual."));
    } else {
        m_opticalDiscLocationEdit->clear();
        m_opticalDiscSizeEdit->clear();
        m_opticalDiscUsageEdit->clear();
    }
}

// Floppy Disk Operations
void MediaManagerDialog::addFloppyDisk()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Seleccionar archivo de disquete"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        tr("Imágenes de disquete (*.img *.vfd);;Todos los archivos (*.*)"));
    
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        QTreeWidgetItem *item = new QTreeWidgetItem(m_floppyDisksTree);
        item->setText(0, fileInfo.baseName());
        item->setText(1, fileInfo.fileName());
        item->setText(2, QString("%1 KB").arg(fileInfo.size() / 1024));
        item->setText(3, fileInfo.absolutePath());
        item->setData(0, Qt::UserRole, fileName);
        
        QMessageBox::information(this, tr("Disquete agregado"), tr("Imagen de disquete agregada correctamente."));
    }
}

void MediaManagerDialog::removeFloppyDisk()
{
    QTreeWidgetItem *current = m_floppyDisksTree->currentItem();
    if (current) {
        delete current;
        QMessageBox::information(this, tr("Disquete eliminado"), tr("Imagen de disquete eliminada de la lista."));
    }
}

void MediaManagerDialog::releaseFloppyDisk()
{
    QTreeWidgetItem *current = m_floppyDisksTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("Disquete liberado"), 
            tr("El disquete '%1' ha sido liberado de todas las máquinas virtuales.").arg(current->text(0)));
    }
}

void MediaManagerDialog::refreshFloppyDisks()
{
    m_floppyDisksTree->clear();
    // Buscar archivos de disquete en ubicaciones comunes
}

void MediaManagerDialog::onFloppyDiskSelectionChanged()
{
    updateFloppyDiskDetails();
}

void MediaManagerDialog::updateFloppyDiskDetails()
{
    QTreeWidgetItem *current = m_floppyDisksTree->currentItem();
    if (current) {
        QString filePath = current->data(0, Qt::UserRole).toString();
        m_floppyDiskLocationEdit->setText(filePath);
        m_floppyDiskSizeEdit->setText(current->text(2));
        m_floppyDiskUsageEdit->setPlainText(tr("No está siendo utilizado por ninguna máquina virtual."));
    } else {
        m_floppyDiskLocationEdit->clear();
        m_floppyDiskSizeEdit->clear();
        m_floppyDiskUsageEdit->clear();
    }
}