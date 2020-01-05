#pragma once

#include <string>
#include <vector>
#include <sstream>

#include <ArduinoJson.h>

struct mqtt_data
{
    std::string topic;
    std::string data;
};

class TopicVisitor
{
public:
    TopicVisitor()
    {
        topics = std::vector<std::string>();
        data = std::vector<mqtt_data>();
        deviceID = nullptr;
        setDeviceID = false;
    };

    void visitFloat(const JsonFloat num)
    {
        std::ostringstream ss;
        ss << num;
        addData(ss.str());
    };

    void visitPositiveInteger(JsonUInt num)
    {
        std::ostringstream ss;
        ss << num;
        addData(ss.str());
    };

    void visitNegativeInteger(JsonUInt num)
    {
        std::ostringstream ss;
        ss << '-' << num;
        addData(ss.str());
    };

    void visitObject(const ARDUINOJSON_NAMESPACE::CollectionData &obj)
    {
        ARDUINOJSON_NAMESPACE::VariantSlot *slot = obj.head();
        while (slot != 0)
        {
            if (topics.empty() && strcmp(slot->key(), "deviceID") == 0)
            {
                setDeviceID = true;
                slot->data()->accept(*this);
                setDeviceID = false;
            }

            pushTopic(slot->key());
            slot->data()->accept(*this);
            popTopic();
            slot = slot->next();
            if (slot == 0)
                break;
        }
    };

    void visitString(const char *str)
    {
        if (setDeviceID)
        {
            deviceID = str;
        }
        else
        {
            addData(std::string(str));
        }
    };

    void visitNull(){};
    void visitArray(const ARDUINOJSON_NAMESPACE::CollectionData &arr){};
    void visitBoolean(bool){};
    void visitRawJson(const char *data, size_t size){};

    std::vector<mqtt_data> getData()
    {
        auto out = std::vector<mqtt_data>();
        std::string id(deviceID);

        for (auto item : data)
        {
            if (item.topic == "/deviceID") // topic is prefixed with slash
                continue;

            out.push_back(mqtt_data{topic : id + item.topic, data : item.data});
        }

        return out;
    };

private:
    std::vector<mqtt_data> data;
    std::vector<std::string> topics;
    const char *deviceID;
    bool setDeviceID;

    void addData(std::string str)
    {
        data.push_back(mqtt_data{topic : getTopic(), data : str});
    };

    void pushTopic(const char *topic)
    {
        topics.push_back(std::string(topic));
    };

    void popTopic()
    {
        topics.pop_back();
    };

    std::string getTopic()
    {
        std::string s;
        for (const auto &topic : topics)
            s += "/" + topic;
        return s;
    };
};
