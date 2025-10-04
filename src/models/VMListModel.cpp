#include "VMListModel.h"
#include "../core/KVMManager.h"
#include "../core/VirtualMachine.h"

VMListModel::VMListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_kvmManager(nullptr)
{
}

void VMListModel::setKVMManager(KVMManager *manager)
{
    if (m_kvmManager != manager) {
        m_kvmManager = manager;
        connectToKVMManager();
        refreshVMs();
    }
}

void VMListModel::connectToKVMManager()
{
    if (!m_kvmManager) {
        return;
    }
    
    connect(m_kvmManager, &KVMManager::vmStateChanged,
            this, &VMListModel::onVMStateChanged);
    connect(m_kvmManager, &KVMManager::vmCreated,
            this, &VMListModel::onVMCreated);
    connect(m_kvmManager, &KVMManager::vmDeleted,
            this, &VMListModel::onVMDeleted);
}

int VMListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_vmNames.count();
}

QVariant VMListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_vmNames.count()) {
        return QVariant();
    }
    
    const QString &vmName = m_vmNames.at(index.row());
    VirtualMachine *vm = getVM(vmName);
    
    if (!vm) {
        return QVariant();
    }
    
    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return vm->getName();
    case StateRole:
        return vm->getState();
    case OSTypeRole:
        return vm->getOSType();
    case MemoryRole:
        return vm->getMemoryMB();
    case CPUCountRole:
        return vm->getCPUCount();
    case DescriptionRole:
        return vm->getDescription();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> VMListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[StateRole] = "state";
    roles[OSTypeRole] = "osType";
    roles[MemoryRole] = "memory";
    roles[CPUCountRole] = "cpuCount";
    roles[DescriptionRole] = "description";
    return roles;
}

void VMListModel::refreshVMs()
{
    if (!m_kvmManager) {
        return;
    }
    
    beginResetModel();
    m_vmNames = m_kvmManager->getVirtualMachines();
    endResetModel();
}

VirtualMachine* VMListModel::getVM(int index) const
{
    if (index < 0 || index >= m_vmNames.count()) {
        return nullptr;
    }
    
    return getVM(m_vmNames.at(index));
}

VirtualMachine* VMListModel::getVM(const QString &name) const
{
    if (!m_kvmManager) {
        return nullptr;
    }
    
    return m_kvmManager->getVirtualMachine(name);
}

void VMListModel::onVMStateChanged(const QString &name, const QString &state)
{
    Q_UNUSED(state)
    
    int index = m_vmNames.indexOf(name);
    if (index >= 0) {
        QModelIndex modelIndex = this->index(index);
        emit dataChanged(modelIndex, modelIndex, {StateRole});
    }
}

void VMListModel::onVMCreated(const QString &name)
{
    if (!m_vmNames.contains(name)) {
        beginInsertRows(QModelIndex(), m_vmNames.count(), m_vmNames.count());
        m_vmNames.append(name);
        endInsertRows();
    }
}

void VMListModel::onVMDeleted(const QString &name)
{
    int index = m_vmNames.indexOf(name);
    if (index >= 0) {
        beginRemoveRows(QModelIndex(), index, index);
        m_vmNames.removeAt(index);
        endRemoveRows();
    }
}