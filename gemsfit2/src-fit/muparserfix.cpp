#include <string>
#include <vector>
#include <muParser.h>
#include "muparserfix.h"

#if defined(_UNICODE)
#include <locale>
#include <codecvt>
#include <boost/locale.hpp>

std::string ws2s(const std::wstring& wstr)
{
//    using convert_typeX = std::codecvt_utf8<wchar_t>;
//    std::wstring_convert<convert_typeX, wchar_t> converterX;

    std::string s = boost::locale::conv::utf_to_utf<char>(wstr);

//    return converterX.to_bytes(wstr);
    return s;
}

std::wstring s2ws(const std::string& str)
{
//    cout << str << endl << "AAAA" << endl;
//    using convert_typeX = std::codecvt_utf8<wchar_t>;
//    std::wstring_convert<convert_typeX, wchar_t> converterX;

//    wstring wstr = converterX.from_bytes(str);

    std::wstring ws = boost::locale::conv::utf_to_utf<wchar_t>(str);

//    cout << ws2s(ws)<< endl << "BBBB" << endl;

    return ws;

//    wstring wstr (str.begin(), str.end());

//    return wstr;
}
#else

#endif


#if defined(_UNICODE)
double* AddVariable(const wchar_t *a_szName, void *pUserData)
#else
double* AddVariable(const char *a_szName, void *pUserData)
#endif
{
   static double afValBuf[500];
   int iVal = -1;
#if defined(_UNICODE)
  std::vector<std::wstring> *test = reinterpret_cast<std::vector<std::wstring> *>(pUserData);
#else
  std::vector<std::string> *test = reinterpret_cast<std::vector<std::string> *>(pUserData);
#endif
  iVal++;
  test->push_back(a_szName);
  afValBuf[iVal] = 0;

  if (iVal>=499)
#if defined(_UNICODE)
    throw mu::ParserError(L"Variable buffer overflow.");
#else
    throw mu::ParserError("Variable buffer overflow.");
#endif
  else
    return &afValBuf[iVal];
}
