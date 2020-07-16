#pragma once
#include <cassert>
#include <sstream>

#define JSON_GET(type, key, v, json)\
    do {\
    auto itr = (json).FindMember(key);\
    if (itr != (json).MemberEnd()) {\
        if (itr->value.Is##type()) {\
            (v) = itr->value.Get##type();\
        } else {\
            assert(0 && "Type mismatch, get json value fail: "#key);\
        }\
    } else {\
        assert(0 && "Cannot find key, get json value fail: "#key);\
    }\
    }while (0)

#include <string>

std::string UTF8ToANSI(const std::string &str);


#define FUND_DATA_URL "http://fundgz.1234567.com.cn/js/"

inline std::string GenerateFundUrl(const std::string& code) {
    std::ostringstream os;
    os << FUND_DATA_URL << code << ".js";
    return os.str();
}

int StringWidth(const std::string &str) ;

int FloatWidth(float f);
