#pragma once


const int MAX_NUMBER_OF_CUSTOM_CIVILIANS = 64;

extern int maxNumberOfVehiclesInArray;


static_assert(MAX_NUMBER_OF_CUSTOM_CIVILIANS >= 12, "MAX_NUMBER_OF_CUSTOM_CIVILIANS is not large enough!");

struct AIVehicleClass
{
    char buffer[0x7C];

    //AIVehicleClass(int seed)
    //{
    //    reinterpret_cast<AIVehicleClass * (__thiscall *)(AIVehicleClass *, int)>(0x43DCF0)(this, seed);
    //}
    //
    //AIVehicleClass(const AIVehicleClass &other)
    //{
    //    reinterpret_cast<AIVehicleClass * (__thiscall *)(AIVehicleClass *, const AIVehicleClass &)>(0x43DB50)(this, other);
    //}
};

ASSERT_SIZEOF(AIVehicleClass, 0x7C);

extern void patch_vehicle_size_check(intptr_t cmp_addr, intptr_t mov_addr);

template <size_t extra_size>
void patch_vehicle_size_check(intptr_t addr)
{
    // cmp <reg>, 12    {3}
    //                  {extra_size}
    // jb ...           {2}
    // mov <reg>, 12
    patch_vehicle_size_check(addr, addr+3+extra_size+2);
}

class AIManagerClass
{
public:
    static bool sm_bEnableOverrides;

    AIManagerClass * __ctor();
    void __dtor();

    static AIManagerClass * Get();

    int GetMaxNumberOfAIVehicles(int type);
    void SetMaxNumberOfAIVehicles(int type, int value);
};