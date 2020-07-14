#pragma once
#include <cassert>

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
    
    