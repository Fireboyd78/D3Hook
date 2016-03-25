#pragma once
#include "driv3r.h"

/*
[-] CFileChunker::`scalar deleting destructor'(uint)                  .text 00495B90 0000001E 00000004 00000001
[x] CFileChunker::GetChunkCount(void)                                 .text 005A8550 00000007
[x] CFileChunker::GetChunkContext(uint)                               .text 005AB970 0000000E 00000000 00000004
[x] CFileChunker::GetChunkVersion(uint)                               .text 005AB980 0000000E 00000000 00000004
[x] CFileChunker::GetChunkData(uint)                                  .text 005AB990 0000000E 00000000 00000004
[x] CFileChunker::GetChunkSize(uint)                                  .text 005AB9A0 0000000E 00000000 00000004
[-] CFileChunker::SetChunkDescription(uint,char *)                    .text 005AB9B0 00000067 00000010 00000008
[-] CFileChunker::SetChunkCommon(uint,uint,CHUNKCOPYTYPE,uint)        .text 005ABA20 0000006A 00000008 00000010
[-] CFileChunker::SetChunk(uint,void const *,uint,CHUNKCOPYTYPE,uint) .text 005B0630 0000007C 00000008 00000014
[x] CFileChunker::CFileChunker(void)                                  .text 005B1CF0 00000042
[x] CFileChunker::AddChunk(void *,uint,CHUNKCOPYTYPE,uint)            .text 005B5470 00000097 00000024 00000010
[-] CFileChunker::ReadFileChunkData2(FILECHUNKIO *,CHUNKCOPYTYPE)     .text 005B5510 0000011A 0000012C 00000008
[-] CFileChunker::ReadFileChunkData3(FILECHUNKIO *,CHUNKCOPYTYPE)     .text 005B5630 000000EB 00000020 00000008
[x] CFileChunker::CloseChunks(void)                                   .text 005BAC70 0000008F 0000000C 00000000
[x] CFileChunker::IsBusy(void)                                        .text 005BAD00 0000000B 0000000C 00000000
[x] CFileChunker::GetPaddedChunkSize(uint)                            .text 005BAD10 000000DF 00000004 00000004
[x] CFileChunker::ReadFileChunkData(FILECHUNKIO *,CHUNKCOPYTYPE)      .text 005BADF0 0000005F 00000004 00000008
[x] CFileChunker::LoadChunks(char const *)                            .text 005BAE50 00000216 00000028 00000004
[x] CFileChunker::Release(void)                                       .text 005BB070 00000005 0000000C 00000000
[x] CFileChunker::~CFileChunker(void)                                 .text 005BBE50 0000000B
[x] CFileChunker::GetChunkData(uint,void *,CHUNKCOPYTYPE)             .text 005BBE60 00000143 00004048 0000000C
*/

namespace FileChunkerHookSystem
{
    int Initialize(int gameVersion);
};

class CFileChunkerHook {
private:
    LPVOID lpVtbl;

    CHUNKHEADER chunks[272];
    UINT nChunks;               // 0x1104

    DWORD chunkData[272];
    UINT nChunkData;            // 0x1548

    BYTE fileData[0x18];
public:
    NOTHROW CFileChunkerHook();
    NOTHROW ~CFileChunkerHook();
    INT GetChunkCount(void) const;
    UINT GetChunkContext(UINT index) const;
    void SetChunkContext(UINT index, UINT value);
    BYTE GetChunkVersion(UINT index) const;
    void SetChunkVersion(UINT index, BYTE value);
    LPVOID GetChunkData(UINT index) const;
    LPVOID GetChunkData(UINT index, LPVOID buffer, INT copyType) const;
    UINT GetChunkSize(UINT index) const;
    UINT GetPaddedChunkSize(UINT index) const;
    UINT AddChunk(LPVOID buffer, UINT size, CHUNKCOPYTYPE copyType, UINT alignment);
    bool ReadFileChunkData(FILECHUNKIO *fileChunk, INT copyType);
    BOOL IsBusy(void) const;
    BOOL LoadChunks(LPCSTR fileName);
    void CloseChunks(void);
    void Release(void);
};