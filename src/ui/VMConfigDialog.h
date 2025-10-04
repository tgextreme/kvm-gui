#ifndef VMCONFIGDIALOG_H
#define VMCONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QProgressBar>

class VirtualMachine;
class KVMManager;

class VMConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VMConfigDialog(VirtualMachine *vm, QWidget *parent = nullptr);
    explicit VMConfigDialog(VirtualMachine *vm, KVMManager *kvmManager, QWidget *parent = nullptr);
    
    void loadVMSettings();
    void saveVMSettings();
    
public slots:
    void accept() override;

private slots:
    void onMemorySliderChanged(int value);
    void onCPUCountChanged(int value);
    void addHardDisk();
    void removeHardDisk();
    void addCDDVD();
    void selectBootISO();
    void addNetworkAdapter();
    void removeNetworkAdapter();
    void addSharedFolder();
    void removeSharedFolder();
    void selectSharedFolderPath();
    void enableEncryption(bool enabled);
    void testAudioDevice();

private:
    void setupUI();
    void createGeneralTab();
    void createSystemTab();
    void createDisplayTab();
    void createStorageTab();
    void createAudioTab();
    void createNetworkTab();
    void createSerialPortsTab();
    void createUSBTab();
    void createSharedFoldersTab();
    void createUserInterfaceTab();
    
    void updateMemoryLabel(int memoryMB);
    void updateCPULabel(int cpuCount);
    
    // UI Components
    QTabWidget *m_tabWidget;
    
    // General Tab
    QWidget *m_generalTab;
    QLineEdit *m_nameEdit;
    QComboBox *m_osTypeCombo;
    QComboBox *m_osVersionCombo;
    QTextEdit *m_descriptionEdit;
    QLineEdit *m_snapshotFolderEdit;
    QPushButton *m_browseSnapshotButton;
    QComboBox *m_clipboardCombo;
    QComboBox *m_dragDropCombo;
    QCheckBox *m_encryptionCheckBox;
    QLineEdit *m_encryptionPasswordEdit;
    
    // System Tab
    QWidget *m_systemTab;
    QSlider *m_memorySlider;
    QLabel *m_memoryLabel;
    QSpinBox *m_memorySpin;
    QListWidget *m_bootOrderList;
    QPushButton *m_moveBootUpButton;
    QPushButton *m_moveBootDownButton;
    QComboBox *m_chipsetCombo;
    QCheckBox *m_utcTimeCheckBox;
    QCheckBox *m_efiCheckBox;
    QSpinBox *m_cpuCountSpin;
    QLabel *m_cpuCountLabel;
    QSlider *m_cpuExecutionSlider;
    QCheckBox *m_paeCheckBox;
    QCheckBox *m_nestedVirtCheckBox;
    QComboBox *m_paravirtCombo;
    QCheckBox *m_vtxCheckBox;
    QCheckBox *m_nestedPagingCheckBox;
    
    // Display Tab
    QWidget *m_displayTab;
    QSlider *m_videoMemorySlider;
    QLabel *m_videoMemoryLabel;
    QSpinBox *m_monitorCountSpin;
    QSlider *m_scaleFactorSlider;
    QCheckBox *m_3dAccelCheckBox;
    QCheckBox *m_2dAccelCheckBox;
    QCheckBox *m_enableRemoteCheckBox;
    QSpinBox *m_remotePortSpin;
    QComboBox *m_authMethodCombo;
    QCheckBox *m_enableRecordingCheckBox;
    QComboBox *m_recordingModeCombo;
    QLineEdit *m_recordingFileEdit;
    QPushButton *m_browseRecordingButton;
    
    // Storage Tab
    QWidget *m_storageTab;
    QTreeWidget *m_storageTree;
    QPushButton *m_addControllerButton;
    QPushButton *m_removeControllerButton;
    QPushButton *m_addAttachmentButton;
    QPushButton *m_removeAttachmentButton;
    QGroupBox *m_attachmentDetails;
    QLineEdit *m_diskFileEdit;
    QPushButton *m_browseDiskButton;
    QComboBox *m_diskTypeCombo;
    QCheckBox *m_solidStateCheckBox;
    QCheckBox *m_hostCacheCheckBox;
    
    // Audio Tab
    QWidget *m_audioTab;
    QCheckBox *m_enableAudioCheckBox;
    QComboBox *m_hostAudioDriverCombo;
    QComboBox *m_audioControllerCombo;
    QCheckBox *m_enableInputCheckBox;
    QCheckBox *m_enableOutputCheckBox;
    QPushButton *m_testAudioButton;
    
    // Network Tab
    QWidget *m_networkTab;
    QTabWidget *m_networkAdapters;
    
    // Serial Ports Tab
    QWidget *m_serialPortsTab;
    QTabWidget *m_serialPorts;
    
    // USB Tab
    QWidget *m_usbTab;
    QComboBox *m_usbControllerCombo;
    QListWidget *m_usbFiltersList;
    QPushButton *m_addUSBFilterButton;
    QPushButton *m_removeUSBFilterButton;
    QPushButton *m_editUSBFilterButton;
    QListWidget *m_availableUSBList;
    
    // Shared Folders Tab
    QWidget *m_sharedFoldersTab;
    QListWidget *m_sharedFoldersList;
    QPushButton *m_addFolderButton;
    QPushButton *m_removeFolderButton;
    QPushButton *m_editFolderButton;
    QLineEdit *m_folderNameEdit;
    QLineEdit *m_folderPathEdit;
    QPushButton *m_browseFolderButton;
    QCheckBox *m_folderReadOnlyCheckBox;
    QCheckBox *m_folderAutoMountCheckBox;
    QCheckBox *m_folderPermanentCheckBox;
    
    // User Interface Tab
    QWidget *m_userInterfaceTab;
    QCheckBox *m_showMenuBarCheckBox;
    QCheckBox *m_showStatusBarCheckBox;
    QCheckBox *m_showMiniToolbarCheckBox;
    QComboBox *m_miniToolbarPositionCombo;
    
    // Buttons
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_applyButton;
    
    VirtualMachine *m_virtualMachine;
    KVMManager *m_kvmManager;
};

#endif // VMCONFIGDIALOG_H