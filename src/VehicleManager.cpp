#include "VehicleManager.h"

void CVehicleManager::DeleteObject(hamster::CAutoPtr<const CVehicle> &vehicle) {

}

hamster::CAutoPtr<CVehicle> & CVehicleManager::CreateVehicle(const void *a1, EVehicleType vehicleType) {
    return hamster::CAutoPtr<CVehicle>(nullptr);
}

hamster::CAutoPtr<CVehicle> & CVehicleManager::CreateVehicle(EVehicleType vehicleType, MAv4 position, float a3, bool a4, bool a5) {
    return hamster::CAutoPtr<CVehicle>(nullptr);
}