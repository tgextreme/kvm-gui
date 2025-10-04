#include "PreferencesDialog.h"
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
#include <QPushButton>
#include <QFileDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QSplitter>
#include <QSettings>
#include <QStandardPaths>
#include <QApplication>
#include <QMessageBox>

PreferencesDialog::PreferencesDialog(KVMManager *kvmManager, QWidget *parent)
    : QDialog(parent)
    , m_kvmManager(kvmManager)
{
    setWindowTitle(tr("Preferencias de KVM Manager"));
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    resize(800, 600);
    
    setupUI();
    loadSettings();
}

void PreferencesDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget();
    
    createGeneralTab();
    createInputTab();
    createUpdateTab();
    createExtensionsTab();
    createNetworkTab();
    createProxyTab();
    createInterfaceTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    m_applyButton = buttonBox->button(QDialogButtonBox::Apply);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &PreferencesDialog::saveSettings);
    
    mainLayout->addWidget(buttonBox);
}

void PreferencesDialog::createGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);
    
    // Language section
    QGroupBox *languageGroup = new QGroupBox(tr("🏷️ Idioma"));
    QGridLayout *langLayout = new QGridLayout(languageGroup);
    
    langLayout->addWidget(new QLabel(tr("Idioma de la interfaz:")), 0, 0);
    m_languageCombo = new QComboBox();
    m_languageCombo->addItems({tr("Español"), tr("English"), tr("Français"), tr("Deutsch")});
    langLayout->addWidget(m_languageCombo, 0, 1);
    
    layout->addWidget(languageGroup);
    
    // Default folders section
    QGroupBox *foldersGroup = new QGroupBox(tr("🗂️ Carpetas Predeterminadas"));
    QGridLayout *foldersLayout = new QGridLayout(foldersGroup);
    
    // Default VM folder
    foldersLayout->addWidget(new QLabel(tr("Carpeta de máquinas virtuales:")), 0, 0);
    m_defaultVMFolderEdit = new QLineEdit();
    m_browseVMFolderButton = new QPushButton(tr("Examinar..."));
    connect(m_browseVMFolderButton, &QPushButton::clicked, this, &PreferencesDialog::selectDefaultVMFolder);
    
    QHBoxLayout *vmFolderLayout = new QHBoxLayout();
    vmFolderLayout->addWidget(m_defaultVMFolderEdit);
    vmFolderLayout->addWidget(m_browseVMFolderButton);
    foldersLayout->addLayout(vmFolderLayout, 0, 1);
    
    // Snapshot folder
    foldersLayout->addWidget(new QLabel(tr("Carpeta de instantáneas:")), 1, 0);
    m_snapshotFolderEdit = new QLineEdit();
    m_browseSnapshotButton = new QPushButton(tr("Examinar..."));
    connect(m_browseSnapshotButton, &QPushButton::clicked, this, &PreferencesDialog::selectSnapshotFolder);
    
    QHBoxLayout *snapshotLayout = new QHBoxLayout();
    snapshotLayout->addWidget(m_snapshotFolderEdit);
    snapshotLayout->addWidget(m_browseSnapshotButton);
    foldersLayout->addLayout(snapshotLayout, 1, 1);
    
    layout->addWidget(foldersGroup);
    
    // Auto-update section
    QGroupBox *updateGroup = new QGroupBox(tr("🔄 Actualizaciones"));
    QGridLayout *updateLayout = new QGridLayout(updateGroup);
    
    m_autoUpdateCheckBox = new QCheckBox(tr("Buscar actualizaciones automáticamente"));
    updateLayout->addWidget(m_autoUpdateCheckBox, 0, 0, 1, 2);
    
    updateLayout->addWidget(new QLabel(tr("Frecuencia:")), 1, 0);
    m_updateFrequencyCombo = new QComboBox();
    m_updateFrequencyCombo->addItems({tr("Diaria"), tr("Semanal"), tr("Mensual")});
    updateLayout->addWidget(m_updateFrequencyCombo, 1, 1);
    
    layout->addWidget(updateGroup);
    
    layout->addStretch();
    
    m_tabWidget->addTab(m_generalTab, tr("General"));
}

