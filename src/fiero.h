#pragma once

#include <memory>
#include <vector>
#include <string>
#include <codecvt>

#include <type_traits>

#include "util.h"

#if !defined(FX_DEBUG) && defined(_DEBUG)
# define FX_DEBUG 1
#endif

// represents an empty macro body
#define __empty(x) ((void)x)
#define __empty__ __empty(0)

#ifdef FX_DEBUG
void fxDebugf(LPCSTR format, ...);

# define fx_assert_(condition,message,exception) \
        if (!(condition)) \
            throw exception(message)
# define fx_assert(condition,message) fx_assert_(condition,message,std::exception)
# define fx_error_(message,exception) throw exception(message)
# define fx_error(message) fx_error_(message, std::exception)
# define fx_warning(message) OutputDebugStringA(message)
#else
# define fxDebugf(fmt, ...) __empty__

# define fx_assert_(condition,message,exception) __empty__
# define fx_assert(condition,message) __empty__
# define fx_error_(message,exception) __empty__
# define fx_error(message) __empty__
# define fx_warning(message) __empty__
#endif

struct module_info_t {
    HINSTANCE instance;

    intptr_t begin;
    intptr_t end;

    size_t size;
};

struct section_info_t {
    intptr_t begin;
    intptr_t end;

    constexpr section_info_t(intptr_t begin, intptr_t end)
        : begin(begin), end(end) {}
};

struct import_info_t {
    intptr_t base; // address of PIMAGE_IMPORT_BY_NAME
    LPCSTR name;

    FARPROC *func_ref;
    intptr_t *thunk_ref;
};

struct import_descriptor_t {
    intptr_t base; // address of PIMAGE_IMPORT_DESCRIPTOR
    LPCSTR name;

    std::vector<import_info_t> infos;
};

struct export_descriptor_t {
    PDWORD func_ptr;
    LPCSTR name;
};

enum class fxModuleLoadType {
    Normal,
    DontResolveRefs,
    DataFile,
};

class fxModule : module_info_t {
private:
    std::vector<section_info_t> sections;

    std::vector<import_descriptor_t> imports;
    std::vector<export_descriptor_t> exports;
protected:
    bool InitModuleInfo(HINSTANCE instance);
public:
    static bool Initialize(fxModule &module, HINSTANCE instance);

    fxModule() { /* do nothing */ }

    fxModule(LPCSTR libName);
    fxModule(LPCSTR libName, fxModuleLoadType loadType);

    fxModule(LPCWSTR libName);
    fxModule(LPCWSTR libName, fxModuleLoadType loadType);

    fxModule(HINSTANCE instance);

    ~fxModule();

    void GetModuleInfo(module_info_t &moduleInfo);

    constexpr bool operator==(const std::nullptr_t &rhs) const
    {
        return instance == nullptr;
    }
    constexpr bool operator!=(const std::nullptr_t &rhs) const
    {
        return instance != nullptr;
    }

    constexpr bool isLoaded() const { return instance != nullptr; }

    operator HINSTANCE() const {
        return instance;
    }

    //
    // Releases the module instance.
    // Recommended only if you're in full control of the library/module you loaded!
    //
    BOOL Free();

    auto_ptr FindImport(LPCSTR fnName);
    auto_ptr FindExport(LPCSTR fnName);

    //auto_ptr ReplaceImport(LPCSTR fnName, auto_ptr target);
    //auto_ptr ReplaceExport(LPCSTR fnName, auto_ptr target);
};

template <typename TRet, typename... TArgs>
using MethodCall = TRet(__cdecl *)(TArgs...);

template <typename TRet, class TThis, typename... TArgs>
using MemberCall = TRet(__thiscall *)(const TThis *, TArgs...);

template <typename TRet, class TThis, typename... TArgs>
using VirtualCall = TRet(__thiscall TThis::*)(TArgs...);

template <typename TRet, typename... TArgs>
using StdMethodCall = TRet(__stdcall *)(TArgs...);

template <typename TRet, class TThis, typename... TArgs>
using StdMemberCall = TRet(__stdcall *)(const TThis *, TArgs...);

template <typename TRet, class TThis, typename... TArgs>
using StdVirtualCall = TRet(__stdcall TThis::*)(const TThis *, TArgs...);

