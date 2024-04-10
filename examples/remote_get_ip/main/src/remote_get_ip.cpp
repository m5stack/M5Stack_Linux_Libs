#include "hv/hlog.h"
#include "hv/mqtt_client.h"
#include "hv/ifconfig.h"
#include <functional>
#include <vector>
#include <string>

hv::MqttClient push_ip;

void mqtt_connect_callback(hv::MqttClient *cli)
{
    if (!cli->isConnected())
    {
        hlogi("connect faile");
    }
    else
    {
        cli->subscribe("/nihao");
    }
}
void mqtt_message_callback(hv::MqttClient *cli, mqtt_message_t *msg)
{
    // hlogi("Recieve a message of %.*s: %.*s.", msg->topic_len, (char *)msg->topic, msg->payload_len, (char *)msg->payload);
    std::string tmp_payload = std::string((char *)msg->payload, msg->payload_len);
    std::string tmp_topic = std::string((char *)msg->topic, msg->topic_len);

    std::vector<ifconfig_t> ifcs;
    ifconfig(ifcs);
    if (tmp_topic == "/get")
    {
        for (auto &item : ifcs)
        {
            char tmpbuf[256];
            sprintf(tmpbuf, "%s\nip: %s\nmask: %s\nbroadcast: %s\nmac: %s\n\n",
                    item.name,
                    item.ip,
                    item.mask,
                    item.broadcast,
                    item.mac);
            cli->publish(std::string("/get_ip"), std::string(tmpbuf));
        }
    }
}

int main()
{
    hlog_set_level(LOG_LEVEL_INFO);
    // hlog_set_level(LOG_LEVEL_SILENT);
    push_ip.onConnect = std::bind(&mqtt_connect_callback, std::placeholders::_1);
    push_ip.onClose = std::bind(&mqtt_connect_callback, std::placeholders::_1);
    push_ip.onMessage = std::bind(&mqtt_message_callback, std::placeholders::_1, std::placeholders::_2);
    push_ip.setID("asdxejzass");
    push_ip.setAuth("xxx", "xxx");
    push_ip.setPingInterval(30);
    push_ip.connect("xxx", 12524);

    reconn_setting_t parm;
    parm.max_delay = 10000;
    push_ip.setReconnect(&parm);
    push_ip.run();

    return 0;
}
