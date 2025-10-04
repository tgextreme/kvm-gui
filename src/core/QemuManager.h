#ifndef QEMUMANAGER_H
#define QEMUMANAGER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>

class VirtualMachine;

class QemuManager : public QObject
{
    Q_OBJECT

public:
    explicit QemuManager(QObject *parent = nullptr);
    
    // Disk management
    bool createDisk(const QString &path, const QString &format, qint64 sizeGB, bool preallocated = false);
    bool resizeDisk(const QString &path, qint64 newSizeGB);
    bool convertDisk(const QString &sourcePath, const QString &destPath, const QString &destFormat);
    bool copyDisk(const QString &sourcePath, const QString &destPath);
    qint64 getDiskSize(const QString &path);
    QString getDiskFormat(const QString &path);
    
    // VM execution  
    bool startVM(VirtualMachine *vm);
    bool stopVM(const QString &vmName);
    bool pauseVM(const QString &vmName);
    bool resumeVM(const QString &vmName);
    
    // System checks
    bool isQemuAvailable();
    QString getQemuVersion();
    QStringList getSupportedFormats();
    
signals:
    void processStarted(const QString &vmName);
    void processFinished(const QString &vmName, int exitCode);
    void errorOccurred(const QString &error);
    void outputReceived(const QString &vmName, const QString &output);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessOutput();

private:
    QStringList buildQemuCommand(VirtualMachine *vm);
    QString findQemuExecutable();
    bool validateDiskPath(const QString &path);
    
    QMap<QString, QProcess*> m_runningVMs;
    QString m_qemuPath;
    
    // Helper methods
    QString formatSizeString(qint64 sizeGB);
    bool createDirectoryIfNotExists(const QString &path);
};

#endif // QEMUMANAGER_H