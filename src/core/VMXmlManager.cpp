#include "VMXmlManager.h"
#include "VirtualMachine.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QRegularExpression>
#include <QStringConverter>

VMXmlManager::VMXmlManager(QObject *parent)
    : QObject(parent)
{
    // Carpeta por defecto en el directorio home del usuario
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.VM";
    setVMFolder(defaultPath);
}

void VMXmlManager::setVMFolder(const QString &folderPath)
{
    m_vmFolderPath = folderPath;
    if (!isVMFolderValid()) {
        createVMFolder();
    }
}

QString VMXmlManager::getVMFolder() const
{
    return m_vmFolderPath;
}

bool VMXmlManager::createVMFolder()
{
    QDir dir;
    if (dir.mkpath(m_vmFolderPath)) {
        qDebug() << "Carpeta .VM creada en:" << m_vmFolderPath;
        return true;
    }
    
    QString error = tr("No se pudo crear la carpeta .VM en: %1").arg(m_vmFolderPath);
    emit errorOccurred(error);
    qWarning() << error;
    return false;
}

bool VMXmlManager::isVMFolderValid()
{
    QDir dir(m_vmFolderPath);
    return dir.exists();
}

QString VMXmlManager::getVMFilePath(const QString &vmName)
{
    QString sanitizedName = sanitizeFileName(vmName);
    return m_vmFolderPath + "/" + sanitizedName + ".xml";
}

QString VMXmlManager::sanitizeFileName(const QString &name)
{
    QString sanitized = name;
    // Reemplazar caracteres no válidos para nombres de archivo
    sanitized.replace(QRegularExpression("[<>:\"/\\\\|?*]"), "_");
    sanitized.replace(" ", "_");
    return sanitized;
}

bool VMXmlManager::saveVM(VirtualMachine *vm)
{
    if (!vm) {
        emit errorOccurred(tr("Máquina virtual nula"));
        return false;
    }
    
    if (!isVMFolderValid() && !createVMFolder()) {
        return false;
    }
    
    QString filePath = getVMFilePath(vm->getName());
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString error = tr("No se pudo abrir el archivo para escritura: %1").arg(filePath);
        emit errorOccurred(error);
        return false;
    }
    
    QDomDocument doc = createVMDocument(vm);
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << doc.toString(4);
    
    file.close();
    
    qDebug() << "VM guardada:" << vm->getName() << "en" << filePath;
    emit vmSaved(vm->getName());
    emit vmListChanged();
    
    return true;
}

VirtualMachine* VMXmlManager::loadVM(const QString &vmName)
{
    QString filePath = getVMFilePath(vmName);
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error = tr("No se pudo abrir el archivo: %1").arg(filePath);
        emit errorOccurred(error);
        return nullptr;
    }
    
    QDomDocument doc;
    
    QString content = file.readAll();
    file.close();
    
    qDebug() << "VMXmlManager: Contenido XML leído para" << vmName << ":" << content.left(200) << "...";
    
    QString errorMessage;
    int errorLine, errorColumn;
    bool parseResult = doc.setContent(content, &errorMessage, &errorLine, &errorColumn);
    if (!parseResult) {
        QString error = tr("Error XML en %1 (línea %2, columna %3): %4")
                       .arg(filePath).arg(errorLine).arg(errorColumn).arg(errorMessage);
        emit errorOccurred(error);
        qDebug() << "VMXmlManager: Error en parseo XML:" << error;
        return nullptr;
    }
    
    VirtualMachine *vm = new VirtualMachine(vmName, this);
    
    qDebug() << "VMXmlManager: Iniciando parseo para VM:" << vmName;
    if (parseVMDocument(doc, vm)) {
        qDebug() << "VMXmlManager: VM cargada exitosamente:" << vmName << "desde" << filePath;
        return vm;
    } else {
        qDebug() << "VMXmlManager: Error en parseVMDocument para VM:" << vmName;
        delete vm;
        return nullptr;
    }
}

bool VMXmlManager::deleteVM(const QString &vmName)
{
    QString filePath = getVMFilePath(vmName);
    QFile file(filePath);
    
    if (file.remove()) {
        qDebug() << "VM eliminada:" << vmName;
        emit vmDeleted(vmName);
        emit vmListChanged();
        return true;
    } else {
        QString error = tr("No se pudo eliminar el archivo: %1").arg(filePath);
        emit errorOccurred(error);
        return false;
    }
}

