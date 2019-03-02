#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Comp/VirtualComponent.hpp>
#include <Os/Task.hpp>
#include <Os/Baremetal/TaskRunner/BareTaskHandle.hpp>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>
namespace Os {

Task::Task() :
    m_handle(0),
    m_identifier(0),
    m_affinity(-1),
    m_started(false),
    m_suspendedOnPurpose(false)
{}
    
Task::TaskStatus Task::start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity) {
    //Get a task handle, and set it up
    BareTaskHandle* handle = new BareTaskHandle();
    if (handle == NULL) {
       return Task::TASK_UNKNOWN_ERROR;
    }
    printf("A Task is Registered: %s Function: 0x%p and Arg: 0x%p\n", name.toChar(), routine, arg);
    handle->m_enabled = true;
    handle->m_priority = priority;
    handle->m_routine = routine;
    handle->m_argument = arg;
    //Fw::ActiveComponentBase* tmp = reinterpret_cast<Fw::ActiveComponentBase*>(arg);
    Fw::VirtualComponent::s_start_comp(arg);
    handle->m_routine = Fw::VirtualComponent::s_step_comp;
    //Register this task
    m_handle = reinterpret_cast<POINTER_CAST>(handle);
    this->m_name = "BR_";
    this->m_name += name;
    this->m_identifier = identifier;
    // If a registry has been registered, register task
    if (Task::s_taskRegistry) {
        Task::s_taskRegistry->addTask(this);
    }
    return Task::TASK_OK;
}

Task::TaskStatus Task::delay(NATIVE_UINT_TYPE milliseconds)
{
    return Task::TASK_DELAY_ERROR;
}

Task::~Task() {
    if (this->m_handle) {
        delete reinterpret_cast<BareTaskHandle*>(this->m_handle);
    }
    // If a registry has been registered, remove task
    if (Task::s_taskRegistry) {
        Task::s_taskRegistry->removeTask(this);
    }
}

void Task::suspend(bool onPurpose) {
    FW_ASSERT(reinterpret_cast<BareTaskHandle*>(this->m_handle) != NULL);
    reinterpret_cast<BareTaskHandle*>(this->m_handle)->m_enabled = false;
}
                    
void Task::resume(void) {
    FW_ASSERT(reinterpret_cast<BareTaskHandle*>(this->m_handle) != NULL);
    reinterpret_cast<BareTaskHandle*>(this->m_handle)->m_enabled = true;
}

bool Task::isSuspended(void) {
    FW_ASSERT(reinterpret_cast<BareTaskHandle*>(this->m_handle) != NULL);
    return !reinterpret_cast<BareTaskHandle*>(this->m_handle)->m_enabled;
}

}
