#include "NetworkManagerDialog.h"
#include "../core/KVMManager.h"

#include <QApplication>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QInputDialog>

NetworkManagerDialog::NetworkManagerDialog(KVMManager *kvmManager, QWidget *parent)
    : QDialog(parent)
    , m_kvmManager(kvmManager)
{
    setWindowTitle(tr("Administrador de red"));
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    resize(700, 500);
    setModal(true);
    
    setupUI();
    refreshNetworks();
}

void NetworkManagerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget();
    
    createNATNetworksTab();
    createHostOnlyNetworksTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *closeButton = new QPushButton(tr("&Cerrar"));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void NetworkManagerDialog::createNATNetworksTab()
{
    m_natNetworksTab = new QWidget();
    m_tabWidget->addTab(m_natNetworksTab, QApplication::style()->standardIcon(QStyle::SP_ComputerIcon), tr("Redes NAT"));
    
    QVBoxLayout *layout = new QVBoxLayout(m_natNetworksTab);
    
    // Tree widget
    m_natNetworksTree = new QTreeWidget();
    m_natNetworksTree->setHeaderLabels({tr("Nombre de red"), tr("CIDR IPv4"), tr("DHCP"), tr("IPv6")});
    m_natNetworksTree->header()->setStretchLastSection(true);
    connect(m_natNetworksTree, &QTreeWidget::itemSelectionChanged, this, &NetworkManagerDialog::onNATNetworkSelectionChanged);
    
    layout->addWidget(m_natNetworksTree);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addNATButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Agregar"));
    m_removeNATButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Quitar"));
    m_editNATButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("&Editar"));
    m_refreshNATButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("A&ctualizar"));
    
    buttonLayout->addWidget(m_addNATButton);
    buttonLayout->addWidget(m_removeNATButton);
    buttonLayout->addWidget(m_editNATButton);
    buttonLayout->addWidget(m_refreshNATButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Details group
    m_natDetailsGroup = new QGroupBox(tr("Propiedades de red NAT"));
    QGridLayout *detailsLayout = new QGridLayout(m_natDetailsGroup);
    
    detailsLayout->addWidget(new QLabel(tr("Nombre de red:")), 0, 0);
    m_natNameEdit = new QLineEdit();
    m_natNameEdit->setReadOnly(true);
    detailsLayout->addWidget(m_natNameEdit, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("CIDR IPv4:")), 1, 0);
    m_natCIDREdit = new QLineEdit();
    m_natCIDREdit->setReadOnly(true);
    detailsLayout->addWidget(m_natCIDREdit, 1, 1);
    
    m_natDHCPCheckBox = new QCheckBox(tr("Servidor DHCP habilitado"));
    m_natDHCPCheckBox->setEnabled(false);
    detailsLayout->addWidget(m_natDHCPCheckBox, 2, 0, 1, 2);
    
    m_natIPv6CheckBox = new QCheckBox(tr("Soporte IPv6 habilitado"));
    m_natIPv6CheckBox->setEnabled(false);
    detailsLayout->addWidget(m_natIPv6CheckBox, 3, 0, 1, 2);
    
    detailsLayout->addWidget(new QLabel(tr("Reenvío de puertos:")), 4, 0);
    m_natPortForwardingEdit = new QTextEdit();
    m_natPortForwardingEdit->setReadOnly(true);
    m_natPortForwardingEdit->setMaximumHeight(80);
    detailsLayout->addWidget(m_natPortForwardingEdit, 4, 1);
    
    layout->addWidget(m_natDetailsGroup);
    
    // Connect signals
    connect(m_addNATButton, &QPushButton::clicked, this, &NetworkManagerDialog::addNATNetwork);
    connect(m_removeNATButton, &QPushButton::clicked, this, &NetworkManagerDialog::removeNATNetwork);
    connect(m_editNATButton, &QPushButton::clicked, this, &NetworkManagerDialog::editNATNetwork);
    connect(m_refreshNATButton, &QPushButton::clicked, this, &NetworkManagerDialog::refreshNATNetworks);
}