bool VMXmlManager::vmExists(const QString &vmName)
{
    QString filePath = getVMFilePath(vmName);
    return QFile::exists(filePath);
}

QStringList VMXmlManager::getAvailableVMs()
{
    QStringList vmList;
    
    if (!isVMFolderValid()) {
        qDebug() << "VMXmlManager: Carpeta VM no válida:" << m_vmFolderPath;
        return vmList;
    }
    
    QDir dir(m_vmFolderPath);
    QStringList xmlFiles = dir.entryList(QStringList("*.xml"), QDir::Files);
    qDebug() << "VMXmlManager: Archivos XML encontrados:" << xmlFiles;
    
    for (const QString &fileName : xmlFiles) {
        QFileInfo fileInfo(fileName);
        QString vmName = fileInfo.baseName().replace("_", " "); // Deshacer sanitización básica
        vmList.append(vmName);
        qDebug() << "VMXmlManager: VM detectada:" << vmName << "desde archivo:" << fileName;
    }
    
    return vmList;
}

QStringList VMXmlManager::getVMFiles()
{
    QStringList fileList;
    
    if (!isVMFolderValid()) {
        return fileList;
    }
    
    QDir dir(m_vmFolderPath);
    return dir.entryList(QStringList("*.xml"), QDir::Files);
}

QString VMXmlManager::getVMDescription(const QString &vmName)
{
    QString filePath = getVMFilePath(vmName);
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return QString();
    }
    
    file.close();
    
    QDomElement root = doc.documentElement();
    QDomElement basicInfo = root.firstChildElement("BasicInfo");
    if (!basicInfo.isNull()) {
        return basicInfo.firstChildElement("Description").text();
    }
    
    return QString();
}

QDateTime VMXmlManager::getVMCreationDate(const QString &vmName)
{
    QString filePath = getVMFilePath(vmName);
    QFileInfo fileInfo(filePath);
    return fileInfo.birthTime();
}

QDateTime VMXmlManager::getVMLastModified(const QString &vmName)
{
    QString filePath = getVMFilePath(vmName);
    QFileInfo fileInfo(filePath);
    return fileInfo.lastModified();
}

QDomDocument VMXmlManager::createVMDocument(VirtualMachine *vm)
{
    QDomDocument doc;
    QDomProcessingInstruction xmlDecl = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xmlDecl);
    
    QDomElement root = doc.createElement("VirtualMachine");
    root.setAttribute("version", "1.0");
    root.setAttribute("created", QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));
    root.setAttribute("modified", QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));
    doc.appendChild(root);
    
    addBasicInfoElement(doc, root, vm);
    addSystemElement(doc, root, vm);
    addStorageElement(doc, root, vm);
    addNetworkElement(doc, root, vm);
    addDisplayElement(doc, root, vm);
    addAudioElement(doc, root, vm);
    addSharedFoldersElement(doc, root, vm);
    
    return doc;
}

void VMXmlManager::addBasicInfoElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement basicInfo = doc.createElement("BasicInfo");
    
    QDomElement name = doc.createElement("Name");
    name.appendChild(doc.createTextNode(vm->getName()));
    basicInfo.appendChild(name);
    
    QDomElement uuid = doc.createElement("UUID");
    uuid.appendChild(doc.createTextNode(vm->getUUID()));
    basicInfo.appendChild(uuid);
    
    QDomElement description = doc.createElement("Description");
    description.appendChild(doc.createTextNode(vm->getDescription()));
    basicInfo.appendChild(description);
    
    QDomElement osType = doc.createElement("OSType");
    osType.appendChild(doc.createTextNode(vm->getOSType()));
    basicInfo.appendChild(osType);
    
    QDomElement state = doc.createElement("State");
    state.appendChild(doc.createTextNode(vm->getState()));
    basicInfo.appendChild(state);
    
    root.appendChild(basicInfo);
}

