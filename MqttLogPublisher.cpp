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
    // connOpts.set_clean_session(false);
    // connOpts.set_user_name(userName);
    // connOpts.set_password(password);
    // client.connect(connOpts)->wait();

    connOpts.set_keep_alive_interval(30); // 设置心跳包发送间隔为30秒
    connOpts.set_clean_session(false);
    connOpts.set_user_name(userName);
    connOpts.set_password(password);
    connOpts.set_automatic_reconnect(true); // 启用自动重连
    connOpts.set_connect_timeout(5);        // 设置连接超时时间为5秒

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
    // if (!client.is_connected()) {
    //     logconnect();
    //     if (!client.is_connected()) {
    //         std::cerr << "Failed to connect to MQTT broker. Cannot publish message." << std::endl;
    //         return;
    //     }
    // }

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
