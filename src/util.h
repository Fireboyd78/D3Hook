#pragma once

//
// Useful preprocessor directives
//

#define EXPAND( x ) x
#define STRFY(x) #x

#define ASSERT_SIZEOF(t,s)      static_assert(sizeof(t) <= s, "Size mismatch for type '" STRFY(t) "'; too big (expected " STRFY(s) ")"); \
                                static_assert(sizeof(t) == s, "Size mismatch for type '" STRFY(t) "'; too small (expected " STRFY(s) ")")

// <3 Python
#define PASS {}

#define ENUMCASE2STR(x) case x: return #x;

#define FASTCALL(method)        void __fastcall method
#define FASTCALL_(type,method)  type __fastcall method

#define STDCALL(method)         HRESULT STDMETHODCALLTYPE method
#define STDCALL_(type,method)   type STDMETHODCALLTYPE method

#define NAKED                   __declspec(naked)

#define NOINLINE                __declspec(noinline)
#define NOTHROW                 __declspec(nothrow)
#define NOVTABLE                __declspec(novtable)

#define INLINE_CONSTEXPR        constexpr __forceinline

// short-hand for NOINLINE
#define NO_INL                  NOINLINE

#define DONOTCALL               = delete
#define FORWARD_THUNK           {}

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
// Macros for classes with virtual tables
//

#define PUSH_VTABLE_(x)         DWORD x = *(DWORD*)this
#define POP_VTABLE_(x)          *(DWORD*)this = x
#define PUSH_VTABLE()           PUSH_VTABLE_(vtbl)
#define POP_VTABLE()            POP_VTABLE_(vtbl)

#define THUNK                   {}

//
// Pointer macros for data types
//

#define _PTR(p)                 (DWORD*)p
#define _PTR_(t,p)              (t*)p

#define _OFFPTR(p,o)            (*_PTR(p) + o)

#define PTR(p)                  (DWORD*)p
#define PTR_(p,o)               PTR(((BYTE*)p + o))

//
// Console color macros
//

#define TEXTCOLOR_BLACK             0
#define TEXTCOLOR_BLUE              1
#define TEXTCOLOR_GREEN             2
#define TEXTCOLOR_CYAN              3
#define TEXTCOLOR_RED               4
#define TEXTCOLOR_MAGENTA           5
#define TEXTCOLOR_BROWN             6
#define TEXTCOLOR_LIGHTGRAY         7
#define TEXTCOLOR_DARKGRAY          8
#define TEXTCOLOR_LIGHTBLUE         9
#define TEXTCOLOR_LIGHTGREEN        10
#define TEXTCOLOR_LIGHTCYAN         11
#define TEXTCOLOR_LIGHTRED          12
#define TEXTCOLOR_LIGHTMAGENTA      13
#define TEXTCOLOR_YELLOW            14
#define TEXTCOLOR_WHITE             15

#define BACKCOLOR_BLACK             ( 0  << 4 )
#define BACKCOLOR_BLUE              ( 1  << 4 )
#define BACKCOLOR_GREEN             ( 2  << 4 )
#define BACKCOLOR_CYAN              ( 3  << 4 )
#define BACKCOLOR_RED               ( 4  << 4 )
#define BACKCOLOR_MAGENTA           ( 5  << 4 )
#define BACKCOLOR_BROWN             ( 6  << 4 )
#define BACKCOLOR_LIGHTGRAY         ( 7  << 4 )
#define BACKCOLOR_DARKGRAY          ( 8  << 4 )
#define BACKCOLOR_LIGHTBLUE         ( 9  << 4 )
#define BACKCOLOR_LIGHTGREEN        ( 10 << 4 )
#define BACKCOLOR_LIGHTCYAN         ( 11 << 4 )
#define BACKCOLOR_LIGHTRED          ( 12 << 4 )
#define BACKCOLOR_LIGHTMAGENTA      ( 13 << 4 )
#define BACKCOLOR_YELLOW            ( 14 << 4 )
#define BACKCOLOR_WHITE             ( 15 << 4 )

//
// Pointer templates
//

template <typename TRet, typename ...TArgs>
using method_t = TRet(*)(TArgs...);