void PreferencesDialog::createInputTab()
{
    m_inputTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_inputTab);
    
    // Host key section
    QGroupBox *hostKeyGroup = new QGroupBox(tr("🖱️ Tecla Anfitrión"));
    QGridLayout *hostKeyLayout = new QGridLayout(hostKeyGroup);
    
    hostKeyLayout->addWidget(new QLabel(tr("Tecla anfitrión:")), 0, 0);
    m_hostKeyCombo = new QComboBox();
    m_hostKeyCombo->addItems({
        tr("Ctrl Derecha"), tr("Ctrl Izquierda"), 
        tr("Alt Derecha"), tr("Alt Izquierda"),
        tr("Cmd (⌘)"), tr("Meta")
    });
    hostKeyLayout->addWidget(m_hostKeyCombo, 0, 1);
    
    QLabel *hostKeyInfo = new QLabel(tr("La tecla anfitrión se usa para liberar el ratón y teclado de la máquina virtual."));
    hostKeyInfo->setWordWrap(true);
    hostKeyInfo->setStyleSheet("color: gray; font-size: 10px;");
    hostKeyLayout->addWidget(hostKeyInfo, 1, 0, 1, 2);
    
    layout->addWidget(hostKeyGroup);
    
    // Keyboard shortcuts section
    QGroupBox *shortcutsGroup = new QGroupBox(tr("🎹 Atajos de Teclado"));
    QVBoxLayout *shortcutsLayout = new QVBoxLayout(shortcutsGroup);
    
    m_shortcutsTree = new QTreeWidget();
    m_shortcutsTree->setHeaderLabels({tr("Acción"), tr("Atajo"), tr("Descripción")});
    m_shortcutsTree->header()->setStretchLastSection(true);
    
    // Add sample shortcuts
    QTreeWidgetItem *fileItem = new QTreeWidgetItem({tr("Archivo"), "", ""});
    fileItem->addChild(new QTreeWidgetItem({tr("Nueva VM"), "Ctrl+N", tr("Crear nueva máquina virtual")}));
    fileItem->addChild(new QTreeWidgetItem({tr("Agregar VM"), "Ctrl+A", tr("Agregar máquina virtual existente")}));
    fileItem->addChild(new QTreeWidgetItem({tr("Preferencias"), "Ctrl+,", tr("Abrir preferencias")}));
    m_shortcutsTree->addTopLevelItem(fileItem);
    
    QTreeWidgetItem *machineItem = new QTreeWidgetItem({tr("Máquina"), "", ""});
    machineItem->addChild(new QTreeWidgetItem({tr("Iniciar"), "Ctrl+T", tr("Iniciar máquina virtual")}));
    machineItem->addChild(new QTreeWidgetItem({tr("Detener"), "Ctrl+H", tr("Detener máquina virtual")}));
    machineItem->addChild(new QTreeWidgetItem({tr("Configurar"), "Ctrl+S", tr("Configurar máquina virtual")}));
    m_shortcutsTree->addTopLevelItem(machineItem);
    
    QTreeWidgetItem *viewItem = new QTreeWidgetItem({tr("Ver"), "", ""});
    viewItem->addChild(new QTreeWidgetItem({tr("Pantalla completa"), "Host+F", tr("Cambiar a pantalla completa")}));
    viewItem->addChild(new QTreeWidgetItem({tr("Modo escalado"), "Host+C", tr("Activar modo escalado")}));
    m_shortcutsTree->addTopLevelItem(viewItem);
    
    m_shortcutsTree->expandAll();
    shortcutsLayout->addWidget(m_shortcutsTree);
    
    layout->addWidget(shortcutsGroup);
    
    m_tabWidget->addTab(m_inputTab, tr("Entrada"));
}

