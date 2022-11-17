/*
 * CredintialProcessor.h
 *
 *  Created on: 15 Nov 2022
 *      Author: tzachi
 */

#ifndef REQPROCESSOR_H_
#define REQPROCESSOR_H_
#include <map>
#include <set>
#include "RequestProvisioner.h"
#include "JsonParser.h"
#include "FIfoQueue.h"
#include "EventQueue.h"

using namespace common;

typedef std::map< std::string , std::list<std::shared_ptr <UserRequest> > > HOST_reqORD_MAP_T;

class ReqProcessor : public EventQueue {
	friend class ReqEvent;
public:
	ReqProcessor(int id, const std::set<std::string> &secure_hosts, const std::set<std::string> &insecure_passwords, PCQueue< std::shared_ptr<StringWrapper> > &msg_queue) :
		EventQueue(id), m_secure_hosts(secure_hosts), m_insecure_passwords(insecure_passwords), m_msg_queue(msg_queue){}
	virtual ~ReqProcessor(){}
	int PrintAllreqs();
	int Start();
	int Stop();
	bool AddReq(std::shared_ptr<UserRequest> user_req);
private:
	class ReqEvent : public IEvent
	{
	public:
		ReqEvent(ReqProcessor* req_proccessor, std::shared_ptr<UserRequest> user_req) : m_req_proccessor(req_proccessor), m_user_req(user_req){}
		virtual ~ReqEvent() {}

		virtual void Execute();
	private:
		ReqProcessor* m_req_proccessor;
		std::shared_ptr<UserRequest> m_user_req;
	};
	int ProcessReq(std::shared_ptr<UserRequest> user_req);
	int UnCacheOldReqs(const std::chrono::system_clock::time_point &timestamp);
	int CacheReq(std::shared_ptr<UserRequest> user_req, const std::string &username, const std::string &password, const std::string &host);
	REQ_LIST_T m_user_req_list;
	std::map< std::string, std::map<std::string, HOST_reqORD_MAP_T> > m_user_map;
	const std::set<std::string> &m_secure_hosts;
	const std::set<std::string> &m_insecure_passwords;
	PCQueue< std::shared_ptr<StringWrapper> >& m_msg_queue;
};


#endif /* REQPROCESSOR_H_ */
