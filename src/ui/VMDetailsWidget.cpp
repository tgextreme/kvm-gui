#include "VMDetailsWidget.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QProgressBar>
#include <QFont>
#include <QPixmap>
#include <QApplication>
#include <QStyle>

VMDetailsWidget::VMDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_currentVM("")
{
    setupUI();
    clearDetails();
}

void VMDetailsWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create scroll area
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Create content widget
    m_contentWidget = new QWidget();
    m_mainLayout = new QVBoxLayout(m_contentWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Create sections
    createGeneralSection();
    createSystemSection();
    createStorageSection();
    createNetworkSection();
    createAudioSection();
    createUSBSection();
    createDisplaySection();
    createSharedFoldersSection();
    
    // Add stretch to push content to top
    m_mainLayout->addStretch();
    
    m_scrollArea->setWidget(m_contentWidget);
    layout->addWidget(m_scrollArea);
}

QGroupBox* VMDetailsWidget::createSection(const QString &title)
{
    QGroupBox *groupBox = new QGroupBox(title);
    QFont font = groupBox->font();
    font.setBold(true);
    groupBox->setFont(font);
    groupBox->setStyleSheet(
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 2px solid #555555;"
        "    border-radius: 5px;"
        "    margin-top: 1ex;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
    return groupBox;
}

QWidget* VMDetailsWidget::createInfoRow(const QString &label, const QString &value, QWidget *parent)
{
    QWidget *row = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 2, 0, 2);
    
    QLabel *labelWidget = new QLabel(label);
    labelWidget->setMinimumWidth(120);
    labelWidget->setStyleSheet("font-weight: normal; color: #CCCCCC;");
    
    QLabel *valueWidget = new QLabel(value);
    valueWidget->setStyleSheet("font-weight: normal; color: white;");
    valueWidget->setWordWrap(true);
    
    layout->addWidget(labelWidget);
    layout->addWidget(valueWidget, 1);
    
    return row;
}

void VMDetailsWidget::createGeneralSection()
{
    m_generalSection = createSection(tr("🧩 General"));
    QVBoxLayout *layout = new QVBoxLayout(m_generalSection);
    
    // VM name and icon
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(48, 48);
    iconLabel->setStyleSheet("border: 1px solid #555; background: #333;");
    
    m_vmNameLabel = new QLabel(tr("Ninguna máquina virtual seleccionada"));
    QFont nameFont = m_vmNameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSize(nameFont.pointSize() + 2);
    m_vmNameLabel->setFont(nameFont);
    
    nameLayout->addWidget(iconLabel);
    nameLayout->addWidget(m_vmNameLabel, 1);
    layout->addLayout(nameLayout);
    
    // VM details
    QWidget *osRow = createInfoRow(tr("Sistema Operativo:"), tr("No disponible"));
    m_vmOSLabel = qobject_cast<QLabel*>(osRow->layout()->itemAt(1)->widget());
    layout->addWidget(osRow);
    
    QWidget *stateRow = createInfoRow(tr("Estado:"), tr("No disponible"));
    m_vmStateLabel = qobject_cast<QLabel*>(stateRow->layout()->itemAt(1)->widget());
    layout->addWidget(stateRow);
    
    QWidget *locationRow = createInfoRow(tr("Ubicación:"), tr("No disponible"));
    m_vmLocationLabel = qobject_cast<QLabel*>(locationRow->layout()->itemAt(1)->widget());
    layout->addWidget(locationRow);
    
    QWidget *uuidRow = createInfoRow(tr("UUID:"), tr("No disponible"));
    m_vmUUIDLabel = qobject_cast<QLabel*>(uuidRow->layout()->itemAt(1)->widget());
    layout->addWidget(uuidRow);
    
    QWidget *descRow = createInfoRow(tr("Descripción:"), tr("Sin descripción"));
    m_vmDescriptionLabel = qobject_cast<QLabel*>(descRow->layout()->itemAt(1)->widget());
    layout->addWidget(descRow);
    
    m_mainLayout->addWidget(m_generalSection);
}

void VMDetailsWidget::createSystemSection()
{
    m_systemSection = createSection(tr("⚙️ Sistema"));
    QVBoxLayout *layout = new QVBoxLayout(m_systemSection);
    
    QWidget *memoryRow = createInfoRow(tr("Memoria RAM:"), tr("No disponible"));
    m_systemMemoryLabel = qobject_cast<QLabel*>(memoryRow->layout()->itemAt(1)->widget());
    layout->addWidget(memoryRow);
    
    QWidget *cpuRow = createInfoRow(tr("Procesadores:"), tr("No disponible"));
    m_systemProcessorsLabel = qobject_cast<QLabel*>(cpuRow->layout()->itemAt(1)->widget());
    layout->addWidget(cpuRow);
    
    QWidget *bootRow = createInfoRow(tr("Orden de arranque:"), tr("No disponible"));
    m_systemBootOrderLabel = qobject_cast<QLabel*>(bootRow->layout()->itemAt(1)->widget());
    layout->addWidget(bootRow);
    
    QWidget *virtRow = createInfoRow(tr("Virtualización:"), tr("No disponible"));
    m_systemVirtualizationLabel = qobject_cast<QLabel*>(virtRow->layout()->itemAt(1)->widget());
    layout->addWidget(virtRow);
    
    m_mainLayout->addWidget(m_systemSection);
}

