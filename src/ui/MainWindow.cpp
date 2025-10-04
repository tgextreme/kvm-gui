#include "MainWindow.h"
#include "VMListWidget.h"
#include "VMDetailsWidget.h"
#include "PreferencesDialog.h"
#include "VMConfigDialog.h"
#include "AdvancedVMConfigDialog.h"
#include "VMCreationWizard.h"
#include "DiskManagerDialog.h"
#include "MediaManagerDialog.h"
#include "NetworkManagerDialog.h"
#include "SnapshotManagerDialog.h"
#include "../core/KVMManager.h"
#include "../core/VirtualMachine.h"

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QIcon>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_kvmManager(new KVMManager(this))
    , m_centralSplitter(nullptr)
    , m_vmListWidget(nullptr)
    , m_vmDetailsWidget(nullptr)
    , m_statusLabel(nullptr)
{
    setWindowTitle("KVM Manager");
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    resize(1200, 800);
    
    setupUI();
    updateUIState();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    createActions();
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupStatusBar();
}

void MainWindow::createActions()
{
    // File menu actions
    m_newVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Nueva máquina virtual..."), this);
    m_newVMAction->setShortcut(QKeySequence::New);
    m_newVMAction->setStatusTip(tr("Crear una nueva máquina virtual"));
    connect(m_newVMAction, &QAction::triggered, this, &MainWindow::newVM);

    m_addVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton), tr("&Agregar..."), this);
    m_addVMAction->setShortcut(QKeySequence("Ctrl+A"));
    m_addVMAction->setStatusTip(tr("Agregar una máquina virtual existente"));
    connect(m_addVMAction, &QAction::triggered, this, &MainWindow::addVM);

    m_preferencesAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("&Preferencias..."), this);
    m_preferencesAction->setShortcut(QKeySequence::Preferences);
    m_preferencesAction->setStatusTip(tr("Configurar preferencias globales"));
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);

    m_exitAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton), tr("&Salir"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Salir de la aplicación"));
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    // Machine menu actions
    m_removeVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Eliminar..."), this);
    m_removeVMAction->setShortcut(QKeySequence::Delete);
    m_removeVMAction->setStatusTip(tr("Eliminar la máquina virtual seleccionada"));
    connect(m_removeVMAction, &QAction::triggered, this, &MainWindow::removeVM);

    m_configureVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon), tr("&Configuración..."), this);
    m_configureVMAction->setShortcut(QKeySequence("Ctrl+S"));
    m_configureVMAction->setStatusTip(tr("Configurar la máquina virtual seleccionada"));
    connect(m_configureVMAction, &QAction::triggered, this, &MainWindow::configureVM);

    m_startVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), tr("&Iniciar"), this);
    m_startVMAction->setShortcut(QKeySequence("Ctrl+T"));
    m_startVMAction->setStatusTip(tr("Iniciar la máquina virtual seleccionada"));
    connect(m_startVMAction, &QAction::triggered, this, &MainWindow::startVM);

    m_pauseVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MediaPause), tr("&Pausar"), this);
    m_pauseVMAction->setStatusTip(tr("Pausar la máquina virtual en ejecución"));
    connect(m_pauseVMAction, &QAction::triggered, this, &MainWindow::pauseVM);

    m_stopVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MediaStop), tr("&Detener"), this);
    m_stopVMAction->setShortcut(QKeySequence("Ctrl+H"));
    m_stopVMAction->setStatusTip(tr("Detener la máquina virtual en ejecución"));
    connect(m_stopVMAction, &QAction::triggered, this, &MainWindow::stopVM);

    // New manager actions
    m_diskManagerAction = new QAction(QIcon(":/icons/disk-manager.png"), tr("Administrador de &discos virtuales..."), this);
    m_diskManagerAction->setStatusTip(tr("Administrar discos duros virtuales - crear, copiar, modificar y eliminar"));
    connect(m_diskManagerAction, &QAction::triggered, this, &MainWindow::showDiskManager);

    m_mediaManagerAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon), tr("Administrador de &medios virtuales..."), this);
    m_mediaManagerAction->setStatusTip(tr("Administrar discos duros virtuales, CD/DVD e imágenes de disquete"));
    connect(m_mediaManagerAction, &QAction::triggered, this, &MainWindow::showMediaManager);

    m_networkManagerAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon), tr("Administrador de &red..."), this);
    m_networkManagerAction->setStatusTip(tr("Administrar redes NAT y adaptadores solo anfitrión"));
    connect(m_networkManagerAction, &QAction::triggered, this, &MainWindow::showNetworkManager);

    m_snapshotManagerAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Instantáneas..."), this);
    m_snapshotManagerAction->setStatusTip(tr("Administrar instantáneas de la máquina virtual"));
    connect(m_snapshotManagerAction, &QAction::triggered, this, &MainWindow::showSnapshotManager);

    m_importVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_ArrowDown), tr("&Importar servicio virtualizado..."), this);
    m_importVMAction->setStatusTip(tr("Importar una máquina virtual desde un archivo OVA/OVF"));
    connect(m_importVMAction, &QAction::triggered, this, &MainWindow::importVM);

    m_exportVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_ArrowUp), tr("&Exportar servicio virtualizado..."), this);
    m_exportVMAction->setStatusTip(tr("Exportar la máquina virtual a un archivo OVA/OVF"));
    connect(m_exportVMAction, &QAction::triggered, this, &MainWindow::exportVM);

    m_cloneVMAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Clonar..."), this);
    m_cloneVMAction->setStatusTip(tr("Crear una copia de la máquina virtual"));
    connect(m_cloneVMAction, &QAction::triggered, this, &MainWindow::cloneVM);

    // Help menu actions
    m_aboutAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("&Acerca de KVM Manager"), this);
    m_aboutAction->setStatusTip(tr("Mostrar información sobre la aplicación"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    m_aboutQtAction = new QAction(tr("Acerca de &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Mostrar información sobre Qt"));
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::setupMenuBar()
{
    // File menu
    m_fileMenu = menuBar()->addMenu(tr("&Archivo"));
    m_fileMenu->addAction(m_newVMAction);
    m_fileMenu->addAction(m_addVMAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_importVMAction);
    m_fileMenu->addAction(m_exportVMAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_diskManagerAction);
    m_fileMenu->addAction(m_mediaManagerAction);
    m_fileMenu->addAction(m_networkManagerAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_preferencesAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    // Machine menu
    m_machineMenu = menuBar()->addMenu(tr("&Máquina"));
    m_machineMenu->addAction(m_removeVMAction);
    m_machineMenu->addAction(m_cloneVMAction);
    m_machineMenu->addAction(m_configureVMAction);
    m_machineMenu->addSeparator();
    m_machineMenu->addAction(m_startVMAction);
    m_machineMenu->addAction(m_pauseVMAction);
    m_machineMenu->addAction(m_stopVMAction);
    m_machineMenu->addSeparator();
    m_machineMenu->addAction(m_snapshotManagerAction);

    // View menu
    m_viewMenu = menuBar()->addMenu(tr("&Ver"));

    // Help menu
    m_helpMenu = menuBar()->addMenu(tr("A&yuda"));
    m_helpMenu->addAction(m_aboutAction);
    m_helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar(tr("Principal"));
    m_mainToolBar->setObjectName("MainToolBar");
    
    m_mainToolBar->addAction(m_newVMAction);
    m_mainToolBar->addAction(m_addVMAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_removeVMAction);
    m_mainToolBar->addAction(m_configureVMAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_startVMAction);
    m_mainToolBar->addAction(m_pauseVMAction);
    m_mainToolBar->addAction(m_stopVMAction);
}

void MainWindow::setupCentralWidget()
{
    m_centralSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Create VM list widget (left panel)
    m_vmListWidget = new VMListWidget(m_kvmManager, this);
    m_vmListWidget->setMinimumWidth(300);
    m_vmListWidget->setMaximumWidth(400);
    
    // Create VM details widget (right panel)
    m_vmDetailsWidget = new VMDetailsWidget(this);
    
    // Add widgets to splitter
    m_centralSplitter->addWidget(m_vmListWidget);
    m_centralSplitter->addWidget(m_vmDetailsWidget);
    
    // Set splitter proportions (30% left, 70% right)
    m_centralSplitter->setSizes({300, 700});
    
    setCentralWidget(m_centralSplitter);
    
    // Connect signals
    connect(m_vmListWidget, &VMListWidget::vmSelectionChanged,
            this, &MainWindow::onVMSelectionChanged);
    connect(m_vmListWidget, &VMListWidget::vmSelectionChanged,
            m_vmDetailsWidget, &VMDetailsWidget::setSelectedVM);
    
    // Connect KVMManager signals to refresh VM list
    connect(m_kvmManager, &KVMManager::vmListChanged,
            m_vmListWidget, &VMListWidget::refreshVMList);
    
    // Connect error signals
    connect(m_kvmManager, &KVMManager::errorOccurred,
            this, [this](const QString &error) {
                QMessageBox::critical(this, tr("Error KVM"), error);
            });
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("Listo"));
    statusBar()->addWidget(m_statusLabel);
    statusBar()->showMessage(tr("KVM Manager iniciado correctamente"), 2000);
}

void MainWindow::newVM()
{
    VMCreationWizard wizard(m_kvmManager, this);
    if (wizard.exec() == QDialog::Accepted) {
        // El wizard ya ha creado la VM, solo necesitamos actualizar la UI
        m_vmListWidget->refreshVMList();
        m_statusLabel->setText(tr("Nueva máquina virtual creada correctamente"));
    }
}

void MainWindow::addVM()
{
    // TODO: Implement add existing VM dialog
    QMessageBox::information(this, tr("Agregar VM"), 
                           tr("Funcionalidad para agregar máquina virtual existente será implementada próximamente."));
}

void MainWindow::removeVM()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    if (selectedVM.isEmpty()) {
        QMessageBox::warning(this, tr("Eliminar VM"), 
                           tr("Por favor seleccione una máquina virtual para eliminar."));
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Eliminar VM"),
                                  tr("¿Está seguro de que desea eliminar la máquina virtual '%1'?").arg(selectedVM),
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // TODO: Implement VM removal
        QMessageBox::information(this, tr("Eliminar VM"), 
                               tr("Funcionalidad de eliminación será implementada próximamente."));
    }
}

void MainWindow::configureVM()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    if (selectedVM.isEmpty()) {
        QMessageBox::warning(this, tr("Configurar VM"), 
                           tr("Por favor seleccione una máquina virtual para configurar."));
        return;
    }
    
    VirtualMachine *vm = m_kvmManager->getVirtualMachine(selectedVM);
    if (vm) {
        AdvancedVMConfigDialog dialog(vm, m_kvmManager, this);
        if (dialog.exec() == QDialog::Accepted) {
            // Refresh VM list and details after configuration changes
            m_vmListWidget->refreshVMList();
        }
    }
}

