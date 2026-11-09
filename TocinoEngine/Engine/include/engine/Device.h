#pragma once
#include"Engine/Prerequisitos.h"

class
	Divice {
public:
	Divice() = default;
	~Divice() = default;

	void
		init();
	void
		destroy();

	ID3D11Device*
		getDevice() const
	{
		return device;
	}

private:
	ID3D11Device* device = nullptr;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};