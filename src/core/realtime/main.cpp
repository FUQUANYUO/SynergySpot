#include "RealtimeCommHandler.h"

#include "trtc-realtime-comm/video-audio-call-page/VideoAudioInvitePage.h"
#include "trtc-realtime-comm/video-audio-call-page/VideoAudioCallPage.h"

#include <ela-widget-tools/ElaApplication.h>

#include "help.h"

#include <QApplication>

#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

// 异常处理函数
LONG WINAPI GenerateDump(struct _EXCEPTION_POINTERS* pExceptionPointers)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    // 生成带时间戳的dump文件名
    wchar_t dumpPath[MAX_PATH];
    swprintf_s(dumpPath, L"crash_%04d%02d%02d_%02d%02d%02d.dmp",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    HANDLE hFile = CreateFileW(dumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) return EXCEPTION_EXECUTE_HANDLER;

    MINIDUMP_EXCEPTION_INFORMATION ei;
    ei.ThreadId = GetCurrentThreadId();
    ei.ExceptionPointers = pExceptionPointers;
    ei.ClientPointers = TRUE;

    // 写入完整dump信息
    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        static_cast<MINIDUMP_TYPE>(MiniDumpWithFullMemory |
                                  MiniDumpWithHandleData |
                                  MiniDumpWithThreadInfo),
        &ei,
        NULL,
        NULL);

    CloseHandle(hFile);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[]){
    SetUnhandledExceptionFilter(GenerateDump);

    QApplication a(argc,argv);
    eApp->init();
    // enable mica style
    eApp->setIsEnableMica(true);

    std::string logName = "SynergySpot_gRPC";
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Beginning ----------------------------")
    RealtimeCommHandler realtime;
    int res = realtime.startGrpcService();
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Ending ----------------------------")
    return res;
}