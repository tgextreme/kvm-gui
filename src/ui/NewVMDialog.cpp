#include "NewVMDialog.h"
#include "../core/KVMManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QMessageBox>

NewVMDialog::NewVMDialog(KVMManager *kvmManager, QWidget *parent)
    : QWizard(parent)
    , m_kvmManager(kvmManager)
{
    setWindowTitle(tr("Asistente para nueva máquina virtual"));
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    resize(600, 500);
    
    setupPages();
}

void NewVMDialog::setupPages()
{
    addPage(new WelcomePage());
    addPage(new SystemPage());
    addPage(new HardDiskPage());
    addPage(new SummaryPage());
    
    // Set page titles
    setButtonText(QWizard::NextButton, tr("&Siguiente >"));
    setButtonText(QWizard::BackButton, tr("< &Anterior"));
    setButtonText(QWizard::FinishButton, tr("&Crear"));
    setButtonText(QWizard::CancelButton, tr("&Cancelar"));
}

void NewVMDialog::accept()
{
    if (createVM()) {
        QWizard::accept();
    }
}

bool NewVMDialog::createVM()
{
    QString vmName = field("vmName").toString();
    QString osType = field("osType").toString();
    int memory = field("memory").toInt();
    
    if (m_kvmManager->createVirtualMachine(vmName, osType, memory, 25)) {
        QMessageBox::information(this, tr("VM Creada"), 
            tr("La máquina virtual '%1' ha sido creada correctamente.").arg(vmName));
        return true;
    } else {
        QMessageBox::warning(this, tr("Error"), 
            tr("No se pudo crear la máquina virtual '%1'.").arg(vmName));
        return false;
    }
}

// Welcome Page Implementation
WelcomePage::WelcomePage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Información de la máquina virtual"));
    setSubTitle(tr("Especifique el nombre y el tipo de sistema operativo para la nueva máquina virtual."));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QGroupBox *nameGroup = new QGroupBox(tr("Nombre y sistema operativo"));
    QGridLayout *nameLayout = new QGridLayout(nameGroup);
    
    // VM Name
    nameLayout->addWidget(new QLabel(tr("Nombre:")), 0, 0);
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Mi máquina virtual"));
    nameLayout->addWidget(m_nameEdit, 0, 1);
    
    // OS Type
    nameLayout->addWidget(new QLabel(tr("Tipo:")), 1, 0);
    m_osTypeCombo = new QComboBox();
    m_osTypeCombo->addItems({
        tr("Linux"), tr("Microsoft Windows"), tr("macOS"), 
        tr("BSD"), tr("Solaris"), tr("Otros")
    });
    nameLayout->addWidget(m_osTypeCombo, 1, 1);
    
    // OS Version
    nameLayout->addWidget(new QLabel(tr("Versión:")), 2, 0);
    m_osVersionCombo = new QComboBox();
    m_osVersionCombo->addItems({
        tr("Ubuntu 22.04 LTS"), tr("Ubuntu 20.04 LTS"),
        tr("Debian 12 (Bookworm)"), tr("Debian 11 (Bullseye)"),
        tr("Fedora 39"), tr("CentOS 9 Stream"),
        tr("Red Hat Enterprise Linux 9"),
        tr("Windows 11"), tr("Windows 10"),
        tr("Windows Server 2022"), tr("Windows Server 2019"),
        tr("macOS Monterey"), tr("macOS Big Sur"),
        tr("FreeBSD 14"), tr("OpenBSD 7")
    });
    nameLayout->addWidget(m_osVersionCombo, 2, 1);
    
    layout->addWidget(nameGroup);
    
    // Add some helpful text
    QLabel *infoLabel = new QLabel(tr(
        "<b>Consejo:</b> El nombre debe ser único y el tipo de SO ayuda a "
        "configurar automáticamente los valores óptimos para la máquina virtual."
    ));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #888; background: #2a2a2a; padding: 10px; border-radius: 5px;");
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    
    // Register fields for the wizard
    registerField("vmName*", m_nameEdit);
    registerField("osType", m_osTypeCombo, "currentText");
    registerField("osVersion", m_osVersionCombo, "currentText");
}

