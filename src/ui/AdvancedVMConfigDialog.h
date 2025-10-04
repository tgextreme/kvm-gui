#ifndef ADVANCEDVMCONFIGDIALOG_H
#define ADVANCEDVMCONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QRadioButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QSplitter>
#include <QHeaderView>

class VirtualMachine;
class KVMManager;
class QemuManager;

class AdvancedVMConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedVMConfigDialog(VirtualMachine *vm, KVMManager *kvmManager, QWidget *parent = nullptr);
    
public slots:
    void accept() override;

private slots:
    // General
    void onNameChanged();
    void onDescriptionChanged();
    
    // System
    void onMemorySliderChanged(int value);
    void onMemorySpinChanged(int value);
    void onCPUCountChanged(int value);
    void onBootOrderChanged();
    
    // Storage
    void addHardDisk();
    void removeHardDisk();
    void modifyHardDisk();
    void addOpticalDrive();
    void removeOpticalDrive();
    void selectISO();
    void onStorageSelectionChanged();
    
    // Network
    void addNetworkAdapter();
    void removeNetworkAdapter();
    void onNetworkAdapterChanged();
    void generateMACAddress();
    
    // Audio
    void onAudioEnabledChanged();
    void onAudioControllerChanged();
    
    // Shared Folders
    void addSharedFolder();
    void removeSharedFolder();
    void modifySharedFolder();
    void selectFolderPath();

private:
    void setupUI();
    void setupGeneralTab();
    void setupSystemTab();
    void setupStorageTab();
    void setupNetworkTab();
    void setupAudioTab();
    void setupSharedFoldersTab();
    
    void loadVMSettings();
    void saveVMSettings();
    bool validateSettings();
    
    void updateMemoryInfo();
    void updateStorageList();
    void updateNetworkList();
    void updateSharedFoldersList();
    
    VirtualMachine *m_vm;
    KVMManager *m_kvmManager;
    QemuManager *m_qemuManager;
    
    // Main UI
    QTabWidget *m_tabWidget;
    
    // General Tab
    QWidget *m_generalTab;
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QComboBox *m_osTypeCombo;
    QComboBox *m_osVersionCombo;
    
    // System Tab  
    QWidget *m_systemTab;
    QSlider *m_memorySlider;
    QSpinBox *m_memorySpin;
    QLabel *m_memoryInfoLabel;
    QProgressBar *m_hostMemoryBar;
    QSpinBox *m_cpuCountSpin;
    QComboBox *m_chipsetCombo;
    QCheckBox *m_enablePAECheck;
    QCheckBox *m_enableVTxCheck;
    QListWidget *m_bootOrderList;
    QPushButton *m_bootUpButton;
    QPushButton *m_bootDownButton;
    
    // Storage Tab
    QWidget *m_storageTab;
    QSplitter *m_storageSplitter;
    QTreeWidget *m_storageTree;
    QWidget *m_storageDetailsPanel;
    QLabel *m_storageDetailsLabel;
    QPushButton *m_addHardDiskButton;
    QPushButton *m_addOpticalButton;
    QPushButton *m_removeStorageButton;
    QPushButton *m_modifyStorageButton;
    
    // Network Tab
    QWidget *m_networkTab;
    QTableWidget *m_networkTable;
    QPushButton *m_addNetworkButton;
    QPushButton *m_removeNetworkButton;
    QGroupBox *m_networkDetailsGroup;
    QComboBox *m_networkModeCombo;
    QComboBox *m_networkAdapterCombo;
    QLineEdit *m_macAddressEdit;
    QPushButton *m_generateMACButton;
    QCheckBox *m_cableConnectedCheck;
    
    // Audio Tab
    QWidget *m_audioTab;
    QCheckBox *m_enableAudioCheck;
    QComboBox *m_audioControllerCombo;
    QComboBox *m_audioDriverCombo;
    
    // Shared Folders Tab
    QWidget *m_sharedFoldersTab;
    QTableWidget *m_sharedFoldersTable;
    QPushButton *m_addFolderButton;
    QPushButton *m_removeFolderButton;
    QPushButton *m_modifyFolderButton;
};

// Dialog para añadir disco duro
class AddHardDiskDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddHardDiskDialog(QemuManager *qemuManager, QWidget *parent = nullptr);
    
    QString getDiskPath() const;
    QString getDiskFormat() const;
    qint64 getDiskSize() const;
    bool isDynamicAllocation() const;
    bool createNewDisk() const;
    
private slots:
    void onCreateNewToggled(bool checked);
    void onUseExistingToggled(bool checked);
    void onBrowseNewPath();
    void onBrowseExistingPath();
    void onFormatChanged();
    void onSizeChanged();
    
private:
    void setupUI();
    bool validateInput();
    void updateEstimatedSize();
    
    QemuManager *m_qemuManager;
    
    QRadioButton *m_createNewRadio;
    QRadioButton *m_useExistingRadio;
    
    // Create new
    QGroupBox *m_createNewGroup;
    QLineEdit *m_newPathEdit;
    QPushButton *m_browseNewButton;
    QComboBox *m_formatCombo;
    QSpinBox *m_sizeSpin;
    QComboBox *m_sizeUnitCombo;
    QComboBox *m_allocationCombo;
    QLabel *m_estimatedSizeLabel;
    
    // Use existing
    QGroupBox *m_useExistingGroup;
    QLineEdit *m_existingPathEdit;
    QPushButton *m_browseExistingButton;
};

// Dialog para añadir unidad óptica
class AddOpticalDriveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddOpticalDriveDialog(QWidget *parent = nullptr);
    
    QString getISOPath() const;
    bool isConnected() const;
    
    // Methods to configure dialog with existing values
    void setISOPath(const QString &path);
    void setConnected(bool connected);
    
private slots:
    void onBrowseISO();
    void onEmptyToggled(bool checked);
    void onISOToggled(bool checked);
    
private:
    void setupUI();
    
    QRadioButton *m_emptyRadio;
    QRadioButton *m_isoRadio;
    
    QGroupBox *m_isoGroup;
    QLineEdit *m_isoPathEdit;
    QPushButton *m_browseISOButton;
    QCheckBox *m_connectedCheck;
};

// Dialog para añadir carpeta compartida
class AddSharedFolderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddSharedFolderDialog(QWidget *parent = nullptr);
    
    QString getFolderName() const;
    QString getFolderPath() const;
    bool isReadOnly() const;
    bool isAutoMount() const;
    
private slots:
    void onBrowsePath();
    void onNameChanged();
    
private:
    void setupUI();
    bool validateInput();
    
    QLineEdit *m_nameEdit;
    QLineEdit *m_pathEdit;
    QPushButton *m_browseButton;
    QCheckBox *m_readOnlyCheck;
    QCheckBox *m_autoMountCheck;
};

#endif // ADVANCEDVMCONFIGDIALOG_H