#pragma once

using namespace DirectX;

struct SpriteVertex
{
	XMFLOAT3  Pos;
	XMFLOAT2  Tex;
};


class Sprite
{
public:
	~Sprite() { Release(); }
	void Init() {
		Create_Shader();
		Create_Texture();
		Create_InputLayOut();
		Creat_Mesh();
	}

	// Texture
	ID3D11ShaderResourceView* g_pTextureView = nullptr;
	ID3D11SamplerState* g_pSampler = nullptr;
	HRESULT Create_Texture();

	// Shader
	const wchar_t* mVSfileName = L"TriShader.fx";
	const wchar_t* mPSfileName = L"TriShader.fx";

	ID3DBlob* g_pVSBlob = nullptr;
	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;

	HRESULT Create_Shader();

	// InputLayOut
	ID3D11InputLayout* g_pVertexLayout = nullptr;

	HRESULT Create_InputLayOut();


	// Mesh
	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_IndexBuffer = nullptr;
	SpriteVertex  g_SV[4];

	void Move(int left, int top, int width, int height);
	HRESULT Creat_Mesh();
	void UpdateVertexData();
	void Render();
	void Release();
};

XMFLOAT3 ScreenToNDC(int x, int y);