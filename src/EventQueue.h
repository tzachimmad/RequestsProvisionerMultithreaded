/*
 * TaskQueue.h
 *
 *  Created on: Apr 27, 2015
 *      Author: root
 */

#ifndef COMMON_TASKQUEUE_H_
#define COMMON_TASKQUEUE_H_


#include <list>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <mutex>

typedef unsigned int UINT32;
#define QUEUE_MAX_SIZE 50
namespace common
{

class IEvent
{
public:
    IEvent() {}
    virtual ~IEvent() {}

    virtual void Execute() = 0;
};

class DummyEvent : public IEvent
{
public:
	DummyEvent() {}
    virtual ~DummyEvent() {}

    virtual void Execute(){}
};

class EventQueue
{
public:
    EventQueue(int id) ;
    virtual ~EventQueue();

    bool Start();
    bool Stop();
    void LockMutex(){m_lock.lock();}
    void UnLockMutex(){m_lock.unlock();}
    struct event_base* GetEventBase(){return m_base;}
    pthread_cond_t* GetPthreadCond(){return &m_initCond;}
    bool IsRunning();
    bool AddEvent(IEvent *event);

protected:
    static void ExecuteCallback(evutil_socket_t fd, short what, void *ctx);
    static void* ThreadFunc(void* arg);
    bool m_stopping;
    bool m_running;
    int m_id;
    int m_events_list_size;
    pthread_t m_tid;
    pthread_cond_t m_initCond;
    struct event_base* m_base;
    struct event *m_execute_event;
    std::list<IEvent*> m_events_list;
    std::mutex m_lock;
};

} /* namespace common */

#endif /* COMMON_TASKQUEUE_H_ */
