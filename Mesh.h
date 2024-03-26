#pragma once

using namespace DirectX;

ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;

struct SimpleVertex 
{
	XMFLOAT3  Pos;
	XMFLOAT2  Tex;
};

void Release_Model()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}


HRESULT Init_Model()
{
	SimpleVertex v[] = {
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = v;

	HRESULT hr = pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
	if (FAILED(hr)) 
		return hr;


	// Create index buffer
	WORD indices[] =
	{
		0,1,2,
		2,3,0
	};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = indices;
	hr = pd3dDevice->CreateBuffer(&bd, &initData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

}


void Render_Model()
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pd3dContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	pd3dContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dContext->DrawIndexed(6, 0, 0);


}

