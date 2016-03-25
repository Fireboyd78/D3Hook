#pragma once
struct CHUNKHEADER {
    UINT Context;
    UINT DataOffset;
    BYTE Version;
    BYTE Alignment;
    BYTE StrLen;
    BYTE Reserved;
    UINT Size;
};

struct FILECHUNKIO {
    UINT Magic;
    UINT Size;
    INT Count;
    INT Version;

    CHUNKHEADER &Chunks;
};

enum CHUNKCOPYTYPE {
    CHUNKCOPYTYPE_LOAD,
    CHUNKCOPYTYPE_COPY
};