#pragma once
#include "common.h"

//
// Log file functions
//

#define __LOGFMT_BUF_SIZE 4096

class LogFileStream {
protected:
    filename_t m_filename;
    FILE *m_file;

    LogFileStream(filename_t filename, bool append = false);

    void SetAppendMode(bool append);
public:
    static LogFileStream* Create(filename_t filename, LPCSTR title);
    static LogFileStream* Create(filename_t filename);

    static LogFileStream* Open(filename_t filename);

    void Close(void);
    void Flush(bool force);

    void AppendLine(void);

    void Format(LPCSTR format, ...);

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);
};

namespace LogFile {
    void Initialize(filename_t filename);
    void Initialize(filename_t filename, LPCSTR title);

    void Close(void);
    void Flush(bool force);
    
    void AppendLine(void);

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);

    void Print(int level, LPCSTR str);
    void Printf(int level, LPCSTR format, ...);

    void Format(LPCSTR format, ...);
};