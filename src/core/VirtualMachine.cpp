#include "VirtualMachine.h"

VirtualMachine::VirtualMachine(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_uuid("")
    , m_description("")
    , m_osType("Linux")
    , m_state("shut off")
    , m_memoryMB(2048)
    , m_cpuCount(1)
    , m_audioController("PulseAudio")
    , m_usbController("USB 3.0 (xHCI)")
    , m_videoMemoryMB(128)
    , m_3dAcceleration(false)
    , m_monitorCount(1)
    , m_createdDate(QDateTime::currentDateTime())
{
    // Set default boot order
    m_bootOrder << "Hard Disk" << "CD/DVD" << "Network";
    
    // Set default network adapter
    m_networkAdapters << "NAT";
}

VirtualMachine::~VirtualMachine()
{
}

void VirtualMachine::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit configurationChanged();
    }
}

void VirtualMachine::setState(const QString &state)
{
    if (m_state != state) {
        QString oldState = m_state;
        m_state = state;
        
        if (state == "running") {
            m_lastStarted = QDateTime::currentDateTime();
        }
        
        emit stateChanged(oldState, state);
    }
}

VirtualMachine::State VirtualMachine::getStateEnum() const
{
    if (m_state == "shut off" || m_state == "shutoff") {
        return ShutOff;
    } else if (m_state == "running") {
        return Running;
    } else if (m_state == "paused") {
        return Paused;
    } else if (m_state == "saved") {
        return Saved;
    } else if (m_state == "starting") {
        return Starting;
    } else if (m_state == "stopping") {
        return Stopping;
    } else {
        return Unknown;
    }
}