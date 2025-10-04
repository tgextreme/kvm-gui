#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDateTime>

class VirtualMachine : public QObject
{
    Q_OBJECT

public:
    enum State {
        Unknown,
        ShutOff,
        Running,
        Paused,
        Saved,
        Starting,
        Stopping
    };
    Q_ENUM(State)

    explicit VirtualMachine(const QString &name, QObject *parent = nullptr);
    ~VirtualMachine();
    
    // Basic Properties
    QString getName() const { return m_name; }
    void setName(const QString &name);
    
    QString getUUID() const { return m_uuid; }
    void setUUID(const QString &uuid) { m_uuid = uuid; }
    
    QString getDescription() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }
    
    QString getOSType() const { return m_osType; }
    void setOSType(const QString &osType) { m_osType = osType; }
    
    // State Management
    QString getState() const { return m_state; }
    void setState(const QString &state);
    State getStateEnum() const;
    
    bool isRunning() const { return m_state == "running"; }
    bool isPaused() const { return m_state == "paused"; }
    bool isStopped() const { return m_state == "shut off" || m_state == "shutoff"; }
    
    // System Configuration
    int getMemoryMB() const { return m_memoryMB; }
    void setMemoryMB(int memoryMB) { m_memoryMB = memoryMB; }
    
    int getCPUCount() const { return m_cpuCount; }
    void setCPUCount(int cpuCount) { m_cpuCount = cpuCount; }
    
    // Storage Configuration
    QStringList getHardDisks() const { return m_hardDisks; }
    void setHardDisks(const QStringList &disks) { m_hardDisks = disks; }
    void addHardDisk(const QString &diskPath) { m_hardDisks.append(diskPath); }
    
    QString getCDROMImage() const { return m_cdromImage; }
    void setCDROMImage(const QString &image) { m_cdromImage = image; }
    
    // Network Configuration
    QStringList getNetworkAdapters() const { return m_networkAdapters; }
    void setNetworkAdapters(const QStringList &adapters) { m_networkAdapters = adapters; }
    void addNetworkAdapter(const QString &adapter) { m_networkAdapters.append(adapter); }
    
    // Audio Configuration
    QString getAudioController() const { return m_audioController; }
    void setAudioController(const QString &controller) { m_audioController = controller; }
    
    // USB Configuration
    QString getUSBController() const { return m_usbController; }
    void setUSBController(const QString &controller) { m_usbController = controller; }
    
    // Display Configuration
    int getVideoMemoryMB() const { return m_videoMemoryMB; }
    void setVideoMemoryMB(int memoryMB) { m_videoMemoryMB = memoryMB; }
    
    bool is3DAcceleration() const { return m_3dAcceleration; }
    void set3DAcceleration(bool enabled) { m_3dAcceleration = enabled; }
    
    int getMonitorCount() const { return m_monitorCount; }
    void setMonitorCount(int count) { m_monitorCount = count; }
    
    // Shared Folders
    QMap<QString, QString> getSharedFolders() const { return m_sharedFolders; }
    void setSharedFolders(const QMap<QString, QString> &folders) { m_sharedFolders = folders; }
    void addSharedFolder(const QString &name, const QString &path) { m_sharedFolders[name] = path; }
    void removeSharedFolder(const QString &name) { m_sharedFolders.remove(name); }
    
    // File Paths
    QString getConfigPath() const { return m_configPath; }
    void setConfigPath(const QString &path) { m_configPath = path; }
    
    QString getLogPath() const { return m_logPath; }
    void setLogPath(const QString &path) { m_logPath = path; }
    
    // Statistics
    QDateTime getCreatedDate() const { return m_createdDate; }
    void setCreatedDate(const QDateTime &date) { m_createdDate = date; }
    
    QDateTime getLastStarted() const { return m_lastStarted; }
    void setLastStarted(const QDateTime &date) { m_lastStarted = date; }
    
    // Boot Configuration
    QStringList getBootOrder() const { return m_bootOrder; }
    void setBootOrder(const QStringList &order) { m_bootOrder = order; }

signals:
    void stateChanged(const QString &oldState, const QString &newState);
    void configurationChanged();

private:
    QString m_name;
    QString m_uuid;
    QString m_description;
    QString m_osType;
    QString m_state;
    
    // System configuration
    int m_memoryMB;
    int m_cpuCount;
    
    // Storage configuration
    QStringList m_hardDisks;
    QString m_cdromImage;
    
    // Network configuration
    QStringList m_networkAdapters;
    
    // Audio configuration
    QString m_audioController;
    
    // USB configuration
    QString m_usbController;
    
    // Display configuration
    int m_videoMemoryMB;
    bool m_3dAcceleration;
    int m_monitorCount;
    
    // Shared folders
    QMap<QString, QString> m_sharedFolders;
    
    // File paths
    QString m_configPath;
    QString m_logPath;
    
    // Statistics
    QDateTime m_createdDate;
    QDateTime m_lastStarted;
    
    // Boot configuration
    QStringList m_bootOrder;
};

#endif // VIRTUALMACHINE_H