#ifndef VMLISTMODEL_H
#define VMLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class KVMManager;
class VirtualMachine;

class VMListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum VMRoles {
        NameRole = Qt::UserRole + 1,
        StateRole,
        OSTypeRole,
        MemoryRole,
        CPUCountRole,
        DescriptionRole
    };

    explicit VMListModel(QObject *parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // Custom methods
    void setKVMManager(KVMManager *manager);
    void refreshVMs();
    VirtualMachine* getVM(int index) const;
    VirtualMachine* getVM(const QString &name) const;

public slots:
    void onVMStateChanged(const QString &name, const QString &state);
    void onVMCreated(const QString &name);
    void onVMDeleted(const QString &name);

private:
    void connectToKVMManager();
    
    KVMManager *m_kvmManager;
    QStringList m_vmNames;
};

#endif // VMLISTMODEL_H