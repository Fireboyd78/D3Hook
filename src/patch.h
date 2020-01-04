#pragma once
#include "common.h"

#ifdef _DEBUG
# define MEM_DEBUG_ASSERTS  1
#endif

#define MEM_DEAD            0
#define MEM_OKAY            1
#define MEM_DEFERRED        2
#define MEM_PREPARED        (MEM_DEAD | MEM_DEFERRED)

namespace mem
{
    namespace fx
    {
        template <typename Func, typename... Args>
        inline void unpack_each(Func func, Args&&... args)
        {
            using unpacker_t = int[];

            (void)unpacker_t
            {
                (func(std::forward<Args>(args)), 0)..., 0
            };
        }

        template <typename... _Args>
        inline void unpack(void *dst, _Args&& ...args)
        {
            unpack_each([&](const auto &value) {
                memcpy(dst, &value, sizeof(value));
                dst = static_cast<char *>(dst) + sizeof(value);
            }, std::forward<_Args>(args)...);
        }

        inline BOOL protect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
        {
        #if MEM_DEBUG_ASSERTS
            BOOL result = VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
            assert(result != MEM_DEAD);

            return result;
        #else
            return VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
        #endif
        }

        inline bool read(const void *src, void *dst, size_t size)
        {
            memcpy(dst, src, size);
            return true;
        }

        inline bool write(void *dst, const void *src, size_t size)
        {
            memcpy(dst, src, size);
            return true;
        }

        template <typename... _Args>
        inline bool write(void *dst, size_t size, _Args ...args)
        {
            static_assert(sizeof...(args) > 0,
                "No arguments provided");

            static_assert(variadic::true_for_all<std::is_trivially_copyable<_Args>::value...>,
                "Not all arguments are trivially copyable");

            constexpr size_t totalSize = variadic::sum<sizeof(_Args)...>;

            if (size < totalSize)
                return false; // gg

            unpack(dst, args...);
            return true;
        }
    }

    class memory_region {
    protected:
        auto_ptr _Addr;

        size_t _Size;
        size_t _State;

        inline memory_region(auto_ptr addr, size_t size, size_t state)
            : _Addr(addr), _Size(size), _State(state) {}
    public:
        inline memory_region(const memory_region &) = delete;

        inline memory_region(auto_ptr addr, size_t size)
            : memory_region(addr, size, MEM_OKAY) {}

        inline auto_ptr addr() const {
            return _Addr;
        }

        inline size_t size() const {
            return _Size;
        }

        inline size_t state() const {
            return _State;
        }
    };

    class memory_protector : public memory_region {
        friend class scoped_protector;
    private:
        DWORD _Protection;

        inline void _Protect(size_t access) {
            // MEM_OKAY or MEM_DEAD
            _State = (fx::protect(_Addr, _Size, access, &_Protection) & 1);
        }

        inline void _Unprotect() {
            size_t result = (fx::protect(_Addr, _Size, _Protection, &_Protection) & 1);
            
            // MEM_DEFERRED or MEM_PREPARED
            _State = (result | MEM_DEFERRED);
        }
    public:
        memory_protector(const memory_protector &) = delete;

        inline memory_protector(auto_ptr addr)
            : memory_region(addr, 0, MEM_DEFERRED) {};

        inline memory_protector(auto_ptr addr, size_t size)
            : memory_region(addr, size, MEM_PREPARED) {};

        inline ~memory_protector() { /* do nothing */ }

        inline bool protect(size_t access) {
            if (_State == MEM_PREPARED)
                _Protect(access);

            return (_State == MEM_OKAY);
        }

        inline bool protect(size_t size, size_t access) {
            if (_State == MEM_OKAY)
                return false;

            _Size = size;
            _Protect(access);

            return (_State == MEM_OKAY);
        }

        inline bool unprotect() {
            if (_State != MEM_OKAY)
                return false;

            _Unprotect();
            return true;
        }
    };

    class scoped_protector : public memory_protector {
    public:
        scoped_protector(const scoped_protector &) = delete;
        scoped_protector(auto_ptr addr, size_t size) = delete;

        inline scoped_protector(auto_ptr addr, size_t size, size_t access)
            : memory_protector(addr, size) {
            _Protect(access);
        }

        inline ~scoped_protector() {
            if (_State == MEM_OKAY)
                _Unprotect();
        }
    };

    inline bool copy(auto_ptr dest, const void *src, size_t size)
    {
        scoped_protector xp(dest, size, PAGE_EXECUTE_READWRITE);

        return fx::write(dest, src, size);
    }

    template <typename TType>
    inline TType read(auto_ptr address) {
        static_assert(std::is_trivially_copyable<TType>::value, "Type is not trivially copyable");

        scoped_protector xp(address, sizeof(TType), PAGE_EXECUTE_READWRITE);

        if (xp.state() == MEM_OKAY)
            return *address.ptr<TType>(offset);

        return NULL;
    }

    template <typename ...TArgs>
    inline bool write(auto_ptr address, TArgs ...args)
    {
        static_assert(sizeof...(args) > 0,
            "No arguments provided");

        static_assert(variadic::true_for_all<std::is_trivially_copyable<TArgs>::value...>,
            "Not all arguments are trivially copyable");

        constexpr size_t totalSize = variadic::sum<sizeof(TArgs)...>;
        {
            scoped_protector xp(address, totalSize, PAGE_EXECUTE_READWRITE);

            fx::unpack(address, args...);
        }
        
        return true;
    }

    template <typename ...TArgs>
    inline bool write(std::initializer_list<auto_ptr> addresses, TArgs ...args)
    {
        static_assert(sizeof...(args) > 0,
            "No arguments provided");

        static_assert(variadic::true_for_all<std::is_trivially_copyable<TArgs>::value...>,
            "Not all arguments are trivially copyable");

        constexpr size_t totalSize = variadic::sum<sizeof(TArgs)...>;

        for (auto addr : addresses)
        {
            scoped_protector xp(addr, totalSize, PAGE_EXECUTE_READWRITE);

            LogFile::Format("** writing %d bytes to %X\n", totalSize, addr);
            fx::unpack(addr, args...);
        }

        return true;
    }

    inline bool copy(memory_region &xp, const void *src, size_t size)
    {
        if (xp.state() != MEM_OKAY)
            return false;

        size_t length = xp.size();

        if (length < size)
            return false;

        auto_ptr addr = xp.addr();

        return fx::read(src, addr, size);
    }

    template <typename TType>
    inline TType read(memory_region &xp) {
        static_assert(std::is_trivially_copyable<TType>::value, "Type is not trivially copyable");

        if (xp.state() == MEM_OKAY)
        {
            auto_ptr addr = xp.addr();

            return *addr.ptr<TType>();
        }

        return NULL;
    }

    inline bool write(memory_region &xp, const void *src, size_t size) {
        if (xp.state() != MEM_OKAY)
            return false;

        size_t length = xp.size();

        if (length < size)
            return false;

        auto_ptr addr = xp.addr();

        return fx::write(addr, src, size);
    }

    template <typename ...TArgs>
    inline bool write(memory_region &xp, TArgs ...args) {
        if (xp.state() != MEM_OKAY)
            return false;

        auto addr = xp.addr();

        return fx::write(addr, xp.size(), args...);
    }
}
