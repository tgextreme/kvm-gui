#include "KVMManager.h"
#include "VirtualMachine.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include <QTimer>
#include <QRegularExpression>

KVMManager::KVMManager(QObject *parent)
    : QObject(parent)
    , m_stateCheckTimer(new QTimer(this))
    , m_defaultVMPath("")
    , m_kvmAvailable(false)
    , m_libvirtRunning(false)
{
    // Set default VM path
    m_defaultVMPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) 
                      + "/.local/share/kvm";
    
    // Initialize KVM and check availability
    initializeKVM();
    
    // Set up timer to periodically check VM states
    m_stateCheckTimer->setInterval(5000); // Check every 5 seconds
    connect(m_stateCheckTimer, &QTimer::timeout, this, &KVMManager::checkVMStates);
    
    if (m_libvirtRunning) {
        loadVirtualMachines();
        m_stateCheckTimer->start();
    }
}

KVMManager::~KVMManager()
{
    qDeleteAll(m_virtualMachines);
}

void KVMManager::initializeKVM()
{
    // Check if KVM is available
    QProcess kvmCheck;
    kvmCheck.start("lsmod");
    kvmCheck.waitForFinished();
    
    QString output = kvmCheck.readAllStandardOutput();
    m_kvmAvailable = output.contains("kvm");
    
    if (!m_kvmAvailable) {
        emit errorOccurred(tr("KVM no está disponible en este sistema"));
        return;
    }
    
    // Check if libvirt is running
    QProcess libvirtCheck;
    libvirtCheck.start("virsh", QStringList() << "version");
    libvirtCheck.waitForFinished();
    
    m_libvirtRunning = (libvirtCheck.exitCode() == 0);
    
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está ejecutándose. Por favor inicie el servicio libvirtd"));
    }
}

void KVMManager::loadVirtualMachines()
{
    // Clear existing VMs
    qDeleteAll(m_virtualMachines);
    m_virtualMachines.clear();
    
    if (!m_libvirtRunning) {
        return;
    }
    
    // Get list of all VMs (defined and running)
    QStringList output;
    if (executeLibvirtCommand("virsh list --all", output)) {
        for (const QString &line : output) {
            // Parse virsh list output
            // Format: " Id    Name                           State"
            QRegularExpression re(R"(\s*(\d+|-)\s+(\S+)\s+(.+))");
            QRegularExpressionMatch match = re.match(line);
            
            if (match.hasMatch()) {
                QString name = match.captured(2);
                QString state = match.captured(3).trimmed();
                
                if (!name.isEmpty() && name != "Name") {
                    VirtualMachine *vm = new VirtualMachine(name, this);
                    vm->setState(state);
                    m_virtualMachines.append(vm);
                }
            }
        }
    }
}

bool KVMManager::executeLibvirtCommand(const QString &command, QStringList &output)
{
    QProcess process;
    process.start(command);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        qDebug() << "Libvirt command failed:" << command << "Error:" << error;
        return false;
    }
    
    QString result = process.readAllStandardOutput();
    output = result.split('\n', Qt::SkipEmptyParts);
    return true;
}

QStringList KVMManager::getVirtualMachines() const
{
    QStringList names;
    for (const VirtualMachine *vm : m_virtualMachines) {
        names.append(vm->getName());
    }
    return names;
}

VirtualMachine* KVMManager::getVirtualMachine(const QString &name) const
{
    for (VirtualMachine *vm : m_virtualMachines) {
        if (vm->getName() == name) {
            return vm;
        }
    }
    return nullptr;
}

bool KVMManager::createVirtualMachine(const QString &name, const QString &osType, 
                                    int memoryMB, int diskSizeGB)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    // TODO: Implement actual VM creation using virt-install or libvirt API
    // This is a placeholder implementation
    
    Q_UNUSED(name)
    Q_UNUSED(osType)
    Q_UNUSED(memoryMB)
    Q_UNUSED(diskSizeGB)
    
    emit errorOccurred(tr("Creación de VM no implementada aún"));
    return false;
}

