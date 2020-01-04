#pragma once
#include "common.h"

#include "d3dhook.h"
#include "hook.h"
#include "util.h"

//
// Hamster shared resources
//

#include "types.h"
#include "FileChunker.h"
#include "IUserCommandProxy.h"

enum HA_SOBJ_TYPE;

enum HA_ADDR_TYPE {
    HA_ADDR_WINDOW,
    HA_ADDR_VIEWPORT,

    HA_ADDR_DIRECT3D,
    HA_ADDR_DIRECT3D_DEVICE,

    /**** END OF DATA ****/
    HA_ADDR_COUNT,
};

namespace hamster {
    template <typename TValuePtr = void, typename TValue = intptr_t>
    class CAutoPtr {
    protected:
        using TAutoPtr = CAutoPtr<TValuePtr, TValue>;
        using ValuePtr_t = TValuePtr *;

        union {
            ValuePtr_t value_ptr;
            TValue value;
        };
    public:
        constexpr CAutoPtr(const std::nullptr_t &ptr)
            : value(0) {}

        constexpr CAutoPtr(TValue ptr)
            : value(ptr) {}

        constexpr CAutoPtr(ValuePtr_t ptr)
            : value_ptr(ptr) {}

        constexpr TValue get() {
            return value;
        }

        constexpr ValuePtr_t ptr() {
            return value_ptr;
        }

        constexpr operator TValue() const                           { return value; }
        constexpr operator ValuePtr_t() const                       { return value_ptr; }

        constexpr TAutoPtr & operator+(TAutoPtr &rhs) const         { return TAutoPtr(value + rhs.value); }
        constexpr TAutoPtr & operator-(TAutoPtr &rhs) const         { return TAutoPtr(value - rhs.value); }
        constexpr TAutoPtr & operator*(TAutoPtr &rhs) const         { return TAutoPtr(value * rhs.value); }
        constexpr TAutoPtr & operator/(TAutoPtr &rhs) const         { return TAutoPtr(value / rhs.value); }

        // ???
        constexpr TAutoPtr & operator=(ValuePtr_t value_ptr) {
            this->value_ptr = value_ptr;
            return *this;
        }

        constexpr bool operator==(const std::nullptr_t &rhs) const  { return value == NULL; }
        constexpr bool operator!=(const std::nullptr_t &rhs) const  { return value != NULL; }

        constexpr bool operator==(const TAutoPtr &rhs) const        { return value == rhs.value; }
        constexpr bool operator!=(const TAutoPtr &rhs) const        { return value != rhs.value; }
        constexpr bool operator<(const TAutoPtr &rhs) const         { return value < rhs.value; }
        constexpr bool operator<=(const TAutoPtr &rhs) const        { return value <= rhs.value; }
        constexpr bool operator>(const TAutoPtr &rhs) const         { return value > rhs.value; }
        constexpr bool operator>=(const TAutoPtr &rhs) const        { return value >= rhs.value; }
    };

    DWORD GetPointer(HA_ADDR_TYPE type);
    void SetPointer(HA_ADDR_TYPE type, DWORD value);

    DWORD GetSingletonObject(HA_SOBJ_TYPE index);
    void SetSingletonObject(HA_SOBJ_TYPE index, DWORD ptr);

    DWORD GetSingletonObjectsPointer(void);

    template <typename TType>
    constexpr inline TType GetPointer(HA_ADDR_TYPE type) {
        return reinterpret_cast<TType>(GetPointer(type));
    }

    template <typename TType>
    constexpr inline void SetPointer(HA_ADDR_TYPE type, TType *value) {
        return SetPointer(type, (DWORD)value);
    }

    template <typename TType>
    constexpr inline TType GetSingletonObject(HA_SOBJ_TYPE index) {
        return reinterpret_cast<TType>(GetSingletonObject(index));
    }

