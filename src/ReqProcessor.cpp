/*
 * EventProcessor.cpp
 *
 *  Created on: 15 Nov 2022
 *      Author: tzachi
 */
#include "ReqProcessor.h"

#include <iostream>
#include <chrono>
#include "date/date.h"
#include "date/tz.h"
#include "JsonParser.h"

using namespace std;
using namespace std::chrono;

typedef duration<long, std::ratio<1, 1000>> ms_rate;

int ReqProcessor::PrintAllreqs() {
	for (auto req : m_user_req_list) {
		cout << req->GetStrTimestamp() << "     result: " << req->GetAnalysis()  << endl;
	}
	return 0;
}

int ReqProcessor::Start() {
	bool ret = EventQueue::Start();
	return ret ? 0 : 1;
}

int ReqProcessor::Stop() {
	bool ret = EventQueue::Stop();
	return ret ? 0 : 1;
}

int ReqProcessor::UnCacheOldReqs(const std::chrono::system_clock::time_point &timestamp) {
	nanoseconds ten_sec_ns(10000000000);
	while (m_user_req_list.size() && (timestamp - m_user_req_list.front()->GetTimestamp() > ten_sec_ns)) {
		auto user_req = m_user_req_list.front();
		m_user_req_list.pop_front();
		string username = user_req->GetUserName();
		string password = user_req->GetPassword();
		string host = user_req->GetHost();
		stringstream ss_msg;
		ss_msg << "ts: " << user_req->GetStrTimestamp() << "     result: " << user_req->GetAnalysis()  << endl;
		user_req->SetOutMsg(ss_msg.str());
		user_req->SetProcessed();
		if (username.empty() || password.empty()) {
			return 0;
		}
		m_user_map[username][password][host].pop_front();
		if (m_user_map[username][password][host].empty()) {
			m_user_map[username][password].erase(host);
		}
		if (m_user_map[username][password].empty()) {
			m_user_map[username].erase(password);
		}
		if (m_user_map[username].empty()) {
			m_user_map.erase(username);
		}
	}
	return 0;
}

int ReqProcessor::CacheReq(std::shared_ptr<UserRequest> user_req, const string &username, const string &password, const string &host) {
	m_user_req_list.push_back(user_req);
	if (username.empty() || password.empty()) {
		return 0;
	}
	if (m_user_map.count(username)) {
		if (m_user_map[username].count(password)) {
			if (m_user_map[username][password].count(host)) {
				m_user_map[username][password][host].push_back(user_req);
			}
			else {
				REQ_LIST_T tmp_list;
				tmp_list.push_back(user_req);
			    m_user_map[username][password][host] = tmp_list;
			}
		}
		else {
			REQ_LIST_T tmp_list;
			tmp_list.push_back(user_req);
			HOST_reqORD_MAP_T tmp_map;
			tmp_map[host] = tmp_list;
			m_user_map[username][password] = tmp_map;
		}
	}
	else {
		REQ_LIST_T tmp_list;
		tmp_list.push_back(user_req);
		HOST_reqORD_MAP_T tmp_host_map;
		tmp_host_map[host] = tmp_list;
		map<string, HOST_reqORD_MAP_T> tmp_password_map;
		tmp_password_map[password] = tmp_host_map;
		m_user_map[username] = tmp_password_map;
	}
	return 0;
}

int ReqProcessor::ProcessReq(std::shared_ptr<UserRequest> user_req) {
	string username = user_req->GetUserName();
	string password = user_req->GetPassword();
	string host = user_req->GetHost();
	string timestampstr = user_req->GetStrTimestamp();
	auto timestamp = user_req->GetTimestamp();
	if (username.empty()) {
		m_user_req_list.push_back(user_req);
		return 0;
	}
	if (m_insecure_passwords.count(password)  && m_secure_hosts.count(host)) {
		stringstream ss_msg;
		ss_msg << "Common password found: " << password << " on secure host " <<  host << endl;
		shared_ptr<StringWrapper> ptr = make_shared<StringWrapper>(ss_msg.str());
		m_msg_queue.push(ptr);
		user_req->IncAnalysis(2);
	}
	UnCacheOldReqs(timestamp);
	nanoseconds ten_sec_ns(10000000000);
	for (auto &host_map_elem : m_user_map[username][password]) {
		if (host_map_elem.first == host) {
			continue;
		}
		auto diff = timestamp - host_map_elem.second.front()->GetTimestamp();
		if (diff > ten_sec_ns) {
			continue;
		}
		using date::operator<<;
		stringstream ss_msg;
		ss_msg << "Identical User/Password pair found diff under 10s: duration "  << floor<ms_rate>(diff) << " in host " << host << " user/pass: " << username << "/" << password << endl;
		shared_ptr<StringWrapper> ptr = make_shared<StringWrapper>(ss_msg.str());
		m_msg_queue.push(ptr);
		for (auto req : host_map_elem.second) {
			user_req->IncAnalysis(1);
			req->IncAnalysis(1);
		}

	}
	CacheReq(user_req, username, password, host);
	return 0;
}

bool ReqProcessor::AddReq(std::shared_ptr<UserRequest> user_req) {
	ReqEvent *req_event = new ReqEvent(this, user_req);
	bool ret = EventQueue::AddEvent(req_event);
	if (!ret)
	{
		delete req_event;
	}
	return ret;
}

void ReqProcessor::ReqEvent::Execute()
{
	m_req_proccessor->ProcessReq(m_user_req);
}
