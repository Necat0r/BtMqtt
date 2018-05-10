#pragma once

#include <functional>
#include <mosquittopp.h>
#include <map>

class Mqtt : private mosqpp::mosquittopp
{
public:
	// Topic, payload & payload length
	using Callback = std::function<void(const char*, const void*, int)>;
	using Mappings = std::map<std::string, Callback>;

	Mqtt(const char * id, const char * host, int port, Mappings&& mappings);
	~Mqtt();

	void publish(const char* topic, const void* payload = 0, int payloadLength = 0);

	virtual void on_connect(int rc) override;
	virtual void on_message(const struct mosquitto_message * message) override;
	virtual void on_log(int level, const char * str) override;

private:
	Mappings m_mappings;
};
