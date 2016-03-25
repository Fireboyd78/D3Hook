#pragma once

//
// Useful preprocessor directives
//

#define EXPAND( x ) x

#define FASTCALL(method)        void __fastcall method
#define FASTCALL_(type,method)  type __fastcall method

#define STDCALL(method)         HRESULT STDMETHODCALLTYPE method
#define STDCALL_(type,method)   type STDMETHODCALLTYPE method

#define NAKED                   __declspec(naked)

#define NOINLINE                __declspec(noinline)
#define NOTHROW                 __declspec(nothrow)

// short-hand for NOINLINE
#define NO_INL                  NOINLINE

// hooked __thiscall functions require a 'this' pointer parameter
#define _THIS                   void *This
#define _THIS_                  _THIS,

#define FNPTR(t,m)              t (*m)
#define FNPTR_(t,d,m)           t (d *m)
#define FNPTR_CDECL(t,m)        FNPTR_(t,__cdecl,m)
#define FNPTR_FASTCALL(t,m)     FNPTR_(t,__fastcall,m)
#define FNPTR_STDCALL(t,m)      FNPTR_(t,__stdcall,m)
#define FNPTR_THISCALL(t,m)     FNPTR_(t,__thiscall,m)

//
// Pointer macros for data types
//

#define _PTR(p)                 (DWORD*)p
#define _PTR_(t,p)              (t*)p

#define _OFFPTR(p,o)            (*_PTR(p) + o)

//
// Pointer macros for assembler directives
//

#define __PTR(src)              ptr[src]
#define __PTR_DS(src)           ptr ds:[src]

#define __PTR_(t,src)           t __PTR(src) // <t> ptr[src]
#define __PTR_DS_(t,src)        t __PTR_DS(src) // <t> ptr ds:[src]

#define __BPTR(src)             __PTR_(byte,src) // byte ptr[src]
#define __BPTR_DS(src)          __PTR_DS_(byte,src) // byte ptr ds:[src]

#define __WPTR(src)             __PTR_(word,src) // word ptr[src]
#define __WPTR_DS(src)          __PTR_DS_(word,src) // word ptr ds:[src]

#define __DWPTR(src)            __PTR_(dword,src) // dword ptr[src]
#define __DWPTR_DS(src)         __PTR_DS_(dword,src) // dword ptr ds:[src]

//
// Debugging utilities
//

inline void debug(LPCSTR lpOutputStr) {
    OutputDebugStringA(lpOutputStr);
};

void debugf(const char *format, ...);

#define LOG(str)                LogFile::WriteLine(str)

//
// Empty function templates
//

void    NullSub(void);

int     ReturnNullOrZero(void);
bool    ReturnFalse(void);

//
// Useful helper functions
//

bool LoadSystemLibrary(LPCSTR lpDllName, HMODULE *out);
bool GetHookProcAddress(HMODULE hModule, LPCSTR lpProcName, FARPROC *out);

//
// CODEVIEW data
//

typedef struct RSDS_DATA {
    DWORD magic;
    GUID guid;
    DWORD age;
    const char *filename; // null-terminated string
} *LPRSDS_DATA;

struct RSDSEntry {
    DWORD offset;
    int version;
    float fancy_version; // for printing stuff like "V1.00"
    bool isSupported;
    GUID guid;
    DWORD age;
    LPCSTR filename;
};

//
// CODEVIEW macros
//

#define RSDS_MAGIC_NUMBER               0x53445352

#define IS_RSDS_VALID(r)                (r->magic == RSDS_MAGIC_NUMBER)

#define IS_RSDS_GUID_EQUAL(r,g)         (r->guid == g)
#define IS_RSDS_AGE_EQUAL(r,a)          (r->age == a)
#define IS_RSDS_NAME_EQUAL(r,n)         (strcmp((char *)&r->filename, n) == 0)

#define IS_RSDS_DATA_EQUAL(r,g,a,n)     (IS_RSDS_GUID_EQUAL(r,g) && IS_RSDS_AGE_EQUAL(r,a) && IS_RSDS_NAME_EQUAL(r,n))

//
// Patch data
//

struct FnPatchInfo {
private:
    DWORD lpHook;

    BYTE **patch;
    DWORD patchSize;
public:

};