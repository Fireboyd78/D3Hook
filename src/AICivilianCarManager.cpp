#include "hooksystem.h"
#include "fiero.h"

#include "fuz.h"
#include "driv3r.h"

#include "AIManager.h"
#include "AICivilianCarManager.h"

using CustomCivCarVehicleList = fuz::fixed_list<AICivilianCarClass_t, MAX_NUMBER_OF_CUSTOM_CIVILIANS>;
using CustomCivCarIteratorList = fuz::array<CivCarVehicleList::iterator, MAX_NUMBER_OF_CUSTOM_CIVILIANS>;

struct AICivilianCarManagerClass_new
{
    CivCarVehicleList m_AICivilianCarList;
    // offset to here is same as the original, but we now have more slots available
    CustomCivCarIteratorList m_AICivilianCarListIteratorArray;
    unsigned int m_AICivilianCarListIteratorArrayIndex;
};

const size_t sizeof_CustomAICivilianCarManagerClass = sizeof(AICivilianCarManagerClass_new);
const size_t offsetof_NewCivCarListIteratorArrayIndex = offsetof(AICivilianCarManagerClass_new, m_AICivilianCarListIteratorArrayIndex);

AICivilianCarManagerClass * AICivilianCarManagerClass::Get()
{
    return hamster::SingletonVar<hamster::ESingletonType::AICivilianCarManager, AICivilianCarManagerClass>::Get();
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

    patch_vehicle_size_check(0x437F23, NULL);
    patch_vehicle_size_check(0x439FCE, NULL); // default is 30?!
    //patch_vehicle_size_check(0x439FAE, NULL); // this one is really weird...

    patch_vehicle_size_check(0x43A769+4, NULL); // add vehicle check
}

AICivilianCarManagerClass_new * (__cdecl *AICivilianCarManagerNew)();

CustomCivCarVehicleList g_AICivCarList;

AICivilianCarManagerClass_new * AICivilianCarManagerNew_Hook()
{
    AICivilianCarManagerClass_new *pAICivCarManager = AICivilianCarManagerNew();

    if (pAICivCarManager->m_AICivilianCarList.set_linked_list(g_AICivCarList))
    {
        LogFile::WriteLine("**** SUCCESSFULLY LINKED NEW AICIVCARMANAGER STORAGE");
        PatchVehicleArraySize(true);
    }
    else
    {
        LogFile::WriteLine("**** FAILED TO LINK NEW AICIVCARMANAGER STORAGE");
        PatchVehicleArraySize(false);
    }

    // reset the end of the memory so we know it'll be valid
    memset(&pAICivCarManager->m_AICivilianCarListIteratorArray, 0, sizeof(AICivilianCarManagerClass_new) - sizeof(CivCarVehicleList));

    pAICivCarManager->m_AICivilianCarListIteratorArrayIndex = 0;

    auto &iterlist = pAICivCarManager->m_AICivilianCarListIteratorArray;

    // fill in the default node pointers
    for (auto i = iterlist.begin(); i != iterlist.end(); i++)
    {
        *fuz::node_ptr(i) = &pAICivCarManager->m_AICivilianCarList.m_end;
    }

    return pAICivCarManager;
}

init_handler AICivilianCarManagerHandler("AI Civilian Car Manager", []() {
    InstallCallback("AICivilianCarManagerNew", &AICivilianCarManagerNew_Hook, addressof<CALL>(0x43E0E1, BADADDR), reinterpret_cast<intptr_t *>(&AICivilianCarManagerNew));

    // allocate some more room for the bigger iterator list
    mem::write<size_t>(addressof(0x43A970+1, BADADDR), sizeof_CustomAICivilianCarManagerClass);

    // patch AICivilianCarManagerClass methods to point to new iterator array index
    mem::write<size_t>({
        addressof(0x437D90+2, BADADDR), // Reset
        addressof(0x437F1D+2, BADADDR), // AICivilianCarManagerManipulateCars
        addressof(0x439FC5+2, BADADDR), // ^
        addressof(0x439FD1+2, BADADDR), // ^
        addressof(0x439FD9+2, BADADDR), // ^
        addressof(0x437F1D+2, BADADDR), // ^
        addressof(0x43A7E1+2, BADADDR), // AICivilianCarManagerAddAIVehicle
        addressof(0x43A855+2, BADADDR), // AICivilianCarManagerRemoveAIVehicle
    }, offsetof_NewCivCarListIteratorArrayIndex);

    return true;
});
