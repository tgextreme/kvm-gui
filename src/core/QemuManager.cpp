#include "QemuManager.h"
#include "VirtualMachine.h"

#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QRegularExpression>

QemuManager::QemuManager(QObject *parent)
    : QObject(parent)
{
    m_qemuPath = findQemuExecutable();
}

bool QemuManager::createDisk(const QString &path, const QString &format, qint64 sizeGB, bool preallocated)
{
    if (!isQemuAvailable()) {
        emit errorOccurred(tr("QEMU no está disponible en el sistema"));
        return false;
    }
    
    if (!validateDiskPath(path)) {
        emit errorOccurred(tr("Ruta de disco inválida: %1").arg(path));
        return false;
    }
    
    // Crear directorio si no existe
    QFileInfo info(path);
    if (!createDirectoryIfNotExists(info.absolutePath())) {
        emit errorOccurred(tr("No se pudo crear el directorio: %1").arg(info.absolutePath()));
        return false;
    }
    
    QStringList arguments;
    arguments << "create";
    arguments << "-f" << format.toLower();
    
    // Opciones específicas del formato
    if (format.toLower() == "qcow2") {
        arguments << "-o" << "cluster_size=65536";
        if (preallocated) {
            arguments << "-o" << "preallocation=full";
        }
    } else if (format.toLower() == "raw" || format.toLower() == "img") {
        if (preallocated) {
            arguments << "-o" << "preallocation=full";
        }
    }
    
    arguments << path;
    arguments << formatSizeString(sizeGB);
    
    QProcess process;
    process.setProgram("qemu-img");
    process.setArguments(arguments);
    
    qDebug() << "Ejecutando:" << "qemu-img" << arguments.join(" ");
    
    process.start();
    if (!process.waitForStarted(5000)) {
        emit errorOccurred(tr("No se pudo iniciar qemu-img"));
        return false;
    }
    
    if (!process.waitForFinished(30000)) {
        emit errorOccurred(tr("Timeout creando disco virtual"));
        process.kill();
        return false;
    }
    
    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error creando disco: %1").arg(error));
        return false;
    }
    
    qDebug() << "Disco creado exitosamente:" << path;
    return true;
}

bool QemuManager::resizeDisk(const QString &path, qint64 newSizeGB)
{
    if (!QFileInfo::exists(path)) {
        emit errorOccurred(tr("El archivo de disco no existe: %1").arg(path));
        return false;
    }
    
    QStringList arguments;
    arguments << "resize";
    arguments << path;
    arguments << formatSizeString(newSizeGB);
    
    QProcess process;
    process.start("qemu-img", arguments);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error redimensionando disco: %1").arg(error));
        return false;
    }
    
    return true;
}

bool QemuManager::convertDisk(const QString &sourcePath, const QString &destPath, const QString &destFormat)
{
    QStringList arguments;
    arguments << "convert";
    arguments << "-f" << getDiskFormat(sourcePath);
    arguments << "-O" << destFormat.toLower();
    arguments << sourcePath;
    arguments << destPath;
    
    QProcess process;
    process.start("qemu-img", arguments);
    process.waitForFinished(60000); // 1 minuto timeout para conversiones
    
    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        emit errorOccurred(tr("Error convirtiendo disco: %1").arg(error));
        return false;
    }
    
    return true;
}

bool QemuManager::copyDisk(const QString &sourcePath, const QString &destPath)
{
    QString sourceFormat = getDiskFormat(sourcePath);
    return convertDisk(sourcePath, destPath, sourceFormat);
}

qint64 QemuManager::getDiskSize(const QString &path)
{
    QStringList arguments;
    arguments << "info" << path;
    
    QProcess process;
    process.start("qemu-img", arguments);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        return 0;
    }
    
    QString output = process.readAllStandardOutput();
    // Parsear output para obtener el tamaño virtual
    // Formato típico: "virtual size: 25G (26843545600 bytes)"
    QRegularExpression sizeRegex("virtual size:\\s+\\d+\\w+\\s+\\((\\d+)\\s+bytes\\)");
    QRegularExpressionMatch match = sizeRegex.match(output);
    if (match.hasMatch()) {
        return match.captured(1).toLongLong();
    }
    
    return 0;
}

QString QemuManager::getDiskFormat(const QString &path)
{
    QStringList arguments;
    arguments << "info" << path;
    
    QProcess process;
    process.start("qemu-img", arguments);
    process.waitForFinished();
    
    if (process.exitCode() != 0) {
        // Fallback: usar extensión del archivo
        QFileInfo info(path);
        QString suffix = info.suffix().toLower();
        if (suffix == "qcow2") return "qcow2";
        if (suffix == "vdi") return "vdi";
        if (suffix == "vmdk") return "vmdk";
        if (suffix == "img") return "raw";
        return "raw";
    }
    
    QString output = process.readAllStandardOutput();
    // Parsear output para obtener el formato
    // Formato típico: "file format: qcow2"
    QRegularExpression formatRegex("file format:\\s+(\\w+)");
    QRegularExpressionMatch match = formatRegex.match(output);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    
    return "raw";
}