void MainWindow::startVM()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    if (selectedVM.isEmpty()) {
        QMessageBox::warning(this, tr("Iniciar VM"), 
                           tr("Por favor seleccione una máquina virtual para iniciar."));
        return;
    }
    
    // Intentar iniciar la VM usando KVMManager
    if (m_kvmManager->startVM(selectedVM)) {
        m_statusLabel->setText(tr("Máquina virtual '%1' iniciada correctamente").arg(selectedVM));
        // Actualizar estado de botones
        updateUIState();
    } else {
        // El error ya se mostrará via señal errorOccurred
        m_statusLabel->setText(tr("Error al iniciar la máquina virtual"));
    }
}

void MainWindow::pauseVM()
{
    // TODO: Implement VM pause
    QMessageBox::information(this, tr("Pausar VM"), 
                           tr("Funcionalidad de pausa será implementada próximamente."));
}

void MainWindow::stopVM()
{
    // TODO: Implement VM stop
    QMessageBox::information(this, tr("Detener VM"), 
                           tr("Funcionalidad de detener VM será implementada próximamente."));
}

void MainWindow::showPreferences()
{
    PreferencesDialog dialog(m_kvmManager, this);
    dialog.exec();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("Acerca de KVM Manager"),
                      tr("<h2>KVM Manager 1.0</h2>"
                         "<p>Una interfaz gráfica moderna para gestionar máquinas virtuales KVM.</p>"
                         "<p>Inspirado en VirtualBox, pero diseñado específicamente para KVM.</p>"
                         "<p><b>Características:</b></p>"
                         "<ul>"
                         "<li>Gestión intuitiva de máquinas virtuales</li>"
                         "<li>Interfaz moderna y familiar</li>"
                         "<li>Soporte completo para KVM</li>"
                         "</ul>"));
}

