// court.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <chrono>  
#include <ctime>   
#include <sstream> 
#include <iomanip> 
#include <string>  


// base 64 encoder 
static const unsigned char base64_table[65] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
* base64_encode - Base64 encode
* @src: Data to be encoded
* @len: Length of the data to be encoded
* @out_len: Pointer to output length variable, or %NULL if not used
* Returns: Allocated buffer of out_len bytes of encoded data,
* or empty string on failure
*/
std::string base64_encode(const unsigned char* src, size_t len)
{
    unsigned char* out, * pos;
    const unsigned char* end, * in;

    size_t olen;

    olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

    if (olen < len)
        return std::string(); /* integer overflow */

    std::string outStr;
    outStr.resize(olen);
    out = (unsigned char*)&outStr[0];

    end = src + len;
    in = src;
    pos = out;
    while (end - in >= 3) {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) |
                (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
    }

    return outStr;
}

std::string get_date(tm newtime)
{
    char date[100]; 
    std::strftime(date, sizeof(date), "%Y-%m-%d", &newtime);
    std::string ret = date;
 
    const unsigned char* u_str = (const unsigned char*)ret.c_str();
    return base64_encode(u_str, ret.size());
}

std::string get_causelist(tm newtime)
{
    char date[100];
    std::strftime(date, sizeof(date), "%d%m%Y", &newtime);

    std::string ret = std::string("cause_" + std::string(date) + ".xml");
    const unsigned char* u_str = (const unsigned char *)ret.c_str();
    return base64_encode(u_str, ret.size());
}

std::string get_final_url(std::string name)
{
    struct tm newtime;
    time_t now = time(0);
    localtime_s(&newtime, &now);
    if (newtime.tm_wday == 0)
        newtime.tm_mday += 1;
    else if (newtime.tm_wday == 6)
        newtime.tm_mday += 2;
    std::string base_url = "https://mhc.tn.gov.in/judis/clists/clists-madras/views/a.php?result=";

    std::string date = get_date(newtime);
    std::string cause_list = get_causelist(newtime);
   
    std::string res = base_url + cause_list + "&cdate=" + date + "&ft=2" + "&fil=" + name;
    return res;
}
int main(int argc, char *argv[])
{
    std::string name = "test";     // change name!
    std::string final_url = get_final_url(name);

    std::wstringstream cls;
    cls << final_url.c_str();
    std::wstring w_final= cls.str();

    ShellExecute(0, 0, w_final.c_str() , 0, 0, SW_SHOW);
}
