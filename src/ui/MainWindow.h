#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QActionGroup>
#include <QLabel>

class VMListWidget;
class VMDetailsWidget;
class KVMManager;
class MediaManagerDialog;
class NetworkManagerDialog;
class SnapshotManagerDialog;
class AdvancedVMConfigDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newVM();
    void addVM();
    void removeVM();
    void configureVM();
    void startVM();
    void pauseVM();
    void stopVM();
    void showPreferences();
    void showAbout();
    void onVMSelectionChanged(const QString &vmName);
    
    // New managers
    void showDiskManager();
    void showMediaManager();
    void showNetworkManager();
    void showSnapshotManager();
    void importVM();
    void exportVM();
    void cloneVM();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();
    void createActions();
    void updateUIState();

    // Core components
    KVMManager *m_kvmManager;
    
    // UI components
    QSplitter *m_centralSplitter;
    VMListWidget *m_vmListWidget;
    VMDetailsWidget *m_vmDetailsWidget;
    
    // Actions
    QAction *m_newVMAction;
    QAction *m_addVMAction;
    QAction *m_removeVMAction;
    QAction *m_configureVMAction;
    QAction *m_startVMAction;
    QAction *m_pauseVMAction;
    QAction *m_stopVMAction;
    QAction *m_preferencesAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_aboutQtAction;
    
    // New manager actions
    QAction *m_diskManagerAction;
    QAction *m_mediaManagerAction;
    QAction *m_networkManagerAction;
    QAction *m_snapshotManagerAction;
    QAction *m_importVMAction;
    QAction *m_exportVMAction;
    QAction *m_cloneVMAction;
    
    // Menus
    QMenu *m_fileMenu;
    QMenu *m_machineMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    
    // Toolbars
    QToolBar *m_mainToolBar;
    
    // Status bar
    QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H