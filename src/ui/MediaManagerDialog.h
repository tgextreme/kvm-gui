#ifndef MEDIAMANAGERDIALOG_H
#define MEDIAMANAGERDIALOG_H

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
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QTextEdit>

class KVMManager;

/**
 * @brief Administrador de medios virtuales (discos duros, CD/DVD, disquetes)
 * Equivalente al "Administrador de medios virtuales" de VirtualBox
 */
class MediaManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MediaManagerDialog(KVMManager *kvmManager, QWidget *parent = nullptr);
    
    void refreshMediaList();

private slots:
    // Hard Disks Tab
    void addHardDisk();
    void removeHardDisk();
    void releaseHardDisk();
    void refreshHardDisks();
    void copyHardDisk();
    void moveHardDisk();
    void resizeHardDisk();
    void onHardDiskSelectionChanged();
    
    // Optical Discs Tab
    void addOpticalDisc();
    void removeOpticalDisc();
    void releaseOpticalDisc();
    void refreshOpticalDiscs();
    void onOpticalDiscSelectionChanged();
    
    // Floppy Disks Tab
    void addFloppyDisk();
    void removeFloppyDisk();
    void releaseFloppyDisk();
    void refreshFloppyDisks();
    void onFloppyDiskSelectionChanged();

private:
    void setupUI();
    void createHardDisksTab();
    void createOpticalDiscsTab();
    void createFloppyDisksTab();
    void updateHardDiskDetails();
    void updateOpticalDiscDetails();
    void updateFloppyDiskDetails();
    
    KVMManager *m_kvmManager;
    
    // Main UI
    QTabWidget *m_tabWidget;
    QSplitter *m_splitter;
    
    // Hard Disks Tab
    QWidget *m_hardDisksTab;
    QTreeWidget *m_hardDisksTree;
    QPushButton *m_addHardDiskButton;
    QPushButton *m_removeHardDiskButton;
    QPushButton *m_releaseHardDiskButton;
    QPushButton *m_refreshHardDisksButton;
    QPushButton *m_copyHardDiskButton;
    QPushButton *m_moveHardDiskButton;
    QPushButton *m_resizeHardDiskButton;
    
    // Hard Disk Details
    QGroupBox *m_hardDiskDetailsGroup;
    QLineEdit *m_hardDiskLocationEdit;
    QLineEdit *m_hardDiskSizeEdit;
    QLineEdit *m_hardDiskFormatEdit;
    QLineEdit *m_hardDiskVariantEdit;
    QTextEdit *m_hardDiskUsageEdit;
    
    // Optical Discs Tab
    QWidget *m_opticalDiscsTab;
    QTreeWidget *m_opticalDiscsTree;
    QPushButton *m_addOpticalDiscButton;
    QPushButton *m_removeOpticalDiscButton;
    QPushButton *m_releaseOpticalDiscButton;
    QPushButton *m_refreshOpticalDiscsButton;
    
    // Optical Disc Details
    QGroupBox *m_opticalDiscDetailsGroup;
    QLineEdit *m_opticalDiscLocationEdit;
    QLineEdit *m_opticalDiscSizeEdit;
    QTextEdit *m_opticalDiscUsageEdit;
    
    // Floppy Disks Tab
    QWidget *m_floppyDisksTab;
    QTreeWidget *m_floppyDisksTree;
    QPushButton *m_addFloppyDiskButton;
    QPushButton *m_removeFloppyDiskButton;
    QPushButton *m_releaseFloppyDiskButton;
    QPushButton *m_refreshFloppyDisksButton;
    
    // Floppy Disk Details
    QGroupBox *m_floppyDiskDetailsGroup;
    QLineEdit *m_floppyDiskLocationEdit;
    QLineEdit *m_floppyDiskSizeEdit;
    QTextEdit *m_floppyDiskUsageEdit;
};

#endif // MEDIAMANAGERDIALOG_H