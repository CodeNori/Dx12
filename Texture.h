#include "DDSTextureLoader.h"

ID3D11ShaderResourceView* g_pTextureView = nullptr;
ID3D11SamplerState* g_pSampler = nullptr;

HRESULT Init_Texture()
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

void Release_Texture()
{
	SAFE_RELEASE(g_pTextureView);
	SAFE_RELEASE(g_pSampler);
}

void Render_Texture()
{
	pd3dContext->PSSetShaderResources(0, 1, &g_pTextureView);
	pd3dContext->PSSetSamplers(0, 1, &g_pSampler);
}

