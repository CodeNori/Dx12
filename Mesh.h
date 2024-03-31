#pragma once

using namespace DirectX;

ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_IndexBuffer = nullptr;

struct SimpleVertex 
{
	XMFLOAT3  Pos;
	XMFLOAT2  Tex;
};

SimpleVertex g_SV[4];

void UpdateVertexData(SimpleVertex* vert, int vert_count);

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

RECT  gNDC = { 400,200, 500,300 };

void UpdateSimpleVertex(int left, int top, int width, int height)
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


HRESULT Init_Model()
{
	
	//SimpleVertex v[6];
	SimpleVertex v[4];
	v[0].Pos = ScreenToNDC(gNDC.left, gNDC.top);
	v[0].Tex = XMFLOAT2(0.f, 0.f);
	v[1].Pos = ScreenToNDC(gNDC.right, gNDC.top);
	v[1].Tex = XMFLOAT2(1.f, 0.f);
	v[2].Pos = ScreenToNDC(gNDC.right, gNDC.bottom);
	v[2].Tex = XMFLOAT2(1.f, 1.f);
	v[3].Pos = ScreenToNDC(gNDC.left, gNDC.bottom);
	v[3].Tex = XMFLOAT2(0.f, 1.f);

	UpdateSimpleVertex(500, 400, 100, 100);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = v;

	HRESULT hr = pd3dDevice->CreateBuffer(&bd, 
		&initData, &g_pVertexBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"VertexBuffer 积己 角菩!!", L"ERROR", MB_OK);
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
		MessageBox(0, L"index buffer 积己俊矾 !!", L"ERROR", MB_OK);
	}

	return hr;
}


void Render_Model()
{
	UpdateVertexData(g_SV, 4);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pd3dContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	pd3dContext->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dContext->DrawIndexed(6, 0, 0);

}

void Release_Model()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_IndexBuffer);
}

void UpdateVertexData(SimpleVertex* vert, int vert_count)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(pd3dContext->Map(g_pVertexBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource)))
	{
		SimpleVertex* pV = 
			static_cast<SimpleVertex*>(mappedResource.pData);
		for (int i = 0; i < vert_count; ++i)
			pV[i] = vert[i];

		pd3dContext->Unmap(g_pVertexBuffer, 0);
	}
}


ID3D11InputLayout* g_pVertexLayout = nullptr;

HRESULT Init_InputLayOut()
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
		MessageBox(0, L"Layout 积己 角菩!!", L"ERROR", MB_OK);
	}

	return hr;
}

void Render_InputLayout()
{
	pd3dContext->IASetInputLayout(g_pVertexLayout);
}

void Release_InputLayout()
{
	SAFE_RELEASE(g_pVertexLayout);
}





