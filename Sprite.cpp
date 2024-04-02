#include "pch.h"
#include "Sprite.h"
#include "DDSTextureLoader.h"

extern INT  gWindowWidth;
extern INT  gWindowHeight;
extern ID3D11Device* pd3dDevice;
extern ID3D11DeviceContext* pd3dContext;

XMFLOAT3 ScreenToNDC(int x, int y)
{
	int half_W = gWindowWidth / 2;
	int half_H = gWindowHeight / 2;

	x -= half_W;
	y -= half_H;

	float fx = (float)x / (float)half_W;
	float fy = (float)y / (float)half_H;

	fy = -fy;

	return XMFLOAT3(fx, fy, 0.5f);

}

void Sprite::Move(int left, int top, int width, int height)
{
	g_SV[0].Pos = ScreenToNDC(left, top);
	g_SV[0].Tex = XMFLOAT2(0.f, 0.f);
	g_SV[1].Pos = ScreenToNDC(left + width, top);
	g_SV[1].Tex = XMFLOAT2(1.f, 0.f);
	g_SV[2].Pos = ScreenToNDC(left + width, top + height);
	g_SV[2].Tex = XMFLOAT2(1.f, 1.f);
	g_SV[3].Pos = ScreenToNDC(left, top + height);
	g_SV[3].Tex = XMFLOAT2(0.f, 1.f);
}

HRESULT Sprite::Creat_Mesh()
{
	Move(200, 300, 100, 100);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SpriteVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = g_SV;

	HRESULT hr = pd3dDevice->CreateBuffer(&bd,
		&initData, &g_pVertexBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"VertexBuffer 생성 실패!!", L"ERROR", MB_OK);
		return hr;
	}

	WORD  indexs[] =
	{
		0,1,2,
		0,2,3
	};

	D3D11_BUFFER_DESC bd1 = {};
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(WORD) * 6;
	bd1.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd1.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData1 = {};
	initData1.pSysMem = indexs;
	hr = pd3dDevice->CreateBuffer(&bd1, &initData1, &g_IndexBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"index buffer 생성에러 !!", L"ERROR", MB_OK);
	}

	return hr;
}


void Sprite::UpdateVertexData()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(pd3dContext->Map(g_pVertexBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource)))
	{
		SpriteVertex* pV =
			static_cast<SpriteVertex*>(mappedResource.pData);
		for (int i = 0; i < 4; ++i)
			pV[i] = g_SV[i];

		pd3dContext->Unmap(g_pVertexBuffer, 0);
	}
}

void Sprite::Render()
{
	// Shader
	pd3dContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pd3dContext->PSSetShader(g_pPixelShader, nullptr, 0);

	// Texture
	pd3dContext->PSSetShaderResources(0, 1, &g_pTextureView);
	pd3dContext->PSSetSamplers(0, 1, &g_pSampler);

	// Vertex InputLayout
	pd3dContext->IASetInputLayout(g_pVertexLayout);


	// Mesh
	UpdateVertexData();

	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	pd3dContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	pd3dContext->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dContext->DrawIndexed(6, 0, 0);

}

void Sprite::Release()
{
	// Texture
	SAFE_RELEASE(g_pTextureView);
	SAFE_RELEASE(g_pSampler);

	// Shader
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVSBlob);

	// InputLayout
	SAFE_RELEASE(g_pVertexLayout);

	// Mesh
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_IndexBuffer);
}


HRESULT Sprite::Create_InputLayOut()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		   D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		   D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(layout);

	HRESULT hr = pd3dDevice->CreateInputLayout(layout, numElements,
		g_pVSBlob->GetBufferPointer(), g_pVSBlob->GetBufferSize(),
		&g_pVertexLayout);

	if (FAILED(hr)) {
		MessageBox(0, L"Layout 생성 실패!!", L"ERROR", MB_OK);
	}

	return hr;
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

HRESULT Sprite::Create_Shader()
{
	HRESULT hr = CompileShaderFromFile(
		mVSfileName,
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
		mPSfileName,
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


HRESULT Sprite::Create_Texture()
{
	HRESULT hr;

	hr = CreateDDSTextureFromFile(pd3dDevice,
		L"seafloor.dds",
		nullptr,
		&g_pTextureView);
	if (FAILED(hr)) {
		MessageBox(0, L"Texture 로딩 에러!!!", L"Error", MB_OK);
		return hr;
	}

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = pd3dDevice->CreateSamplerState(&desc, &g_pSampler);
	if (FAILED(hr)) {
		MessageBox(0, L"Sampler 만들기 실퍠 !!", L"Error", MB_OK);

	}

	return hr;
}