// System Page Implementation
SystemPage::SystemPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuración del sistema"));
    setSubTitle(tr("Configure la cantidad de memoria RAM y procesadores para la máquina virtual."));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Memory configuration
    QGroupBox *memoryGroup = new QGroupBox(tr("Memoria"));
    QGridLayout *memoryLayout = new QGridLayout(memoryGroup);
    
    memoryLayout->addWidget(new QLabel(tr("Memoria RAM:")), 0, 0);
    
    QHBoxLayout *memLayout = new QHBoxLayout();
    m_memorySpin = new QSpinBox();
    m_memorySpin->setRange(512, 32768);
    m_memorySpin->setValue(2048);
    m_memorySpin->setSuffix(" MB");
    
    m_memoryLabel = new QLabel(tr("2048 MB (Recomendado: 2048 MB)"));
    
    connect(m_memorySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SystemPage::updateMemoryLabel);
    
    memLayout->addWidget(m_memorySpin);
    memLayout->addWidget(m_memoryLabel);
    memLayout->addStretch();
    
    memoryLayout->addLayout(memLayout, 0, 1);
    
    layout->addWidget(memoryGroup);
    
    // CPU configuration
    QGroupBox *cpuGroup = new QGroupBox(tr("Procesador"));
    QGridLayout *cpuLayout = new QGridLayout(cpuGroup);
    
    cpuLayout->addWidget(new QLabel(tr("Procesadores:")), 0, 0);
    m_cpuSpin = new QSpinBox();
    m_cpuSpin->setRange(1, 8);
    m_cpuSpin->setValue(1);
    cpuLayout->addWidget(m_cpuSpin, 0, 1);
    
    m_enableVTX = new QCheckBox(tr("Habilitar virtualización por hardware (VT-x/AMD-V)"));
    m_enableVTX->setChecked(true);
    cpuLayout->addWidget(m_enableVTX, 1, 0, 1, 2);
    
    layout->addWidget(cpuGroup);
    
    // Performance tip
    QLabel *perfLabel = new QLabel(tr(
        "<b>Recomendación:</b> Para mejor rendimiento, asigne hasta el 50% de la RAM del host "
        "y habilite la virtualización por hardware si está disponible."
    ));
    perfLabel->setWordWrap(true);
    perfLabel->setStyleSheet("color: #888; background: #2a2a2a; padding: 10px; border-radius: 5px;");
    layout->addWidget(perfLabel);
    
    layout->addStretch();
    
    // Register fields
    registerField("memory", m_memorySpin);
    registerField("cpuCount", m_cpuSpin);
    registerField("enableVTX", m_enableVTX);
}

void SystemPage::updateMemoryLabel(int value)
{
    QString recommendation;
    if (value < 1024) {
        recommendation = tr("Muy poco - puede ser lento");
    } else if (value < 2048) {
        recommendation = tr("Mínimo para sistemas modernos");
    } else if (value <= 4096) {
        recommendation = tr("Recomendado para uso general");
    } else {
        recommendation = tr("Ideal para tareas intensivas");
    }
    
    m_memoryLabel->setText(tr("%1 MB (%2)").arg(value).arg(recommendation));
}

// Hard Disk Page Implementation
HardDiskPage::HardDiskPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Disco duro virtual"));
    setSubTitle(tr("Configure el almacenamiento para la máquina virtual."));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QGroupBox *diskGroup = new QGroupBox(tr("Disco duro"));
    QVBoxLayout *diskLayout = new QVBoxLayout(diskGroup);
    
    m_createDiskCheckBox = new QCheckBox(tr("Crear un disco duro virtual ahora"));
    m_createDiskCheckBox->setChecked(true);
    diskLayout->addWidget(m_createDiskCheckBox);
    
    // Disk location
    QGridLayout *locationLayout = new QGridLayout();
    
    locationLayout->addWidget(new QLabel(tr("Ubicación:")), 0, 0);
    QHBoxLayout *pathLayout = new QHBoxLayout();
    m_diskLocationEdit = new QLineEdit();
    m_diskLocationEdit->setText(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/VirtualBox VMs/nueva-vm/nueva-vm.vdi");
    m_browseDiskButton = new QPushButton(tr("Examinar..."));
    connect(m_browseDiskButton, &QPushButton::clicked, this, &HardDiskPage::selectDiskLocation);
    
    pathLayout->addWidget(m_diskLocationEdit);
    pathLayout->addWidget(m_browseDiskButton);
    locationLayout->addLayout(pathLayout, 0, 1);
    
    // Disk size
    locationLayout->addWidget(new QLabel(tr("Tamaño:")), 1, 0);
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    m_diskSizeSpin = new QSpinBox();
    m_diskSizeSpin->setRange(8, 2048);
    m_diskSizeSpin->setValue(25);
    m_diskSizeSpin->setSuffix(" GB");
    
    m_diskSizeLabel = new QLabel(tr("25 GB (Recomendado para SO moderno)"));
    connect(m_diskSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &HardDiskPage::updateDiskSizeLabel);
    
    sizeLayout->addWidget(m_diskSizeSpin);
    sizeLayout->addWidget(m_diskSizeLabel);
    sizeLayout->addStretch();
    locationLayout->addLayout(sizeLayout, 1, 1);
    
    // Disk type
    locationLayout->addWidget(new QLabel(tr("Tipo:")), 2, 0);
    m_diskTypeCombo = new QComboBox();
    m_diskTypeCombo->addItems({
        tr("Asignado dinámicamente (crece según se necesite)"),
        tr("Tamaño fijo (mejor rendimiento)")
    });
    locationLayout->addWidget(m_diskTypeCombo, 2, 1);
    
    diskLayout->addLayout(locationLayout);
    
    layout->addWidget(diskGroup);
    
    // Storage tip
    QLabel *storageLabel = new QLabel(tr(
        "<b>Consejo:</b> Los discos dinámicos ahorran espacio inicial, mientras que "
        "los de tamaño fijo ofrecen mejor rendimiento. 25 GB es suficiente para la mayoría de sistemas operativos."
    ));
    storageLabel->setWordWrap(true);
    storageLabel->setStyleSheet("color: #888; background: #2a2a2a; padding: 10px; border-radius: 5px;");
    layout->addWidget(storageLabel);
    
    layout->addStretch();
    
    // Enable/disable controls based on checkbox
    connect(m_createDiskCheckBox, &QCheckBox::toggled, m_diskLocationEdit, &QLineEdit::setEnabled);
    connect(m_createDiskCheckBox, &QCheckBox::toggled, m_browseDiskButton, &QPushButton::setEnabled);
    connect(m_createDiskCheckBox, &QCheckBox::toggled, m_diskSizeSpin, &QSpinBox::setEnabled);
    connect(m_createDiskCheckBox, &QCheckBox::toggled, m_diskTypeCombo, &QComboBox::setEnabled);
    
    // Register fields
    registerField("createDisk", m_createDiskCheckBox);
    registerField("diskLocation", m_diskLocationEdit);
    registerField("diskSize", m_diskSizeSpin);
    registerField("diskType", m_diskTypeCombo, "currentText");
}