void PreferencesDialog::createUpdateTab()
{
    m_updateTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_updateTab);
    
    QGroupBox *updateGroup = new QGroupBox(tr("Configuración de Actualizaciones"));
    QGridLayout *updateLayout = new QGridLayout(updateGroup);
    
    m_checkUpdatesCheckBox = new QCheckBox(tr("Comprobar actualizaciones automáticamente"));
    updateLayout->addWidget(m_checkUpdatesCheckBox, 0, 0, 1, 2);
    
    updateLayout->addWidget(new QLabel(tr("Canal de actualizaciones:")), 1, 0);
    m_updateChannelCombo = new QComboBox();
    m_updateChannelCombo->addItems({tr("Estable"), tr("Beta"), tr("Desarrollo")});
    updateLayout->addWidget(m_updateChannelCombo, 1, 1);
    
    updateLayout->addWidget(new QLabel(tr("Frecuencia de comprobación:")), 2, 0);
    m_checkFrequencyCombo = new QComboBox();
    m_checkFrequencyCombo->addItems({tr("Diaria"), tr("Semanal"), tr("Mensual"), tr("Manual")});
    updateLayout->addWidget(m_checkFrequencyCombo, 2, 1);
    
    layout->addWidget(updateGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_updateTab, tr("Actualización"));
}

void PreferencesDialog::createExtensionsTab()
{
    m_extensionsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_extensionsTab);
    
    QGroupBox *extensionsGroup = new QGroupBox(tr("Extension Packs Instalados"));
    QVBoxLayout *extLayout = new QVBoxLayout(extensionsGroup);
    
    m_extensionsList = new QListWidget();
    
    // Add sample extension
    QListWidgetItem *sampleExt = new QListWidgetItem("KVM Extension Pack 1.0");
    sampleExt->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    m_extensionsList->addItem(sampleExt);
    
    extLayout->addWidget(m_extensionsList);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_installExtensionButton = new QPushButton(tr("Instalar Extension Pack..."));
    m_removeExtensionButton = new QPushButton(tr("Eliminar"));
    m_removeExtensionButton->setEnabled(false);
    
    buttonLayout->addWidget(m_installExtensionButton);
    buttonLayout->addWidget(m_removeExtensionButton);
    buttonLayout->addStretch();
    
    extLayout->addLayout(buttonLayout);
    
    layout->addWidget(extensionsGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_extensionsTab, tr("Extensiones"));
}

