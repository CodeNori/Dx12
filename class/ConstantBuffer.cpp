#include "pch.h"
#include "ConstantBuffer.h"

using namespace DirectX;

//--------------------------------------------------------------------------------------


HRESULT Create_ConstantBuffer(ID3D11Buffer** pcb, const void* src, UINT size)
{
	ID3D11Device* pd3dDevice = GetD3DDevice();

	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = src;
	//return pd3dDevice->CreateBuffer(&bd, &initData, pcb);
	return pd3dDevice->CreateBuffer(&bd, (src) ? &initData : nullptr, pcb);
}

