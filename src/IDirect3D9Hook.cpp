#include "IDirect3D9Hook.h"

#define D3D pD3D

/*** IUnknown methods ***/
STDMETHODIMP IDirect3D9Hook::QueryInterface(REFIID riid, void** ppvObj) {
    return D3D->QueryInterface(riid, ppvObj);
}

STDMETHODIMP_(ULONG) IDirect3D9Hook::AddRef(THIS) {
    return D3D->AddRef();
}

STDMETHODIMP_(ULONG) IDirect3D9Hook::Release(THIS) {
    return D3D->Release();
}

/*** IDirect3D9 methods ***/
STDMETHODIMP IDirect3D9Hook::RegisterSoftwareDevice(THIS_ void* pInitializeFunction) {
    return D3D->RegisterSoftwareDevice(pInitializeFunction);
}

STDMETHODIMP_(UINT) IDirect3D9Hook::GetAdapterCount(THIS) {
    return D3D->GetAdapterCount();
}

STDMETHODIMP IDirect3D9Hook::GetAdapterIdentifier(THIS_ UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) {
    return D3D->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

STDMETHODIMP_(UINT) IDirect3D9Hook::GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format) {
    return D3D->GetAdapterModeCount(Adapter, Format);
}

STDMETHODIMP IDirect3D9Hook::EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) {
    return D3D->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

STDMETHODIMP IDirect3D9Hook::GetAdapterDisplayMode(THIS_ UINT Adapter, D3DDISPLAYMODE* pMode) {
    return D3D->GetAdapterDisplayMode(Adapter, pMode);
}

STDMETHODIMP IDirect3D9Hook::CheckDeviceType(THIS_ UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
    return D3D->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

STDMETHODIMP IDirect3D9Hook::CheckDeviceFormat(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
    return D3D->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

STDMETHODIMP IDirect3D9Hook::CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) {
    return D3D->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

STDMETHODIMP IDirect3D9Hook::CheckDepthStencilMatch(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) {
    return D3D->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

STDMETHODIMP IDirect3D9Hook::CheckDeviceFormatConversion(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) {
    return D3D->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

STDMETHODIMP IDirect3D9Hook::GetDeviceCaps(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) {
    return D3D->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

STDMETHODIMP_(HMONITOR) IDirect3D9Hook::GetAdapterMonitor(THIS_ UINT Adapter) {
    return D3D->GetAdapterMonitor(Adapter);
}

STDMETHODIMP IDirect3D9Hook::CreateDevice(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) {
    return D3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
}