void PreferencesDialog::createNetworkTab()
{
    m_networkTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_networkTab);
    
    m_networkSubTabs = new QTabWidget();
    
    // NAT Networks tab
    QWidget *natTab = new QWidget();
    QVBoxLayout *natLayout = new QVBoxLayout(natTab);
    
    QLabel *natInfo = new QLabel(tr("Las redes NAT permiten que múltiples máquinas virtuales compartan una conexión a Internet "
                                   "y puedan comunicarse entre sí, mientras permanecen aisladas del host."));
    natInfo->setWordWrap(true);
    natInfo->setStyleSheet("color: gray; padding: 10px; background: #2a2a2a; border-radius: 5px;");
    natLayout->addWidget(natInfo);
    
    m_natNetworksList = new QListWidget();
    natLayout->addWidget(m_natNetworksList);
    
    QHBoxLayout *natButtonLayout = new QHBoxLayout();
    m_addNATNetworkButton = new QPushButton(tr("Agregar"));
    m_removeNATNetworkButton = new QPushButton(tr("Eliminar"));
    m_editNATNetworkButton = new QPushButton(tr("Editar"));
    
    connect(m_addNATNetworkButton, &QPushButton::clicked, this, &PreferencesDialog::addNATNetwork);
    connect(m_removeNATNetworkButton, &QPushButton::clicked, this, &PreferencesDialog::removeNATNetwork);
    connect(m_editNATNetworkButton, &QPushButton::clicked, this, &PreferencesDialog::editNATNetwork);
    
    natButtonLayout->addWidget(m_addNATNetworkButton);
    natButtonLayout->addWidget(m_removeNATNetworkButton);
    natButtonLayout->addWidget(m_editNATNetworkButton);
    natButtonLayout->addStretch();
    
    natLayout->addLayout(natButtonLayout);
    m_networkSubTabs->addTab(natTab, tr("Redes NAT"));
    
    // Host-Only Networks tab
    QWidget *hostOnlyTab = new QWidget();
    QVBoxLayout *hostOnlyLayout = new QVBoxLayout(hostOnlyTab);
    
    QLabel *hostOnlyInfo = new QLabel(tr("Las redes solo-anfitrión permiten la comunicación entre el host y las máquinas virtuales, "
                                        "pero no proporcionan acceso a Internet."));
    hostOnlyInfo->setWordWrap(true);
    hostOnlyInfo->setStyleSheet("color: gray; padding: 10px; background: #2a2a2a; border-radius: 5px;");
    hostOnlyLayout->addWidget(hostOnlyInfo);
    
    m_hostOnlyNetworksList = new QListWidget();
    hostOnlyLayout->addWidget(m_hostOnlyNetworksList);
    
    QHBoxLayout *hostOnlyButtonLayout = new QHBoxLayout();
    m_addHostOnlyButton = new QPushButton(tr("Agregar"));
    m_removeHostOnlyButton = new QPushButton(tr("Eliminar"));
    m_editHostOnlyButton = new QPushButton(tr("Editar"));
    
    connect(m_addHostOnlyButton, &QPushButton::clicked, this, &PreferencesDialog::addHostOnlyNetwork);
    connect(m_removeHostOnlyButton, &QPushButton::clicked, this, &PreferencesDialog::removeHostOnlyNetwork);
    connect(m_editHostOnlyButton, &QPushButton::clicked, this, &PreferencesDialog::editHostOnlyNetwork);
    
    hostOnlyButtonLayout->addWidget(m_addHostOnlyButton);
    hostOnlyButtonLayout->addWidget(m_removeHostOnlyButton);
    hostOnlyButtonLayout->addWidget(m_editHostOnlyButton);
    hostOnlyButtonLayout->addStretch();
    
    hostOnlyLayout->addLayout(hostOnlyButtonLayout);
    m_networkSubTabs->addTab(hostOnlyTab, tr("Redes Solo-Anfitrión"));
    
    layout->addWidget(m_networkSubTabs);
    
    m_tabWidget->addTab(m_networkTab, tr("Red"));
}

void PreferencesDialog::createProxyTab()
{
    m_proxyTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_proxyTab);
    
    QGroupBox *proxyGroup = new QGroupBox(tr("Configuración de Proxy"));
    QGridLayout *proxyLayout = new QGridLayout(proxyGroup);
    
    m_enableProxyCheckBox = new QCheckBox(tr("Habilitar proxy"));
    proxyLayout->addWidget(m_enableProxyCheckBox, 0, 0, 1, 2);
    
    proxyLayout->addWidget(new QLabel(tr("Tipo:")), 1, 0);
    m_proxyTypeCombo = new QComboBox();
    m_proxyTypeCombo->addItems({tr("HTTP"), tr("HTTPS"), tr("SOCKS4"), tr("SOCKS5")});
    proxyLayout->addWidget(m_proxyTypeCombo, 1, 1);
    
    proxyLayout->addWidget(new QLabel(tr("Host:")), 2, 0);
    m_proxyHostEdit = new QLineEdit();
    proxyLayout->addWidget(m_proxyHostEdit, 2, 1);
    
    proxyLayout->addWidget(new QLabel(tr("Puerto:")), 3, 0);
    m_proxyPortSpin = new QSpinBox();
    m_proxyPortSpin->setRange(1, 65535);
    m_proxyPortSpin->setValue(8080);
    proxyLayout->addWidget(m_proxyPortSpin, 3, 1);
    
    proxyLayout->addWidget(new QLabel(tr("Usuario:")), 4, 0);
    m_proxyUserEdit = new QLineEdit();
    proxyLayout->addWidget(m_proxyUserEdit, 4, 1);
    
    proxyLayout->addWidget(new QLabel(tr("Contraseña:")), 5, 0);
    m_proxyPasswordEdit = new QLineEdit();
    m_proxyPasswordEdit->setEchoMode(QLineEdit::Password);
    proxyLayout->addWidget(m_proxyPasswordEdit, 5, 1);
    
    layout->addWidget(proxyGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_proxyTab, tr("Proxy"));
}

