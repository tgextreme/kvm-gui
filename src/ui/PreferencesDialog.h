#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
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
#include <QTreeWidgetItem>

class KVMManager;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(KVMManager *kvmManager, QWidget *parent = nullptr);
    
    void loadSettings();
    void saveSettings();

private slots:
    void selectDefaultVMFolder();
    void selectSnapshotFolder();
    void onLanguageChanged();
    void onUpdateCheckChanged();
    void addNATNetwork();
    void removeNATNetwork();
    void editNATNetwork();
    void addHostOnlyNetwork();
    void removeHostOnlyNetwork();
    void editHostOnlyNetwork();

private:
    void setupUI();
    void createGeneralTab();
    void createInputTab();
    void createUpdateTab();
    void createExtensionsTab();
    void createNetworkTab();
    void createProxyTab();
    void createInterfaceTab();
    
    // UI Components
    QTabWidget *m_tabWidget;
    
    // General Tab
    QWidget *m_generalTab;
    QComboBox *m_languageCombo;
    QLineEdit *m_defaultVMFolderEdit;
    QPushButton *m_browseVMFolderButton;
    QLineEdit *m_snapshotFolderEdit;
    QPushButton *m_browseSnapshotButton;
    QCheckBox *m_autoUpdateCheckBox;
    QComboBox *m_updateFrequencyCombo;
    
    // Input Tab
    QWidget *m_inputTab;
    QTreeWidget *m_shortcutsTree;
    QComboBox *m_hostKeyCombo;
    
    // Update Tab
    QWidget *m_updateTab;
    QCheckBox *m_checkUpdatesCheckBox;
    QComboBox *m_updateChannelCombo;
    QComboBox *m_checkFrequencyCombo;
    
    // Extensions Tab
    QWidget *m_extensionsTab;
    QListWidget *m_extensionsList;
    QPushButton *m_installExtensionButton;
    QPushButton *m_removeExtensionButton;
    
    // Network Tab
    QWidget *m_networkTab;
    QTabWidget *m_networkSubTabs;
    QListWidget *m_natNetworksList;
    QListWidget *m_hostOnlyNetworksList;
    QPushButton *m_addNATNetworkButton;
    QPushButton *m_removeNATNetworkButton;
    QPushButton *m_editNATNetworkButton;
    QPushButton *m_addHostOnlyButton;
    QPushButton *m_removeHostOnlyButton;
    QPushButton *m_editHostOnlyButton;
    
    // Proxy Tab
    QWidget *m_proxyTab;
    QCheckBox *m_enableProxyCheckBox;
    QComboBox *m_proxyTypeCombo;
    QLineEdit *m_proxyHostEdit;
    QSpinBox *m_proxyPortSpin;
    QLineEdit *m_proxyUserEdit;
    QLineEdit *m_proxyPasswordEdit;
    
    // Interface Tab
    QWidget *m_interfaceTab;
    QCheckBox *m_showToolbarCheckBox;
    QCheckBox *m_showStatusbarCheckBox;
    QComboBox *m_themeCombo;
    QComboBox *m_iconSizeCombo;
    
    // Buttons
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_applyButton;
    
    KVMManager *m_kvmManager;
};

#endif // PREFERENCESDIALOG_H