bool QemuManager::startVM(VirtualMachine *vm)
{
    if (!vm) {
        emit errorOccurred(tr("Máquina virtual inválida"));
        return false;
    }
    
    QString vmName = vm->getName();
    
    // Verificar si ya está ejecutándose
    if (m_runningVMs.contains(vmName)) {
        emit errorOccurred(tr("La máquina virtual '%1' ya está ejecutándose").arg(vmName));
        return false;
    }
    
    if (!isQemuAvailable()) {
        emit errorOccurred(tr("QEMU no está disponible en el sistema"));
        return false;
    }
    
    QStringList arguments = buildQemuCommand(vm);
    
    QProcess *process = new QProcess(this);
    process->setProgram(m_qemuPath);
    process->setArguments(arguments);
    
    // Conectar señales
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &QemuManager::onProcessFinished);
    connect(process, &QProcess::errorOccurred,
            this, &QemuManager::onProcessError);
    connect(process, &QProcess::readyReadStandardOutput,
            this, &QemuManager::onProcessOutput);
    
    qDebug() << "Iniciando VM:" << vmName;
    qDebug() << "Comando:" << m_qemuPath << arguments.join(" ");
    
    process->start();
    
    if (!process->waitForStarted(5000)) {
        emit errorOccurred(tr("No se pudo iniciar QEMU para la VM '%1'").arg(vmName));
        delete process;
        return false;
    }
    
    m_runningVMs[vmName] = process;
    vm->setState("running");
    emit processStarted(vmName);
    
    return true;
}

bool QemuManager::stopVM(const QString &vmName)
{
    if (!m_runningVMs.contains(vmName)) {
        emit errorOccurred(tr("La máquina virtual '%1' no está ejecutándose").arg(vmName));
        return false;
    }
    
    QProcess *process = m_runningVMs[vmName];
    process->terminate();
    
    if (!process->waitForFinished(10000)) {
        process->kill();
        process->waitForFinished(3000);
    }
    
    return true;
}

bool QemuManager::pauseVM(const QString &vmName)
{
    // TODO: Implementar pausa via monitor de QEMU
    emit errorOccurred(tr("Funcionalidad de pausa no implementada aún"));
    return false;
}

bool QemuManager::resumeVM(const QString &vmName)
{
    // TODO: Implementar reanudación via monitor de QEMU
    emit errorOccurred(tr("Funcionalidad de reanudación no implementada aún"));
    return false;
}

bool QemuManager::isQemuAvailable()
{
    return !m_qemuPath.isEmpty();
}

QString QemuManager::getQemuVersion()
{
    QProcess process;
    process.start(m_qemuPath, QStringList() << "--version");
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        return process.readAllStandardOutput().trimmed();
    }
    
    return tr("No disponible");
}

QStringList QemuManager::getSupportedFormats()
{
    QStringList formats;
    
    QProcess process;
    process.start("qemu-img", QStringList() << "--help");
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        // Parsear formatos soportados desde la ayuda
        formats << "qcow2" << "raw" << "vdi" << "vmdk" << "vhd";
    } else {
        // Formatos por defecto
        formats << "qcow2" << "raw";
    }
    
    return formats;
}

void QemuManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (!process) return;
    
    // Encontrar la VM asociada
    QString vmName;
    for (auto it = m_runningVMs.begin(); it != m_runningVMs.end(); ++it) {
        if (it.value() == process) {
            vmName = it.key();
            break;
        }
    }
    
    if (!vmName.isEmpty()) {
        m_runningVMs.remove(vmName);
        emit processFinished(vmName, exitCode);
        
        if (exitStatus == QProcess::CrashExit || exitCode != 0) {
            QString errorMsg = tr("La máquina virtual '%1' terminó con código de error %2").arg(vmName).arg(exitCode);
            
            // Add stderr output for debugging
            QString stderr = process->readAllStandardError();
            if (!stderr.isEmpty()) {
                errorMsg += "\nDetalles: " + stderr;
                qDebug() << "QEMU stderr:" << stderr;
            }
            
            emit errorOccurred(errorMsg);
        }
    }
    
    process->deleteLater();
}

void QemuManager::onProcessError(QProcess::ProcessError error)
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    QString errorString;
    
    switch (error) {
        case QProcess::FailedToStart:
            errorString = tr("No se pudo iniciar QEMU");
            break;
        case QProcess::Crashed:
            errorString = tr("QEMU se cerró inesperadamente");
            break;
        case QProcess::Timedout:
            errorString = tr("Timeout ejecutando QEMU");
            break;
        default:
            errorString = tr("Error desconocido en QEMU");
    }
    
    // Add stderr output for better debugging
    if (process) {
        QString stderr = process->readAllStandardError();
        if (!stderr.isEmpty()) {
            errorString += "\nDetalles: " + stderr;
        }
    }
    
    qDebug() << "Error en QemuManager:" << errorString;
    emit errorOccurred(errorString);
}

