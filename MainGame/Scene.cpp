#include "stdafx.h"
#include "Scene.h"

// ���� ���� ��, ������ Ÿ��Ʋ ȭ���� Ŭ����
TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
	ReleaseObject();
}

void TitleScene::BuildObject(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList)
{
	m_GraphicsRootSignature = CreateGraphicsRootSignature(Device);

	m_Viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.f, 1.f };
	m_ScissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	m_UI = new UI(Device, CommandList, m_GraphicsRootSignature);
}

void TitleScene::ReleaseObject()
{
	if (m_GraphicsRootSignature) m_GraphicsRootSignature->Release();

	if (m_UI) delete m_UI;
}

ID3D12RootSignature *TitleScene::CreateGraphicsRootSignature(ID3D12Device *Device)
{
	ID3D12RootSignature *GraphicsRootSignature = NULL;
	
	// ��ũ���� ������
	D3D12_DESCRIPTOR_RANGE DescriptorRange;
	DescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange.NumDescriptors = 1;
	DescriptorRange.BaseShaderRegister = 0;
	DescriptorRange.RegisterSpace = 0;
	DescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ��Ʈ �Ķ����
	D3D12_ROOT_PARAMETER RootParameter[2];
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[0].Descriptor.ShaderRegister = 1;
	RootParameter[0].Descriptor.RegisterSpace = 0;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[1].DescriptorTable.pDescriptorRanges = &DescriptorRange;
	RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// ���÷�
	D3D12_STATIC_SAMPLER_DESC SamplerDesc;
	::ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc.ShaderRegister = 0;
	SamplerDesc.RegisterSpace = 0;
	SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	::ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.NumParameters = _countof(RootParameter);
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumStaticSamplers = 1;
	RootSignatureDesc.pStaticSamplers = &SamplerDesc;
	RootSignatureDesc.Flags = RootSignatureFlag;

	ID3DBlob *SignatureBlob = NULL;
	ID3DBlob *ErrorBlob = NULL;
	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &SignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, SignatureBlob->GetBufferPointer(), SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&GraphicsRootSignature);
	
	if (SignatureBlob) SignatureBlob->Release();
	if (ErrorBlob) ErrorBlob->Release();

	return GraphicsRootSignature;
}

void TitleScene::Render(ID3D12GraphicsCommandList *CommandList)
{
	// ��Ʈ �ñ׳��ĸ� ����
	CommandList->SetGraphicsRootSignature(m_GraphicsRootSignature);

	// ����Ʈ�� ������Ʈ ������ ����
	CommandList->RSSetViewports(1, &m_Viewport);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// ���� ������ ������Ʈ���� ������
	if (m_UI) m_UI->Render(CommandList);
}


// �̱� �� ��Ƽ�� ĳ����, ���� ���� ���� ��, ������ �����ϴ� ���� ��
GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::BuildObject(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList)
{
	m_GraphicsRootSignature = CreateGraphicsRootSignature(Device);

	m_Player = new Player(Device, CommandList, m_GraphicsRootSignature);


}

void GameScene::ReleaseObject()
{
	if (m_GraphicsRootSignature) m_GraphicsRootSignature->Release();
}

ID3D12RootSignature *GameScene::CreateGraphicsRootSignature(ID3D12Device *Device)
{
	ID3D12RootSignature *GraphicsRootSignature = NULL;

	// ��ũ���� ������
	D3D12_DESCRIPTOR_RANGE DescriptorRange;
	DescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescriptorRange.NumDescriptors = 1;
	DescriptorRange.BaseShaderRegister = 0;
	DescriptorRange.RegisterSpace = 0;
	DescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ��Ʈ �Ķ����
	D3D12_ROOT_PARAMETER RootParameter[2];
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[0].Descriptor.ShaderRegister = 1;
	RootParameter[0].Descriptor.RegisterSpace = 0;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
	RootParameter[1].DescriptorTable.pDescriptorRanges = &DescriptorRange;
	RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;




	// ���÷�
	D3D12_STATIC_SAMPLER_DESC SamplerDesc;
	::ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc.ShaderRegister = 0;
	SamplerDesc.RegisterSpace = 0;
	SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	::ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.NumParameters = _countof(RootParameter);
	RootSignatureDesc.pParameters = RootParameter;
	RootSignatureDesc.NumStaticSamplers = 1;
	RootSignatureDesc.pStaticSamplers = &SamplerDesc;
	RootSignatureDesc.Flags = RootSignatureFlag;

	ID3DBlob *SignatureBlob = NULL;
	ID3DBlob *ErrorBlob = NULL;
	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &SignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, SignatureBlob->GetBufferPointer(), SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&GraphicsRootSignature);

	if (SignatureBlob) SignatureBlob->Release();
	if (ErrorBlob) ErrorBlob->Release();

	return GraphicsRootSignature;
}

void GameScene::CreateShaderVariable(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList)
{

}

void GameScene::Animate(float ElapsedTime)
{
	m_Player->Update(ElapsedTime);

}

void GameScene::Render(ID3D12GraphicsCommandList *CommandList)
{
	// ��Ʈ �ñ׳��ĸ� ����
	CommandList->SetGraphicsRootSignature(m_GraphicsRootSignature);

	// ����Ʈ�� ������Ʈ ������ ���� �� ���̴� ���� ����
	m_Player->UpdateCameraSet(CommandList);

	m_Player->Render(CommandList);
	
	// ���ӿ� ������ ������Ʈ�� ������

}