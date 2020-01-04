#include "renderer.h"

extern CDriv3r *pDriv3r;

IDirect3DDevice9 * GetDirect3DDevice(void) {
    return hamster::GetD3DDevice();
}

/*
    ShaderFX
*/

inline HRESULT ShaderFX::Release(void) {
    if (pEffect != NULL)
        pEffect = (pEffect->Release(), NULL);
    if (pErrors != NULL)
        pErrors = (pErrors->Release(), NULL);
    if (pPool != NULL)
        pPool = (pPool->Release(), NULL);

    if (pTechnique != NULL) {
        delete pTechnique;
        pTechnique = NULL;
    }

    return D3D_OK;
}

HRESULT ShaderFX::Reset(int level)
{
    const char *techniques[] = {
        "t_11",
        "t_13", // V2
        "t_14", // V2
        "t_2x",
        "t_30",
    };

    bool invalid = true;
    D3DXHANDLE hTechnique = NULL;

    // find best technique
    for (int i = level; i > 0; i--) {
        hTechnique = pEffect->GetTechniqueByName(techniques[level]);

        if (hTechnique && SUCCEEDED(pEffect->ValidateTechnique(hTechnique))) {
            invalid = false;
            break;
        }
    }

    // find a valid technique, or else abort
    if (invalid && FAILED(pEffect->FindNextValidTechnique(NULL, &hTechnique)))
        return E_FAIL;

    // we're good to go, set the technique
    auto hr = pEffect->SetTechnique(hTechnique);

    if (SUCCEEDED(hr))
        return D3D_OK;

    return hr;
}

HRESULT ShaderFX::Initialise(HRESULT hr, LPD3DXEFFECTPOOL pPool) {
    if (pErrors != NULL) {
        char buf[4096] { NULL };
        memcpy_s(buf, sizeof(buf), pErrors->GetBufferPointer(), pErrors->GetBufferSize());

        debugf("ERROR -- %s\n", buf);
    }

    if (FAILED(hr))
        return hr;

    if (pPool != NULL) {
        this->pPool = pPool;
        pPool->AddRef();
    }

    hr = D3D_OK;
    bool success = SUCCEEDED(hr = pEffect->FindNextValidTechnique(NULL, &pTechnique))
        && SUCCEEDED(hr = pEffect->SetTechnique(pTechnique));

    return (success) ? D3D_OK : hr;
}

HRESULT ShaderFX::OnDeviceReset(void) {
    return pEffect->OnResetDevice();
}

HRESULT ShaderFX::OnDeviceLost(void) {
    return pEffect->OnLostDevice();
}

HRESULT ShaderFX::LoadFile(LPCSTR pSrcFile, LPD3DXEFFECTPOOL pPool) {
    auto device = GetDirect3DDevice();

    auto hr = D3DXCreateEffectFromFile(device,
        pSrcFile,
        NULL,
        NULL,
        D3DXSHADER_NO_PRESHADER,
        pPool,
        &this->pEffect,
        &this->pErrors
    );

    return this->Initialise(hr, pPool);
}

HRESULT ShaderFX::Load(LPVOID pSrcData, UINT srcDataLen, LPD3DXEFFECTPOOL pPool) {
    auto device = GetDirect3DDevice();

    auto hr = D3DXCreateEffect(device,
        pSrcData,
        srcDataLen,
        NULL,
        NULL,
        D3DXSHADER_NO_PRESHADER,
        pPool,
        &this->pEffect,
        &this->pErrors
    );

    return this->Initialise(hr, pPool);
}

/*
    ModelEffect
*/