template <class TThis, typename TRet, typename ...TArgs>
using member_t = TRet(TThis::*)(TArgs...);

template <typename TType>
using value_t = typename std::remove_pointer<TType>::type;

template <typename TType, int N>
using array_t = TType(*)[N];

class auto_ptr {
    intptr_t _Ptr;
public:
    constexpr auto_ptr(intptr_t ptr)
        : _Ptr(ptr) {}

    template<typename TType>
    constexpr auto_ptr(TType ptr)
        : _Ptr(*reinterpret_cast<intptr_t *>(&ptr)) {}

    constexpr auto_ptr(const std::nullptr_t &ptr)
        : _Ptr(NULL) {}

    constexpr auto_ptr(const auto_ptr &rhs)
        : _Ptr(rhs._Ptr) {}

    constexpr intptr_t ptr(intptr_t offset = 0) {
        return _Ptr + offset;
    }

    template <typename TType>
    constexpr TType * ptr(intptr_t offset = 0) {
        return reinterpret_cast<TType *>(_Ptr + offset);
    }

    constexpr operator intptr_t() const                         { return _Ptr; }

    template <typename TType>
    constexpr operator TType *()                                { return reinterpret_cast<TType *>(_Ptr); }

    template <typename TType>
    constexpr operator TType &()                                { return *reinterpret_cast<TType *>(_Ptr); }

    constexpr bool operator==(const std::nullptr_t &rhs) const  { return _Ptr == NULL; }
    constexpr bool operator!=(const std::nullptr_t &rhs) const  { return _Ptr != NULL; }

    constexpr bool operator==(const auto_ptr &rhs) const        { return _Ptr == rhs._Ptr; }
    constexpr bool operator!=(const auto_ptr &rhs) const        { return _Ptr != rhs._Ptr; }
    constexpr bool operator<(const auto_ptr &rhs) const         { return _Ptr < rhs._Ptr; }
    constexpr bool operator<=(const auto_ptr &rhs) const        { return _Ptr <= rhs._Ptr; }
    constexpr bool operator>(const auto_ptr &rhs) const         { return _Ptr > rhs._Ptr; }
    constexpr bool operator>=(const auto_ptr &rhs) const        { return _Ptr >= rhs._Ptr; }
};

template <typename TR = void>
inline TR* getPtr(void *p, int offset) {
    return (TR*)((intptr_t)p + offset);
};

template <typename TR = void, class TC>
inline TR* getPtr(const TC *p, int offset) {
    return (TR*)getPtr((void *)p, offset);
};

template <typename TV>
inline void setPtr(void *p, int offset, TV value) {
    *(TV*)((intptr_t)p + offset) = value;
};

template <class TC, typename TV>
inline void setPtr(const TC *p, int offset, TV value) {
    setPtr((void *)p, offset, value);
};

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

// jump to function at pointer
#define JMP_PTR(x) \
    __asm jmp dword ptr ds:x

//
// Debugging utilities
//

void debug(const char *lpOutputStr);
void debugf(const char *format, ...);

#define LOG(str)                LogFile::WriteLine(str)

//
// Empty function templates
//

void ReturnVoid(void);

int ReturnNullOrZero(void);
bool ReturnFalse(void);

//
// Useful helper functions
//

bool LoadSystemLibrary(LPCSTR lpDllName, HMODULE *out);
bool GetHookProcAddress(HMODULE hModule, LPCSTR lpProcName, FARPROC *out);

bool GetPathSpec(char *path, char *dest, int destLen);

#if defined(HOOK_EXE)
std::wstring makeGamePath(const std::wstring& rel);
std::wstring makeToolPath(const std::wstring& rel);
#endif

inline bool file_exists(LPCSTR filename) {
    return GetFileAttributesA(filename) != INVALID_FILE_ATTRIBUTES;
}

namespace variadic
{
    template <unsigned long long...>
    constexpr unsigned long long sum = 0;

    template <unsigned long long S, unsigned long long... SS>
    constexpr unsigned long long sum<S, SS...> = S + sum<SS...>;

    // true_for_all
    template <bool...>
    constexpr bool true_for_all = true;