void MainWindow::onVMSelectionChanged(const QString &vmName)
{
    updateUIState();
    
    if (!vmName.isEmpty()) {
        m_statusLabel->setText(tr("Máquina virtual seleccionada: %1").arg(vmName));
    } else {
        m_statusLabel->setText(tr("Ninguna máquina virtual seleccionada"));
    }
}

void MainWindow::updateUIState()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    bool hasSelection = !selectedVM.isEmpty();
    
    // Enable/disable actions based on selection
    m_removeVMAction->setEnabled(hasSelection);
    m_configureVMAction->setEnabled(hasSelection);
    m_startVMAction->setEnabled(hasSelection);
    m_pauseVMAction->setEnabled(false); // TODO: Enable when VM is running
    m_stopVMAction->setEnabled(false);  // TODO: Enable when VM is running
    m_cloneVMAction->setEnabled(hasSelection);
    m_exportVMAction->setEnabled(hasSelection);
    m_snapshotManagerAction->setEnabled(hasSelection);
}

// New manager implementations
void MainWindow::showDiskManager()
{
    DiskManagerDialog dialog(m_kvmManager, this);
    dialog.exec();
}

void MainWindow::showMediaManager()
{
    MediaManagerDialog dialog(m_kvmManager, this);
    dialog.exec();
}

