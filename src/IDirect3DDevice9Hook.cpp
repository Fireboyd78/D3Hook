#include "IDirect3DDevice9Hook.h"
#include <algorithm>

/*** IUnknown methods ***/
STDCALL(IDirect3DDevice9Hook::QueryInterface)(THIS_ REFIID riid, void** ppvObj){
    return DEVICE->QueryInterface(riid, ppvObj);
}

STDCALL_(ULONG, IDirect3DDevice9Hook::AddRef)(THIS){
    return DEVICE->AddRef();
}

STDCALL_(ULONG, IDirect3DDevice9Hook::Release)(THIS){
    return DEVICE->Release();
}


/*** IDirect3DDevice9 methods ***/
STDCALL(IDirect3DDevice9Hook::TestCooperativeLevel)(THIS){
    return DEVICE->TestCooperativeLevel();
}

STDCALL_(UINT, IDirect3DDevice9Hook::GetAvailableTextureMem)(THIS){
    return DEVICE->GetAvailableTextureMem();
}

STDCALL(IDirect3DDevice9Hook::EvictManagedResources)(THIS){
    return DEVICE->EvictManagedResources();
}

STDCALL(IDirect3DDevice9Hook::GetDirect3D)(THIS_ IDirect3D9** ppD3D9){
    if (!pD3D) {
        return DEVICE->GetDirect3D(ppD3D9);
    }
    *ppD3D9 = pD3D;
    return D3D_OK;
}

STDCALL(IDirect3DDevice9Hook::GetDeviceCaps)(THIS_ D3DCAPS9* pCaps){
    return DEVICE->GetDeviceCaps(pCaps);
}

STDCALL(IDirect3DDevice9Hook::GetDisplayMode)(THIS_ UINT iSwapChain, D3DDISPLAYMODE* pMode){
    return DEVICE->GetDisplayMode(iSwapChain, pMode);
}

STDCALL(IDirect3DDevice9Hook::GetCreationParameters)(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters){
    return DEVICE->GetCreationParameters(pParameters);
}