namespace fiero
{
    template <typename TRet, typename... TArgs>
    inline auto func(intptr_t address) {
        return reinterpret_cast<MethodCall<TRet, TArgs...>>(address);
    }

    template <typename TType,
        bool is_pointer = std::is_pointer<TType>::value,
        bool is_array   = std::is_array<TType>::value>
    class Type {};

    /*
        Hook template for value types
    */
    template <typename TType>
    class Type<TType, false, false> {
    protected:
        TType *lpValue;
    public:
        constexpr Type(intptr_t address)
            : lpValue(reinterpret_cast<TType *>(address)) {};
        
        inline TType & get() const                          { return *lpValue; }
        inline void set(TType value)                        { *lpValue = value; }

        inline TType * ptr() const                          { return lpValue; }

        /*
            Operators
        */

        inline TType * operator->() const                   { return lpValue; };
        inline TType * operator&() const                    { return lpValue; };
        inline TType & operator*() const                    { return *lpValue; };
        inline TType * operator[](int index) const          { return &lpValue[index]; }
        inline TType & operator=(TType value)               { return (*lpValue = value); }

        inline operator TType &() const                     { return *lpValue; }

        /*
            Comparison operators
        */

        inline bool operator==(const TType &rhs) const      { return *lpValue == rhs; }
        inline bool operator!=(const TType &rhs) const      { return *lpValue != rhs; }

        /*
            Value-type operators
        */

        inline bool operator<(const TType &rhs) const       { return *lpValue < rhs; }
        inline bool operator>(const TType &rhs) const       { return *lpValue > rhs; }
        inline bool operator<=(const TType &rhs) const      { return *lpValue <= rhs; }
        inline bool operator>=(const TType &rhs) const      { return *lpValue >= rhs; }
        
        inline TType operator+() const                      { return +(*lpValue); }
        inline TType operator-() const                      { return -(*lpValue); }
        inline TType operator~() const                      { return ~(*lpValue); }
        
        inline TType operator+(const TType &rhs) const      { return *lpValue + rhs; }
        inline TType operator-(const TType &rhs) const      { return *lpValue - rhs; }
        inline TType operator*(const TType &rhs) const      { return *lpValue * rhs; }
        inline TType operator/(const TType &rhs) const      { return *lpValue / rhs; }
        inline TType operator%(const TType &rhs) const      { return *lpValue % rhs; }
        inline TType operator&(const TType &rhs) const      { return *lpValue & rhs; }
        inline TType operator|(const TType &rhs) const      { return *lpValue | rhs; }
        inline TType operator^(const TType &rhs) const      { return *lpValue ^ rhs; }
        inline TType operator<<(const TType &rhs) const     { return *lpValue << rhs; }
        inline TType operator>>(const TType &rhs) const     { return *lpValue >> rhs; }
        
        inline TType operator+=(const TType &rhs)           { return (*lpValue += rhs); }
        inline TType operator-=(const TType &rhs)           { return (*lpValue -= rhs); }
        inline TType operator*=(const TType &rhs)           { return (*lpValue *= rhs); }
        inline TType operator/=(const TType &rhs)           { return (*lpValue /= rhs); }
        inline TType operator%=(const TType &rhs)           { return (*lpValue %= rhs); }
        inline TType operator&=(const TType &rhs)           { return (*lpValue &= rhs); }
        inline TType operator|=(const TType &rhs)           { return (*lpValue |= rhs); }
        inline TType operator^=(const TType &rhs)           { return (*lpValue ^= rhs); }
        inline TType operator<<=(const TType &rhs)          { return (*lpValue <<= rhs); }
        inline TType operator>>=(const TType &rhs)          { return (*lpValue >>= rhs); }
    };

    /*
        Hook template for pointer types
    */
    template <typename TType>
    class Type<TType, true, false> {
    protected:
        TType *lpValue;
    public:
        constexpr Type(intptr_t address)
            : lpValue(reinterpret_cast<TType *>(address)) {};

        inline TType & get() const                          { return *lpValue; }
        inline void set(TType value)                        { *lpValue = value; }

        inline TType * ptr() const                          { return lpValue; }

        /*
            Operators
        */

        inline TType & operator->() const                   { return *lpValue; };
        inline TType * operator&() const                    { return lpValue; };
        inline TType & operator*() const                    { return *lpValue; };
        inline TType operator[](int index) const            { return lpValue[index]; }
        inline TType & operator=(TType value)               { return (*lpValue = value); }

