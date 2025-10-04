#ifndef DISKMANAGERDIALOG_H
#define DISKMANAGERDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QComboBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>

class KVMManager;

class DiskManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiskManagerDialog(KVMManager *kvmManager, QWidget *parent = nullptr);

private slots:
    void onCreateDisk();
    void onCopyDisk();
    void onModifyDisk();
    void onRemoveDisk();
    void onReleaseDisk();
    void onRefreshList();
    void onDiskSelectionChanged();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupHardDiskTab();
    void setupOpticalTab();
    void setupFloppyTab();
    void updateDiskList();
    void updateDiskInfo();
    
    KVMManager *m_kvmManager;
    
    // Main UI components
    QTabWidget *m_tabWidget;
    QMenuBar *m_menuBar;
    QToolBar *m_toolBar;
    QStatusBar *m_statusBar;
    
    // Hard Disk tab
    QWidget *m_hardDiskTab;
    QSplitter *m_hardDiskSplitter;
    QTreeWidget *m_hardDiskTree;
    QWidget *m_hardDiskInfoPanel;
    QLabel *m_hardDiskInfoLabel;
    
    // Optical Disk tab
    QWidget *m_opticalTab;
    QTreeWidget *m_opticalTree;
    QWidget *m_opticalInfoPanel;
    
    // Floppy Disk tab
    QWidget *m_floppyTab;
    QTreeWidget *m_floppyTree;
    QWidget *m_floppyInfoPanel;
    
    // Actions
    QAction *m_createAction;
    QAction *m_copyAction;
    QAction *m_modifyAction;
    QAction *m_removeAction;
    QAction *m_releaseAction;
    QAction *m_refreshAction;
    QAction *m_propertiesAction;
    
    // Current selection
    QString m_selectedDisk;
    int m_currentTab;
    
    enum TabType {
        HardDisksTab = 0,
        OpticalDisksTab = 1,
        FloppyDisksTab = 2
    };
};

// Dialog para crear nuevo disco duro
class CreateVirtualDiskDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CreateVirtualDiskDialog(QWidget *parent = nullptr);
    
    QString getDiskPath() const;
    QString getDiskFormat() const;
    qint64 getDiskSize() const;
    bool isDynamicAllocation() const;
    
private slots:
    void onBrowsePath();
    void onFormatChanged();
    void onSizeChanged();
    
private:
    void setupUI();
    bool validateInput();
    
    QLineEdit *m_pathEdit;
    QPushButton *m_browseButton;
    QComboBox *m_formatCombo;
    QComboBox *m_sizeUnitCombo;
    QLineEdit *m_sizeEdit;
    QComboBox *m_allocationCombo;
    QLabel *m_estimatedSizeLabel;
    
    void updateEstimatedSize();
};

// Dialog para propiedades de disco
class DiskPropertiesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DiskPropertiesDialog(const QString &diskPath, QWidget *parent = nullptr);
    
private:
    void setupUI();
    void loadDiskInfo();
    
    QString m_diskPath;
    QLabel *m_pathLabel;
    QLabel *m_formatLabel;
    QLabel *m_sizeLabel;
    QLabel *m_actualSizeLabel;
    QLabel *m_usageLabel;
    QLabel *m_attachmentsLabel;
};

#endif // DISKMANAGERDIALOG_H