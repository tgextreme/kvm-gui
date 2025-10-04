#ifndef VMCREATIONWIZARD_H
#define VMCREATIONWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFileDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QTabWidget>

class KVMManager;

// Página principal del wizard
class VMCreationWizard : public QWizard
{
    Q_OBJECT

public:
    explicit VMCreationWizard(KVMManager *kvmManager, QWidget *parent = nullptr);
    
    void accept() override;

private slots:
    void onCurrentIdChanged(int id);

private:
    void setupWizardPages();
    bool createVirtualMachine();
    
    KVMManager *m_kvmManager;
    
    // IDs de páginas
    enum {
        WelcomePageId,
        BasicConfigPageId,
        MemoryPageId,
        HardDiskPageId,
        NetworkPageId,
        SummaryPageId
    };
};

// Página de bienvenida
class VMWelcomePage : public QWizardPage
{
    Q_OBJECT

public:
    VMWelcomePage(QWidget *parent = nullptr);
    
protected:
    void initializePage() override;
};

// Página de configuración básica
class VMBasicConfigPage : public QWizardPage
{
    Q_OBJECT

public:
    VMBasicConfigPage(QWidget *parent = nullptr);
    
    bool validatePage() override;
    void initializePage() override;

private slots:
    void onOSTypeChanged();
    void onOSVersionChanged();

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_folderEdit;
    QPushButton *m_folderButton;
    QComboBox *m_osTypeCombo;
    QComboBox *m_osVersionCombo;
    QLabel *m_osIconLabel;
    QTextEdit *m_descriptionEdit;
    
    void updateOSVersions();
    void updateOSIcon();
};

// Página de configuración de memoria
class VMMemoryPage : public QWizardPage
{
    Q_OBJECT

public:
    VMMemoryPage(QWidget *parent = nullptr);
    
    void initializePage() override;
    bool validatePage() override;

private slots:
    void onMemorySliderChanged(int value);
    void onMemorySpinChanged(int value);

private:
    QSlider *m_memorySlider;
    QSpinBox *m_memorySpin;
    QLabel *m_memoryRecommendedLabel;
    QLabel *m_memoryWarningLabel;
    QProgressBar *m_hostMemoryBar;
    
    void updateMemoryInfo();
};

// Página de configuración de disco duro
class VMHardDiskPage : public QWizardPage
{
    Q_OBJECT

public:
    VMHardDiskPage(QWidget *parent = nullptr);
    
    void initializePage() override;
    bool validatePage() override;

private slots:
    void onDiskOptionChanged();
    void onDiskSizeChanged();
    void onDiskTypeChanged();
    void onBrowseExistingDisk();

private:
    QRadioButton *m_createNewDiskRadio;
    QRadioButton *m_useExistingDiskRadio;
    QRadioButton *m_noDiskRadio;
    
    // Configuración de nuevo disco
    QGroupBox *m_newDiskGroup;
    QLineEdit *m_diskLocationEdit;
    QPushButton *m_diskLocationButton;
    QSlider *m_diskSizeSlider;
    QSpinBox *m_diskSizeSpin;
    QComboBox *m_diskTypeCombo;
    QCheckBox *m_preallocateCheck;
    
    // Configuración de disco existente
    QGroupBox *m_existingDiskGroup;
    QLineEdit *m_existingDiskEdit;
    QPushButton *m_existingDiskButton;
    
    void updateDiskControls();
    void updateDiskSizeInfo();
    void updateDefaultDiskName();
};

// Página de configuración de red
class VMNetworkPage : public QWizardPage
{
    Q_OBJECT

public:
    VMNetworkPage(QWidget *parent = nullptr);
    
    void initializePage() override;

private slots:
    void onNetworkEnabledChanged();
    void onNetworkModeChanged();

private:
    QCheckBox *m_networkEnabledCheck;
    QComboBox *m_networkModeCombo;
    QComboBox *m_networkAdapterCombo;
    QLineEdit *m_macAddressEdit;
    QPushButton *m_generateMacButton;
    QCheckBox *m_cableConnectedCheck;
    
    void updateNetworkControls();
    void generateMacAddress();
};

// Página de resumen
class VMSummaryPage : public QWizardPage
{
    Q_OBJECT

public:
    VMSummaryPage(QWidget *parent = nullptr);
    
    void initializePage() override;

private:
    QTextEdit *m_summaryText;
    QCheckBox *m_startAfterCreationCheck;
    
    void updateSummary();
};

#endif // VMCREATIONWIZARD_H