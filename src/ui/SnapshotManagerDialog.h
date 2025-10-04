#ifndef SNAPSHOTMANAGERDIALOG_H
#define SNAPSHOTMANAGERDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QSplitter>
#include <QProgressBar>
#include <QDateTimeEdit>
#include <QDateTime>

class VirtualMachine;

/**
 * @brief Administrador de instantáneas - Gestiona snapshots de VM
 * Equivalente a la ventana "Instantáneas" de VirtualBox
 */
class SnapshotManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SnapshotManagerDialog(VirtualMachine *vm, QWidget *parent = nullptr);
    
    void refreshSnapshots();

private slots:
    void takeSnapshot();
    void deleteSnapshot();
    void restoreSnapshot();
    void showSnapshotDetails();
    void cloneFromSnapshot();
    void onSnapshotSelectionChanged();

private:
    void setupUI();
    void updateSnapshotDetails();
    void populateSnapshotTree();
    void addSnapshotToTree(const QString &name, const QString &description, 
                          const QDateTime &timestamp, QTreeWidgetItem *parent = nullptr);
    
    VirtualMachine *m_vm;
    
    // Main UI
    QSplitter *m_splitter;
    
    // Snapshot Tree
    QTreeWidget *m_snapshotTree;
    QPushButton *m_takeSnapshotButton;
    QPushButton *m_deleteSnapshotButton;
    QPushButton *m_restoreSnapshotButton;
    QPushButton *m_showDetailsButton;
    QPushButton *m_cloneButton;
    
    // Snapshot Details
    QGroupBox *m_detailsGroup;
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QLabel *m_timestampLabel;
    QLabel *m_sizeLabel;
    QLabel *m_vmStateLabel;
    
    // Current snapshot indicator
    QLabel *m_currentSnapshotLabel;
};

#endif // SNAPSHOTMANAGERDIALOG_H