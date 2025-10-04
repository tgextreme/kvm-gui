#include "KVMManager.h"
#include "VirtualMachine.h"
#include "VMXmlManager.h"
#include "QemuManager.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include <QTimer>
#include <QUuid>
#include <QRegularExpression>

KVMManager::KVMManager(QObject *parent)
    : QObject(parent)
    , m_stateCheckTimer(new QTimer(this))
    , m_defaultVMPath("")
    , m_kvmAvailable(false)
    , m_xmlManager(new VMXmlManager(this))
    , m_qemuManager(new QemuManager(this))
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
    
    // Connect XML manager signals
    connect(m_xmlManager, &VMXmlManager::vmListChanged, this, &KVMManager::vmListChanged);
    connect(m_xmlManager, &VMXmlManager::errorOccurred, this, [this](const QString &error) {
        qWarning() << "XML Manager Error:" << error;
    });
    
    // Connect QEMU manager signals
    connect(m_qemuManager, &QemuManager::errorOccurred, this, &KVMManager::errorOccurred);
    connect(m_qemuManager, &QemuManager::processStarted, this, [this](const QString &vmName) {
        emit vmStateChanged(vmName, "running");
    });
    connect(m_qemuManager, &QemuManager::processFinished, this, [this](const QString &vmName, int exitCode) {
        emit vmStateChanged(vmName, "shut off");
        if (exitCode != 0) {
            emit errorOccurred(tr("La VM '%1' terminó con código de error %2").arg(vmName).arg(exitCode));
        }
    });
    
    // Always load VMs from XML files, regardless of libvirt status
    loadVirtualMachines();
    
    if (m_libvirtRunning) {
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
    
    // Load VMs from XML files
    QStringList vmNames = m_xmlManager->getAvailableVMs();
    qDebug() << "KVMManager: Cargando VMs desde XML:" << vmNames;
    
    for (const QString &vmName : vmNames) {
        VirtualMachine *vm = m_xmlManager->loadVM(vmName);
        if (vm) {
            m_virtualMachines.append(vm);
            qDebug() << "KVMManager: VM cargada:" << vmName;
        } else {
            qDebug() << "KVMManager: Error cargando VM:" << vmName;
        }
    }
    
    
    emit vmListChanged();
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
                                    int memoryMB, int /*diskSizeGB*/)
{
    // Check if VM already exists
    if (m_xmlManager->vmExists(name)) {
        emit errorOccurred(tr("Ya existe una máquina virtual con el nombre '%1'").arg(name));
        return false;
    }
    
    // Create new VM object
    VirtualMachine *vm = new VirtualMachine(name, this);
    vm->setOSType(osType);
    vm->setMemoryMB(memoryMB);
    vm->setState("shut off");
    
    // Generate UUID
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    vm->setUUID(uuid);
    
    // Create VM directory and disk if needed
    QString vmDir = QDir::homePath() + "/.VM/" + name;
    QDir().mkpath(vmDir);
    
    // Create disk only if diskSizeGB > 0 (from wizard)
    QString diskPath = vmDir + "/" + name + ".qcow2";
    if (!m_qemuManager->createDisk(diskPath, "qcow2", 25, false)) {
        delete vm;
        emit errorOccurred(tr("No se pudo crear el disco virtual para '%1'").arg(name));
        return false;
    }
    
    vm->addHardDisk(diskPath);
    
    // Save VM to XML
    if (m_xmlManager->saveVM(vm)) {
        m_virtualMachines.append(vm);
        emit vmCreated(name);
        
        qDebug() << "VM creada:" << name;
        return true;
    } else {
        delete vm;
        return false;
    }
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
    
    // Delete XML file
    if (m_xmlManager->deleteVM(name)) {
        // Remove from memory list
        m_virtualMachines.removeAll(vm);
        delete vm;
        
        emit vmDeleted(name);
        qDebug() << "VM eliminada:" << name;
        return true;
    } else {
        return false;
    }
}

bool KVMManager::startVM(const QString &name)
{
    VirtualMachine *vm = getVirtualMachine(name);
    if (!vm) {
        emit errorOccurred(tr("Máquina virtual '%1' no encontrada").arg(name));
        return false;
    }
    
    // Usar QemuManager para iniciar la VM
    if (m_qemuManager->startVM(vm)) {
        emit vmStateChanged(name, "running");
        return true;
    } else {
        // El error ya se emitirá desde QemuManager
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

bool KVMManager::saveVMConfiguration(VirtualMachine *vm)
{
    if (!vm) {
        emit errorOccurred(tr("Máquina virtual nula"));
        return false;
    }
    
    return m_xmlManager->saveVM(vm);
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