void QemuManager::onProcessOutput()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (!process) return;
    
    QString vmName;
    for (auto it = m_runningVMs.begin(); it != m_runningVMs.end(); ++it) {
        if (it.value() == process) {
            vmName = it.key();
            break;
        }
    }
    
    if (!vmName.isEmpty()) {
        QString output = process->readAllStandardOutput();
        emit outputReceived(vmName, output);
    }
}

QStringList QemuManager::buildQemuCommand(VirtualMachine *vm)
{
    QStringList args;
    
    // Configuración básica - Try KVM first, fallback to TCG
    args << "-machine" << "pc-i440fx-2.12,accel=kvm:tcg";
    args << "-cpu" << "qemu64";
    
    // CPUs dinámicos
    int cpuCount = vm->getCPUCount();
    if (cpuCount < 1) cpuCount = 1;
    args << "-smp" << QString::number(cpuCount);
    
    // Memoria
    args << "-m" << QString::number(vm->getMemoryMB());
    
    // Display
    args << "-vga" << "std";
    args << "-display" << "gtk,show-cursor=on";
    
    // Audio
    args << "-audiodev" << "pa,id=audio0";
    args << "-device" << "AC97,audiodev=audio0";
    
    // Configurar discos duros
    QStringList hardDisks = vm->getHardDisks();
    for (int i = 0; i < hardDisks.size(); ++i) {
        const QString &diskPath = hardDisks[i];
        if (QFileInfo::exists(diskPath)) {
            args << "-drive" << QString("file=%1,format=qcow2,if=ide,index=%2,media=disk").arg(diskPath).arg(i);
        }
    }
    
    // Configurar CDROM/ISO
    QString cdromImage = vm->getCDROMImage();
    if (!cdromImage.isEmpty() && QFileInfo::exists(cdromImage)) {
        args << "-drive" << QString("file=%1,if=ide,index=2,media=cdrom,readonly=on").arg(cdromImage);
    } else {
        // Añadir unidad CDROM vacía
        args << "-drive" << "if=ide,index=2,media=cdrom,readonly=on";
    }
    
    // Orden de arranque
    QStringList bootOrder = vm->getBootOrder();
    if (!bootOrder.isEmpty()) {
        QString bootString;
        for (const QString &device : bootOrder) {
            if (device == "Disco Duro") {
                bootString += "c";  // hard disk
            } else if (device == "CD/DVD") {
                bootString += "d";  // cdrom
            } else if (device == "Red") {
                bootString += "n";  // network
            } else if (device == "Disquete") {
                bootString += "a";  // floppy
            }
        }
        if (!bootString.isEmpty()) {
            args << "-boot" << QString("order=%1").arg(bootString);
        }
    } else {
        // Orden por defecto: disco duro, luego CD
        args << "-boot" << "order=cd";
    }
    
    // Red (NAT por defecto)
    args << "-netdev" << "user,id=net0";
    args << "-device" << "e1000,netdev=net0";
    
    // Monitor (para control)
    args << "-monitor" << "stdio";
    
    // Nombre de la VM
    args << "-name" << vm->getName();
    
    // UUID
    if (!vm->getUUID().isEmpty()) {
        args << "-uuid" << vm->getUUID();
    }
    
    qDebug() << "Comando QEMU construido para" << vm->getName() << ":" << args.join(" ");
    
    return args;
}

QString QemuManager::findQemuExecutable()
{
    // Intentar diferentes nombres y ubicaciones de QEMU
    QStringList possibleNames = {
        "qemu-system-x86_64",
        "qemu-kvm",
        "/usr/bin/qemu-system-x86_64",
        "/usr/bin/qemu-kvm",
        "/usr/local/bin/qemu-system-x86_64"
    };
    
    for (const QString &name : possibleNames) {
        QProcess process;
        process.start(name, QStringList() << "--version");
        process.waitForFinished(3000);
        
        if (process.exitCode() == 0) {
            qDebug() << "QEMU encontrado:" << name;
            return name;
        }
    }
    
    qWarning() << "QEMU no encontrado en el sistema";
    return QString();
}

bool QemuManager::validateDiskPath(const QString &path)
{
    if (path.isEmpty()) return false;
    
    QFileInfo info(path);
    
    // Verificar que el directorio padre existe o se puede crear
    QDir parentDir = info.dir();
    if (!parentDir.exists()) {
        if (!parentDir.mkpath(".")) {
            return false;
        }
    }
    
    // Verificar extensión válida
    QStringList validExtensions = {"qcow2", "img", "vdi", "vmdk"};
    if (!validExtensions.contains(info.suffix().toLower())) {
        return false;
    }
    
    return true;
}

QString QemuManager::formatSizeString(qint64 sizeGB)
{
    return QString("%1G").arg(sizeGB);
}

bool QemuManager::createDirectoryIfNotExists(const QString &path)
{
    QDir dir;
    return dir.mkpath(path);
}