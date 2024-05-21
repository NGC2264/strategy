#include "mqttlib/MqttLogPublisher.h"

// 过滤无效的 UTF-8 字符
// std::string filterInvalidUTF8(const std::string& input) {
//     std::string filtered;
//     for (char ch : input) {
//         if (isValidUTF8Byte(static_cast<unsigned char>(ch))) {
//             filtered += ch;  // 有效的 UTF-8 字符，保留
//         }
//     }
//     return filtered;
// }

MqttLogPublisher::MqttLogPublisher(const std::string &serverAddr, const std::string &clientId,const std::string& logTopic)
    : serverAddress(serverAddr), clientId(clientId), client(serverAddress, clientId),logTopic(logTopic)
{
    // 设置回调函数等初始化操作
    // ...
}

void MqttLogPublisher::logconnect()
{
    mqtt::connect_options connOpts;
    // 设置连接选项
    // ...
    connOpts.set_clean_session(false);
    connOpts.set_user_name(USER_NAME);
    connOpts.set_password(PASSWORD);
    // client.connect(connOpts)->wait();
    client.connect(connOpts);
}

void MqttLogPublisher::disconnect()
{
    client.disconnect()->wait();
}

// void MqttLogPublisher::publishLogWithDetails(const std::string &timestamp, const std::string &level, const std::string &message, const std::string &sourceItem, const std::string &logsourceId)
// {
//     nlohmann::json jsonData;
//     jsonData["ts"] = timestamp;
//     jsonData["level"] = level;
//     jsonData["message"] = message;
//     jsonData["sourceItem"] = sourceItem;
//     jsonData["logsourceId"] = logsourceId;
//     std::string jsonStr = jsonData.dump(4);

//     mqtt::message_ptr pubmsg = mqtt::make_message("mg/log", "hello");
//     client.publish(pubmsg);
// }

void MqttLogPublisher::publishLogWithDetails(const std::string &timestamp, const std::string &level, const std::string &message, const std::string &sourceItem, const std::string &logsourceId)
{
    // 检查是否已经连接，如果没有，则尝试连接
    if (!client.is_connected()) {
        logconnect();
        if (!client.is_connected()) {
            std::cerr << "Failed to connect to MQTT broker. Cannot publish message." << std::endl;
            return;
        }
    }

    nlohmann::json jsonData;
    jsonData["ts"] = timestamp;
    jsonData["level"] = level;
    jsonData["message"] = message;
    jsonData["sourceItem"] = sourceItem;
    jsonData["logsourceId"] = logsourceId;
    std::string jsonStr = jsonData.dump(4);

    // 修正消息内容，使用jsonStr而不是"hello"
    mqtt::message_ptr pubmsg = mqtt::make_message(logTopic, jsonStr);
    client.publish(pubmsg); // 发布消息并等待操作完成
}

// int main() {
//     std::string serverAddress = "tcp://192.168.122.17:1883";
//     std::string clientId = "mglog";
//     std::string topic = "mg/log";
//     std::string userName = "nyw";
//     std::string passWord = "nyw123456";

//     MqttLogPublisher logPublisher(serverAddress, clientId, topic);
//     logPublisher.connect(userName,passWord);

//     // 发布包含详细信息的日志消息
//     std::string timestamp = "1709014426";
//     std::string level = "ERROR";
//     std::string message = "PCS voltage store failed";
//     std::string sourceItem = "InsertYC2TD()";
//     std::string logsourceId = "40010";
//     while (1)
//     {
//         std::this_thread::sleep_for(std::chrono::seconds(5));
//         logPublisher.publishLogWithDetails(timestamp, level, message, sourceItem, logsourceId);

//     }

//     logPublisher.disconnect();

//     return 0;
// }