        inline operator TType &() const                     { return *lpValue; }

        /*
            Comparison operators
        */

        inline bool operator==(const TType &rhs) const      { return *lpValue == rhs; }
        inline bool operator!=(const TType &rhs) const      { return *lpValue != rhs; }
        
        inline bool operator==(const std::nullptr_t &rhs) const
                                                            { return *lpValue == nullptr; }
        inline bool operator!=(const std::nullptr_t &rhs) const
                                                            { return *lpValue != nullptr; }

        template <typename... TArgs>
        inline auto operator()(TArgs... args) {
            return (*lpValue)(args...);
        }
    };

    /*
        Hook template for array types
    */
    template <typename TArray>
    class Type<TArray, false, true> {
        /*
            we need all this spaghett to resolve the actual array type
            because the fucking template isn't smart enough to do so
        */

        template <typename _T, int N>
        using array_type = _T(*)[N];

        template <typename _T, int N>
        static constexpr _T _type(array_type<_T, N>);

        template <typename _T, int N>
        static constexpr int _count(array_type<_T, N>) {
            return N;
        };

        using type = decltype(_type((TArray *)nullptr));

        template <typename TRet, typename ...TArgs>
        using rtype = TRet;
    protected:
        using TValue = rtype<type>;

        TValue *lpValue;
    public:
        constexpr Type(intptr_t address)
            : lpValue(reinterpret_cast<TValue *>(address)) {};

        inline int count() const {
            return _count((TArray *)nullptr);
        }

        inline size_t length() {
            return count() * sizeof(TValue);
        }

        inline TValue * ptr() const                         { return lpValue; }
        inline TValue * ptr(int index) const                { return lpValue + index; }

        /*
            Operators
        */

        inline TValue * operator&() const                   { return lpValue; };
        inline TValue & operator[](int index) const         { return lpValue[index]; }

        template <typename TType>
        inline operator TType *() const                     { return reinterpret_cast<TType *>(lpValue); }
    };

    template <typename TType>
    class Proxy {
    protected:
        TType *lpValue;
    public:
        static_assert(!std::is_pointer<TType>::value, "Proxy cannot be that of a pointer type.");

        constexpr Proxy(intptr_t address)
            : lpValue(reinterpret_cast<TType *>(address)) {};

        inline void read(TType &value)              { memcpy(&value, lpValue, sizeof(TType)); }
        inline void write(TType &value)             { memcpy(lpValue, &value, sizeof(TType)); }

        inline TType* operator->() const            { return lpValue; }
        inline TType* operator&() const             { return lpValue; }
        inline TType& operator*() const             { return *lpValue; }
        inline TType& operator[](int index) const   { return &lpValue[index]; }

        inline operator TType*() const              { return lpValue; }
        inline operator TType&() const              { return *lpValue; }
    };

    template<intptr_t _Offset, typename _Type>
    class Field {
    public:
        using type = _Type;
        using pointer = type *;

        template <class TThis>
        static INLINE_CONSTEXPR pointer ptr(const TThis *p) {
            return reinterpret_cast<pointer>(reinterpret_cast<intptr_t>(p) + _Offset);
        };

        template <class TThis>
        static INLINE_CONSTEXPR type get(const TThis *p) {
            return *ptr(p);
        };

        template <class TThis>
        static INLINE_CONSTEXPR void set(const TThis *p, type value) {
            *ptr(p) = value;
        };
    };

    template <intptr_t _Address>
    class Thunk {
        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR const TRet _ConstCall(intptr_t callback, const TThis *This, TArgs ...args) {
            return (This->*reinterpret_cast<VirtualCall<const TRet, const TThis, TArgs...> &>(callback))(args...);
        };

        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR TRet _ThisCall(intptr_t callback, TThis *This, TArgs ...args) {
            return (This->*reinterpret_cast<VirtualCall<TRet, TThis, TArgs...> &>(callback))(args...);
        };
    public:
        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR TRet Call(const TThis &&This, TArgs ...args) {
            return reinterpret_cast<MemberCall<TRet, TThis, TArgs...>>(_Address)(This, args...);
        };

        template<class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR const TThis Call(const TThis *This, TArgs ...args) {
            return _ConstCall<TThis>(address, This, args...);
        };