void PreferencesDialog::createInterfaceTab()
{
    m_interfaceTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_interfaceTab);
    
    QGroupBox *appearanceGroup = new QGroupBox(tr("Apariencia"));
    QGridLayout *appearanceLayout = new QGridLayout(appearanceGroup);
    
    m_showToolbarCheckBox = new QCheckBox(tr("Mostrar barra de herramientas"));
    m_showToolbarCheckBox->setChecked(true);
    appearanceLayout->addWidget(m_showToolbarCheckBox, 0, 0, 1, 2);
    
    m_showStatusbarCheckBox = new QCheckBox(tr("Mostrar barra de estado"));
    m_showStatusbarCheckBox->setChecked(true);
    appearanceLayout->addWidget(m_showStatusbarCheckBox, 1, 0, 1, 2);
    
    appearanceLayout->addWidget(new QLabel(tr("Tema:")), 2, 0);
    m_themeCombo = new QComboBox();
    m_themeCombo->addItems({tr("Oscuro"), tr("Claro"), tr("Sistema")});
    appearanceLayout->addWidget(m_themeCombo, 2, 1);
    
    appearanceLayout->addWidget(new QLabel(tr("Tamaño de iconos:")), 3, 0);
    m_iconSizeCombo = new QComboBox();
    m_iconSizeCombo->addItems({tr("Pequeño (16px)"), tr("Mediano (24px)"), tr("Grande (32px)")});
    m_iconSizeCombo->setCurrentIndex(1);
    appearanceLayout->addWidget(m_iconSizeCombo, 3, 1);
    
    layout->addWidget(appearanceGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_interfaceTab, tr("Interfaz"));
}

