#include "stdafx.h"
#include <windows.h>
#include "CApplication.h"
#include <DbgHelp.h>
#include <iostream>

/*
const wchar_t* kDumpFileName = L"cdr_app_crash.dmp";

// 自定义异常处理函数
LONG WINAPI CustomExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo) {
	// 生成 minidump 文件
	HANDLE hFile = CreateFile(kDumpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = ExceptionInfo;
		mdei.ClientPointers = FALSE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, NULL, NULL);
		CloseHandle(hFile);
	}

	// 继续默认的异常处理
	return EXCEPTION_CONTINUE_SEARCH;
}*/

#pragma comment(lib, "Dbghelp.lib")
LONG WINAPI UnhandledExceptionFilterFunc(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	// 输出错误信息
	std::cerr << "程序崩溃！\n";

	// 获取调用栈
	void* array[10];
	unsigned short frames;
	SYMBOL_INFO* symbol;
	HANDLE process;

	process = GetCurrentProcess();

	SymInitialize(process, NULL, TRUE);
	frames = CaptureStackBackTrace(0, 10, array, NULL);
	symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	std::cerr << "调用栈信息：\n";
	for (unsigned int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64)(array[i]), 0, symbol);
		std::cerr << i << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::endl;
	}

	free(symbol);
	SymCleanup(process);

	// 返回继续执行默认的异常处理
	return EXCEPTION_CONTINUE_SEARCH;
}

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
	switch (type) {
	case QtDebugMsg:
		fprintf(stderr, "%s - %s (%s:%u)\n", current_date.toLocal8Bit().constData(), localMsg.constData(), context.file, context.line);
		break;
	case QtInfoMsg:
		fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		abort();
	}
}

int main(int argc, char *argv[])
{
	SetUnhandledExceptionFilter(UnhandledExceptionFilterFunc);

	qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--no-sandbox");
	if (CD_DEBUG) {
		qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--no-sandbox --webEngineArgs --remote-debugging-port=2222");
	}

	qInstallMessageHandler(messageOutput);

	CApplication a(argc, argv);

	a.setStyle(QStyleFactory::create("Fusion"));

    return a.exec();
}
