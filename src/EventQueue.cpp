/*
 * TaskQueue.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: root
 */

#include "EventQueue.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <vector>
namespace common
{
using namespace std;

EventQueue::EventQueue(int id) : m_id(id), m_events_list_size(0), m_stopping(false)
{
	m_base = event_base_new();
	m_execute_event = event_new(m_base, -1, EV_TIMEOUT, ExecuteCallback, this);
    m_running = false;
}

EventQueue::~EventQueue()
{
    if (m_execute_event)
    {
        event_free(m_execute_event);
    }

    for (auto taskIt : m_events_list)
    {
        delete taskIt;
    }

}

bool EventQueue::Start()
{
	m_stopping = false;
	int threadErrorNumber;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	if ((threadErrorNumber = pthread_create(&m_tid, &attr, ThreadFunc, this)))
	{
		cout << "Failed to start a thread id: " << m_id<< endl;
		return false;
	}
	m_running = true;

	return true;
}

bool EventQueue::Stop()
{
	m_stopping  = true;
	return true;
}

bool EventQueue::IsRunning()
{
	return m_running;
}

bool EventQueue::AddEvent(IEvent *event)
{
	if (m_stopping) {
		return true;
	}
	bool out = true;
	int tasks_list_size;
	m_lock.lock();
	if (m_events_list_size < QUEUE_MAX_SIZE)
	{
		m_events_list.push_back(event);
		m_events_list_size++;
	}
	else
	{
		out = false;
	}

	tasks_list_size = m_events_list_size;

	m_lock.unlock();

	if (!out)
	{
		cout << "Queue size is over " << QUEUE_MAX_SIZE << "  long, aborting new events until number reduces" << endl;
	}
	return out;
}
void* EventQueue::ThreadFunc(void* arg)
{
	int result;
	EventQueue *curr = reinterpret_cast<EventQueue*> (arg);
	timeval tm;
	tm.tv_sec = 0.1;
	tm.tv_usec = 0;
	event_add(curr->m_execute_event, &tm);
	if ((result = event_base_dispatch(curr->GetEventBase())) < 0)
	{
		cout <<  "Event base loop failed to start " << endl;
		exit(EXIT_FAILURE);
	}
	return NULL;
}


void EventQueue::ExecuteCallback(evutil_socket_t fd, short what, void *ctx)
{
	EventQueue *instance = static_cast<EventQueue*>(ctx);
	vector<IEvent*> taskVec;
	instance->m_lock.lock();
	while (instance->m_events_list.size()>0)
	{
		taskVec.push_back(instance->m_events_list.front());
		instance->m_events_list.pop_front();
		instance->m_events_list_size--;
	}
	instance->m_lock.unlock();
	for (auto task : taskVec)
	{
		task->Execute();
	}
	for (auto task : taskVec)
	{
		delete task;
	}
	if (instance->m_stopping) {
		instance->m_running = false;
		return;
	}

	timeval tm;
	tm.tv_sec = 0.1;
	tm.tv_usec = 0;
	event_add(instance->m_execute_event, &tm);
}

} /* namespace common */