void HardDiskPage::selectDiskLocation()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
        tr("Seleccionar ubicación del disco virtual"), 
        m_diskLocationEdit->text(),
        tr("Imágenes de disco virtual (*.vdi *.vmdk *.vhd)"));
    
    if (!fileName.isEmpty()) {
        m_diskLocationEdit->setText(fileName);
    }
}

void HardDiskPage::updateDiskSizeLabel(int value)
{
    QString description;
    if (value < 15) {
        description = tr("Muy pequeño - solo para sistemas mínimos");
    } else if (value < 25) {
        description = tr("Adecuado para sistemas ligeros");
    } else if (value <= 50) {
        description = tr("Recomendado para uso general");
    } else {
        description = tr("Amplio espacio para aplicaciones");
    }
    
    m_diskSizeLabel->setText(tr("%1 GB (%2)").arg(value).arg(description));
}

// Summary Page Implementation
SummaryPage::SummaryPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Resumen"));
    setSubTitle(tr("Revise la configuración antes de crear la máquina virtual."));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *summaryLabel = new QLabel(tr("Se creará una nueva máquina virtual con la siguiente configuración:"));
    layout->addWidget(summaryLabel);
    
    m_summaryText = new QTextEdit();
    m_summaryText->setReadOnly(true);
    m_summaryText->setMaximumHeight(300);
    layout->addWidget(m_summaryText);
    
    QLabel *finalLabel = new QLabel(tr(
        "Haga clic en <b>Crear</b> para crear la máquina virtual. "
        "Puede modificar estos ajustes más tarde en la configuración de la VM."
    ));
    finalLabel->setWordWrap(true);
    finalLabel->setStyleSheet("color: #888; background: #2a2a2a; padding: 10px; border-radius: 5px;");
    layout->addWidget(finalLabel);
}

void SummaryPage::initializePage()
{
    QString summary = QString(
        "<h3>%1</h3>"
        "<table style='width:100%; color: white;'>"
        "<tr><td><b>Nombre:</b></td><td>%2</td></tr>"
        "<tr><td><b>Sistema operativo:</b></td><td>%3 %4</td></tr>"
        "<tr><td><b>Memoria RAM:</b></td><td>%5 MB</td></tr>"
        "<tr><td><b>Procesadores:</b></td><td>%6</td></tr>"
        "<tr><td><b>Virtualización por hardware:</b></td><td>%7</td></tr>"
        "<tr><td><b>Disco duro:</b></td><td>%8</td></tr>"
        "%9"
        "</table>"
    ).arg(
        tr("Configuración de la nueva máquina virtual"),
        field("vmName").toString(),
        field("osType").toString(),
        field("osVersion").toString(),
        field("memory").toString(),
        field("cpuCount").toString(),
        field("enableVTX").toBool() ? tr("Habilitada") : tr("Deshabilitada"),
        field("createDisk").toBool() ? tr("Nuevo disco virtual") : tr("Sin disco")
    ).arg(
        field("createDisk").toBool() ? QString(
            "<tr><td><b>Ubicación del disco:</b></td><td>%1</td></tr>"
            "<tr><td><b>Tamaño del disco:</b></td><td>%2 GB</td></tr>"
            "<tr><td><b>Tipo de disco:</b></td><td>%3</td></tr>"
        ).arg(
            field("diskLocation").toString(),
            field("diskSize").toString(),
            field("diskType").toString()
        ) : ""
    );
    
    m_summaryText->setHtml(summary);
}