
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3DBlob* g_pVSBlob = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;

void Release_Shader()
{
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVSBlob);
}


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
        MessageBox(nullptr, L"TriShader.fx VS 컴파일 실패 !!",
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
        MessageBox(nullptr, L"TriShader.fx PS 컴파일 실패 !!",
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


HRESULT Init_InputLayout()
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
    HRESULT hr = pd3dDevice->CreateInputLayout(layout, numElements, g_pVSBlob->GetBufferPointer(), g_pVSBlob->GetBufferSize(), &g_pVertexLayout);
	if (FAILED(hr))
		return hr;

	return S_OK;
}


void Render_Shader()
{
	// Set the input layout
	pd3dContext->IASetInputLayout(g_pVertexLayout);         //그리드 레이아웃 인풋
	pd3dContext->VSSetShader(g_pVertexShader, nullptr, 0);  //그리드 버텍스 쉐이더 
	pd3dContext->PSSetShader(g_pPixelShader, nullptr, 0);   //그리드 버텍스 쉐이더
}