void VMXmlManager::addSystemElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement system = doc.createElement("System");
    
    QDomElement memory = doc.createElement("Memory");
    memory.setAttribute("mb", vm->getMemoryMB());
    system.appendChild(memory);
    
    QDomElement cpu = doc.createElement("CPU");
    cpu.setAttribute("count", vm->getCPUCount());
    system.appendChild(cpu);
    
    QDomElement bootOrder = doc.createElement("BootOrder");
    for (const QString &device : vm->getBootOrder()) {
        QDomElement bootDevice = doc.createElement("Device");
        bootDevice.appendChild(doc.createTextNode(device));
        bootOrder.appendChild(bootDevice);
    }
    system.appendChild(bootOrder);
    
    root.appendChild(system);
}

void VMXmlManager::addStorageElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement storage = doc.createElement("Storage");
    
    QDomElement hardDisks = doc.createElement("HardDisks");
    for (const QString &disk : vm->getHardDisks()) {
        QDomElement diskElement = doc.createElement("Disk");
        diskElement.setAttribute("path", disk);
        hardDisks.appendChild(diskElement);
    }
    storage.appendChild(hardDisks);
    
    QDomElement cdrom = doc.createElement("CDROM");
    cdrom.setAttribute("image", vm->getCDROMImage());
    storage.appendChild(cdrom);
    
    root.appendChild(storage);
}

void VMXmlManager::addNetworkElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement network = doc.createElement("Network");
    
    QDomElement adapters = doc.createElement("Adapters");
    for (const QString &adapter : vm->getNetworkAdapters()) {
        QDomElement adapterElement = doc.createElement("Adapter");
        adapterElement.setAttribute("type", adapter);
        adapters.appendChild(adapterElement);
    }
    network.appendChild(adapters);
    
    root.appendChild(network);
}

void VMXmlManager::addDisplayElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement display = doc.createElement("Display");
    
    QDomElement videoMemory = doc.createElement("VideoMemory");
    videoMemory.setAttribute("mb", vm->getVideoMemoryMB());
    display.appendChild(videoMemory);
    
    QDomElement monitors = doc.createElement("Monitors");
    monitors.setAttribute("count", vm->getMonitorCount());
    display.appendChild(monitors);
    
    QDomElement acceleration3D = doc.createElement("Acceleration3D");
    acceleration3D.setAttribute("enabled", vm->is3DAcceleration() ? "true" : "false");
    display.appendChild(acceleration3D);
    
    root.appendChild(display);
}

void VMXmlManager::addAudioElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement audio = doc.createElement("Audio");
    
    QDomElement controller = doc.createElement("Controller");
    controller.appendChild(doc.createTextNode(vm->getAudioController()));
    audio.appendChild(controller);
    
    root.appendChild(audio);
}

void VMXmlManager::addSharedFoldersElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm)
{
    QDomElement sharedFolders = doc.createElement("SharedFolders");
    
    QMap<QString, QString> folders = vm->getSharedFolders();
    QMapIterator<QString, QString> it(folders);
    while (it.hasNext()) {
        it.next();
        QDomElement folder = doc.createElement("Folder");
        folder.setAttribute("name", it.key());
        folder.setAttribute("path", it.value());
        sharedFolders.appendChild(folder);
    }
    
    root.appendChild(sharedFolders);
}

bool VMXmlManager::parseVMDocument(const QDomDocument &doc, VirtualMachine *vm)
{
    qDebug() << "VMXmlManager: parseVMDocument iniciado";
    
    QDomElement root = doc.documentElement();
    qDebug() << "VMXmlManager: Elemento raíz:" << root.tagName();
    
    if (root.tagName() != "VirtualMachine") {
        emit errorOccurred(tr("Documento XML no válido: elemento raíz incorrecto"));
        return false;
    }
    
    // Parsear cada sección
    QDomElement basicInfo = root.firstChildElement("BasicInfo");
    if (!basicInfo.isNull()) {
        parseBasicInfo(basicInfo, vm);
    }
    
    QDomElement system = root.firstChildElement("System");
    if (!system.isNull()) {
        parseSystemInfo(system, vm);
    }
    
    QDomElement storage = root.firstChildElement("Storage");
    if (!storage.isNull()) {
        parseStorageInfo(storage, vm);
    }
    
    QDomElement network = root.firstChildElement("Network");
    if (!network.isNull()) {
        parseNetworkInfo(network, vm);
    }
    
    QDomElement display = root.firstChildElement("Display");
    if (!display.isNull()) {
        parseDisplayInfo(display, vm);
    }
    
    QDomElement audio = root.firstChildElement("Audio");
    if (!audio.isNull()) {
        parseAudioInfo(audio, vm);
    }
    
    QDomElement sharedFolders = root.firstChildElement("SharedFolders");
    if (!sharedFolders.isNull()) {
        parseSharedFolders(sharedFolders, vm);
    }
    
    return true;
}

