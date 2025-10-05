#ifndef VMXMLMANAGER_H
#define VMXMLMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDateTime>

class VirtualMachine;

/**
 * @brief Administrador de archivos XML de máquinas virtuales
 * Maneja la persistencia de configuraciones VM en formato XML
 */
class VMXmlManager : public QObject
{
    Q_OBJECT

public:
    explicit VMXmlManager(QObject *parent = nullptr);
    
    // Configuración de carpeta
    void setVMFolder(const QString &folderPath);
    QString getVMFolder() const;
    
    // Operaciones con VMs
    bool saveVM(VirtualMachine *vm);
    VirtualMachine* loadVM(const QString &vmName);
    bool deleteVM(const QString &vmName);
    bool cloneVM(const QString &sourceName, const QString &cloneName);
    bool vmExists(const QString &vmName);
    
    // Lista de VMs disponibles
    QStringList getAvailableVMs();
    QStringList getVMFiles();
    
    // Utilidades
    bool createVMFolder();
    bool isVMFolderValid();
    QString getVMFilePath(const QString &vmName);
    
    // Metadatos
    QString getVMDescription(const QString &vmName);
    QDateTime getVMCreationDate(const QString &vmName);
    QDateTime getVMLastModified(const QString &vmName);

signals:
    void vmListChanged();
    void vmSaved(const QString &vmName);
    void vmDeleted(const QString &vmName);
    void errorOccurred(const QString &error);

private:
    QString m_vmFolderPath;
    
    // Métodos privados de XML
    QDomDocument createVMDocument(VirtualMachine *vm);
    bool parseVMDocument(const QDomDocument &doc, VirtualMachine *vm);
    
    // Elementos XML
    void addBasicInfoElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    void addSystemElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    void addStorageElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    void addNetworkElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    void addDisplayElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    void addAudioElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    void addSharedFoldersElement(QDomDocument &doc, QDomElement &root, VirtualMachine *vm);
    
    // Parseo XML
    void parseBasicInfo(const QDomElement &element, VirtualMachine *vm);
    void parseSystemInfo(const QDomElement &element, VirtualMachine *vm);
    void parseStorageInfo(const QDomElement &element, VirtualMachine *vm);
    void parseNetworkInfo(const QDomElement &element, VirtualMachine *vm);
    void parseDisplayInfo(const QDomElement &element, VirtualMachine *vm);
    void parseAudioInfo(const QDomElement &element, VirtualMachine *vm);
    void parseSharedFolders(const QDomElement &element, VirtualMachine *vm);
    
    QString sanitizeFileName(const QString &name);
};

#endif // VMXMLMANAGER_H