void NetworkManagerDialog::createHostOnlyNetworksTab()
{
    m_hostOnlyNetworksTab = new QWidget();
    m_tabWidget->addTab(m_hostOnlyNetworksTab, QApplication::style()->standardIcon(QStyle::SP_DriveNetIcon), tr("Redes solo anfitrión"));
    
    QVBoxLayout *layout = new QVBoxLayout(m_hostOnlyNetworksTab);
    
    // Tree widget
    m_hostOnlyNetworksTree = new QTreeWidget();
    m_hostOnlyNetworksTree->setHeaderLabels({tr("Nombre"), tr("Dirección IPv4"), tr("Máscara IPv4"), tr("DHCP")});
    m_hostOnlyNetworksTree->header()->setStretchLastSection(true);
    connect(m_hostOnlyNetworksTree, &QTreeWidget::itemSelectionChanged, this, &NetworkManagerDialog::onHostOnlyNetworkSelectionChanged);
    
    layout->addWidget(m_hostOnlyNetworksTree);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addHostOnlyButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Agregar"));
    m_removeHostOnlyButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Quitar"));
    m_editHostOnlyButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("&Editar"));
    m_refreshHostOnlyButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_BrowserReload), tr("A&ctualizar"));
    
    buttonLayout->addWidget(m_addHostOnlyButton);
    buttonLayout->addWidget(m_removeHostOnlyButton);
    buttonLayout->addWidget(m_editHostOnlyButton);
    buttonLayout->addWidget(m_refreshHostOnlyButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Details group
    m_hostOnlyDetailsGroup = new QGroupBox(tr("Propiedades de adaptador"));
    QGridLayout *detailsLayout = new QGridLayout(m_hostOnlyDetailsGroup);
    
    detailsLayout->addWidget(new QLabel(tr("Nombre:")), 0, 0);
    m_hostOnlyNameEdit = new QLineEdit();
    m_hostOnlyNameEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hostOnlyNameEdit, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Dirección IPv4:")), 1, 0);
    m_hostOnlyIPEdit = new QLineEdit();
    m_hostOnlyIPEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hostOnlyIPEdit, 1, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Máscara de red IPv4:")), 2, 0);
    m_hostOnlyMaskEdit = new QLineEdit();
    m_hostOnlyMaskEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hostOnlyMaskEdit, 2, 1);
    
    m_hostOnlyDHCPCheckBox = new QCheckBox(tr("Servidor DHCP habilitado"));
    m_hostOnlyDHCPCheckBox->setEnabled(false);
    detailsLayout->addWidget(m_hostOnlyDHCPCheckBox, 3, 0, 1, 2);
    
    detailsLayout->addWidget(new QLabel(tr("IP inicial DHCP:")), 4, 0);
    m_hostOnlyDHCPStartEdit = new QLineEdit();
    m_hostOnlyDHCPStartEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hostOnlyDHCPStartEdit, 4, 1);
    
    detailsLayout->addWidget(new QLabel(tr("IP final DHCP:")), 5, 0);
    m_hostOnlyDHCPEndEdit = new QLineEdit();
    m_hostOnlyDHCPEndEdit->setReadOnly(true);
    detailsLayout->addWidget(m_hostOnlyDHCPEndEdit, 5, 1);
    
    layout->addWidget(m_hostOnlyDetailsGroup);
    
    // Connect signals
    connect(m_addHostOnlyButton, &QPushButton::clicked, this, &NetworkManagerDialog::addHostOnlyNetwork);
    connect(m_removeHostOnlyButton, &QPushButton::clicked, this, &NetworkManagerDialog::removeHostOnlyNetwork);
    connect(m_editHostOnlyButton, &QPushButton::clicked, this, &NetworkManagerDialog::editHostOnlyNetwork);
    connect(m_refreshHostOnlyButton, &QPushButton::clicked, this, &NetworkManagerDialog::refreshHostOnlyNetworks);
}

void NetworkManagerDialog::refreshNetworks()
{
    refreshNATNetworks();
    refreshHostOnlyNetworks();
}

// NAT Network Operations
void NetworkManagerDialog::addNATNetwork()
{
    bool ok;
    QString networkName = QInputDialog::getText(this, tr("Nueva red NAT"),
        tr("Nombre de la red NAT:"), QLineEdit::Normal, "NATNetwork", &ok);
    
    if (ok && !networkName.isEmpty()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_natNetworksTree);
        item->setText(0, networkName);
        item->setText(1, "10.0.2.0/24");  // Default CIDR
        item->setText(2, tr("Sí"));       // DHCP enabled
        item->setText(3, tr("No"));       // IPv6 disabled
        
        m_natNetworksTree->addTopLevelItem(item);
        QMessageBox::information(this, tr("Red creada"), 
            tr("Red NAT '%1' creada correctamente.").arg(networkName));
    }
}

void NetworkManagerDialog::removeNATNetwork()
{
    QTreeWidgetItem *current = m_natNetworksTree->currentItem();
    if (current) {
        int ret = QMessageBox::question(this, tr("Confirmar eliminación"),
            tr("¿Está seguro de que desea eliminar la red NAT '%1'?").arg(current->text(0)),
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            delete current;
            QMessageBox::information(this, tr("Red eliminada"), tr("Red NAT eliminada correctamente."));
        }
    }
}

void NetworkManagerDialog::editNATNetwork()
{
    QTreeWidgetItem *current = m_natNetworksTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("Editar red NAT"), 
            tr("Función en desarrollo: Editar propiedades de red NAT."));
    }
}

void NetworkManagerDialog::refreshNATNetworks()
{
    m_natNetworksTree->clear();
    
    // Agregar algunas redes NAT de ejemplo
    QTreeWidgetItem *defaultNAT = new QTreeWidgetItem(m_natNetworksTree);
    defaultNAT->setText(0, "NATNetwork");
    defaultNAT->setText(1, "10.0.2.0/24");
    defaultNAT->setText(2, tr("Sí"));
    defaultNAT->setText(3, tr("No"));
    
    QTreeWidgetItem *customNAT = new QTreeWidgetItem(m_natNetworksTree);
    customNAT->setText(0, "CustomNAT");
    customNAT->setText(1, "192.168.100.0/24");
    customNAT->setText(2, tr("Sí"));
    customNAT->setText(3, tr("Sí"));
}

