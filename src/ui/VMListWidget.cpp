#include "VMListWidget.h"
#include "../models/VMListModel.h"
#include "../core/KVMManager.h"
#include "../core/VirtualMachine.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QApplication>
#include <QStyle>

VMListWidget::VMListWidget(KVMManager *kvmManager, QWidget *parent)
    : QWidget(parent)
    , m_kvmManager(kvmManager)
    , m_mainLayout(nullptr)
    , m_searchLayout(nullptr)
    , m_filterLayout(nullptr)
    , m_buttonLayout(nullptr)
    , m_searchEdit(nullptr)
    , m_filterCombo(nullptr)
    , m_createGroupButton(nullptr)
    , m_vmListWidget(nullptr)
    , m_vmCountLabel(nullptr)
    , m_model(new VMListModel(this))
    , m_currentFilter("Todos")
{
    setupUI();
    populateVMList();
}

void VMListWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(5);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Search bar
    m_searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(tr("Buscar máquinas virtuales..."));
    m_searchLayout->addWidget(m_searchEdit);
    m_mainLayout->addLayout(m_searchLayout);
    
    // Filter combo
    m_filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel(tr("Filtrar:"));
    m_filterCombo = new QComboBox();
    m_filterCombo->addItems({tr("Todos"), tr("En ejecución"), tr("Apagadas"), tr("Pausadas"), tr("Guardadas")});
    m_filterLayout->addWidget(filterLabel);
    m_filterLayout->addWidget(m_filterCombo);
    m_filterLayout->addStretch();
    m_mainLayout->addLayout(m_filterLayout);
    
    // VM List
    m_vmListWidget = new QListWidget();
    m_vmListWidget->setAlternatingRowColors(true);
    m_vmListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_vmListWidget->setIconSize(QSize(32, 32));
    m_mainLayout->addWidget(m_vmListWidget);
    
    // Group creation button
    m_buttonLayout = new QHBoxLayout();
    m_createGroupButton = new QPushButton(tr("Crear Grupo"));
    m_createGroupButton->setEnabled(false); // TODO: Enable when implemented
    m_buttonLayout->addWidget(m_createGroupButton);
    m_buttonLayout->addStretch();
    m_mainLayout->addLayout(m_buttonLayout);
    
    // VM count label
    m_vmCountLabel = new QLabel(tr("0 máquinas virtuales"));
    m_vmCountLabel->setStyleSheet("color: gray; font-size: 10px;");
    m_mainLayout->addWidget(m_vmCountLabel);
    
    // Connect signals
    connect(m_vmListWidget, &QListWidget::itemSelectionChanged,
            this, &VMListWidget::onSelectionChanged);
    connect(m_vmListWidget, &QListWidget::itemDoubleClicked,
            this, &VMListWidget::onItemDoubleClicked);
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &VMListWidget::onSearchTextChanged);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VMListWidget::onFilterChanged);
    connect(m_createGroupButton, &QPushButton::clicked,
            this, &VMListWidget::onCreateGroupClicked);
}

void VMListWidget::populateVMList()
{
    m_vmListWidget->clear();
    m_allVMs.clear();
    
    if (!m_kvmManager) {
        qDebug() << "VMListWidget: KVMManager es null";
        return;
    }
    
    // Load VMs from XML files
    QStringList vmNames = m_kvmManager->getVirtualMachines();

    
    for (const QString &vmName : vmNames) {
        VirtualMachine *vm = m_kvmManager->getVirtualMachine(vmName);
        if (vm) {
            QListWidgetItem *item = createVMItem(vm->getName(), vm->getOSType(), vm->getState());
            m_vmListWidget->addItem(item);
            m_allVMs.append(vm->getName());
        }
    }
    
    filterVMs();
}

