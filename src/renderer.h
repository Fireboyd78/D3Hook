#pragma once

#include "driv3r.h"
#include "hamster.h"

//
// Driv3r Renderer
//

class ShaderFX;
class ModelEffect;

static IDirect3DDevice9 *GetDirect3DDevice(void);

class ShaderFX {
private:
    LPD3DXEFFECT pEffect;
    LPD3DXBUFFER pErrors;
    LPD3DXEFFECTPOOL pPool;
    D3DXHANDLE pTechnique;
public:
    explicit ShaderFX(const ShaderFX &&)    = delete;

    explicit ShaderFX(void)
        : pEffect(NULL), pErrors(NULL), pPool(NULL), pTechnique(NULL)
    { }

    HRESULT Release(void);

    HRESULT Reset(int level);

    HRESULT Initialise(HRESULT hr, LPD3DXEFFECTPOOL pPool);

    HRESULT OnDeviceReset(void);
    HRESULT OnDeviceLost(void);

    HRESULT LoadFile(LPCSTR pSrcFile, LPD3DXEFFECTPOOL pPool);
    HRESULT Load(LPVOID pSrcData, UINT srcDataLen,  LPD3DXEFFECTPOOL pPool);
};

class ModelEffect {
private:
    ShaderFX *pShader;
    UINT uid;
    UINT unk_08;
    void *pTexture;
public:
    explicit ModelEffect(const ModelEffect &&) = delete;

    explicit ModelEffect(UINT uid)
        : pShader(NULL), uid(uid), unk_08(0xCCCCCCCC), pTexture(NULL)
    { }

    virtual BOOL LoadFile(LPCSTR lpSrcFile, LPD3DXEFFECTPOOL pPool) THUNK;
    virtual BOOL Load(LPVOID lpSrcData, UINT srcDataLen, LPD3DXEFFECTPOOL pPool) THUNK;

    virtual HRESULT Release(void) THUNK;
    virtual HRESULT Reset(void) THUNK;

    virtual HRESULT Begin(UINT *pPasses) THUNK;
    virtual HRESULT BeginPass(UINT pass, struct Substance *substance) THUNK;
    virtual HRESULT EndPass(void) THUNK;
    virtual HRESULT End(void) THUNK;

    virtual HRESULT BeginScene(void) THUNK;
    virtual HRESULT Draw(void *a0, struct SubModel *subModel) THUNK;
    virtual HRESULT EndScene(void) THUNK;

    virtual HRESULT Initialise(void) THUNK;
    virtual HRESULT Update(void *a0) THUNK;
    virtual HRESULT Update(void *a0, struct Substance *substance, struct SubModel *subModel) THUNK;

    virtual ~ModelEffect(void) THUNK;
};