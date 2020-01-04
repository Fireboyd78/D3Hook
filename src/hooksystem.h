#pragma once
#include "common.h"
#include "patch.h"
#include "hook.h"

#define ADDRESS_PLACEHOLDER 0xDEADBEEF

enum hookType : unsigned int
{
    JMP,
    CALL,
    PUSH,

    COUNT,
};

struct cbInfo
{
    auto_ptr addr;
    hookType type;
};

template<hookType type>
struct cbHook
{
    cbInfo info;

    constexpr cbHook(intptr_t addr) : info{ addr, type } {};

    constexpr operator cbInfo() const {
        return info;
    };
};

template <typename... _Args>
inline intptr_t addressof(_Args&& ...args) {
    int index = 0;
    void *addr = 0;

    mem::fx::unpack_each([&](const auto &value) {
        if (index++ == CDriv3r::Version()) {
            addr = reinterpret_cast<void *>(value);
            return;
        }
    }, std::forward<_Args>(args)...);

    assert(addr < (void *)ADDRESS_PLACEHOLDER);
    return reinterpret_cast<intptr_t>(addr);
}

template <hookType type, typename... _Args>
inline cbInfo addressof(_Args&& ...args) {
    return cbInfo { addressof(args...), type };
}

template <class _Elem>
static std::initializer_list<uint8_t> pack(std::initializer_list<_Elem> elems) {
    const uint8_t * first = reinterpret_cast<const uint8_t *>(elems.begin());
    const uint8_t * last = reinterpret_cast<const uint8_t *>(elems.end());

    return std::initializer_list<uint8_t>(first, last);
}

typedef bool(*init_t)(void);

void InstallHandler(LPCSTR name, init_t installHandler);

/*
    Assumes THandler is a class that implements a public,
    static method called 'Install' with no return type.
*/
template <class THandler>
inline void InstallHandler(LPCSTR name) {
    InstallHandler(name, &THandler::Install);
};

class init_base {
protected:
    init_base *next;

    void Register();
public:
    init_base(init_base &) = delete;
    init_base(const init_base &&) = delete;

    init_base() {
        Register();
    }

    virtual ~init_base() = default;

    virtual void Run() {
        /* do nothing */
    }

    static void RunAll();
};

class init_func : public init_base {
protected:
    init_t fnInit;
public:
    explicit init_func(init_t fnInit)
        : fnInit(fnInit), init_base() {}

    void Run() override {
        fnInit();
    }
};

class init_handler : public init_func {
protected:
    const char *name;
public:
    explicit init_handler(init_t) = delete;
    
    explicit init_handler(const char *name, init_t fnInit)
        : name(name), init_func(fnInit) {}

    void Run() override {
        InstallHandler(name, fnInit);
    }
};

template <typename THandler>
static init_handler CreateHandler(const char *name) {
    return init_handler(name, &THandler::Install);
}

void InstallPatch(LPCSTR description, std::initializer_list<uint8_t> bytes, std::initializer_list<intptr_t> addresses);

void InstallCallback(auto_ptr hook, cbInfo info, intptr_t *cb);
void InstallCallback(LPCSTR description, auto_ptr hook, cbInfo info, intptr_t *cb);
void InstallCallback(LPCSTR name, LPCSTR description, auto_ptr hook, std::initializer_list<cbInfo> callbacks);

void InstallVTableHook(LPCSTR name, auto_ptr hook, std::initializer_list<intptr_t> addresses);

inline void InstallPatch(std::initializer_list<uint8_t> bytes, std::initializer_list<intptr_t> addresses) {
    InstallPatch(nullptr, bytes, addresses);
}

inline void InstallCallback(auto_ptr hook, cbInfo info) {
    InstallCallback(hook, info, nullptr);
}

inline void InstallCallback(LPCSTR description, auto_ptr hook, cbInfo info) {
    InstallCallback(description, hook, info, nullptr);
}

inline void InstallCallback(LPCSTR name, auto_ptr hook, std::initializer_list<cbInfo> callbacks) {
    InstallCallback(name, nullptr, hook, callbacks);
}