void VMDetailsWidget::createStorageSection()
{
    m_storageSection = createSection(tr("💽 Almacenamiento"));
    QVBoxLayout *layout = new QVBoxLayout(m_storageSection);
    
    QWidget *controllerRow = createInfoRow(tr("Controlador:"), tr("No disponible"));
    m_storageControllerLabel = qobject_cast<QLabel*>(controllerRow->layout()->itemAt(1)->widget());
    layout->addWidget(controllerRow);
    
    QWidget *hddRow = createInfoRow(tr("Disco duro:"), tr("No disponible"));
    m_storageHardDiskLabel = qobject_cast<QLabel*>(hddRow->layout()->itemAt(1)->widget());
    layout->addWidget(hddRow);
    
    QWidget *cdRow = createInfoRow(tr("CD/DVD:"), tr("No disponible"));
    m_storageCDDVDLabel = qobject_cast<QLabel*>(cdRow->layout()->itemAt(1)->widget());
    layout->addWidget(cdRow);
    
    m_mainLayout->addWidget(m_storageSection);
}

void VMDetailsWidget::createNetworkSection()
{
    m_networkSection = createSection(tr("🌐 Red"));
    QVBoxLayout *layout = new QVBoxLayout(m_networkSection);
    
    QWidget *adapter1Row = createInfoRow(tr("Adaptador 1:"), tr("No disponible"));
    m_networkAdapter1Label = qobject_cast<QLabel*>(adapter1Row->layout()->itemAt(1)->widget());
    layout->addWidget(adapter1Row);
    
    QWidget *adapter2Row = createInfoRow(tr("Adaptador 2:"), tr("Deshabilitado"));
    m_networkAdapter2Label = qobject_cast<QLabel*>(adapter2Row->layout()->itemAt(1)->widget());
    layout->addWidget(adapter2Row);
    
    m_mainLayout->addWidget(m_networkSection);
}

void VMDetailsWidget::createAudioSection()
{
    m_audioSection = createSection(tr("🔊 Audio"));
    QVBoxLayout *layout = new QVBoxLayout(m_audioSection);
    
    QWidget *controllerRow = createInfoRow(tr("Controlador:"), tr("No disponible"));
    m_audioControllerLabel = qobject_cast<QLabel*>(controllerRow->layout()->itemAt(1)->widget());
    layout->addWidget(controllerRow);
    
    m_mainLayout->addWidget(m_audioSection);
}

void VMDetailsWidget::createUSBSection()
{
    m_usbSection = createSection(tr("📱 USB"));
    QVBoxLayout *layout = new QVBoxLayout(m_usbSection);
    
    QWidget *controllerRow = createInfoRow(tr("Controlador USB:"), tr("No disponible"));
    m_usbControllerLabel = qobject_cast<QLabel*>(controllerRow->layout()->itemAt(1)->widget());
    layout->addWidget(controllerRow);
    
    m_mainLayout->addWidget(m_usbSection);
}

void VMDetailsWidget::createDisplaySection()
{
    m_displaySection = createSection(tr("📱 Pantalla"));
    QVBoxLayout *layout = new QVBoxLayout(m_displaySection);
    
    QWidget *memoryRow = createInfoRow(tr("Memoria de vídeo:"), tr("No disponible"));
    m_displayVideoMemoryLabel = qobject_cast<QLabel*>(memoryRow->layout()->itemAt(1)->widget());
    layout->addWidget(memoryRow);
    
    QWidget *accelRow = createInfoRow(tr("Aceleración:"), tr("No disponible"));
    m_displayAccelerationLabel = qobject_cast<QLabel*>(accelRow->layout()->itemAt(1)->widget());
    layout->addWidget(accelRow);
    
    QWidget *monitorsRow = createInfoRow(tr("Monitores:"), tr("No disponible"));
    m_displayMonitorsLabel = qobject_cast<QLabel*>(monitorsRow->layout()->itemAt(1)->widget());
    layout->addWidget(monitorsRow);
    
    m_mainLayout->addWidget(m_displaySection);
}

void VMDetailsWidget::createSharedFoldersSection()
{
    m_sharedFoldersSection = createSection(tr("🔗 Carpetas Compartidas"));
    QVBoxLayout *layout = new QVBoxLayout(m_sharedFoldersSection);
    
    QWidget *countRow = createInfoRow(tr("Carpetas compartidas:"), tr("No disponible"));
    m_sharedFoldersCountLabel = qobject_cast<QLabel*>(countRow->layout()->itemAt(1)->widget());
    layout->addWidget(countRow);
    
    m_mainLayout->addWidget(m_sharedFoldersSection);
}

