#ifndef VMDETAILSWIDGET_H
#define VMDETAILSWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QProgressBar>

class VMDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VMDetailsWidget(QWidget *parent = nullptr);

public slots:
    void setSelectedVM(const QString &vmName);
    void refreshDetails();

private:
    void setupUI();
    void createGeneralSection();
    void createSystemSection();
    void createStorageSection();
    void createNetworkSection();
    void createAudioSection();
    void createUSBSection();
    void createDisplaySection();
    void createSharedFoldersSection();
    void updateVMDetails(const QString &vmName);
    void clearDetails();
    
    QWidget* createInfoRow(const QString &label, const QString &value, QWidget *parent = nullptr);
    QGroupBox* createSection(const QString &title);

    // UI components
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;
    
    // Sections
    QGroupBox *m_generalSection;
    QGroupBox *m_systemSection;
    QGroupBox *m_storageSection;
    QGroupBox *m_networkSection;
    QGroupBox *m_audioSection;
    QGroupBox *m_usbSection;
    QGroupBox *m_displaySection;
    QGroupBox *m_sharedFoldersSection;
    
    // Labels for dynamic content
    QLabel *m_vmNameLabel;
    QLabel *m_vmOSLabel;
    QLabel *m_vmStateLabel;
    QLabel *m_vmLocationLabel;
    QLabel *m_vmUUIDLabel;
    QLabel *m_vmDescriptionLabel;
    
    QLabel *m_systemMemoryLabel;
    QLabel *m_systemProcessorsLabel;
    QLabel *m_systemBootOrderLabel;
    QLabel *m_systemVirtualizationLabel;
    
    QLabel *m_storageControllerLabel;
    QLabel *m_storageHardDiskLabel;
    QLabel *m_storageCDDVDLabel;
    
    QLabel *m_networkAdapter1Label;
    QLabel *m_networkAdapter2Label;
    
    QLabel *m_audioControllerLabel;
    
    QLabel *m_usbControllerLabel;
    
    QLabel *m_displayVideoMemoryLabel;
    QLabel *m_displayAccelerationLabel;
    QLabel *m_displayMonitorsLabel;
    
    QLabel *m_sharedFoldersCountLabel;
    
    // Current VM name
    QString m_currentVM;
};

#endif // VMDETAILSWIDGET_H