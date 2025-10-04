#ifndef NETWORKMANAGERDIALOG_H
#define NETWORKMANAGERDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QSplitter>

class KVMManager;

/**
 * @brief Administrador de red - Gestiona redes NAT y Host-Only globales
 * Equivalente al "Administrador de red" de VirtualBox
 */
class NetworkManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkManagerDialog(KVMManager *kvmManager, QWidget *parent = nullptr);
    
    void refreshNetworks();

private slots:
    // NAT Networks Tab
    void addNATNetwork();
    void removeNATNetwork();
    void editNATNetwork();
    void refreshNATNetworks();
    void onNATNetworkSelectionChanged();
    
    // Host-Only Networks Tab
    void addHostOnlyNetwork();
    void removeHostOnlyNetwork();
    void editHostOnlyNetwork();
    void refreshHostOnlyNetworks();
    void onHostOnlyNetworkSelectionChanged();

private:
    void setupUI();
    void createNATNetworksTab();
    void createHostOnlyNetworksTab();
    void updateNATNetworkDetails();
    void updateHostOnlyNetworkDetails();
    
    KVMManager *m_kvmManager;
    
    // Main UI
    QTabWidget *m_tabWidget;
    
    // NAT Networks Tab
    QWidget *m_natNetworksTab;
    QTreeWidget *m_natNetworksTree;
    QPushButton *m_addNATButton;
    QPushButton *m_removeNATButton;
    QPushButton *m_editNATButton;
    QPushButton *m_refreshNATButton;
    
    // NAT Network Details
    QGroupBox *m_natDetailsGroup;
    QLineEdit *m_natNameEdit;
    QLineEdit *m_natCIDREdit;
    QCheckBox *m_natDHCPCheckBox;
    QCheckBox *m_natIPv6CheckBox;
    QTextEdit *m_natPortForwardingEdit;
    
    // Host-Only Networks Tab
    QWidget *m_hostOnlyNetworksTab;
    QTreeWidget *m_hostOnlyNetworksTree;
    QPushButton *m_addHostOnlyButton;
    QPushButton *m_removeHostOnlyButton;
    QPushButton *m_editHostOnlyButton;
    QPushButton *m_refreshHostOnlyButton;
    
    // Host-Only Network Details
    QGroupBox *m_hostOnlyDetailsGroup;
    QLineEdit *m_hostOnlyNameEdit;
    QLineEdit *m_hostOnlyIPEdit;
    QLineEdit *m_hostOnlyMaskEdit;
    QCheckBox *m_hostOnlyDHCPCheckBox;
    QLineEdit *m_hostOnlyDHCPStartEdit;
    QLineEdit *m_hostOnlyDHCPEndEdit;
};

#endif // NETWORKMANAGERDIALOG_H