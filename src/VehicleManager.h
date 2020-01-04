#pragma once

#include "Vehicles_Common.h"

class IVehicleManager;
class CVehicleManager;
class CIVehicleManager;

extern class CVehicle;

class IVehicleManager {
public:
    virtual ~IVehicleManager() {};
};

class CIVehicleManager : protected IVehicleManager {
protected:
    friend CVehicleManager;

    CVehicleManager *manager;
public:
    static CIVehicleManager * Get() {
        return hamster::SingletonVar<hamster::ESingletonType::VehicleManager, CIVehicleManager>::Get();
    };
};

class CVehicleManager : public CIVehicleManager {
public:
    static CVehicleManager * Get() {
        return CIVehicleManager::Get()->manager;
    };

    virtual ~CVehicleManager() {}

    virtual void DeleteObject(hamster::CAutoPtr<const CVehicle> &);

    virtual hamster::CAutoPtr<CVehicle> & CreateVehicle(const void *, EVehicleType);
    virtual hamster::CAutoPtr<CVehicle> & CreateVehicle(EVehicleType, MAv4, float, bool, bool);
};