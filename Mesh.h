#pragma once

using namespace DirectX;

ID3D11Buffer* g_pVertexBuffer = nullptr;

struct SimpleVertex 
{
	XMFLOAT3  Pos;
};

HRESULT Init_Model()
{
	SimpleVertex v[] = {
		XMFLOAT3(0.f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f)
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = v;

	HRESULT hr = pd3dDevice->CreateBuffer(&bd, 
		&initData, &g_pVertexBuffer);
	if (FAILED(hr)) 
		return hr;

}


void Render_Model()
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pd3dContext->IASetVertexBuffers(0, 1,
		&g_pVertexBuffer, &stride, &offset);
	pd3dContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

