#ifndef JSON_PACK_
#define JSON_PACK_


#include <iostream>
#include <memory>
#include <unordered_map>
#include <functional>
#include "nlohmann_json.hpp"
#include <optional>

using json_t = nlohmann::json;

template <typename T>
void assign_if_exists(const json_t& j, const std::string& key, std::optional<T>& opt);


class __Data {
public:
    virtual ~__Data() = default;

    virtual void from_json(const json_t& j) = 0;

    virtual void to_json(json_t& j) const = 0;

};

class _Code : public __Data
{
    
public:
    std::optional<int> _enum;
    std::optional<std::string> name;
    _Code() {};
    _Code(int _enum, std::string name) : _enum(_enum), name(name) {}
    void from_json(const json_t& j) override 
    {
        assign_if_exists(j, "enum", _enum);
        assign_if_exists(j, "name", name);
    }
    void to_json(json_t& j) const override 
    {
        if(_enum.has_value()) j["enum"] = _enum.value();
        if(name.has_value()) j["name"] = name.value();
    }

};

class DataConfigBody: public __Data {
private:

public:
    std::optional<std::string> value;
    std::optional<std::string> action;
    std::optional<std::string> enable;
    std::optional<std::string> soft_version;
    std::optional<std::string> hard_version;
    std::optional<std::string> serial_number;
    std::optional<std::vector<std::string>> rtsp_url;
    std::optional<std::string> type;

    void from_json(const json_t& j) override 
    {
        assign_if_exists(j, "value", value);
        assign_if_exists(j, "action", action);
        assign_if_exists(j, "enable", enable);
        assign_if_exists(j, "soft_version", soft_version);
        assign_if_exists(j, "hard_version", hard_version);
        assign_if_exists(j, "serial_number", serial_number);
        assign_if_exists(j, "rtsp_url", rtsp_url);
        assign_if_exists(j, "type", type);
    }

    void to_json(json_t& j) const override 
    {
        if(value.has_value()) j["value"] = value.value();
        if(action.has_value()) j["action"] = action.value();
        if(enable.has_value()) j["enable"] = enable.value();
        if(soft_version.has_value()) j["soft_version"] = soft_version.value();
        if(hard_version.has_value()) j["hard_version"] = hard_version.value();
        if(serial_number.has_value()) j["serial_number"] = serial_number.value();
        if(rtsp_url.has_value()) j["rtsp_url"] = rtsp_url.value();
        if(type.has_value()) j["type"] = type.value();
    }
};

class DataConfigResponse : public __Data {

private:

public:
    std::optional<_Code> code;
    std::optional<std::string> method;
    std::optional<std::string> status;
    DataConfigResponse() {};
    DataConfigResponse(_Code code, std::string method, std::string status) : code(code), method(method), status(status) {}
    void from_json(const json_t& j) override 
    {
        assign_if_exists(j, "value", code);
        assign_if_exists(j, "method", method);
        assign_if_exists(j, "status", status);
    }

    void to_json(json_t& j) const override 
    {
        if(code.has_value()) code->to_json(j["code"]); 
        if(method.has_value()) j["method"] = method.value();
        if(status.has_value()) j["status"] = status.value();
    }
};

class DataReportBody : public __Data{
private:

public:
    std::optional<std::string> type;
    std::optional<std::string> state;
    std::optional<std::string> time;

    void from_json(const json_t& j) override 
    {
        assign_if_exists(j, "type", type);
        assign_if_exists(j, "state", state);
        assign_if_exists(j, "time", time);
    }

    void to_json(json_t& j) const override 
    {
        if(type.has_value()) j["type"] = type.value();
        if(state.has_value()) j["state"] = state.value();
        if(time.has_value()) j["time"] = time.value();
    }
};

class ExternData {
public:
    virtual ~ExternData() = default;

    virtual void from_json(const json_t& j) = 0;

    virtual void to_json(json_t& j) const = 0;

};

class JsonConfigExt : public ExternData{
public:
    std::optional<_Code> code;
    std::optional<std::string> method;
    std::optional<DataConfigBody> data;
    JsonConfigExt() {};
    JsonConfigExt(_Code code, std::string method) : code(code), method(method) {}
    void from_json(const json_t& j) 
    {
        assign_if_exists(j, "code", code);
        assign_if_exists(j, "method", method);
        assign_if_exists(j, "data", data);
    }

    void to_json(json_t& j) const 
    {
        if(code.has_value()) code->to_json(j["code"]); 
        if(method.has_value()) j["method"] = method.value();
        if(data.has_value()) data->to_json(j["data"]);
    }
};

class JsonReportExt : public ExternData{
public:
    std::optional<_Code> code;
    std::optional<std::string> method;
    std::optional<DataReportBody> data;
    JsonReportExt() {};
    JsonReportExt(_Code code, std::string method) : code(code), method(method) {}
    
    void from_json(const json_t& j) 
    {
        assign_if_exists(j, "code", code);
        assign_if_exists(j, "method", method);
        assign_if_exists(j, "data", data);
    }

    void to_json(json_t& j) const 
    {
        if(code.has_value()) code->to_json(j["code"]); 
        if(method.has_value()) j["method"] = method.value();
        if(data.has_value()) data->to_json(j["data"]);
    }
};



template <typename T>
int JsonPackData(T& opt, std::string &json_string)
{
    json_t j;
    opt.to_json(j);
    // if constexpr (std::is_same<T, JsonConfigExt>::value) 
    // {
       
    // }
    // else if constexpr (std::is_same<T, JsonReportExt>::value)
    // {

    // }
    json_string = j.dump();
    return 0;
}

template <typename T>
int JsonParseData(T& opt, std::string &json_string)
{
    json_t j;
    j = json_t::parse(json_string);
    if(j.is_discarded())
    {
        printf("json parse is err \n");
        return -1;
    }
    //if constexpr (std::is_same<T, JsonConfigExt>::value) 
    {
        opt.from_json(j);
    }

    return 0;
}


template <typename T>
void assign_if_exists(const json_t& j, const std::string& key, std::optional<T>& opt) 
{
    if (j.contains(key)) 
    {
        if constexpr (std::is_same<T, int>::value) 
        {
            opt = j[key].get<int>();
        }
        else if constexpr (std::is_same<T, std::string>::value) 
        {
            opt = j[key].get<std::string>();
        }
        else if constexpr(std::is_same<T, DataConfigBody>::value)
        {
            opt = DataConfigBody();
            opt->from_json(j[key]);
        }
        else if constexpr(std::is_same<T, DataConfigResponse>::value)
        {
            opt = DataConfigResponse();
            opt->from_json(j[key]);
        }
        else if constexpr(std::is_same<T, DataReportBody>::value)
        {
            opt = DataReportBody();
            opt->from_json(j[key]);
        }
        else if constexpr(std::is_same<T, std::vector<std::string>>::value)
        {
            opt = j[key].get<std::vector<std::string>>();
        }
        else if constexpr(std::is_same<T, _Code>::value)
        {
            opt = _Code();
            opt->from_json(j[key]);
        }
    }
}

#endif