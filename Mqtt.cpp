#include "Mqtt.h"

#include <iostream>

#pragma comment(lib, "mosquittopp.lib")

Mqtt::Mqtt(const char* id, const char* host, int port, Mappings&& mappings)
: mosquittopp(id, false /*clean_session*/)
, m_mappings(std::move(mappings))
{
	mosqpp::lib_init();
	connect_async(host, port);
	loop_start();
}

Mqtt::~Mqtt()
{
	loop_stop();
	mosqpp::lib_cleanup();
}

void Mqtt::publish(const char * topic, const void* payload, int payloadLength)
{
	int qos = 1; // At least once
	bool retain = false;

	mosqpp::mosquittopp::publish(nullptr, topic, payloadLength, payload, qos, retain);
}

void Mqtt::on_connect(int rc)
{
	if (rc == 0)
	{
		for (auto&& mapping : m_mappings)
		{
			const char* topic = mapping.first.c_str();
			int qos = 1; // At least once

			subscribe(NULL, topic, qos);
		}
	}
}

void Mqtt::on_message(const mosquitto_message * message)
{
	auto it = m_mappings.find(message->topic);
	if (it == m_mappings.end())
		return;

	// Found mapping
	it->second(message->topic, message->payload, message->payloadlen);
}

void Mqtt::on_log(int /*level*/, const char *str)
{
	printf("MQTT %s\n", str);
}
