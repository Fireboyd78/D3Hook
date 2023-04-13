#pragma once

using AICivilianCarClass_t = std::aligned_storage<0x9F4, 4>::type;

using CivCarVehicleList = fuz::fixed_list<AICivilianCarClass_t, 12>;
using CivCarIteratorList = fuz::array<CivCarVehicleList::iterator, 12>;

ASSERT_SIZEOF(CivCarVehicleList, 0x77E4);

class AICivilianCarManagerClass
{
public:

    CivCarVehicleList m_AICivilianCarList;
    CivCarIteratorList m_AICivilianCarListIteratorArray;
    unsigned int m_AICivilianCarListIteratorArrayIndex;

    //AICivilianCarManagerClass * __ctor();
    //void __dtor();

    static AICivilianCarManagerClass * Get();
};

ASSERT_SIZEOF(AICivilianCarManagerClass, 0x7818);