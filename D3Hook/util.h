///
/// Useful preprocessor directives
///

#pragma once
#define FASTCALL(method)        void __fastcall method
#define FASTCALL_(type,method)  type __fastcall method

#define STDCALL(method)         HRESULT STDMETHODCALLTYPE method
#define STDCALL_(type,method)   type STDMETHODCALLTYPE method

#define NAKED                   __declspec(naked)

#define LOG(str)                OutputDebugString(L##str)

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