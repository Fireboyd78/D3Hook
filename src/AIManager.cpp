#include "hooksystem.h"
#include "fiero.h"

#include "fuz.h"
#include "driv3r.h"

#include "AIManager.h"

using AIDataPointer_t = std::aligned_storage<4, 1>::type;
using AIManagerCharacterClass_t = std::aligned_storage<16, 1>::type;

using VehicleList = fuz::fixed_list<AIVehicleClass, 12>;
using CharacterList = fuz::fixed_list<AIManagerCharacterClass_t, 12>;
using DataPtrList = fuz::fixed_list<AIDataPointer_t, 12>;

ASSERT_SIZEOF(CharacterList, 0x134);
ASSERT_SIZEOF(DataPtrList, 0xA4);
ASSERT_SIZEOF(VehicleList, 0x644);
ASSERT_SIZEOF(VehicleList[9], 0x3864);

static fiero::Field<0xA0, int> _maximumNumberOfTypeAIVehiclesArray;
static fiero::Field<0xC4, float> _recprocalVehicleDensityArray;
static fiero::Field<0xE8, VehicleList> _AIVehicleList; // array[9]
static fiero::Field<0x394c, CharacterList> _AICharacterList;
static fiero::Field<0x3a84, DataPtrList> _field_3a84;
static fiero::Field<0xb9c0, DataPtrList> _field_b9c0;

using CustomVehicleStorage = fuz::fixed_list<AIVehicleClass, MAX_NUMBER_OF_CUSTOM_CIVILIANS>;
using CustomCharacterStorage = fuz::fixed_list<AIManagerCharacterClass_t, MAX_NUMBER_OF_CUSTOM_CIVILIANS>;
using CustomDataPtrStorage = fuz::fixed_list<AIDataPointer_t, MAX_NUMBER_OF_CUSTOM_CIVILIANS>;

CustomVehicleStorage g_AIVehicleList[9];
CustomCharacterStorage g_AIUnkThingList;
CustomDataPtrStorage g_AIDataPtrList;
CustomDataPtrStorage g_AISpecPtrList;

AIManagerClass * (__thiscall *_AIManagerClass__ctor)(AIManagerClass *);
void (__thiscall *_AIManagerClass__dtor)(AIManagerClass *);

int maxNumberOfVehiclesInArray = 12;

void patch_vehicle_size_check(intptr_t cmp_addr, intptr_t mov_addr)
{
    mem::write<uint8_t>(cmp_addr+2, maxNumberOfVehiclesInArray);

    if (mov_addr != NULL)
        mem::write<int32_t>(mov_addr+1, maxNumberOfVehiclesInArray);
}

static void PatchVehicleArraySize(bool success)
{
    static bool patched = false;

    // ensure one-time init upon successful injection
    if (patched && success)
        return;

    // ensure default values reset upon failed injection after successful injection
    if (!patched && !success)
        return;

    patch_vehicle_size_check(0x440791, NULL);
    patch_vehicle_size_check(0x4408E3+4, NULL); // 394c
    patch_vehicle_size_check(0x440B15+4, NULL); // b9c0
    patch_vehicle_size_check(0x440C2E, NULL);
    patch_vehicle_size_check(0x440DFE+4, NULL); // b9c0

    patch_vehicle_size_check<0>(0x43D6AD);

    patch_vehicle_size_check<0>(0x43DFA9);
    patch_vehicle_size_check<6>(0x43DFBD);
    patch_vehicle_size_check<6>(0x43DFF1);
    patch_vehicle_size_check<6>(0x43E00B);
    patch_vehicle_size_check<6>(0x43E025);
    patch_vehicle_size_check<6>(0x43E03F);
    patch_vehicle_size_check<6>(0x43E059);
    patch_vehicle_size_check<6>(0x43E073);

    patch_vehicle_size_check<7*2+3>(0x4411B8);
    patch_vehicle_size_check<6>(0x4411DD);
    patch_vehicle_size_check<6>(0x4411F7);
    patch_vehicle_size_check<6>(0x441211);
    patch_vehicle_size_check<6>(0x44122B);
    patch_vehicle_size_check<6>(0x441245);
    patch_vehicle_size_check<6>(0x44125F);
    patch_vehicle_size_check<6>(0x441279);
    patch_vehicle_size_check<6>(0x441293);

    patched = success;
}

bool AIManagerClass::sm_bEnableOverrides = false;

AIManagerClass * AIManagerClass::__ctor()
{
    _AIManagerClass__ctor(this);

    bool incomplete = true;

    if (_AICharacterList.ptr(this)->set_linked_list(g_AIUnkThingList) &&
        _field_3a84.ptr(this)->set_linked_list(g_AIDataPtrList) &&
        _field_b9c0.ptr(this)->set_linked_list(g_AISpecPtrList))
    {
        auto pVehicleList = _AIVehicleList.ptr(this);

        for (int i = 0; i < 9; i++)
        {
            if (!pVehicleList[i].set_linked_list(g_AIVehicleList[i]))
                break;
        }

        incomplete = false;
    }
    
    if (!incomplete)
    {
        LogFile::WriteLine("**** SUCCESSFULLY LINKED NEW AIMANAGER STORAGE");
        maxNumberOfVehiclesInArray = MAX_NUMBER_OF_CUSTOM_CIVILIANS;

        PatchVehicleArraySize(true);
    }
    else
    {
        LogFile::WriteLine("**** FAILED TO LINK NEW AIMANAGER STORAGE");
        maxNumberOfVehiclesInArray = 12;

        PatchVehicleArraySize(false);
    }


    
    return this;
}

void AIManagerClass::__dtor()
{
    _AIManagerClass__dtor(this);
}

AIManagerClass * AIManagerClass::Get()
{
    return hamster::SingletonVar<hamster::ESingletonType::AIManager, AIManagerClass>::Get();
}

int AIManagerClass::GetMaxNumberOfAIVehicles(int type)
{
    return _maximumNumberOfTypeAIVehiclesArray.ptr(this)[type];
}

void AIManagerClass::SetMaxNumberOfAIVehicles(int type, int value){
    if (value > maxNumberOfVehiclesInArray)
        value = maxNumberOfVehiclesInArray;

    _maximumNumberOfTypeAIVehiclesArray.ptr(this)[type] = value;
    _recprocalVehicleDensityArray.ptr(this)[type] = 0.0f;
}

// __thiscall
void __fastcall LifeNode_CivilianTrafficControl__OnEnable_Hook(void *_this)
{
    if (!AIManagerClass::sm_bEnableOverrides)
        reinterpret_cast<int (__thiscall *)(void *)>(0x4B5CB0)(_this);
}

init_handler AIManagerHandler("AI Manager", []() {

    InstallCallback("AIManagerClass::ctor", &AIManagerClass::__ctor, addressof<CALL>(0x43FC34, BADADDR), reinterpret_cast<intptr_t *>(&_AIManagerClass__ctor));
    InstallCallback("AIManagerClass::dtor", &AIManagerClass::__dtor, addressof<CALL>(0x4446A3, BADADDR), reinterpret_cast<intptr_t *>(&_AIManagerClass__dtor));

    InstallVTableHook("LifeNode_CivilianTrafficControl::OnEnable", &LifeNode_CivilianTrafficControl__OnEnable_Hook, { 0x6F2308 });

    return true;
});