QListWidgetItem* VMListWidget::createVMItem(const QString &name, const QString &os, const QString &state)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(name);
    item->setData(Qt::UserRole, name);
    item->setData(Qt::UserRole + 1, os);
    item->setData(Qt::UserRole + 2, state);
    
    // Create a generic VM icon based on OS
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Choose color based on OS
    QColor bgColor(100, 150, 200); // Default blue
    if (os.contains("Ubuntu", Qt::CaseInsensitive)) {
        bgColor = QColor(233, 84, 32); // Ubuntu orange
    } else if (os.contains("Windows", Qt::CaseInsensitive)) {
        bgColor = QColor(0, 120, 215); // Windows blue
    } else if (os.contains("Debian", Qt::CaseInsensitive)) {
        bgColor = QColor(215, 7, 81); // Debian red
    } else if (os.contains("CentOS", Qt::CaseInsensitive) || os.contains("Red Hat", Qt::CaseInsensitive)) {
        bgColor = QColor(238, 50, 36); // Red Hat red
    } else if (os.contains("Fedora", Qt::CaseInsensitive)) {
        bgColor = QColor(51, 105, 232); // Fedora blue
    }
    
    painter.setBrush(QBrush(bgColor));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(4, 4, 24, 24);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "VM");
    QIcon icon = QIcon(pixmap);
    
    item->setIcon(icon);
    
    // Create tooltip with detailed information
    QString tooltip = QString("<b>%1</b><br>"
                             "Sistema Operativo: %2<br>"
                             "Estado: %3")
                             .arg(name, os, state);
    item->setToolTip(tooltip);
    
    // Set colors based on state
    QColor textColor = Qt::white;
    if (state == "En ejecución") {
        textColor = QColor(100, 255, 100); // Light green
    } else if (state == "Pausada") {
        textColor = QColor(255, 255, 100); // Light yellow
    } else if (state == "Guardada") {
        textColor = QColor(150, 150, 255); // Light blue
    }
    
    item->setForeground(QBrush(textColor));
    
    return item;
}

QString VMListWidget::getSelectedVM() const
{
    QListWidgetItem *currentItem = m_vmListWidget->currentItem();
    if (currentItem) {
        return currentItem->data(Qt::UserRole).toString();
    }
    return QString();
}

void VMListWidget::setSelectedVM(const QString &vmName)
{
    for (int i = 0; i < m_vmListWidget->count(); ++i) {
        QListWidgetItem *item = m_vmListWidget->item(i);
        if (item && item->data(Qt::UserRole).toString() == vmName) {
            m_vmListWidget->setCurrentItem(item);
            break;
        }
    }
}

void VMListWidget::refreshVMList()
{
    // TODO: Implement actual VM discovery from KVM
    populateVMList();
}

void VMListWidget::onSelectionChanged()
{
    QString selectedVM = getSelectedVM();
    emit vmSelectionChanged(selectedVM);
}

void VMListWidget::onItemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        QString vmName = item->data(Qt::UserRole).toString();
        emit vmDoubleClicked(vmName);
    }
}

void VMListWidget::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text)
    filterVMs();
}

void VMListWidget::onFilterChanged()
{
    m_currentFilter = m_filterCombo->currentText();
    filterVMs();
}

void VMListWidget::filterVMs()
{
    QString searchText = m_searchEdit->text().toLower();
    QString filter = m_currentFilter;
    
    int visibleCount = 0;
    
    for (int i = 0; i < m_vmListWidget->count(); ++i) {
        QListWidgetItem *item = m_vmListWidget->item(i);
        if (!item) continue;
        
        QString vmName = item->data(Qt::UserRole).toString().toLower();
        QString vmOS = item->data(Qt::UserRole + 1).toString().toLower();
        QString vmState = item->data(Qt::UserRole + 2).toString();
        
        bool matchesSearch = vmName.contains(searchText) || vmOS.contains(searchText);
        bool matchesFilter = (filter == tr("Todos")) || (vmState == filter);
        
        bool visible = matchesSearch && matchesFilter;
        item->setHidden(!visible);
        
        if (visible) {
            visibleCount++;
        }
    }
    
    // Update count label
    if (visibleCount == m_allVMs.size()) {
        m_vmCountLabel->setText(tr("%1 máquinas virtuales").arg(visibleCount));
    } else {
        m_vmCountLabel->setText(tr("%1 de %2 máquinas virtuales").arg(visibleCount).arg(m_allVMs.size()));
    }
}

void VMListWidget::onCreateGroupClicked()
{
    // TODO: Implement group creation
}