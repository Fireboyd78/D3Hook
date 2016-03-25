#include "CFileChunker.h"

namespace FileChunkerHookSystem
{
    FnHook<void>                        lpfn_ctor;
    FnHook<void>                        lpfn_dtor;

    FnHook<INT>                         lpfnGetChunkCount;
    FnHook<UINT, UINT>                  lpfnGetChunkContext;
    FnHook<BYTE, UINT>                  lpfnGetChunkVersion;
    FnHook<LPVOID, UINT>                lpfnGetChunkData;
    FnHook<LPVOID, UINT, LPVOID, INT>   lpfnGetChunkData_;
    FnHook<UINT, UINT>                  lpfnGetChunkSize;
    FnHook<UINT, UINT>                  lpfnGetPaddedChunkSize;

    FnHook<UINT, LPVOID, UINT,
        CHUNKCOPYTYPE, UINT>            lpfnAddChunk;

    FnHook<bool, FILECHUNKIO*, INT>     lpfnReadFileChunkData;

    FnHook<BOOL>                        lpfnIsBusy;
    FnHook<BOOL, LPCSTR>                lpfnLoadChunks;
    FnHook<void>                        lpfnCloseChunks;

    FnHook<void>                        lpfnRelease;

    int FileChunkerHookSystem::Initialize(int gameVersion) {
        switch (gameVersion) {
            case __DRIV3R_V100:
            {
                lpfn_ctor               = 0x5B1CF0;
                lpfn_dtor               = 0x5BBE50;
                lpfnGetChunkCount       = 0x5A8550;
                lpfnGetChunkContext     = 0x5AB970;
                lpfnGetChunkVersion     = 0x5AB980;
                lpfnGetChunkData        = 0x5AB990;
                lpfnGetChunkData_       = 0x5BBE60;
                lpfnGetChunkSize        = 0x5AB9A0;
                lpfnGetPaddedChunkSize  = 0x5BAD10;
                lpfnAddChunk            = 0x5B5470;
                lpfnReadFileChunkData   = 0x5BADF0;
                lpfnIsBusy              = 0x5BAD00;
                lpfnLoadChunks          = 0x5BAE50;
                lpfnCloseChunks         = 0x5BAC70;
                lpfnRelease             = 0x5BB070;
            } return HOOK_INIT_OK;

            case __DRIV3R_V120:
            {
                lpfn_ctor               = &ReturnNullOrZero;
                lpfn_dtor               = &ReturnNullOrZero;
                lpfnGetChunkCount       = &ReturnNullOrZero;
                lpfnGetChunkContext     = &ReturnNullOrZero;
                lpfnGetChunkVersion     = &ReturnNullOrZero;
                lpfnGetChunkData        = &ReturnNullOrZero;
                lpfnGetChunkData_       = &ReturnNullOrZero;
                lpfnGetChunkSize        = &ReturnNullOrZero;
                lpfnGetPaddedChunkSize  = &ReturnNullOrZero;
                lpfnAddChunk            = &ReturnNullOrZero;
                lpfnReadFileChunkData   = &ReturnFalse;
                lpfnIsBusy              = &ReturnNullOrZero;
                lpfnLoadChunks          = &ReturnNullOrZero;
                lpfnCloseChunks         = &NullSub;
                lpfnRelease             = &NullSub;   
            } return HOOK_INIT_UNSUPPORTED;
        }
        return HOOK_INIT_FAILED;
    };
}

NOTHROW CFileChunkerHook::CFileChunkerHook() {
    FileChunkerHookSystem::lpfn_ctor(this);
};

NOTHROW CFileChunkerHook::~CFileChunkerHook() {
    FileChunkerHookSystem::lpfn_dtor(this);
};

INT CFileChunkerHook::GetChunkCount(void) const {
    return FileChunkerHookSystem::lpfnGetChunkCount(this);
};

UINT CFileChunkerHook::GetChunkContext(UINT index) const {
    return FileChunkerHookSystem::lpfnGetChunkContext(this, index);
};

void CFileChunkerHook::SetChunkContext(UINT index, UINT value) {
    chunks[index].Context = value;
};

BYTE CFileChunkerHook::GetChunkVersion(UINT index) const {
    return FileChunkerHookSystem::lpfnGetChunkVersion(this, index);
};

void CFileChunkerHook::SetChunkVersion(UINT index, BYTE value) {
    chunks[index].Version = value;
};

LPVOID CFileChunkerHook::GetChunkData(UINT index) const {
    return FileChunkerHookSystem::lpfnGetChunkData(this, index);
}

LPVOID CFileChunkerHook::GetChunkData(UINT index, LPVOID buffer, INT copyType) const {
    return FileChunkerHookSystem::lpfnGetChunkData_(this, index, buffer, copyType);
};

UINT CFileChunkerHook::GetChunkSize(UINT index) const {
    return FileChunkerHookSystem::lpfnGetChunkSize(this, index);
};

UINT CFileChunkerHook::GetPaddedChunkSize(UINT index) const {
    return FileChunkerHookSystem::lpfnGetPaddedChunkSize(this, index);
};

UINT CFileChunkerHook::AddChunk(LPVOID buffer, UINT size, CHUNKCOPYTYPE copyType, UINT alignment) {
    return FileChunkerHookSystem::lpfnAddChunk(this, buffer, size, copyType, alignment);
};

bool CFileChunkerHook::ReadFileChunkData(FILECHUNKIO *fileChunk, INT copyType) {
    return FileChunkerHookSystem::lpfnReadFileChunkData(this, fileChunk, copyType);
};

BOOL CFileChunkerHook::IsBusy(void) const {
    return FileChunkerHookSystem::lpfnIsBusy(this);
};

BOOL CFileChunkerHook::LoadChunks(LPCSTR fileName) {
    return FileChunkerHookSystem::lpfnLoadChunks(this, fileName);
};

void CFileChunkerHook::CloseChunks(void) {
    FileChunkerHookSystem::lpfnCloseChunks(this);
};

void CFileChunkerHook::Release(void) {
    FileChunkerHookSystem::lpfnRelease(this);
};