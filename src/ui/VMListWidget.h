#ifndef VMLISTWIDGET_H
#define VMLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

class VMListModel;

class VMListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VMListWidget(QWidget *parent = nullptr);
    
    QString getSelectedVM() const;
    void setSelectedVM(const QString &vmName);
    void refreshVMList();

signals:
    void vmSelectionChanged(const QString &vmName);
    void vmDoubleClicked(const QString &vmName);

private slots:
    void onSelectionChanged();
    void onItemDoubleClicked(QListWidgetItem *item);
    void onSearchTextChanged(const QString &text);
    void onFilterChanged();
    void onCreateGroupClicked();

private:
    void setupUI();
    void populateVMList();
    void filterVMs();
    QListWidgetItem* createVMItem(const QString &name, const QString &os, const QString &state);

    // UI components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_searchLayout;
    QHBoxLayout *m_filterLayout;
    QHBoxLayout *m_buttonLayout;
    
    QLineEdit *m_searchEdit;
    QComboBox *m_filterCombo;
    QPushButton *m_createGroupButton;
    
    QListWidget *m_vmListWidget;
    QLabel *m_vmCountLabel;
    
    // Data
    VMListModel *m_model;
    QStringList m_allVMs;
    QString m_currentFilter;
};

#endif // VMLISTWIDGET_H