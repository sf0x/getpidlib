#define _WIN32_DCOM
#include "pch.h"
#include "get_pid_wmi.h"

#include <Windows.h>
#include <Wbemidl.h>
#include <comutil.h>
#include <sddl.h> // convertstringtosid
#include <string>
#include <UserEnv.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

using namespace std;

LPWSTR getCurrentSID() {
	HANDLE hToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	DWORD dwLength;
	GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwLength);
	TOKEN_USER* pTokenUser = (TOKEN_USER*)malloc(dwLength);
	GetTokenInformation(hToken, TokenUser, pTokenUser, dwLength, &dwLength);
	LPWSTR pSidString;
	ConvertSidToStringSidW(pTokenUser->User.Sid, &pSidString);
	free(pTokenUser);
	CloseHandle(hToken);
	return pSidString;
}

DWORD getPidWmi(const wchar_t* targetProcName)
{
	HRESULT hres;

	// Current user SID
	LPWSTR localUser = getCurrentSID();

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);


	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	IWbemLocator* pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);


	IWbemServices* pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	BSTR MethodNameB = SysAllocString(L"GetOwnerSid");
	BSTR ClassName = SysAllocString(L"Win32_Process");

	IWbemClassObject* pClass = NULL;
	hres = pSvc->GetObject(ClassName, 0, NULL, &pClass, NULL);

	IWbemClassObject* pmethodGetSid = NULL;
	hres = pClass->GetMethod(MethodNameB, 0, NULL, &pmethodGetSid);

	IWbemClassObject* pInInstB = NULL;
	hres = pmethodGetSid->SpawnInstance(0, &pInInstB);

	IEnumWbemClassObject* pEnumerator = NULL;
	bstr_t query = bstr_t("SELECT * FROM Win32_Process WHERE Caption=\"") + _bstr_t(targetProcName) + bstr_t("\"");
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t(query),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (uReturn == 0)
		{
			break;
		}

		VARIANT vtProp;
		VARIANT vtSid;
		VARIANT vtPid;

		hr = pclsObj->Get(L"Handle", 0, &vtPid, 0, 0);

		hr = pclsObj->Get(L"__PATH", 0, &vtProp, 0, 0);

		hres = pSvc->ExecMethod(vtProp.bstrVal, MethodNameB, 0, NULL, NULL, &pmethodGetSid, NULL);

		hres = pmethodGetSid->Get(L"Sid", 0, &vtSid, NULL, 0);

		if (V_BSTR(&vtSid)) {
			if (!wcscmp(V_BSTR(&vtSid), localUser)) {
				wchar_t* endPtr;
				DWORD dwValue = wcstoul(V_BSTR(&vtPid), &endPtr, 10);
				VariantClear(&vtProp);
				VariantClear(&vtPid);
				VariantClear(&vtSid);
				pclsObj->Release();
				pSvc->Release();
				pLoc->Release();
				pEnumerator->Release();
				CoUninitialize();
				return dwValue;
			}
		}
		VariantClear(&vtProp);
		VariantClear(&vtPid);
		VariantClear(&vtSid);
		pclsObj->Release();

	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return 0;

}
