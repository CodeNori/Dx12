
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3DBlob* g_pVSBlob = nullptr;

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Init_Shader()
{
    HRESULT hr = CompileShaderFromFile(
        L"TriShader.fx",
        "VS",
        "vs_4_0",
        &g_pVSBlob
    );
    if (FAILED(hr)) {
        MessageBox(nullptr, L"TriShader.fxh VS 컴파일 실패 !!",
            L"Error", MB_OK);
        return hr;
    }

    hr = pd3dDevice->CreateVertexShader(g_pVSBlob->GetBufferPointer(),
        g_pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
    if (FAILED(hr)) {
        g_pVSBlob->Release();
        g_pVSBlob = nullptr;
    }

    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(
        L"TriShader.fx",
        "PS",
        "ps_4_0",
        &pPSBlob
    );
    if (FAILED(hr)) {
        MessageBox(nullptr, L"TriShader.fxh PS 컴파일 실패 !!",
            L"Error", MB_OK);
        return hr;
    }

    hr = pd3dDevice->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(),
        NULL,
        &g_pPixelShader
    );
    pPSBlob->Release();
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Render_Shader()
{
    pd3dContext->VSSetShader(g_pVertexShader, nullptr, 0);
    pd3dContext->PSSetShader(g_pPixelShader, nullptr, 0);
}

void Release_Shader()
{
    SAFE_RELEASE(g_pVertexShader);
    SAFE_RELEASE(g_pPixelShader);
    SAFE_RELEASE(g_pVSBlob);
}