    template <typename TType>
    constexpr inline void SetSingletonObject(HA_SOBJ_TYPE index, TType *value) {
        SetSingletonObject(index, (DWORD)value);
    }
}

#ifdef USE_OLD_HAMSTER_SHIT
enum HA_ADDR_TYPE {
    HA_ADDR_HWND,

    HA_ADDR_SINGLETON_OBJECTS, // list of singleton objects

    HA_ADDR_VIEWPORT,

    HA_ADDR_IDIRECT3D9,
    HA_ADDR_IDIRECT3DDEVICE9,

    /**** END OF DATA ****/
    HA_ADDR_COUNT
};

struct HA_SOBJ_DATA {
    BYTE type;
    const char *name;
};

struct HA_ADDR_DATA {
    HA_ADDR_TYPE type;
    DWORD offset;
    LPCSTR name;
};

struct HA_ADDR_OFFSET_DATA {
    HA_ADDR_DATA offsets[HA_ADDR_COUNT];
};

class CHamsterGame {
protected:
    DWORD m_offsets[HA_ADDR_COUNT];
    DWORD *m_sobjs_offset;

    int m_gameVersion;

    NOINLINE void Initialize(HA_ADDR_OFFSET_DATA &offsetData) {
        LogFile::WriteLine("Resolving static offsets...");
        for (auto addr : offsetData.offsets)
        {
            DWORD offset = addr.offset;
            m_offsets[addr.type] = offset;

            LogFile::Format(" - '%s' => %08X\n", addr.name, offset);
        }
        LogFile::WriteLine("...Done");

        // used to resolve singleton offsets
        m_sobjs_offset = (DWORD*)m_offsets[HA_ADDR_SINGLETON_OBJECTS];

        // temp. hack cuz lazy
        //for (int i = 0; i < SOBJ_COUNT; i++)
        //    LogFile::Format("D3_SOBJ[%04d] -> %04X\n", i, (((i * 4) + i) * 4));
    };

public:
    CHamsterGame(int gameVersion) {
        m_gameVersion = gameVersion;
    };

    NO_INL int GetGameVersion() const {
        return m_gameVersion;
    };

    NOINLINE DWORD GetPointer(HA_ADDR_TYPE type) {
        return *(DWORD*)m_offsets[type];
    };

    NOINLINE bool SetPointer(HA_ADDR_TYPE type, LPVOID value) {
        return ((LPVOID)(*(DWORD*)m_offsets[type] = (DWORD)value) == value);
    };

    NOINLINE DWORD GetSingletonPointer(BYTE type) {
        return *(DWORD*)&m_sobjs_offset[type];
    };

    NOINLINE bool SetSingletonPointer(BYTE type, LPVOID value) {
        return ((LPVOID)(*(DWORD*)&m_sobjs_offset[type] = (DWORD)value) == value);
    };

    template<typename T>
    T GetPointer(HA_ADDR_TYPE type) {
        return (T)GetPointer(type);
    };

    template<typename T>
    T GetSingletonPointer(BYTE type) {
        return (T)GetSingletonPointer(type);
    };

    NO_INL HWND GetMainWindowHwnd(void)                                 { return GetPointer<HWND>(HA_ADDR_HWND); };

    NO_INL IDirect3D9* GetD3D(void)                                     { return GetPointer<IDirect3D9*>(HA_ADDR_IDIRECT3D9); };
    NO_INL IDirect3DDevice9* GetD3DDevice(void)                         { return GetPointer<IDirect3DDevice9*>(HA_ADDR_IDIRECT3DDEVICE9); };

    NO_INL bool SetD3D(IDirect3D9 *pD3d)                                { return SetPointer(HA_ADDR_IDIRECT3D9, pD3d); };
    NO_INL bool SetD3DDevice(IDirect3DDevice9 *pD3dDevice)              { return SetPointer(HA_ADDR_IDIRECT3DDEVICE9, pD3dDevice); };
};
#endif