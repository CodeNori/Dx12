#pragma once
#include "IModel.h"
using namespace DirectX;


struct CB_WorldViewProj
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Proj;
};

class BoxModel : public IModel
{
public:
	~BoxModel() { Release(); }
	void Init() {
		Create_Shader();
		Create_ConstantBuffer();
		Create_Texture();
		Create_InputLayOut();
		Creat_Mesh();
	}

	// Texture
	ID3D11ShaderResourceView* g_pTextureView = nullptr;
	ID3D11SamplerState* g_pSampler = nullptr;
	HRESULT Create_Texture();

	// Shader
	const wchar_t* mVSfileName = L"BoxShader.fx";
	const wchar_t* mPSfileName = L"BoxShader.fx";

	ID3DBlob* g_pVSBlob = nullptr;
	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;

	HRESULT Create_Shader();

	// Constant Buffer
	CB_WorldViewProj  m_WVP;
	ID3D11Buffer* g_pConstantBuffer;

	HRESULT Create_ConstantBuffer();
	void SetWorldTM(XMMATRIX& m) { m_WVP.World = m; }
	void SetViewTM(XMMATRIX& m) { m_WVP.View = m; }
	void SetProjTM(XMMATRIX& m) { m_WVP.Proj = m; }
	void UpdateConstantData();

	// InputLayOut
	ID3D11InputLayout* g_pVertexLayout = nullptr;

	HRESULT Create_InputLayOut();


	// Mesh
	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_IndexBuffer = nullptr;
	SpriteVertex  g_SV[4];
	UINT          m_IndexCount = 0;

	HRESULT Creat_Mesh();
	void Render();
	void Release();
};



