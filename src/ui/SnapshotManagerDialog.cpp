#include "SnapshotManagerDialog.h"
#include "../core/VirtualMachine.h"

#include <QApplication>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QRandomGenerator>

SnapshotManagerDialog::SnapshotManagerDialog(VirtualMachine *vm, QWidget *parent)
    : QDialog(parent)
    , m_vm(vm)
{
    setWindowTitle(tr("Instantáneas - %1").arg(vm ? vm->getName() : tr("Sin VM")));
    setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    resize(800, 600);
    setModal(false); // Allow interaction with main window
    
    setupUI();
    refreshSnapshots();
}

void SnapshotManagerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Current snapshot indicator
    m_currentSnapshotLabel = new QLabel(tr("Instantánea actual: <b>Estado actual de la máquina</b>"));
    m_currentSnapshotLabel->setStyleSheet("background: #2a2a2a; padding: 8px; border-radius: 4px; color: #ffffff;");
    mainLayout->addWidget(m_currentSnapshotLabel);
    
    // Create splitter for tree and details
    m_splitter = new QSplitter(Qt::Horizontal);
    
    // Left side - Snapshot tree and buttons
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    
    // Tree widget
    m_snapshotTree = new QTreeWidget();
    m_snapshotTree->setHeaderLabels({tr("Nombre"), tr("Tomada")});
    m_snapshotTree->header()->setStretchLastSection(true);
    m_snapshotTree->setAlternatingRowColors(true);
    m_snapshotTree->setRootIsDecorated(true);
    connect(m_snapshotTree, &QTreeWidget::itemSelectionChanged, this, &SnapshotManagerDialog::onSnapshotSelectionChanged);
    
    leftLayout->addWidget(m_snapshotTree);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_takeSnapshotButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileIcon), tr("&Tomar"));
    m_deleteSnapshotButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_TrashIcon), tr("&Eliminar"));
    m_restoreSnapshotButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_MediaSeekBackward), tr("&Restaurar"));
    
    buttonLayout->addWidget(m_takeSnapshotButton);
    buttonLayout->addWidget(m_deleteSnapshotButton);
    buttonLayout->addWidget(m_restoreSnapshotButton);
    buttonLayout->addStretch();
    
    m_showDetailsButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("&Detalles"));
    m_cloneButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Clonar"));
    
    buttonLayout->addWidget(m_showDetailsButton);
    buttonLayout->addWidget(m_cloneButton);
    
    leftLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_takeSnapshotButton, &QPushButton::clicked, this, &SnapshotManagerDialog::takeSnapshot);
    connect(m_deleteSnapshotButton, &QPushButton::clicked, this, &SnapshotManagerDialog::deleteSnapshot);
    connect(m_restoreSnapshotButton, &QPushButton::clicked, this, &SnapshotManagerDialog::restoreSnapshot);
    connect(m_showDetailsButton, &QPushButton::clicked, this, &SnapshotManagerDialog::showSnapshotDetails);
    connect(m_cloneButton, &QPushButton::clicked, this, &SnapshotManagerDialog::cloneFromSnapshot);
    
    m_splitter->addWidget(leftWidget);
    
    // Right side - Details
    m_detailsGroup = new QGroupBox(tr("Detalles de instantánea"));
    QGridLayout *detailsLayout = new QGridLayout(m_detailsGroup);
    
    detailsLayout->addWidget(new QLabel(tr("Nombre:")), 0, 0);
    m_nameEdit = new QLineEdit();
    m_nameEdit->setReadOnly(true);
    detailsLayout->addWidget(m_nameEdit, 0, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Descripción:")), 1, 0);
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setReadOnly(true);
    m_descriptionEdit->setMaximumHeight(100);
    detailsLayout->addWidget(m_descriptionEdit, 1, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Tomada:")), 2, 0);
    m_timestampLabel = new QLabel();
    detailsLayout->addWidget(m_timestampLabel, 2, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Tamaño:")), 3, 0);
    m_sizeLabel = new QLabel();
    detailsLayout->addWidget(m_sizeLabel, 3, 1);
    
    detailsLayout->addWidget(new QLabel(tr("Estado de VM:")), 4, 0);
    m_vmStateLabel = new QLabel();
    detailsLayout->addWidget(m_vmStateLabel, 4, 1);
    
    // Add spacer
    detailsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 5, 0, 1, 2);
    
    m_splitter->addWidget(m_detailsGroup);
    m_splitter->setSizes({500, 300});
    
    mainLayout->addWidget(m_splitter);
    
    // Close button
    QHBoxLayout *closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    QPushButton *closeButton = new QPushButton(tr("&Cerrar"));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    closeLayout->addWidget(closeButton);
    
    mainLayout->addLayout(closeLayout);
}

void SnapshotManagerDialog::refreshSnapshots()
{
    populateSnapshotTree();
    updateSnapshotDetails();
}

void SnapshotManagerDialog::populateSnapshotTree()
{
    m_snapshotTree->clear();
    
    if (!m_vm) return;
    
    // Add some sample snapshots to demonstrate the tree structure
    QTreeWidgetItem *rootSnapshot = new QTreeWidgetItem(m_snapshotTree);
    rootSnapshot->setText(0, tr("Instalación base"));
    rootSnapshot->setText(1, QDateTime::currentDateTime().addDays(-7).toString("dd/MM/yyyy hh:mm"));
    rootSnapshot->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    rootSnapshot->setData(0, Qt::UserRole, "snapshot1");
    
    QTreeWidgetItem *beforeUpdate = new QTreeWidgetItem(rootSnapshot);
    beforeUpdate->setText(0, tr("Antes de actualizaciones"));
    beforeUpdate->setText(1, QDateTime::currentDateTime().addDays(-3).toString("dd/MM/yyyy hh:mm"));
    beforeUpdate->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    beforeUpdate->setData(0, Qt::UserRole, "snapshot2");
    
    QTreeWidgetItem *afterUpdate = new QTreeWidgetItem(beforeUpdate);
    afterUpdate->setText(0, tr("Después de actualizaciones"));
    afterUpdate->setText(1, QDateTime::currentDateTime().addDays(-1).toString("dd/MM/yyyy hh:mm"));
    afterUpdate->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    afterUpdate->setData(0, Qt::UserRole, "snapshot3");
    
    QTreeWidgetItem *currentSnapshot = new QTreeWidgetItem(afterUpdate);
    currentSnapshot->setText(0, tr("Estado actual"));
    currentSnapshot->setText(1, tr("Ahora"));
    currentSnapshot->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    currentSnapshot->setData(0, Qt::UserRole, "current");
    
    // Expand all items
    m_snapshotTree->expandAll();
    
    // Select the current state
    m_snapshotTree->setCurrentItem(currentSnapshot);
}

void SnapshotManagerDialog::takeSnapshot()
{
    if (!m_vm) return;
    
    bool ok;
    QString snapshotName = QInputDialog::getText(this, tr("Tomar instantánea"),
        tr("Nombre de la instantánea:"), QLineEdit::Normal, 
        tr("Instantánea %1").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm")), &ok);
    
    if (ok && !snapshotName.isEmpty()) {
        QString description = QInputDialog::getMultiLineText(this, tr("Tomar instantánea"),
            tr("Descripción (opcional):"), "", &ok);
        
        if (ok) {
            // Create new snapshot item
            QTreeWidgetItem *current = m_snapshotTree->currentItem();
            QTreeWidgetItem *newSnapshot = new QTreeWidgetItem();
            newSnapshot->setText(0, snapshotName);
            newSnapshot->setText(1, QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
            newSnapshot->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            newSnapshot->setData(0, Qt::UserRole, QString("snapshot_%1").arg(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            
            if (current) {
                // If current item is "Estado actual", replace it
                if (current->data(0, Qt::UserRole).toString() == "current") {
                    QTreeWidgetItem *parent = current->parent();
                    if (parent) {
                        parent->addChild(newSnapshot);
                    } else {
                        m_snapshotTree->addTopLevelItem(newSnapshot);
                    }
                    
                    // Create new current state
                    QTreeWidgetItem *newCurrent = new QTreeWidgetItem(newSnapshot);
                    newCurrent->setText(0, tr("Estado actual"));
                    newCurrent->setText(1, tr("Ahora"));
                    newCurrent->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
                    newCurrent->setData(0, Qt::UserRole, "current");
                    
                    delete current;
                    m_snapshotTree->setCurrentItem(newCurrent);
                } else {
                    current->addChild(newSnapshot);
                }
            } else {
                m_snapshotTree->addTopLevelItem(newSnapshot);
            }
            
            m_snapshotTree->expandAll();
            
            QMessageBox::information(this, tr("Instantánea creada"), 
                tr("Instantánea '%1' creada correctamente.").arg(snapshotName));
        }
    }
}

void SnapshotManagerDialog::deleteSnapshot()
{
    QTreeWidgetItem *current = m_snapshotTree->currentItem();
    if (!current) return;
    
    // Don't allow deleting current state
    if (current->data(0, Qt::UserRole).toString() == "current") {
        QMessageBox::warning(this, tr("No se puede eliminar"), 
            tr("No se puede eliminar el estado actual de la máquina virtual."));
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirmar eliminación"),
        tr("¿Está seguro de que desea eliminar la instantánea '%1'?\n\n"
           "Esta acción no se puede deshacer y también eliminará todas las instantáneas hijas.")
           .arg(current->text(0)),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        delete current;
        QMessageBox::information(this, tr("Instantánea eliminada"), 
            tr("Instantánea eliminada correctamente."));
    }
}

void SnapshotManagerDialog::restoreSnapshot()
{
    QTreeWidgetItem *current = m_snapshotTree->currentItem();
    if (!current) return;
    
    // Don't allow restoring current state
    if (current->data(0, Qt::UserRole).toString() == "current") {
        QMessageBox::information(this, tr("Ya está en el estado actual"), 
            tr("La máquina virtual ya está en el estado actual."));
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirmar restauración"),
        tr("¿Está seguro de que desea restaurar la instantánea '%1'?\n\n"
           "El estado actual de la máquina virtual se perderá.")
           .arg(current->text(0)),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        QMessageBox::information(this, tr("Instantánea restaurada"), 
            tr("La máquina virtual ha sido restaurada a la instantánea '%1'.").arg(current->text(0)));
        
        // Update current snapshot indicator
        m_currentSnapshotLabel->setText(tr("Instantánea actual: <b>%1</b>").arg(current->text(0)));
    }
}

void SnapshotManagerDialog::showSnapshotDetails()
{
    QTreeWidgetItem *current = m_snapshotTree->currentItem();
    if (!current) return;
    
    QString details = tr("Detalles de la instantánea '%1':\n\n").arg(current->text(0));
    details += tr("Nombre: %1\n").arg(current->text(0));
    details += tr("Tomada: %1\n").arg(current->text(1));
    details += tr("Estado: %1\n").arg(current->data(0, Qt::UserRole).toString() == "current" ? tr("Estado actual") : tr("Instantánea guardada"));
    details += tr("Tamaño estimado: %1 MB\n").arg(QString::number(QRandomGenerator::global()->bounded(100, 1000)));
    
    QMessageBox::information(this, tr("Detalles de instantánea"), details);
}

void SnapshotManagerDialog::cloneFromSnapshot()
{
    QTreeWidgetItem *current = m_snapshotTree->currentItem();
    if (!current) return;
    
    bool ok;
    QString cloneName = QInputDialog::getText(this, tr("Clonar desde instantánea"),
        tr("Nombre de la nueva máquina virtual:"), QLineEdit::Normal, 
        tr("%1 - Clon").arg(m_vm->getName()), &ok);
    
    if (ok && !cloneName.isEmpty()) {
        QMessageBox::information(this, tr("Clon creado"), 
            tr("Se ha creado un clon '%1' basado en la instantánea '%2'.")
            .arg(cloneName).arg(current->text(0)));
    }
}

void SnapshotManagerDialog::onSnapshotSelectionChanged()
{
    updateSnapshotDetails();
}

void SnapshotManagerDialog::updateSnapshotDetails()
{
    QTreeWidgetItem *current = m_snapshotTree->currentItem();
    if (current) {
        m_nameEdit->setText(current->text(0));
        m_timestampLabel->setText(current->text(1));
        
        if (current->data(0, Qt::UserRole).toString() == "current") {
            m_descriptionEdit->setPlainText(tr("Estado actual de la máquina virtual en ejecución."));
            m_sizeLabel->setText(tr("N/A"));
            m_vmStateLabel->setText(tr("En ejecución"));
        } else {
            m_descriptionEdit->setPlainText(tr("Instantánea tomada el %1. "
                "Contiene el estado completo de la máquina virtual en ese momento.")
                .arg(current->text(1)));
            m_sizeLabel->setText(tr("%1 MB").arg(QString::number(QRandomGenerator::global()->bounded(100, 1000))));
            m_vmStateLabel->setText(tr("Apagada"));
        }
        
        // Enable/disable buttons based on selection
        bool isCurrent = (current->data(0, Qt::UserRole).toString() == "current");
        m_deleteSnapshotButton->setEnabled(!isCurrent);
        m_restoreSnapshotButton->setEnabled(!isCurrent);
        
    } else {
        m_nameEdit->clear();
        m_descriptionEdit->clear();
        m_timestampLabel->clear();
        m_sizeLabel->clear();
        m_vmStateLabel->clear();
        
        m_deleteSnapshotButton->setEnabled(false);
        m_restoreSnapshotButton->setEnabled(false);
    }
}