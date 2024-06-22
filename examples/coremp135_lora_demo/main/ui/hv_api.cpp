#include <iostream>

#include "hv/hv.h"
#include "hv/ifconfig.h"
#include "hv/json.hpp"
#include "hv/base64.h"


extern "C" {
int get_IP_address(char* strbuf) {
    std::vector<ifconfig_t> ifcs;
    ifconfig(ifcs);
    for (auto& item : ifcs) {
        if (strlen(item.ip) != 0) {
            strcpy(strbuf, item.ip);
        }
    }
    return 0;
}

int mqtt_Parse(char *topic, int topic_len, char *payload, int payload_len, char **fport, char **DevEUI, char **Freq, char **BW, char **SF, char **Rssi, char **Msg)
{
    static char Staic_fport[128] = {0};
    static char Staic_DevEUI[128] = {0};
    static char Staic_Freq[128] = {0};
    static char Staic_BW[128] = {0};
    static char Staic_SF[128] = {0};
    static char Staic_Rssi[128] = {0};
    static char Staic_Msg[128] = {0};
    *fport = Staic_fport;
    *DevEUI = Staic_DevEUI;
    *Freq = Staic_Freq;
    *BW = Staic_BW;
    *SF = Staic_SF;
    *Rssi = Staic_Rssi;
    *Msg = Staic_Msg;

    std::string stopic = std::string(topic, topic_len);
    std::string spayload = std::string(payload, payload_len);
    // std::cout << "stopic: "<< stopic << std::endl; 
    // std::cout << "spayload: "<< spayload << std::endl; 
    // char app_id[256] = {0};
    // sscanf(stopic.c_str(), "application/%s/device/%s/event/up", app_id, Staic_DevEUI);

    nlohmann::json lora_up_msg = nlohmann::json::parse(spayload);

    // std::cout << "nlohmann::json decode success!"<< std::endl; 
    unsigned long lfport = lora_up_msg["fPort"];
    unsigned long lFreq = lora_up_msg["txInfo"]["frequency"];
    unsigned long lBW = lora_up_msg["txInfo"]["modulation"]["lora"]["bandwidth"];
    unsigned long lSF = lora_up_msg["txInfo"]["modulation"]["lora"]["spreadingFactor"];
    long lRssi = lora_up_msg["rxInfo"][0]["rssi"];
    std::string sDevEUI = lora_up_msg["deviceInfo"]["devEui"];
    std::string sMsg = lora_up_msg["rxInfo"][0]["context"];

    sprintf(Staic_fport, "%d", lfport);

    sprintf(Staic_Freq, "%.1fM", (float)(lFreq / 1000000.0));

    sprintf(Staic_BW, "%dK", lBW / 1000);

    sprintf(Staic_SF, "%d", lSF);

    sprintf(Staic_Rssi, "%d", lRssi);

    sprintf(Staic_DevEUI, "%s", sDevEUI.c_str());

    std::string data = hv::Base64Decode(sMsg.c_str(), sMsg.length());

    char *pStaic_Msg = Staic_Msg;
    for (auto da : data)
    {
        pStaic_Msg += sprintf(pStaic_Msg, "%X ", (uint8_t)da);
    }
    return 0;
}



}


// // {'deduplicationId': '56529216-ceaa-4489-834a-8b6256b685eb', 'time': '2024-03-17T12:24:21.005342822+00:00',
// // 'deviceInfo': {'tenantId': '52f14cd4-c6f1-4fbd-8f87-4025e1d49242', 'tenantName': 'ChirpStack', 'applicationId':
// // '94d9ff1f-6e2e-4e70-bdf0-2cadbdbb4ef0', 'applicationName': 'm5stack', 'deviceProfileId':
// // '8ed2a8fe-c149-4996-bfe8-76f06641be93', 'deviceProfileName': 'm5stack_dev1', 'deviceName': 'm5stack_dev1', 'devEui':
// // '4bee530d29435bef', 'deviceClassEnabled': 'CLASS_C', 'tags': {}}, 'devAddr': '000fbbc2', 'adr': True, 'dr': 5,
// // 'fCnt': 7533, 'fPort': 10, 'confirmed': True, 'data': 'Ed0=', 'rxInfo': [{'gatewayId': '044e5dc71b1c7348',
// // 'uplinkId': 5594, 'nsTime': '2024-03-17T12:24:20.721087572+00:00', 'rssi': -31, 'snr': 9.8, 'channel': 2, 'rfChain':
// // 1, 'location': {}, 'context': 'fbhPrQ==', 'metadata': {'region_common_name': 'EU868', 'region_config_id': 'eu868'},
// // 'crcStatus': 'CRC_OK'}], 'txInfo': {'frequency': 868500000, 'modulation': {'lora': {'bandwidth': 125000,
// // 'spreadingFactor': 7, 'codeRate': 'CR_4_5'}}}}
// {
//     "deduplicationId": "5c8d0f01-1e76-4210-9a27-4853deb2dd98",
//     "time": "2024-04-30T11:21:25.396352377+00:00",
//     "deviceInfo": {
//         "tenantId": "52f14cd4-c6f1-4fbd-8f87-4025e1d49242",
//         "tenantName": "ChirpStack",
//         "applicationId": "94d9ff1f-6e2e-4e70-bdf0-2cadbdbb4ef0",
//         "applicationName": "m5stack",
//         "deviceProfileId": "8ed2a8fe-c149-4996-bfe8-76f06641be93",
//         "deviceProfileName": "m5stack_dev1",
//         "deviceName": "m5stack_dev1",
//         "devEui": "4bee530d29435bef",
//         "deviceClassEnabled": "CLASS_C",
//         "tags": {}
//     },
//     "devAddr": "0046af09",
//     "adr": true,
//     "dr": 5,
//     "fCnt": 90,
//     "fPort": 10,
//     "confirmed": true,
//     "data": "EVo=",
//     "rxInfo": [
//         {
//             "gatewayId": "badc0f76fec939dc",
//             "uplinkId": 17745,
//             "nsTime": "2024-04-30T11:21:25.137745627+00:00",
//             "rssi": -22,
//             "snr": 13.8,
//             "channel": 3,
//             "location": {},
//             "context": "Dbv5KQ==",
//             "metadata": {
//                 "region_common_name": "EU868",
//                 "region_config_id": "eu868"
//             },
//             "crcStatus": "CRC_OK"
//         }
//     ],
//     "txInfo": {
//         "frequency": 867100000,
//         "modulation": {
//             "lora": {
//                 "bandwidth": 125000,
//                 "spreadingFactor": 7,
//                 "codeRate": "CR_4_5"
//             }
//         }
//     }
// }