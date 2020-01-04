#include "CFileChunker.h"
#include "hooksystem.h"

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
FnHook<BOOL, LPCSTR>                lpfnOpenChunks;
FnHook<void>                        lpfnCloseChunks;

FnHook<void>                        lpfnRelease;

init_handler init("CFileChunker", []() {
    //
    // Thanks for nothing, patch 2!
    //
    lpfn_ctor                = addressof(0x5B1CF0, &ReturnNullOrZero);
    lpfn_dtor                = addressof(0x5BBE50, &ReturnNullOrZero);
    lpfnGetChunkCount        = addressof(0x5A8550, &ReturnNullOrZero);
    lpfnGetChunkContext      = addressof(0x5AB970, &ReturnNullOrZero);
    lpfnGetChunkVersion      = addressof(0x5AB980, &ReturnNullOrZero);
    lpfnGetChunkData         = addressof(0x5AB990, &ReturnNullOrZero);
    lpfnGetChunkData_        = addressof(0x5BBE60, &ReturnNullOrZero);
    lpfnGetChunkSize         = addressof(0x5AB9A0, &ReturnNullOrZero);
    lpfnGetPaddedChunkSize   = addressof(0x5BAD10, &ReturnNullOrZero);
    lpfnAddChunk             = addressof(0x5B5470, &ReturnNullOrZero);
    lpfnReadFileChunkData    = addressof(0x5BADF0, &ReturnFalse     );
    lpfnIsBusy               = addressof(0x5BAD00, &ReturnNullOrZero);
    lpfnOpenChunks           = addressof(0x5BAE50, &ReturnNullOrZero);
    lpfnCloseChunks          = addressof(0x5BAC70, &ReturnVoid      );
    lpfnRelease              = addressof(0x5BB070, &ReturnVoid      );

    if (CDriv3r::Version() == __DRIV3R_V120) {
        HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS();
        return false;
    }

    return true;
});

NOTHROW CFileChunker::CFileChunker() {
    lpfn_ctor(this);
};

NOTHROW CFileChunker::~CFileChunker() {
    lpfn_dtor(this);
};

INT CFileChunker::GetChunkCount(void) const {
    return lpfnGetChunkCount(this);
};

UINT CFileChunker::GetChunkContext(UINT index) const {
    return lpfnGetChunkContext(this, index);
};

void CFileChunker::SetChunkContext(UINT index, UINT value) {
    chunks[index].Context = value;
};

BYTE CFileChunker::GetChunkVersion(UINT index) const {
    return lpfnGetChunkVersion(this, index);
};

void CFileChunker::SetChunkVersion(UINT index, BYTE value) {
    chunks[index].Version = value;
};

LPVOID CFileChunker::GetChunkData(UINT index) const {
    return lpfnGetChunkData(this, index);
}

LPVOID CFileChunker::GetChunkData(UINT index, LPVOID buffer, INT copyType) const {
    return lpfnGetChunkData_(this, index, buffer, copyType);
};

UINT CFileChunker::GetChunkSize(UINT index) const {
    return lpfnGetChunkSize(this, index);
};

UINT CFileChunker::GetPaddedChunkSize(UINT index) const {
    return lpfnGetPaddedChunkSize(this, index);
};

UINT CFileChunker::AddChunk(LPVOID buffer, UINT size, CHUNKCOPYTYPE copyType, UINT alignment) {
    return lpfnAddChunk(this, buffer, size, copyType, alignment);
};

bool CFileChunker::ReadFileChunkData(FILECHUNKIO *fileChunk, INT copyType) {
    return lpfnReadFileChunkData(this, fileChunk, copyType);
};

BOOL CFileChunker::IsBusy(void) const {
    return lpfnIsBusy(this);
};

BOOL CFileChunker::OpenChunks(LPCSTR fileName) {
    return lpfnOpenChunks(this, fileName);
};

void CFileChunker::CloseChunks(void) {
    lpfnCloseChunks(this);
};

void CFileChunker::Release(void) {
    lpfnRelease(this);
};