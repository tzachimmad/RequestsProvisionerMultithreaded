/*
 * JsonParser.h
 *
 *  Created on: 14 Nov 2022
 *      Author: tzachi
 */

#ifndef USERDATA_H_
#define USERDATA_H_

#include <string>
#include <list>
#include <memory>
#include <chrono>
#include <atomic>

typedef std::chrono::system_clock::time_point TIME_POINT_T;

class UserRequest {
public:
	UserRequest(int id, const std::string &username, const std::string &password, const std::string &host, const std::string url, std::string tokenid, std::string comment, const std::string &str_timestamp, const TIME_POINT_T& timestamp)
		: m_id(id), m_username(username), m_password(password), m_host(host), m_url(url), m_tokenid(tokenid), m_comment(comment), m_str_timestamp(str_timestamp), m_timestamp(timestamp), m_out_msg(), m_proccessed(false), m_analysis(0){}
	~UserRequest(){}
	const std::string& GetUserName() {return m_username;}
	const std::string& GetPassword() {return m_password;}
	const std::string& GetHost() {return m_host;}
	const std::string& GetUrl() {return m_url;}
	const std::string& GetTokenId() {return m_tokenid;}
	const std::string& GetComment() {return m_comment;}
	const std::string& GetStrTimestamp() {return m_str_timestamp;}
	const std::string& GetOutMsg() {return m_out_msg;}
	int GetId(){return m_id;}
	void SetProcessed(){m_proccessed = true;}
	bool IsProccessed(){return m_proccessed;}
	void  SetOutMsg(const std::string& msg) {m_out_msg = msg;}
	const std::chrono::system_clock::time_point& GetTimestamp() {return m_timestamp;}
	int GetAnalysis() {return m_analysis;}
	void IncAnalysis(int inc);

private:
	int m_id;
	std::string m_username;
	std::string m_password;
	std::string m_host;
	std::string m_url;
	std::string m_tokenid;
	std::string m_comment;
	std::string m_str_timestamp;
	std::string m_out_msg;
	std::atomic<bool> m_proccessed;
	TIME_POINT_T m_timestamp;
	int m_analysis;
};

class JsonParser {
public:
	JsonParser(){}
	virtual ~JsonParser(){}
	std::shared_ptr<UserRequest> ParseReq(int id, const char* buff);
};


#endif /* USERDATA_H_ */