void PreferencesDialog::loadSettings()
{
    QSettings settings;
    
    // General tab
    m_defaultVMFolderEdit->setText(settings.value("general/defaultVMFolder", 
                                  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.local/share/kvm").toString());
    m_snapshotFolderEdit->setText(settings.value("general/snapshotFolder", 
                                 QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.local/share/kvm/snapshots").toString());
    
    m_languageCombo->setCurrentText(settings.value("general/language", tr("Español")).toString());
    m_autoUpdateCheckBox->setChecked(settings.value("general/autoUpdate", true).toBool());
    m_updateFrequencyCombo->setCurrentText(settings.value("general/updateFrequency", tr("Semanal")).toString());
    
    // Input tab
    m_hostKeyCombo->setCurrentText(settings.value("input/hostKey", tr("Ctrl Derecha")).toString());
    
    // Update tab
    m_checkUpdatesCheckBox->setChecked(settings.value("update/checkUpdates", true).toBool());
    m_updateChannelCombo->setCurrentText(settings.value("update/channel", tr("Estable")).toString());
    m_checkFrequencyCombo->setCurrentText(settings.value("update/frequency", tr("Semanal")).toString());
    
    // Proxy tab
    m_enableProxyCheckBox->setChecked(settings.value("proxy/enabled", false).toBool());
    m_proxyTypeCombo->setCurrentText(settings.value("proxy/type", tr("HTTP")).toString());
    m_proxyHostEdit->setText(settings.value("proxy/host", "").toString());
    m_proxyPortSpin->setValue(settings.value("proxy/port", 8080).toInt());
    m_proxyUserEdit->setText(settings.value("proxy/user", "").toString());
    
    // Interface tab
    m_showToolbarCheckBox->setChecked(settings.value("interface/showToolbar", true).toBool());
    m_showStatusbarCheckBox->setChecked(settings.value("interface/showStatusbar", true).toBool());
    m_themeCombo->setCurrentText(settings.value("interface/theme", tr("Oscuro")).toString());
    m_iconSizeCombo->setCurrentText(settings.value("interface/iconSize", tr("Mediano (24px)")).toString());
}

void PreferencesDialog::saveSettings()
{
    QSettings settings;
    
    // General tab
    settings.setValue("general/defaultVMFolder", m_defaultVMFolderEdit->text());
    settings.setValue("general/snapshotFolder", m_snapshotFolderEdit->text());
    settings.setValue("general/language", m_languageCombo->currentText());
    settings.setValue("general/autoUpdate", m_autoUpdateCheckBox->isChecked());
    settings.setValue("general/updateFrequency", m_updateFrequencyCombo->currentText());
    
    // Input tab
    settings.setValue("input/hostKey", m_hostKeyCombo->currentText());
    
    // Update tab
    settings.setValue("update/checkUpdates", m_checkUpdatesCheckBox->isChecked());
    settings.setValue("update/channel", m_updateChannelCombo->currentText());
    settings.setValue("update/frequency", m_checkFrequencyCombo->currentText());
    
    // Proxy tab
    settings.setValue("proxy/enabled", m_enableProxyCheckBox->isChecked());
    settings.setValue("proxy/type", m_proxyTypeCombo->currentText());
    settings.setValue("proxy/host", m_proxyHostEdit->text());
    settings.setValue("proxy/port", m_proxyPortSpin->value());
    settings.setValue("proxy/user", m_proxyUserEdit->text());
    
    // Interface tab
    settings.setValue("interface/showToolbar", m_showToolbarCheckBox->isChecked());
    settings.setValue("interface/showStatusbar", m_showStatusbarCheckBox->isChecked());
    settings.setValue("interface/theme", m_themeCombo->currentText());
    settings.setValue("interface/iconSize", m_iconSizeCombo->currentText());
    
    // Update KVM Manager settings if needed
    if (m_kvmManager) {
        m_kvmManager->setDefaultVMPath(m_defaultVMFolderEdit->text());
    }
    
    QMessageBox::information(this, tr("Configuración guardada"), 
                           tr("La configuración se ha guardado correctamente."));
}

void PreferencesDialog::selectDefaultVMFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, tr("Seleccionar carpeta de máquinas virtuales"), 
                                                     m_defaultVMFolderEdit->text());
    if (!folder.isEmpty()) {
        m_defaultVMFolderEdit->setText(folder);
    }
}

void PreferencesDialog::selectSnapshotFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, tr("Seleccionar carpeta de instantáneas"), 
                                                     m_snapshotFolderEdit->text());
    if (!folder.isEmpty()) {
        m_snapshotFolderEdit->setText(folder);
    }
}

void PreferencesDialog::onLanguageChanged()
{
    QMessageBox::information(this, tr("Cambio de idioma"), 
                           tr("El cambio de idioma se aplicará al reiniciar la aplicación."));
}

void PreferencesDialog::onUpdateCheckChanged()
{
    // Handle update check change
}

void PreferencesDialog::addNATNetwork()
{
    // TODO: Implement NAT network creation dialog
    QMessageBox::information(this, tr("Red NAT"), tr("Creación de red NAT será implementada próximamente."));
}

void PreferencesDialog::removeNATNetwork()
{
    // TODO: Implement NAT network removal
    QMessageBox::information(this, tr("Red NAT"), tr("Eliminación de red NAT será implementada próximamente."));
}

void PreferencesDialog::editNATNetwork()
{
    // TODO: Implement NAT network editing dialog
    QMessageBox::information(this, tr("Red NAT"), tr("Edición de red NAT será implementada próximamente."));
}

void PreferencesDialog::addHostOnlyNetwork()
{
    // TODO: Implement Host-Only network creation dialog
    QMessageBox::information(this, tr("Red Solo-Anfitrión"), tr("Creación de red solo-anfitrión será implementada próximamente."));
}

void PreferencesDialog::removeHostOnlyNetwork()
{
    // TODO: Implement Host-Only network removal
    QMessageBox::information(this, tr("Red Solo-Anfitrión"), tr("Eliminación de red solo-anfitrión será implementada próximamente."));
}

void PreferencesDialog::editHostOnlyNetwork()
{
    // TODO: Implement Host-Only network editing dialog
    QMessageBox::information(this, tr("Red Solo-Anfitrión"), tr("Edición de red solo-anfitrión será implementada próximamente."));
}