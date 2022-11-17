//============================================================================
// Name        : Main.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <unistd.h>
#include <functional>
#include "JsonParser.h"
#include "ReqProcessor.h"
#include "RequestProvisioner.h"


using namespace std;

int RequestProvisioner::FillSetFromFile(const std::string& path, std::set<std::string> &set) {
	std::fstream f;
	f.open(m_secure_sites_path.c_str(), ios::in);
	if (!f) {
		cout << "Invalid file path " << m_secure_sites_path << " aborting" << endl;
		return 1;
	}
	string line;
	while(getline(f, line) )
	{
		set.insert(line);
	}
	return 0;
}

int RequestProvisioner::Init() {
	if (FillSetFromFile(m_secure_sites_path, m_secure_hosts)) {
		return 1;
	}
	if (FillSetFromFile(m_insecure_passwords_path, m_insecure_passwords)) {
		return 1;
	}

	for (int i = 0; i < 4; i++) {
		auto proccessor = make_shared<ReqProcessor>(i, m_secure_hosts, m_insecure_passwords, m_msg_queue);
		m_worker_vec.push_back(proccessor);
		m_worker_vec[i]->Start();
	}


	return 0;
}

int RequestProvisioner::ProvisionReqAsync(std::shared_ptr<UserRequest> user_req) {

	std::hash<std::string> hasher;
	auto hashed = hasher(user_req->GetUserName())%4;
	m_worker_vec[hashed]->AddReq(user_req);
	return 0;
}

int RequestProvisioner::PrintMessages()
{
	while( m_msg_queue.count())
	{
		cout << m_msg_queue.pop()->m_str;
	}
	while (m_user_req_list.front()->IsProccessed()) {
		cout << m_user_req_list.front()->GetId() << ") " << m_user_req_list.front()->GetOutMsg();
		m_user_req_list.pop_front();
	}
	return 0;
}

int RequestProvisioner::Run() {
	JsonParser json_parser (m_input_json_path);
	if (json_parser.Init()) {
		cout << "Failed to read input json file " << m_input_json_path << endl;
		return 1;
	}
	{
		shared_ptr<UserRequest> ptr;
		while((ptr = json_parser.GetNextReq()) != NULL)
		{
			ProvisionReqAsync(ptr);
			m_user_req_list.push_back(ptr);
			PrintMessages();
		}
	}

	auto ptr = std::make_shared<UserRequest>(-1, "talon-last-req", "", "", "", "", "", "", TIME_POINT_T::max());
	for (auto &proccessor : m_worker_vec) {
		proccessor->AddReq(ptr);
	}

	for (auto &proccessor : m_worker_vec) {
		proccessor->Stop();
	}
	while(true) {
		bool workers_running = 0;
		for (auto &proccessor : m_worker_vec) {
			workers_running = proccessor->IsRunning() ? workers_running + 1 : workers_running;
		}
		if (workers_running == 0) {
			break;
		}
		sleep(1);
	}
	PrintMessages();
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 4)
	{
		cout << "Please enter the correct number of parameters" << endl;
		return 1;
	}

	string secure_sites_path = argv[1];
	string insecure_passwords_path = argv[2];
	string input_json_path = argv[3];
	RequestProvisioner provisioner(secure_sites_path, insecure_passwords_path, input_json_path);
	if (provisioner.Init()) {
		return 1;
	}
	provisioner.Run();
	return 0;
}