void NetworkManagerDialog::onNATNetworkSelectionChanged()
{
    updateNATNetworkDetails();
}

void NetworkManagerDialog::updateNATNetworkDetails()
{
    QTreeWidgetItem *current = m_natNetworksTree->currentItem();
    if (current) {
        m_natNameEdit->setText(current->text(0));
        m_natCIDREdit->setText(current->text(1));
        m_natDHCPCheckBox->setChecked(current->text(2) == tr("Sí"));
        m_natIPv6CheckBox->setChecked(current->text(3) == tr("Sí"));
        m_natPortForwardingEdit->setPlainText(tr("SSH: 2222 -> 22\nHTTP: 8080 -> 80"));
    } else {
        m_natNameEdit->clear();
        m_natCIDREdit->clear();
        m_natDHCPCheckBox->setChecked(false);
        m_natIPv6CheckBox->setChecked(false);
        m_natPortForwardingEdit->clear();
    }
}

// Host-Only Network Operations
void NetworkManagerDialog::addHostOnlyNetwork()
{
    bool ok;
    QString networkName = QInputDialog::getText(this, tr("Nueva red solo anfitrión"),
        tr("Nombre del adaptador:"), QLineEdit::Normal, "vboxnet1", &ok);
    
    if (ok && !networkName.isEmpty()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_hostOnlyNetworksTree);
        item->setText(0, networkName);
        item->setText(1, "192.168.56.1");    // Default IP
        item->setText(2, "255.255.255.0");   // Default mask
        item->setText(3, tr("Sí"));          // DHCP enabled
        
        m_hostOnlyNetworksTree->addTopLevelItem(item);
        QMessageBox::information(this, tr("Adaptador creado"), 
            tr("Adaptador de red solo anfitrión '%1' creado correctamente.").arg(networkName));
    }
}

void NetworkManagerDialog::removeHostOnlyNetwork()
{
    QTreeWidgetItem *current = m_hostOnlyNetworksTree->currentItem();
    if (current) {
        int ret = QMessageBox::question(this, tr("Confirmar eliminación"),
            tr("¿Está seguro de que desea eliminar el adaptador '%1'?").arg(current->text(0)),
            QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            delete current;
            QMessageBox::information(this, tr("Adaptador eliminado"), tr("Adaptador eliminado correctamente."));
        }
    }
}

void NetworkManagerDialog::editHostOnlyNetwork()
{
    QTreeWidgetItem *current = m_hostOnlyNetworksTree->currentItem();
    if (current) {
        QMessageBox::information(this, tr("Editar adaptador"), 
            tr("Función en desarrollo: Editar propiedades del adaptador."));
    }
}

void NetworkManagerDialog::refreshHostOnlyNetworks()
{
    m_hostOnlyNetworksTree->clear();
    
    // Agregar algunos adaptadores de ejemplo
    QTreeWidgetItem *vboxnet0 = new QTreeWidgetItem(m_hostOnlyNetworksTree);
    vboxnet0->setText(0, "vboxnet0");
    vboxnet0->setText(1, "192.168.56.1");
    vboxnet0->setText(2, "255.255.255.0");
    vboxnet0->setText(3, tr("Sí"));
    
    QTreeWidgetItem *vboxnet1 = new QTreeWidgetItem(m_hostOnlyNetworksTree);
    vboxnet1->setText(0, "vboxnet1");
    vboxnet1->setText(1, "192.168.57.1");
    vboxnet1->setText(2, "255.255.255.0");
    vboxnet1->setText(3, tr("No"));
}

void NetworkManagerDialog::onHostOnlyNetworkSelectionChanged()
{
    updateHostOnlyNetworkDetails();
}

void NetworkManagerDialog::updateHostOnlyNetworkDetails()
{
    QTreeWidgetItem *current = m_hostOnlyNetworksTree->currentItem();
    if (current) {
        m_hostOnlyNameEdit->setText(current->text(0));
        m_hostOnlyIPEdit->setText(current->text(1));
        m_hostOnlyMaskEdit->setText(current->text(2));
        m_hostOnlyDHCPCheckBox->setChecked(current->text(3) == tr("Sí"));
        
        if (m_hostOnlyDHCPCheckBox->isChecked()) {
            m_hostOnlyDHCPStartEdit->setText("192.168.56.100");
            m_hostOnlyDHCPEndEdit->setText("192.168.56.254");
        } else {
            m_hostOnlyDHCPStartEdit->clear();
            m_hostOnlyDHCPEndEdit->clear();
        }
    } else {
        m_hostOnlyNameEdit->clear();
        m_hostOnlyIPEdit->clear();
        m_hostOnlyMaskEdit->clear();
        m_hostOnlyDHCPCheckBox->setChecked(false);
        m_hostOnlyDHCPStartEdit->clear();
        m_hostOnlyDHCPEndEdit->clear();
    }
}