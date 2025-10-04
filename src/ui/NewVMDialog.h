#ifndef NEWVMDIALOG_H
#define NEWVMDIALOG_H

#include <QDialog>
#include <QWizard>
#include <QWizardPage>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>

class KVMManager;

class NewVMDialog : public QWizard
{
    Q_OBJECT

public:
    explicit NewVMDialog(KVMManager *kvmManager, QWidget *parent = nullptr);
    
    void accept() override;

private:
    void setupPages();
    bool createVM();
    
    KVMManager *m_kvmManager;
};

// Welcome Page
class WelcomePage : public QWizardPage
{
    Q_OBJECT

public:
    WelcomePage(QWidget *parent = nullptr);

private:
    QLineEdit *m_nameEdit;
    QComboBox *m_osTypeCombo;
    QComboBox *m_osVersionCombo;
};

// Memory and CPU Page
class SystemPage : public QWizardPage
{
    Q_OBJECT

public:
    SystemPage(QWidget *parent = nullptr);

private slots:
    void updateMemoryLabel(int value);

private:
    QSpinBox *m_memorySpin;
    QLabel *m_memoryLabel;
    QSpinBox *m_cpuSpin;
    QCheckBox *m_enableVTX;
};

// Hard Disk Page
class HardDiskPage : public QWizardPage
{
    Q_OBJECT

public:
    HardDiskPage(QWidget *parent = nullptr);

private slots:
    void selectDiskLocation();
    void updateDiskSizeLabel(int value);

private:
    QCheckBox *m_createDiskCheckBox;
    QLineEdit *m_diskLocationEdit;
    QPushButton *m_browseDiskButton;
    QSpinBox *m_diskSizeSpin;
    QLabel *m_diskSizeLabel;
    QComboBox *m_diskTypeCombo;
};

// Summary Page
class SummaryPage : public QWizardPage
{
    Q_OBJECT

public:
    SummaryPage(QWidget *parent = nullptr);
    void initializePage() override;

private:
    QTextEdit *m_summaryText;
};

#endif // NEWVMDIALOG_H