        // HACK: call from a pointer to a class...
        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR TRet ThisCall(TThis *This, TArgs ...args) {
            return _ThisCall<TRet>(address, This, args...);
        };
    };

    template <intptr_t _Address>
    class StaticThunk {
    public:
        template<typename TRet, typename ...TArgs>
        static INLINE_CONSTEXPR TRet Call(TArgs ...args) {
            return reinterpret_cast<MethodCall<TRet, TArgs...>>(_Address)(args...);
        };
    };

    template <typename TRet>
    class Func {
    protected:
        LPVOID lpFunc;
    public:
        constexpr Func(int address) : lpFunc(reinterpret_cast<LPVOID>(address)) {};

        template<typename ...TArgs>
        INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
            return static_cast<MethodCall<TRet, TArgs...>>(lpFunc)(args...);
        };

        class StdCall : protected Func<TRet> {
        public:
            constexpr StdCall(int address) : Func<TRet>(address) {};

            template<typename ...TArgs>
            INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
                return static_cast<StdMethodCall<TRet, TArgs...>>(lpFunc)(args...);
            };

            template <typename TRet, typename... TArgs>
            INLINE_CONSTEXPR operator StdMethodCall<TRet, TArgs...>() const {
                return static_cast<StdMethodCall<TRet, TArgs...>>(lpFunc);
            };
        };

        class ThisCall : protected Func<TRet> {
        public:
            constexpr ThisCall(int address) : Func<TRet>(address) {};

            template<typename ...TArgs, class TThis>
            INLINE_CONSTEXPR TRet operator()(const TThis &&This, TArgs ...args) const {
                return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc)(This, args...);
            };

            template <typename TRet, class TThis, typename... TArgs>
            INLINE_CONSTEXPR operator MemberCall<TRet, TThis, TArgs...>() const {
                return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc);
            };
        };
    };

    template <typename TRet>
    class MemberFunc : protected Func<TRet> {
    public:
        constexpr MemberFunc(int address) : Func<TRet>(address) {};

        template<typename ...TArgs, class TThis>
        INLINE_CONSTEXPR TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc)(This, args...);
        };

        template <typename TRet, class TThis, typename... TArgs>
        INLINE_CONSTEXPR operator MemberCall<TRet, TThis, TArgs...>() const {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc);
        };
    };

    /*
        __cdecl function declaration
    */
    template <typename TRet, typename... TArgs>
    class Func<MethodCall<TRet, TArgs...>> : protected Func<TRet> {
        using MethodCall = MethodCall<TRet, TArgs...>;
    public:
        constexpr Func(int address) : Func<TRet>(address) {};

        INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
            return static_cast<MethodCall>(lpFunc)(args...);
        };

        INLINE_CONSTEXPR operator MethodCall() const {
            return static_cast<MethodCall>(lpFunc);
        };
    };

    /*
        __stdcall function declaration
    */
    template <typename TRet, typename... TArgs>
    class Func<StdMethodCall<TRet, TArgs...>> : protected Func<TRet> {
        using StdMethodCall = StdMethodCall<TRet, TArgs...>;
    public:
        constexpr Func(int address) : Func<TRet>(address) {};

        INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
            return static_cast<StdMethodCall>(lpFunc)(args...);
        };

        INLINE_CONSTEXPR operator StdMethodCall() const {
            return static_cast<StdMethodCall>(lpFunc);
        };
    };

    /*
        __thiscall function declaration
    */
    template <typename TRet, class TThis, typename... TArgs>
    class Func<MemberCall<TRet, TThis, TArgs...>> : protected Func<TRet> {
        using MemberCall = MemberCall<TRet, TThis, TArgs...>;
    public:
        constexpr Func(int address) : Func<TRet>(address) {};

        INLINE_CONSTEXPR TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall>(lpFunc)(args...);
        };

        INLINE_CONSTEXPR operator MemberCall() const {
            return static_cast<MemberCall>(lpFunc);
        };
    };

    template <typename _Type = void, bool is_class = std::is_class<_Type>::value>
    class as {
    public:
        static_assert(true, "Congratulations, you played yourself.");
    };

    template <typename TThis>
    class as<TThis, true> {
    public:
        template <typename TRet, typename... TArgs>
        static inline auto func(const intptr_t address) {
            return reinterpret_cast<MemberCall<TRet, TThis, TArgs...>>(address);
        }
    };
};