void VMXmlManager::parseBasicInfo(const QDomElement &element, VirtualMachine *vm)
{
    vm->setUUID(element.firstChildElement("UUID").text());
    vm->setDescription(element.firstChildElement("Description").text());
    vm->setOSType(element.firstChildElement("OSType").text());
    vm->setState(element.firstChildElement("State").text());
}

void VMXmlManager::parseSystemInfo(const QDomElement &element, VirtualMachine *vm)
{
    QDomElement memory = element.firstChildElement("Memory");
    if (!memory.isNull()) {
        vm->setMemoryMB(memory.attribute("mb").toInt());
    }
    
    QDomElement cpu = element.firstChildElement("CPU");
    if (!cpu.isNull()) {
        vm->setCPUCount(cpu.attribute("count").toInt());
    }
    
    QDomElement bootOrder = element.firstChildElement("BootOrder");
    if (!bootOrder.isNull()) {
        QStringList bootDevices;
        QDomElement device = bootOrder.firstChildElement("Device");
        while (!device.isNull()) {
            bootDevices.append(device.text());
            device = device.nextSiblingElement("Device");
        }
        vm->setBootOrder(bootDevices);
    }
}

void VMXmlManager::parseStorageInfo(const QDomElement &element, VirtualMachine *vm)
{
    QDomElement hardDisks = element.firstChildElement("HardDisks");
    if (!hardDisks.isNull()) {
        QStringList disks;
        QDomElement disk = hardDisks.firstChildElement("Disk");
        while (!disk.isNull()) {
            disks.append(disk.attribute("path"));
            disk = disk.nextSiblingElement("Disk");
        }
        vm->setHardDisks(disks);
    }
    
    QDomElement cdrom = element.firstChildElement("CDROM");
    if (!cdrom.isNull()) {
        vm->setCDROMImage(cdrom.attribute("image"));
    }
}

void VMXmlManager::parseNetworkInfo(const QDomElement &element, VirtualMachine *vm)
{
    QDomElement adapters = element.firstChildElement("Adapters");
    if (!adapters.isNull()) {
        QStringList adapterList;
        QDomElement adapter = adapters.firstChildElement("Adapter");
        while (!adapter.isNull()) {
            adapterList.append(adapter.attribute("type"));
            adapter = adapter.nextSiblingElement("Adapter");
        }
        vm->setNetworkAdapters(adapterList);
    }
}

void VMXmlManager::parseDisplayInfo(const QDomElement &element, VirtualMachine *vm)
{
    QDomElement videoMemory = element.firstChildElement("VideoMemory");
    if (!videoMemory.isNull()) {
        vm->setVideoMemoryMB(videoMemory.attribute("mb").toInt());
    }
    
    QDomElement monitors = element.firstChildElement("Monitors");
    if (!monitors.isNull()) {
        vm->setMonitorCount(monitors.attribute("count").toInt());
    }
    
    QDomElement acceleration3D = element.firstChildElement("Acceleration3D");
    if (!acceleration3D.isNull()) {
        vm->set3DAcceleration(acceleration3D.attribute("enabled") == "true");
    }
}

void VMXmlManager::parseAudioInfo(const QDomElement &element, VirtualMachine *vm)
{
    QDomElement controller = element.firstChildElement("Controller");
    if (!controller.isNull()) {
        vm->setAudioController(controller.text());
    }
}

void VMXmlManager::parseSharedFolders(const QDomElement &element, VirtualMachine *vm)
{
    QMap<QString, QString> folders;
    QDomElement folder = element.firstChildElement("Folder");
    while (!folder.isNull()) {
        QString name = folder.attribute("name");
        QString path = folder.attribute("path");
        folders.insert(name, path);
        folder = folder.nextSiblingElement("Folder");
    }
    vm->setSharedFolders(folders);
}