void VMDetailsWidget::setSelectedVM(const QString &vmName)
{
    m_currentVM = vmName;
    if (vmName.isEmpty()) {
        clearDetails();
    } else {
        updateVMDetails(vmName);
    }
}

void VMDetailsWidget::refreshDetails()
{
    if (!m_currentVM.isEmpty()) {
        updateVMDetails(m_currentVM);
    }
}

void VMDetailsWidget::updateVMDetails(const QString &vmName)
{
    // TODO: Get actual VM details from KVM
    // For now, show example data
    
    m_vmNameLabel->setText(vmName);
    
    // Example data based on VM name
    if (vmName.contains("Ubuntu")) {
        m_vmOSLabel->setText("Ubuntu Linux (64-bit)");
        m_systemMemoryLabel->setText("4096 MB");
        m_systemProcessorsLabel->setText("2 CPU");
        m_storageHardDiskLabel->setText("ubuntu.qcow2 (25.0 GB)");
    } else if (vmName.contains("Windows")) {
        m_vmOSLabel->setText("Microsoft Windows (64-bit)");
        m_systemMemoryLabel->setText("8192 MB");
        m_systemProcessorsLabel->setText("4 CPU");
        m_storageHardDiskLabel->setText("windows11.qcow2 (50.0 GB)");
    } else if (vmName.contains("Debian")) {
        m_vmOSLabel->setText("Debian GNU/Linux (64-bit)");
        m_systemMemoryLabel->setText("2048 MB");
        m_systemProcessorsLabel->setText("2 CPU");
        m_storageHardDiskLabel->setText("debian.qcow2 (20.0 GB)");
    } else {
        m_vmOSLabel->setText("Linux (64-bit)");
        m_systemMemoryLabel->setText("2048 MB");
        m_systemProcessorsLabel->setText("1 CPU");
        m_storageHardDiskLabel->setText("linux.qcow2 (15.0 GB)");
    }
    
    m_vmStateLabel->setText("Apagada");
    m_vmLocationLabel->setText(QString("/home/usuario/.local/share/kvm/%1").arg(vmName.toLower().replace(' ', '-')));
    m_vmUUIDLabel->setText("12345678-1234-1234-1234-123456789abc");
    m_vmDescriptionLabel->setText(QString("Máquina virtual %1 para desarrollo y pruebas").arg(vmName));
    
    m_systemBootOrderLabel->setText("Disco Duro, Red");
    m_systemVirtualizationLabel->setText("KVM habilitado");
    
    m_storageControllerLabel->setText("SATA Controller");
    m_storageCDDVDLabel->setText("Vacío");
    
    m_networkAdapter1Label->setText("NAT");
    m_networkAdapter2Label->setText("Deshabilitado");
    
    m_audioControllerLabel->setText("PulseAudio");
    
    m_usbControllerLabel->setText("USB 3.0 (xHCI)");
    
    m_displayVideoMemoryLabel->setText("128 MB");
    m_displayAccelerationLabel->setText("3D: Deshabilitado");
    m_displayMonitorsLabel->setText("1");
    
    m_sharedFoldersCountLabel->setText("0");
}

void VMDetailsWidget::clearDetails()
{
    m_vmNameLabel->setText(tr("Ninguna máquina virtual seleccionada"));
    
    m_vmOSLabel->setText(tr("No disponible"));
    m_vmStateLabel->setText(tr("No disponible"));
    m_vmLocationLabel->setText(tr("No disponible"));
    m_vmUUIDLabel->setText(tr("No disponible"));
    m_vmDescriptionLabel->setText(tr("Seleccione una máquina virtual para ver los detalles"));
    
    m_systemMemoryLabel->setText(tr("No disponible"));
    m_systemProcessorsLabel->setText(tr("No disponible"));
    m_systemBootOrderLabel->setText(tr("No disponible"));
    m_systemVirtualizationLabel->setText(tr("No disponible"));
    
    m_storageControllerLabel->setText(tr("No disponible"));
    m_storageHardDiskLabel->setText(tr("No disponible"));
    m_storageCDDVDLabel->setText(tr("No disponible"));
    
    m_networkAdapter1Label->setText(tr("No disponible"));
    m_networkAdapter2Label->setText(tr("No disponible"));
    
    m_audioControllerLabel->setText(tr("No disponible"));
    
    m_usbControllerLabel->setText(tr("No disponible"));
    
    m_displayVideoMemoryLabel->setText(tr("No disponible"));
    m_displayAccelerationLabel->setText(tr("No disponible"));
    m_displayMonitorsLabel->setText(tr("No disponible"));
    
    m_sharedFoldersCountLabel->setText(tr("No disponible"));
}