STDCALL(IDirect3DDevice9Hook::SetCursorProperties)(THIS_ UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap){
    return DEVICE->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

STDCALL_(void, IDirect3DDevice9Hook::SetCursorPosition)(THIS_ int X, int Y, DWORD Flags){
    return DEVICE->SetCursorPosition(X, Y, Flags);
}

STDCALL_(BOOL, IDirect3DDevice9Hook::ShowCursor)(THIS_ BOOL bShow){
    return DEVICE->ShowCursor(bShow);
}

STDCALL(IDirect3DDevice9Hook::CreateAdditionalSwapChain)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain){
    return DEVICE->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

STDCALL(IDirect3DDevice9Hook::GetSwapChain)(THIS_ UINT iSwapChain, IDirect3DSwapChain9** pSwapChain){
    return DEVICE->GetSwapChain(iSwapChain, pSwapChain);
}

STDCALL_(UINT, IDirect3DDevice9Hook::GetNumberOfSwapChains)(THIS){
    return DEVICE->GetNumberOfSwapChains();
}

STDCALL(IDirect3DDevice9Hook::Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters){
#if FALSE
    if (pPresentationParameters != NULL)
    {
        HWND window = pPresentationParameters->hDeviceWindow;

        pPresentationParameters->BackBufferCount = 3;
        pPresentationParameters->EnableAutoDepthStencil = true;

        pPresentationParameters->Windowed = true;
        pPresentationParameters->Flags = 0;
        pPresentationParameters->FullScreen_RefreshRateInHz = 0;
        
        SetWindowPos(window, HWND_NOTOPMOST, 0, 0, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight, SWP_SHOWWINDOW);
        SetWindowLong(window, GWL_STYLE, WS_CAPTION|WS_POPUP|WS_THICKFRAME);
    }
#endif

    return DEVICE->Reset(pPresentationParameters);
}

STDCALL(IDirect3DDevice9Hook::Present)(THIS_ CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion){
    return DEVICE->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

STDCALL(IDirect3DDevice9Hook::GetBackBuffer)(THIS_ UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer){
    return DEVICE->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

STDCALL(IDirect3DDevice9Hook::GetRasterStatus)(THIS_ UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus){
    return DEVICE->GetRasterStatus(iSwapChain, pRasterStatus);
}

STDCALL(IDirect3DDevice9Hook::SetDialogBoxMode)(THIS_ BOOL bEnableDialogs){
    return DEVICE->SetDialogBoxMode(bEnableDialogs);
}

STDCALL_(void, IDirect3DDevice9Hook::SetGammaRamp)(THIS_ UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp){
    return DEVICE->SetGammaRamp(iSwapChain, Flags, pRamp);
}

STDCALL_(void, IDirect3DDevice9Hook::GetGammaRamp)(THIS_ UINT iSwapChain, D3DGAMMARAMP* pRamp){
    return DEVICE->GetGammaRamp(iSwapChain, pRamp);
}

STDCALL(IDirect3DDevice9Hook::CreateTexture)(THIS_ UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle){
    return DEVICE->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::CreateVolumeTexture)(THIS_ UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle){
    return DEVICE->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::CreateCubeTexture)(THIS_ UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle){
    return DEVICE->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::CreateVertexBuffer)(THIS_ UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle){
    return DEVICE->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::CreateIndexBuffer)(THIS_ UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle){
    return DEVICE->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::CreateRenderTarget)(THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle){
    return DEVICE->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::CreateDepthStencilSurface)(THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle){
    return DEVICE->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::UpdateSurface)(THIS_ IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint){
    return DEVICE->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

STDCALL(IDirect3DDevice9Hook::UpdateTexture)(THIS_ IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture){
    return DEVICE->UpdateTexture(pSourceTexture, pDestinationTexture);
}

STDCALL(IDirect3DDevice9Hook::GetRenderTargetData)(THIS_ IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface){
    return DEVICE->GetRenderTargetData(pRenderTarget, pDestSurface);
}

STDCALL(IDirect3DDevice9Hook::GetFrontBufferData)(THIS_ UINT iSwapChain, IDirect3DSurface9* pDestSurface){
    return DEVICE->GetFrontBufferData(iSwapChain, pDestSurface);
}

STDCALL(IDirect3DDevice9Hook::StretchRect)(THIS_ IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter){
    return DEVICE->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

STDCALL(IDirect3DDevice9Hook::ColorFill)(THIS_ IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color){
    return DEVICE->ColorFill(pSurface, pRect, color);
}

STDCALL(IDirect3DDevice9Hook::CreateOffscreenPlainSurface)(THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle){
    return DEVICE->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

STDCALL(IDirect3DDevice9Hook::SetRenderTarget)(THIS_ DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget){
    return DEVICE->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

STDCALL(IDirect3DDevice9Hook::GetRenderTarget)(THIS_ DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget){
    return DEVICE->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

STDCALL(IDirect3DDevice9Hook::SetDepthStencilSurface)(THIS_ IDirect3DSurface9* pNewZStencil){
    return DEVICE->SetDepthStencilSurface(pNewZStencil);
}

STDCALL(IDirect3DDevice9Hook::GetDepthStencilSurface)(THIS_ IDirect3DSurface9** ppZStencilSurface){
    return DEVICE->GetDepthStencilSurface(ppZStencilSurface);
}

STDCALL(IDirect3DDevice9Hook::BeginScene)(THIS){
    return DEVICE->BeginScene();
}

STDCALL(IDirect3DDevice9Hook::EndScene)(THIS){
    return DEVICE->EndScene();
}

STDCALL(IDirect3DDevice9Hook::Clear)(THIS_ DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil){
    return DEVICE->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

STDCALL(IDirect3DDevice9Hook::SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix){
    return DEVICE->SetTransform(State, pMatrix);
}

STDCALL(IDirect3DDevice9Hook::GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix){
    return DEVICE->GetTransform(State, pMatrix);
}

STDCALL(IDirect3DDevice9Hook::MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix){
    return DEVICE->MultiplyTransform(State, pMatrix);
}

STDCALL(IDirect3DDevice9Hook::SetViewport)(THIS_ CONST D3DVIEWPORT9* pViewport){
    return DEVICE->SetViewport(pViewport);
}

STDCALL(IDirect3DDevice9Hook::GetViewport)(THIS_ D3DVIEWPORT9* pViewport){
    return DEVICE->GetViewport(pViewport);
}

STDCALL(IDirect3DDevice9Hook::SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial){
    return DEVICE->SetMaterial(pMaterial);
}

STDCALL(IDirect3DDevice9Hook::GetMaterial)(THIS_ D3DMATERIAL9* pMaterial){
    return DEVICE->GetMaterial(pMaterial);
}

STDCALL(IDirect3DDevice9Hook::SetLight)(THIS_ DWORD Index, CONST D3DLIGHT9* pLight){
    return DEVICE->SetLight(Index, pLight);
}

STDCALL(IDirect3DDevice9Hook::GetLight)(THIS_ DWORD Index, D3DLIGHT9* pLight){
    return DEVICE->GetLight(Index, pLight);
}

STDCALL(IDirect3DDevice9Hook::LightEnable)(THIS_ DWORD Index, BOOL Enable){
    return DEVICE->LightEnable(Index, Enable);
}

STDCALL(IDirect3DDevice9Hook::GetLightEnable)(THIS_ DWORD Index, BOOL* pEnable){
    return DEVICE->GetLightEnable(Index, pEnable);
}

STDCALL(IDirect3DDevice9Hook::SetClipPlane)(THIS_ DWORD Index, CONST float* pPlane){
    return DEVICE->SetClipPlane(Index, pPlane);
}

STDCALL(IDirect3DDevice9Hook::GetClipPlane)(THIS_ DWORD Index, float* pPlane){
    return DEVICE->GetClipPlane(Index, pPlane);
}

STDCALL(IDirect3DDevice9Hook::SetRenderState)(THIS_ D3DRENDERSTATETYPE State, DWORD Value){
    return DEVICE->SetRenderState(State, Value);
}

STDCALL(IDirect3DDevice9Hook::GetRenderState)(THIS_ D3DRENDERSTATETYPE State, DWORD* pValue){
    return DEVICE->GetRenderState(State, pValue);
}

STDCALL(IDirect3DDevice9Hook::CreateStateBlock)(THIS_ D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB){
    return DEVICE->CreateStateBlock(Type, ppSB);
}

STDCALL(IDirect3DDevice9Hook::BeginStateBlock)(THIS){
    return DEVICE->BeginStateBlock();
}

STDCALL(IDirect3DDevice9Hook::EndStateBlock)(THIS_ IDirect3DStateBlock9** ppSB){
    return DEVICE->EndStateBlock(ppSB);
}

STDCALL(IDirect3DDevice9Hook::SetClipStatus)(THIS_ CONST D3DCLIPSTATUS9* pClipStatus){
    return DEVICE->SetClipStatus(pClipStatus);
}

STDCALL(IDirect3DDevice9Hook::GetClipStatus)(THIS_ D3DCLIPSTATUS9* pClipStatus){
    return DEVICE->GetClipStatus(pClipStatus);
}

STDCALL(IDirect3DDevice9Hook::GetTexture)(THIS_ DWORD Stage, IDirect3DBaseTexture9** ppTexture){
    return DEVICE->GetTexture(Stage, ppTexture);
}

STDCALL(IDirect3DDevice9Hook::SetTexture)(THIS_ DWORD Stage, IDirect3DBaseTexture9* pTexture){
    return DEVICE->SetTexture(Stage, pTexture);
}

STDCALL(IDirect3DDevice9Hook::GetTextureStageState)(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue){
    return DEVICE->GetTextureStageState(Stage, Type, pValue);
}

STDCALL(IDirect3DDevice9Hook::SetTextureStageState)(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value){
    return DEVICE->SetTextureStageState(Stage, Type, Value);
}

STDCALL(IDirect3DDevice9Hook::GetSamplerState)(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue){
    return DEVICE->GetSamplerState(Sampler, Type, pValue);
}

STDCALL(IDirect3DDevice9Hook::SetSamplerState)(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value){
    const int anisotropicFiltering = 1; // TODO: config...

    static bool anisotropyDetectNeeded = true;
    static int nMaxAnisotropy = 16;

    // Disable AntiAliasing when using point filtering
    //if (Config.AntiAliasing)
    {
        if (Type == D3DSAMP_MINFILTER || Type == D3DSAMP_MAGFILTER)
        {
            if (Value == D3DTEXF_NONE || Value == D3DTEXF_POINT)
            {
                DEVICE->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
            }
            else
            {
                DEVICE->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
            }
        }
    }

    // detect max anisotropy
    if (anisotropyDetectNeeded && (Type == D3DSAMP_MAXANISOTROPY || ((Type == D3DSAMP_MINFILTER || Type == D3DSAMP_MAGFILTER) && Value == D3DTEXF_LINEAR)))
    {
        anisotropyDetectNeeded = false;

        D3DCAPS9 Caps;
        ZeroMemory(&Caps, sizeof(D3DCAPS9));
        if (SUCCEEDED(DEVICE->GetDeviceCaps(&Caps)))
        {
            nMaxAnisotropy = (anisotropicFiltering == 1) ? Caps.MaxAnisotropy : std::min((DWORD)anisotropicFiltering, Caps.MaxAnisotropy);
        }

        if (nMaxAnisotropy && SUCCEEDED(DEVICE->SetSamplerState(Sampler, D3DSAMP_MAXANISOTROPY, nMaxAnisotropy)))
        {
            LogFile::Format("Setting Anisotropy Filtering at %dx\n", nMaxAnisotropy);
        }
        else
        {
            nMaxAnisotropy = 0;
            LogFile::WriteLine("Failed to enable Anisotropy Filtering!");
        }
    }

    // enable anisotropic filtering
    if (nMaxAnisotropy)
    {
        if (Type == D3DSAMP_MAXANISOTROPY)
        {
            if (SUCCEEDED(DEVICE->SetSamplerState(Sampler, D3DSAMP_MAXANISOTROPY, nMaxAnisotropy)))
            {
                return D3D_OK;
            }
        }
        else if ((Type == D3DSAMP_MINFILTER || Type == D3DSAMP_MAGFILTER) && Value == D3DTEXF_LINEAR)
        {
            if (SUCCEEDED(DEVICE->SetSamplerState(Sampler, D3DSAMP_MAXANISOTROPY, nMaxAnisotropy)) &&
                SUCCEEDED(DEVICE->SetSamplerState(Sampler, Type, D3DTEXF_ANISOTROPIC)))
            {
                return D3D_OK;
            }
        }
    }

    return DEVICE->SetSamplerState(Sampler, Type, Value);
}

STDCALL(IDirect3DDevice9Hook::ValidateDevice)(THIS_ DWORD* pNumPasses){
    return DEVICE->ValidateDevice(pNumPasses);
}

STDCALL(IDirect3DDevice9Hook::SetPaletteEntries)(THIS_ UINT PaletteNumber, CONST PALETTEENTRY* pEntries){
    return DEVICE->SetPaletteEntries(PaletteNumber, pEntries);
}

STDCALL(IDirect3DDevice9Hook::GetPaletteEntries)(THIS_ UINT PaletteNumber, PALETTEENTRY* pEntries){
    return DEVICE->GetPaletteEntries(PaletteNumber, pEntries);
}

STDCALL(IDirect3DDevice9Hook::SetCurrentTexturePalette)(THIS_ UINT PaletteNumber){
    return DEVICE->SetCurrentTexturePalette(PaletteNumber);
}

STDCALL(IDirect3DDevice9Hook::GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber){
    return DEVICE->GetCurrentTexturePalette(PaletteNumber);
}

STDCALL(IDirect3DDevice9Hook::SetScissorRect)(THIS_ CONST RECT* pRect){
    return DEVICE->SetScissorRect(pRect);
}

STDCALL(IDirect3DDevice9Hook::GetScissorRect)(THIS_ RECT* pRect){
    return DEVICE->GetScissorRect(pRect);
}

STDCALL(IDirect3DDevice9Hook::SetSoftwareVertexProcessing)(THIS_ BOOL bSoftware){
    return DEVICE->SetSoftwareVertexProcessing(bSoftware);
}

STDCALL_(BOOL, IDirect3DDevice9Hook::GetSoftwareVertexProcessing)(THIS){
    return DEVICE->GetSoftwareVertexProcessing();
}

STDCALL(IDirect3DDevice9Hook::SetNPatchMode)(THIS_ float nSegments){
    return DEVICE->SetNPatchMode(nSegments);
}

STDCALL_(float, IDirect3DDevice9Hook::GetNPatchMode)(THIS){
    return DEVICE->GetNPatchMode();
}

STDCALL(IDirect3DDevice9Hook::DrawPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount){
    return DEVICE->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

STDCALL(IDirect3DDevice9Hook::DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount){
    return DEVICE->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

STDCALL(IDirect3DDevice9Hook::DrawPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride){
    return DEVICE->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

STDCALL(IDirect3DDevice9Hook::DrawIndexedPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride){
    return DEVICE->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

STDCALL(IDirect3DDevice9Hook::ProcessVertices)(THIS_ UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags){
    return DEVICE->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

STDCALL(IDirect3DDevice9Hook::CreateVertexDeclaration)(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl){
    return CreateVertexDeclaration(pVertexElements, ppDecl);
}

STDCALL(IDirect3DDevice9Hook::SetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9* pDecl){
    return DEVICE->SetVertexDeclaration(pDecl);
}

STDCALL(IDirect3DDevice9Hook::GetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9** ppDecl){
    return DEVICE->GetVertexDeclaration(ppDecl);
}

STDCALL(IDirect3DDevice9Hook::SetFVF)(THIS_ DWORD FVF){
    return DEVICE->SetFVF(FVF);
}

STDCALL(IDirect3DDevice9Hook::GetFVF)(THIS_ DWORD* pFVF){
    return DEVICE->GetFVF(pFVF);
}

STDCALL(IDirect3DDevice9Hook::CreateVertexShader)(THIS_ CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader){
    return DEVICE->CreateVertexShader(pFunction, ppShader);
}

STDCALL(IDirect3DDevice9Hook::SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader){
    return DEVICE->SetVertexShader(pShader);
}

STDCALL(IDirect3DDevice9Hook::GetVertexShader)(THIS_ IDirect3DVertexShader9** ppShader){
    return DEVICE->GetVertexShader(ppShader);
}

STDCALL(IDirect3DDevice9Hook::SetVertexShaderConstantF)(THIS_ UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount){
    return DEVICE->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

STDCALL(IDirect3DDevice9Hook::GetVertexShaderConstantF)(THIS_ UINT StartRegister, float* pConstantData, UINT Vector4fCount){
    return DEVICE->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

STDCALL(IDirect3DDevice9Hook::SetVertexShaderConstantI)(THIS_ UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount){
    return DEVICE->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

STDCALL(IDirect3DDevice9Hook::GetVertexShaderConstantI)(THIS_ UINT StartRegister, int* pConstantData, UINT Vector4iCount){
    return DEVICE->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

STDCALL(IDirect3DDevice9Hook::SetVertexShaderConstantB)(THIS_ UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount){
    return DEVICE->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

STDCALL(IDirect3DDevice9Hook::GetVertexShaderConstantB)(THIS_ UINT StartRegister, BOOL* pConstantData, UINT BoolCount){
    return DEVICE->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

STDCALL(IDirect3DDevice9Hook::SetStreamSource)(THIS_ UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride){
    return DEVICE->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

STDCALL(IDirect3DDevice9Hook::GetStreamSource)(THIS_ UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride){
    return DEVICE->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

STDCALL(IDirect3DDevice9Hook::SetStreamSourceFreq)(THIS_ UINT StreamNumber, UINT Setting){
    return DEVICE->SetStreamSourceFreq(StreamNumber, Setting);
}

STDCALL(IDirect3DDevice9Hook::GetStreamSourceFreq)(THIS_ UINT StreamNumber, UINT* pSetting){
    return DEVICE->GetStreamSourceFreq(StreamNumber, pSetting);
}

STDCALL(IDirect3DDevice9Hook::SetIndices)(THIS_ IDirect3DIndexBuffer9* pIndexData){
    return DEVICE->SetIndices(pIndexData);
}

STDCALL(IDirect3DDevice9Hook::GetIndices)(THIS_ IDirect3DIndexBuffer9** ppIndexData){
    return DEVICE->GetIndices(ppIndexData);
}

STDCALL(IDirect3DDevice9Hook::CreatePixelShader)(THIS_ CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader){
    return DEVICE->CreatePixelShader(pFunction, ppShader);
}

STDCALL(IDirect3DDevice9Hook::SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader){
    return DEVICE->SetPixelShader(pShader);
}

STDCALL(IDirect3DDevice9Hook::GetPixelShader)(THIS_ IDirect3DPixelShader9** ppShader){
    return DEVICE->GetPixelShader(ppShader);
}

STDCALL(IDirect3DDevice9Hook::SetPixelShaderConstantF)(THIS_ UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount){
    return DEVICE->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

STDCALL(IDirect3DDevice9Hook::GetPixelShaderConstantF)(THIS_ UINT StartRegister, float* pConstantData, UINT Vector4fCount){
    return DEVICE->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

STDCALL(IDirect3DDevice9Hook::SetPixelShaderConstantI)(THIS_ UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount){
    return DEVICE->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

STDCALL(IDirect3DDevice9Hook::GetPixelShaderConstantI)(THIS_ UINT StartRegister, int* pConstantData, UINT Vector4iCount){
    return DEVICE->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

STDCALL(IDirect3DDevice9Hook::SetPixelShaderConstantB)(THIS_ UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount){
    return DEVICE->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

STDCALL(IDirect3DDevice9Hook::GetPixelShaderConstantB)(THIS_ UINT StartRegister, BOOL* pConstantData, UINT BoolCount){
    return DEVICE->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

STDCALL(IDirect3DDevice9Hook::DrawRectPatch)(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo){
    return DEVICE->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

STDCALL(IDirect3DDevice9Hook::DrawTriPatch)(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo){
    return DEVICE->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

STDCALL(IDirect3DDevice9Hook::DeletePatch)(THIS_ UINT Handle){
    return DEVICE->DeletePatch(Handle);
}

STDCALL(IDirect3DDevice9Hook::CreateQuery)(THIS_ D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery){
    return DEVICE->CreateQuery(Type, ppQuery);
}