bool KVMManager::deleteVirtualMachine(const QString &name)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    VirtualMachine *vm = getVirtualMachine(name);
    if (!vm) {
        emit errorOccurred(tr("Máquina virtual '%1' no encontrada").arg(name));
        return false;
    }
    
    // Stop VM if running
    if (vm->getState() == "running") {
        stopVM(name);
    }
    
    // TODO: Implement actual VM deletion
    // This would involve: virsh undefine <name> and removing disk files
    
    emit errorOccurred(tr("Eliminación de VM no implementada aún"));
    return false;
}

bool KVMManager::startVM(const QString &name)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    QProcess process;
    process.start("virsh", QStringList() << "start" << name);
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        emit vmStateChanged(name, "running");
        return true;
    } else {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error al iniciar VM '%1': %2").arg(name, error));
        return false;
    }
}

bool KVMManager::stopVM(const QString &name)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    QProcess process;
    process.start("virsh", QStringList() << "shutdown" << name);
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        emit vmStateChanged(name, "shut off");
        return true;
    } else {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error al detener VM '%1': %2").arg(name, error));
        return false;
    }
}

bool KVMManager::pauseVM(const QString &name)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    QProcess process;
    process.start("virsh", QStringList() << "suspend" << name);
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        emit vmStateChanged(name, "paused");
        return true;
    } else {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error al pausar VM '%1': %2").arg(name, error));
        return false;
    }
}

bool KVMManager::resumeVM(const QString &name)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    QProcess process;
    process.start("virsh", QStringList() << "resume" << name);
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        emit vmStateChanged(name, "running");
        return true;
    } else {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error al reanudar VM '%1': %2").arg(name, error));
        return false;
    }
}

bool KVMManager::resetVM(const QString &name)
{
    if (!m_libvirtRunning) {
        emit errorOccurred(tr("Libvirt no está disponible"));
        return false;
    }
    
    QProcess process;
    process.start("virsh", QStringList() << "reset" << name);
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        return true;
    } else {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error al resetear VM '%1': %2").arg(name, error));
        return false;
    }
}

QString KVMManager::getVMState(const QString &name) const
{
    VirtualMachine *vm = getVirtualMachine(name);
    if (vm) {
        return vm->getState();
    }
    return "unknown";
}

bool KVMManager::isVMRunning(const QString &name) const
{
    return getVMState(name) == "running";
}

bool KVMManager::isKVMAvailable() const
{
    return m_kvmAvailable;
}

bool KVMManager::isLibvirtRunning() const
{
    return m_libvirtRunning;
}

QString KVMManager::getKVMVersion() const
{
    QProcess process;
    process.start("qemu-system-x86_64", QStringList() << "--version");
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        QRegularExpression re(R"(version\s+([\d.]+))");
        QRegularExpressionMatch match = re.match(output);
        if (match.hasMatch()) {
            return match.captured(1);
        }
    }
    return tr("Desconocido");
}

QString KVMManager::getLibvirtVersion() const
{
    QProcess process;
    process.start("virsh", QStringList() << "version");
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        QRegularExpression re(R"(libvirt\s+([\d.]+))");
        QRegularExpressionMatch match = re.match(output);
        if (match.hasMatch()) {
            return match.captured(1);
        }
    }
    return tr("Desconocido");
}

QString KVMManager::getDefaultVMPath() const
{
    return m_defaultVMPath;
}

void KVMManager::setDefaultVMPath(const QString &path)
{
    m_defaultVMPath = path;
    
    // Create directory if it doesn't exist
    QDir dir;
    if (!dir.exists(path)) {
        dir.mkpath(path);
    }
}

void KVMManager::checkVMStates()
{
    if (!m_libvirtRunning) {
        return;
    }
    
    // Reload VM list to get current states
    loadVirtualMachines();
}

void KVMManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    
    // Handle any process completion if needed
}