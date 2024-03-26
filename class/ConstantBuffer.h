#pragma once



template<typename T>
class ConstantBuffer
{
public:

	void Create()
	{
		Create_ConstantBuffer( mConstantBuffer.ReleaseAndGetAddressOf(), nullptr, sizeof(T));
	}

	// Data ä�ﶧ 
	void SetData(_In_ ID3D11DeviceContext* deviceContext, T const& value) noexcept
	{
		assert(mConstantBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(deviceContext->Map(mConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			*static_cast<T*>(mappedResource.pData) = value;

			deviceContext->Unmap(mConstantBuffer.Get(), 0);
		}
	}

	// Looks up the underlying D3D constant buffer.
	ID3D11Buffer* GetBuffer() const noexcept { return mConstantBuffer.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
};





