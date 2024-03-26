#include"DDSTextureLoader.h"
using namespace DirectX;

ID3D11ShaderResourceView* g_pTextureRV = nullptr;
ID3D11SamplerState* g_pSamplerLinear = nullptr;
UINT img_Width = 0;
UINT img_Height = 0;

void Release_Texture()
{
	SAFE_RELEASE(g_pTextureRV);
	SAFE_RELEASE(g_pSamplerLinear);
}

HRESULT Init_Texture()
{
	HRESULT hr;

	ID3D11Texture2D* pTex2D;
	//ID3D11ShaderResourceView* G_pTextureRV
	//hr = CreateWICTextureFromFile(pd3dDevice, L"2.png", nullptr, &g_pTextureRV);
	hr = CreateDDSTextureFromFile(pd3dDevice, 
									L"seafloor.dds", 
									(ID3D11Resource**)&pTex2D, 
									&g_pTextureRV);
	if (FAILED(hr))
		return hr;

	{
		D3D11_TEXTURE2D_DESC desc;
		pTex2D->GetDesc(&desc);
		img_Width = desc.Width;
		img_Height = desc.Height;
	}

	pTex2D->Release();


	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Render_Texture()
{
	// Texture
	pd3dContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	pd3dContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
}