void MainWindow::showNetworkManager()
{
    NetworkManagerDialog dialog(m_kvmManager, this);
    dialog.exec();
}

void MainWindow::showSnapshotManager()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    if (selectedVM.isEmpty()) {
        QMessageBox::warning(this, tr("Sin selección"), 
            tr("Debe seleccionar una máquina virtual para administrar sus instantáneas."));
        return;
    }
    
    VirtualMachine *vm = m_kvmManager->getVirtualMachine(selectedVM);
    if (vm) {
        SnapshotManagerDialog dialog(vm, this);
        dialog.exec();
    }
}

void MainWindow::importVM()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Importar servicio virtualizado"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        tr("Archivos OVA/OVF (*.ova *.ovf);;Todos los archivos (*.*)"));
    
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, tr("Importar VM"), 
            tr("Función en desarrollo: Importar desde %1").arg(fileName));
    }
}

void MainWindow::exportVM()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    if (selectedVM.isEmpty()) {
        QMessageBox::warning(this, tr("Sin selección"), 
            tr("Debe seleccionar una máquina virtual para exportar."));
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Exportar servicio virtualizado"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" + selectedVM + ".ova",
        tr("Archivos OVA (*.ova);;Archivos OVF (*.ovf)"));
    
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, tr("Exportar VM"), 
            tr("Función en desarrollo: Exportar '%1' a %2").arg(selectedVM).arg(fileName));
    }
}

void MainWindow::cloneVM()
{
    QString selectedVM = m_vmListWidget->getSelectedVM();
    if (selectedVM.isEmpty()) {
        QMessageBox::warning(this, tr("Sin selección"), 
            tr("Debe seleccionar una máquina virtual para clonar."));
        return;
    }
    
    bool ok;
    QString cloneName = QInputDialog::getText(this, tr("Clonar máquina virtual"),
        tr("Nombre del clon:"), QLineEdit::Normal, 
        selectedVM + " - Clon", &ok);
    
    if (ok && !cloneName.isEmpty()) {
        QMessageBox::information(this, tr("Clonar VM"), 
            tr("Función en desarrollo: Crear clon '%1' de '%2'").arg(cloneName).arg(selectedVM));
    }
}