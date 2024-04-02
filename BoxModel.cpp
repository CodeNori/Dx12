#include "pch.h"
#include "BoxModel.h"
#include "DDSTextureLoader.h"

extern INT  gWindowWidth;
extern INT  gWindowHeight;
extern ID3D11Device* pd3dDevice;
extern ID3D11DeviceContext* pd3dContext;



HRESULT BoxModel::Creat_Mesh()
{
	SpriteVertex g_SV[] =
	{
		{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(0.5f,-0.5f, -0.5f), XMFLOAT2(1.f, 1.f)},
		{XMFLOAT3(-0.5f,-0.5f, -0.5f), XMFLOAT2(0.f, 1.f)},

		{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.f, 1.f)},
		{XMFLOAT3(-0.5f,0.5f, -0.5f), XMFLOAT2(0.f, 1.f)}
	};

	WORD  indexs[] =
	{
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7
	};
	m_IndexCount = 12;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SpriteVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// bd.Usage = D3D11_USAGE_DYNAMIC;
	// bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = g_SV;

	HRESULT hr = pd3dDevice->CreateBuffer(&bd,
		&initData, &g_pVertexBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"VertexBuffer 생성 실패!!", L"ERROR", MB_OK);
		return hr;
	}


	D3D11_BUFFER_DESC bd1 = {};
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(WORD) * m_IndexCount;
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


void BoxModel::UpdateConstantData()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(pd3dContext->Map(g_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource)))
	{
		CB_WorldViewProj* pV =
			static_cast<CB_WorldViewProj*>(mappedResource.pData);
		pV->World = XMMatrixTranspose(m_WVP.World);
		pV->View  = XMMatrixTranspose(m_WVP.View);
		pV->Proj = XMMatrixTranspose(m_WVP.Proj);

		pd3dContext->Unmap(g_pConstantBuffer, 0);
	}
}

void BoxModel::Render()
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
	UpdateConstantData();
	pd3dContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// Mesh

	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	pd3dContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	pd3dContext->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dContext->DrawIndexed(m_IndexCount, 0, 0);

}

void BoxModel::Release()
{
	SAFE_RELEASE(g_pConstantBuffer);

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


HRESULT BoxModel::Create_InputLayOut()
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


HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

HRESULT BoxModel::Create_Shader()
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


HRESULT BoxModel::Create_ConstantBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CB_WorldViewProj);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	m_WVP.World = XMMatrixIdentity();

	XMVECTOR Eye = XMVectorSet(2.5f, 2.5f, -2.5f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_WVP.View = XMMatrixLookAtLH(Eye, At, Up);

	m_WVP.Proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, gWindowWidth / (FLOAT)gWindowHeight, 0.01f, 100.0f);

	return S_OK;
}

HRESULT BoxModel::Create_Texture()
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