    template <bool B, bool... BB>
    constexpr bool true_for_all<B, BB...> = B && true_for_all<BB...>;

    // true_for_any
    template <bool...>
    constexpr bool true_for_any = false;

    template <bool B, bool... BB>
    constexpr bool true_for_any<B, BB...> = B || true_for_any<BB...>;
}

template <size_t _size = 0>
struct string_buf {
private:
    char buffer[_size] = { NULL };
public:
    constexpr string_buf() {}

    constexpr string_buf(char (&buf)[_size])
        : buffer(buf) {}

    template <size_t size>
    constexpr string_buf(char (&buf)[size])
        : buffer(buf) {
        static_assert(size <= _size, "not enough space for string");
    }

    constexpr string_buf(const char *str) {
        set(str);
    }

    template <typename ...TArgs>
    constexpr string_buf(const char *fmt, TArgs ...args) {
        format(fmt, args...);
    }

    constexpr char * operator &() {
        return &buffer;
    }

    constexpr char * operator[](int index) {
        return &buffer[index];
    }

    constexpr operator char *() {
        return buffer;
    }

    constexpr operator const char *() {
        return buffer;
    }

    constexpr size_t size() const {
        return _size;
    }

    constexpr bool is_empty() {
        if (buffer[0] == NULL)
            return true;

        return false;
    }

    inline void clear() {
        buffer[0] = NULL;
    }

    inline void reset() {
        // reset the entire buffer
        memset(buffer, NULL, _size);
    }

    inline const char *get() {
        return buffer;
    }

    inline void set(const char *str) {
        strcpy_s(buffer, str);
    }

    template <typename ...TArgs>
    inline size_t format(const char *fmt, TArgs ...args) {
        return sprintf_s(buffer, fmt, args...);
    }

    inline void append(char c) {
        size_t idx = strlen(buffer);

        if (idx < _size)
            buffer[idx] = c;
    }

    inline void append(const char *str) {
        size_t idx = strlen(buffer);
        size_t num = (_size - idx);

        strncpy(&buffer[idx], str, min(num, _size));

        if (num >= _size)
            buffer[_size - 1] = '\0';
    }

    template <typename ...TArgs>
    inline void append(const char *fmt, TArgs ...args) {
        size_t idx = strlen(buffer);
        size_t num = (_size - idx);

        snprintf(&buffer[idx], min(num, _size), fmt, args...);

        if (num >= _size)
            buffer[_size - 1] = '\0';
    }
};

struct stopwatch {
public:
    unsigned int startTime;
    unsigned int endTime;

    static float ticksToSeconds;
    static float ticksToMilliseconds;

    static unsigned int ticks() {
        LARGE_INTEGER perf;
        QueryPerformanceCounter(&perf);
        return perf.LowPart;
    }

    stopwatch() {
        if (ticksToSeconds == 0.0f) {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            ticksToSeconds = (1.0f / freq.LowPart);
            ticksToMilliseconds = (ticksToSeconds * 1000.0f);
        }
        reset();
    }

    inline unsigned int elapsedTicks() {
        int curTime = (endTime != 0) ? endTime : ticks();

        return (curTime - startTime);
    }

    inline float elapsedSeconds() {
        return elapsedTicks() * ticksToSeconds;
    }

    inline float elapsedMilliseconds() {
        return elapsedTicks() * ticksToMilliseconds;
    }

    void reset() {
        startTime = 0;
        endTime = 0;
    }

    void start() {
        if (startTime == 0)
            startTime = ticks();
    }

    void restart() {
        startTime = ticks();
        endTime = 0;
    }

    void stop() {
        if (endTime == 0)
            endTime = ticks();
    }
};

struct scoped_vtable {
private:
    const intptr_t obj;
    const intptr_t vtbl;
public:
    template <typename TClass>
    inline scoped_vtable(const TClass *obj) noexcept :
        obj(reinterpret_cast<const intptr_t>(obj)),
        vtbl(*reinterpret_cast<const intptr_t *>(obj)) { }

    inline ~scoped_vtable() noexcept {
        *reinterpret_cast<intptr_t *>(obj) = vtbl;
    }
};

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