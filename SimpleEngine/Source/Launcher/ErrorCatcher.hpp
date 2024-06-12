#pragma once
#include "External/nlohmann/json.hpp"
#include <Windows.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Strsafe.h>
#include <fstream>

static inline const std::string GetAbsolutePath(const char* aFilePath)
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	const std::string exePath(buffer);
	const std::string outputPath = exePath.substr(0, exePath.find_last_of("\\/")) + "\\" + aFilePath;

	return outputPath;
}

static void ShowErrorPopup(const std::string aPath)
{
	std::string error = "An unexpected error occurred. The program will now exit. A minidump file has been created at ";
	error += aPath;

	std::wstring message(error.begin(), error.end());
	MessageBox(nullptr, message.c_str(), L"Error", MB_ICONERROR | MB_OK);
}

static BOOL CALLBACK MiniDumpCallback(PVOID /*CallbackParam*/, const PMINIDUMP_CALLBACK_INPUT /*CallbackInput*/, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
{
	CallbackOutput->ModuleWriteFlags |= ModuleWriteDataSeg;
	return TRUE;
}

static int CreateMiniDump(EXCEPTION_POINTERS* someExceptionPointers)
{
	const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_ENGINE);

	std::ifstream file(filename);
	assert(file.is_open() && "Failed To Open File");

	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	const nlohmann::json& engineSettings = json["engine_settings"];

	const std::string engineNameString = engineSettings["engine_name"];
	const std::string engineVersionString = engineSettings["version"];

	const std::wstring engineNameWide = std::wstring(engineNameString.begin(), engineNameString.end());
	const std::wstring engineVersionWide = std::wstring(engineVersionString.begin(), engineVersionString.end());

	WCHAR szPath[MAX_PATH];
	WCHAR szFileName[MAX_PATH];
	WCHAR szAppName[MAX_PATH];
	WCHAR szVersion[MAX_PATH];

	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;

	const std::string stringPath = GetAbsolutePath(SIMPLE_DIR_MINIDUMP);
	const std::wstring widePath = std::wstring(stringPath.begin(), stringPath.end());

	wcsncpy_s(szPath, widePath.c_str(), MAX_PATH - 1);
	wcsncpy_s(szAppName, engineNameWide.c_str(), MAX_PATH - 1);
	wcsncpy_s(szVersion, engineVersionWide.c_str(), MAX_PATH - 1);

	szPath[MAX_PATH - 1] = L'\0';
	szAppName[MAX_PATH - 1] = L'\0';
	szVersion[MAX_PATH - 1] = L'\0';

	GetLocalTime(&stLocalTime);

	// Ensure the directory exists
	CreateDirectory(szPath, NULL);

	StringCchPrintf(
		szFileName,
		MAX_PATH,
		L"%s%s_%s_%04d%02d%02d_%02d%02d%02d_%d_%d.dmp",
		szPath,
		szAppName,
		szVersion,
		stLocalTime.wYear,
		stLocalTime.wMonth,
		stLocalTime.wDay,
		stLocalTime.wHour,
		stLocalTime.wMinute,
		stLocalTime.wSecond,
		GetCurrentProcessId(),
		GetCurrentThreadId());

	hDumpFile = CreateFile(
		szFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	MINIDUMP_EXCEPTION_INFORMATION expParam = {};
	expParam.ThreadId = GetCurrentThreadId();
	expParam.ExceptionPointers = someExceptionPointers;
	expParam.ClientPointers = TRUE;

	MINIDUMP_CALLBACK_INFORMATION callbackInfo = {};
	callbackInfo.CallbackRoutine = MiniDumpCallback;
	callbackInfo.CallbackParam = NULL;

	const BOOL bMiniDumpSuccessful = MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		MiniDumpNormal,
		&expParam,
		NULL,
		&callbackInfo);

	CloseHandle(hDumpFile);

	UNREFERENCED_PARAMETER(bMiniDumpSuccessful);

	ShowErrorPopup(stringPath);

	return EXCEPTION_EXECUTE_HANDLER;
}

static LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* exceptionPointers)
{
	CreateMiniDump(exceptionPointers);
	return EXCEPTION_CONTINUE_SEARCH;
}