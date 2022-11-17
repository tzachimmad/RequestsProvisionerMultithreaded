/*
 * RequestProvisioner.h
 *
 *  Created on: 16 Nov 2022
 *      Author: tzachi
 */

#ifndef REQUESTPROVISIONER_H_
#define REQUESTPROVISIONER_H_

#include "JsonParser.h"
#include <set>
#include <vector>
#include "FIfoQueue.h"
#include "EventQueue.h"

class ReqProcessor;
typedef std::list<std::shared_ptr <UserRequest> > REQ_LIST_T;

class RequestProvisioner {
public:
	RequestProvisioner(const std::string& secure_sites_path, const std::string& insecure_passwords_path, const std::string &input_json_path) :
		m_secure_sites_path(secure_sites_path), m_insecure_passwords_path(insecure_passwords_path), m_input_json_path(input_json_path), m_msg_queue(100) {}
	virtual ~RequestProvisioner(){}
	int Init();
	int Run();
private:
	int FillSetFromFile(const std::string& path, std::set<std::string> &set);
	int PrintMessages();
	int ProvisionReqAsync(std::shared_ptr<UserRequest> user_req);
	std::string m_secure_sites_path;
	std::string m_insecure_passwords_path;
	std::string m_input_json_path;
	std::set<std::string> m_secure_hosts;
	std::set<std::string> m_insecure_passwords;
	PCQueue< std::shared_ptr<StringWrapper> > m_msg_queue;
	REQ_LIST_T m_user_req_list;
	std::vector< std::shared_ptr<ReqProcessor> > m_worker_vec;
};

#endif /* REQUESTPROVISIONER_H_ */
