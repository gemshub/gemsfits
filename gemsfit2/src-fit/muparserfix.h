#ifndef MUPARSERFIX_H
#define MUPARSERFIX_H

#if defined(_UNICODE)
#include <locale>
#include <codecvt>
#include <string>

string ws2s(const std::wstring& wstr);

wstring s2ws(const std::string& str);
#else
#endif

#if defined(_UNICODE)
double* AddVariable(const wchar_t *a_szName, void *pUserData);
#else
double* AddVariable(const char *a_szName, void *pUserData);
#endif

#endif // MUPARSERFIX_H
