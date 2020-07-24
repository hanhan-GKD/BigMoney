#include "util.h"
#include <array>
#include <string.h>
#include "port.h"


int StringWidth(const std::string &str) {
  int width = 0;
  for (size_t i = 0; i < str.size();){
    char ch = str[i];
    if (!(ch & 0x80)) {
      width += 1;
      i ++;
    } else {
      width += 2;
      if ((ch & 0xE0) == 0xC0) {
        i += 2;
      } else if ((ch & 0xF0)  == 0xE0) {
        i += 3;
      } else if ((ch & 0xF8) == 0xF0) {
        i += 4;
      } else {
        return -1;
      }
    }
  }
  return width;
}

int FloatWidth(float f, const std::string &format) {
  std::array<char, 255> format_buf;
  snprintf(format_buf.data(), format_buf.size(), format.c_str(), f);
  return strlen(format_buf.data());
}


#ifdef _WIN32
#include <windows.h>
std::string UTF8ToANSI(const std::string &str) {
  BSTR bstr;
  char *psz = nullptr;
  int length;
  char * utf8_str = nullptr;

  length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
  bstr = SysAllocStringLen(nullptr, length);

  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), bstr, length);

  length = WideCharToMultiByte(CP_ACP, 0, bstr, -1, nullptr, 0, nullptr, nullptr);
  utf8_str = new char[length];

  WideCharToMultiByte(CP_ACP, 0, bstr, -1, utf8_str, length, nullptr, nullptr);
  SysFreeString(bstr);

  std::string ret(utf8_str);
  delete[] utf8_str;
  return ret;
}

#endif
