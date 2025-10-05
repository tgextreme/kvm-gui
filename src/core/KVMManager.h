#ifndef KVMMANAGER_H
#define KVMMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QTimer>

class VirtualMachine;
class VMXmlManager;
class QemuManager;

class KVMManager : public QObject
{
    Q_OBJECT

public:
    explicit KVMManager(QObject *parent = nullptr);
    ~KVMManager();
    
    // VM Management
    QStringList getVirtualMachines() const;
    VirtualMachine* getVirtualMachine(const QString &name) const;
    bool createVirtualMachine(const QString &name, const QString &osType, 
                             int memoryMB, int diskSizeGB);
    bool deleteVirtualMachine(const QString &name);
    
    // VM Control
    bool startVM(const QString &name);
    bool stopVM(const QString &name);
    bool pauseVM(const QString &name);
    bool resumeVM(const QString &name);
    bool resetVM(const QString &name);
    
    // VM Status
    QString getVMState(const QString &name) const;
    bool isVMRunning(const QString &name) const;
    
    // VM Configuration
    bool saveVMConfiguration(VirtualMachine *vm);
    
    // System Information
    bool isKVMAvailable() const;
    bool isLibvirtRunning() const;
    QString getKVMVersion() const;
    QString getLibvirtVersion() const;
    
    // Configuration
    QString getDefaultVMPath() const;
    void setDefaultVMPath(const QString &path);

signals:
    void vmListChanged();
    void vmStateChanged(const QString &name, const QString &state);
    void vmCreated(const QString &name);
    void vmDeleted(const QString &name);
    void errorOccurred(const QString &error);

private slots:
    void checkVMStates();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void initializeKVM();
    void loadVirtualMachines();
    bool executeLibvirtCommand(const QString &command, QStringList &output);
    VirtualMachine* parseVMInfo(const QString &vmXML);
    
    QList<VirtualMachine*> m_virtualMachines;
    QTimer *m_stateCheckTimer;
    QString m_defaultVMPath;
    bool m_kvmAvailable;
    VMXmlManager *m_xmlManager;
    QemuManager *m_qemuManager;
    bool m_libvirtRunning;
    bool m_loadingVMs;
};

#endif // KVMMANAGER_H