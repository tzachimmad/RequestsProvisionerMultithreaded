//============================================================================
// Name        : UserData.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <sstream>
#include <type_traits>
#include "date/date.h"
#include "date/tz.h"

#include "JsonParser.h"

#include "ReqProcessor.h"

using namespace std;
using namespace std::chrono;

void UserRequest::IncAnalysis(int inc) {
	if (inc == 1 && (m_analysis == 1 || m_analysis == 3)) {
		return;
	}
	m_analysis += inc;
}

int JsonParser::Init() {
	m_f_input_json.open(m_path.c_str(), ios::in);
	if (!m_f_input_json) {
		cout << "Incorreqt input json path, aborting" << endl;
		return 1;
	}
	m_initiated = true;
	return 0;
}

shared_ptr<UserRequest> JsonParser::GetNextReq() {
	string line;
	while(getline(m_f_input_json, line))
	{
		if(line.find("]") != std::string::npos ) {
			break;
		}
		if (line.find("{") != std::string::npos) {
			m_pos = 0;
		}
		if (line.find("}") != std::string::npos) {
			string tmp = "}\0";
			copy(tmp.c_str(), tmp.c_str()+tmp.length(), m_buffer.begin() + m_pos);
			m_pos = 0;
			auto ptr = ParseReq(m_req_num, &m_buffer[m_pos]);
			if (ptr != NULL) {
				m_req_num +=1;
				return ptr;
			}
			break;

		}
		copy(line.c_str(), line.c_str()+line.length(), m_buffer.begin() + m_pos);
		m_pos += line.length();
	}
	return NULL;
}

shared_ptr<UserRequest> JsonParser::ParseReq(int id, const char* buff) {
	Json::Reader reader;
	Json::Value elem;
	reader.parse(buff,elem);
	string str_timestamp = elem["timestamp"].asString();
	string host = elem["host"].asString();
	string url = elem["url"].toStyledString();
	string comment = elem["comment"].toStyledString();
	string content = elem["content"].toStyledString();
	string password = "";
	string tokenid;
	string username = "";
	string tmp;
	TIME_POINT_T timestamp;
	std::stringstream ss_timestamp{str_timestamp};
	ss_timestamp >> date::parse("%FT%T", timestamp);
	content = content.substr(1, content.size() - 2);
	stringstream ss(content);
	while(getline(ss, tmp, '&')){
		if (tmp.empty()) {
			continue;
		}
		if (tmp.find("user=") != std::string::npos) {
			username = tmp.substr(tmp.find("=") + 1);
		}
		else if (tmp.find("tokenid=") != std::string::npos) {
			tokenid = tmp.substr(tmp.find("=") + 1);
		}
		else if (tmp.find("password=") != std::string::npos) {
			password = tmp.substr(tmp.find("=") + 1);
		}
	}
	if (!url.empty() || !host.empty() || timestamp == TIME_POINT_T()) {
		auto ptr = std::make_shared<UserRequest>(id, username, password, host, url, tokenid, comment, str_timestamp, timestamp);
		return ptr;
	}
	return NULL;
}




