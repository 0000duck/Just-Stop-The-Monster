#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::Mesh(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList)
{

}

Mesh::~Mesh()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_IndexBuffer) m_IndexBuffer->Release();
	if (m_VertexUploadBuffer) m_VertexUploadBuffer->Release();
}

void Mesh::Render(ID3D12GraphicsCommandList *CommandList)
{
	CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	CommandList->IASetVertexBuffers(m_nSlot, 1, &m_VertexBufferView);

	if (m_IndexBuffer) {
		CommandList->IASetIndexBuffer(&m_IndexBufferView);
		CommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		CommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);

}

void Mesh::Render(ID3D12GraphicsCommandList *CommandList, int nSubSet)
{
	CommandList->IASetVertexBuffers(m_nSlot, 1, &m_PositionBufferView);

	CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	if ((m_nSubMesh > 0) & (nSubSet < m_nSubMesh)) {
		CommandList->IASetIndexBuffer(&(m_SubSetIndexBufferView[nSubSet]));
		CommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
		CommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

// �ؽ��� ������ ������ �޽�
TextureMesh::TextureMesh(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, float width, float height, float depth, float x, float y, float z)
{
	m_nVertices = 6;
	m_nStride = sizeof(TextureVertex);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	TextureVertex Vertices[6];

	float objX = width + x;
	float objY = height + y;
	float objZ = depth + z;

	Vertices[0] = TextureVertex(XMFLOAT3(+objX, +objY, objZ), XMFLOAT2(1.0f, 0.0f));
	Vertices[1] = TextureVertex(XMFLOAT3(+objX, -objY, objZ), XMFLOAT2(1.0f, 1.0f));
	Vertices[2] = TextureVertex(XMFLOAT3(-objX, -objY, objZ), XMFLOAT2(0.0f, 1.0f));

	Vertices[3] = TextureVertex(XMFLOAT3(-objX, -objY, objZ), XMFLOAT2(0.0f, 1.0f));
	Vertices[4] = TextureVertex(XMFLOAT3(-objX, +objY, objZ), XMFLOAT2(0.0f, 0.0f));
	Vertices[5] = TextureVertex(XMFLOAT3(+objX, +objY, objZ), XMFLOAT2(1.0f, 0.0f));

	m_VertexBuffer = CreateBufferResource(Device, CommandList, Vertices, m_nStride*m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_VertexUploadBuffer);
	
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = m_nStride;
	m_VertexBufferView.SizeInBytes = m_nStride * m_nVertices;

}

// bin ���Ͽ��� �ҷ����� ������Ʈ�� ����� �޽�
MeshLoadInfo::~MeshLoadInfo()
{
	if (m_Position) delete[] m_Position;
	if (m_Color) delete[] m_Color;
	if (m_Normal) delete[] m_Normal;

	if (m_pnIndices) delete[] m_pnIndices;

	if (m_nSubSetIndices) delete[] m_nSubSetIndices;

	for (int i = 0; i < m_nSubMeshes; ++i)
		if (m_pnSubSetIndices[i]) delete[] m_pnSubSetIndices[i];
	if (m_pnSubSetIndices) delete m_pnSubSetIndices;
}

MeshFromFile::MeshFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, MeshLoadInfo *MeshInfo)
{
	m_nVertices = MeshInfo->m_nVertices;
	m_nType = MeshInfo->m_nType;

	m_PositionBuffer = ::CreateBufferResource(Device, CommandList, MeshInfo->m_Position, sizeof(XMFLOAT3)*m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_PositionUploadBuffer);

	m_PositionBufferView.BufferLocation = m_PositionBuffer->GetGPUVirtualAddress();
	m_PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_PositionBufferView.SizeInBytes = sizeof(XMFLOAT3)*m_nVertices;

	m_nSubMesh = MeshInfo->m_nSubMeshes;

	if (m_nSubMesh > 0) {
		m_SubSetIndexBuffer = new ID3D12Resource*[m_nSubMesh];
		m_SubSetIndexUploadBuffer = new ID3D12Resource*[m_nSubMesh];
		m_SubSetIndexBufferView = new D3D12_INDEX_BUFFER_VIEW[m_nSubMesh];

		m_nSubSetIndices = new int[m_nSubMesh];

		for (int i = 0; i < m_nSubMesh; ++i) {
			m_nSubSetIndices[i] = MeshInfo->m_nSubSetIndices[i];
			m_SubSetIndexBuffer[i] = ::CreateBufferResource(Device, CommandList, MeshInfo->m_pnSubSetIndices[i], sizeof(UINT) * m_nSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_SubSetIndexBuffer[i]);

			m_SubSetIndexBufferView[i].BufferLocation = m_SubSetIndexBuffer[i]->GetGPUVirtualAddress();
			m_SubSetIndexBufferView[i].Format = DXGI_FORMAT_R32_UINT;
			m_SubSetIndexBufferView->SizeInBytes = sizeof(UINT) * MeshInfo->m_nSubSetIndices[i];
		}
	}
}

MeshFromFile::~MeshFromFile()
{
	if (m_PositionBuffer) m_PositionBuffer->Release();

	if (m_nSubMesh > 0) {
		for (int i = 0; i < m_nSubMesh; ++i)
			if (m_SubSetIndexBuffer[i]) m_SubSetIndexBuffer[i]->Release();
		if (m_SubSetIndexBuffer) delete[] m_SubSetIndexBuffer;
		if (m_SubSetIndexBufferView) delete[] m_SubSetIndexBufferView;
		if (m_nSubSetIndices) delete[] m_nSubSetIndices;
	}
}

void MeshFromFile::Render(ID3D12GraphicsCommandList *CommandList, int nSubSet) 
{
	CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	CommandList->IASetVertexBuffers(m_nSlot, 1, &m_PositionBufferView);

	if ((m_nSubMesh > 0) && (nSubSet < m_nSubMesh)) {
		CommandList->IASetIndexBuffer(&(m_SubSetIndexBufferView[nSubSet]));
		CommandList->DrawIndexedInstanced(m_nSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
		CommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

MeshIlluminatedFromFile::MeshIlluminatedFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, MeshLoadInfo *MeshInfo) : ::MeshFromFile(Device, CommandList, MeshInfo)
{
	m_NormalBuffer = ::CreateBufferResource(Device, CommandList, MeshInfo->m_Normal, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_NormalUploadBuffer);

	m_NormalBufferView.BufferLocation = m_NormalBuffer->GetGPUVirtualAddress();
	m_NormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_NormalBufferView.SizeInBytes = sizeof(XMFLOAT3)*m_nVertices;
}

MeshIlluminatedFromFile::~MeshIlluminatedFromFile()
{
	if (m_NormalBuffer) m_NormalBuffer->Release();
}

void MeshIlluminatedFromFile::Render(ID3D12GraphicsCommandList *CommandList, int nSubSet)
{
	CommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView[2] = { m_PositionBufferView, m_NormalBufferView };
	CommandList->IASetVertexBuffers(m_nSlot, 2, VertexBufferView);

	if ((m_nSubMesh > 0) && (nSubSet < m_nSubMesh)) {
		CommandList->IASetIndexBuffer(&(m_SubSetIndexBufferView[nSubSet]));
		CommandList->DrawIndexedInstanced(m_nSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
		CommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

// �ִϸ��̼��� �� 3D �𵨿� ����� �޽�
StandardMesh::StandardMesh(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList) : Mesh(Device, CommandList)
{

}

StandardMesh::~StandardMesh()
{

}

void StandardMesh::LoadMeshFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, FILE *InFile)
{
	char Token[64] = { '\0' };
	BYTE nStrLength = 0;

	int nPosition = 0, nColor = 0, nNormal = 0, nTangent = 0, nBiTangent = 0, nTextureCoord = 0, nIndice = 0, nSubMesh = 0, nSubIndice = 0;

	UINT nRead = (UINT)::fread(&m_nVertices, sizeof(int), 1, InFile);
	nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, InFile);
	nRead = (UINT)::fread(&m_strMeshName, sizeof(char), nStrLength, InFile);
	m_strMeshName[nStrLength] = '\0';

	for (; ;) {
		nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, InFile);
		nRead = (UINT)::fread(&Token, sizeof(char), nStrLength, InFile);
		Token[nStrLength] = '\0';

		if (!strcmp(Token, "<Bounds>:")) {
			nRead = (UINT)::fread(&m_AABBCenter, sizeof(XMFLOAT3), 1, InFile);
			nRead = (UINT)::fread(&m_AABBExtent, sizeof(XMFLOAT3), 1, InFile);
		}
		else if (!strcmp(Token, "<Positions>:")) {
			nRead = (UINT)::fread(&nPosition, sizeof(int), 1, InFile);
			if (nPosition > 0) {
				m_nType |= VERTEXT_POSITION;
				m_Position = new XMFLOAT3[nPosition];
				nRead = (UINT)::fread(m_Position, sizeof(XMFLOAT3), nPosition, InFile);

				m_PositionBuffer = ::CreateBufferResource(Device, CommandList, m_Position, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_PositionUploadBuffer);

				m_PositionBufferView.BufferLocation = m_PositionBuffer->GetGPUVirtualAddress();
				m_PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_PositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "<Colors>:")) {
			nRead = (UINT)::fread(&nColor, sizeof(int), 1, InFile);
			if (nColor > 0) {
				m_nType |= VERTEXT_COLOR;
				m_Color = new XMFLOAT4[nColor];
				nRead = (UINT)::fread(m_Color, sizeof(XMFLOAT4), nColor, InFile);
			}
		}
		else if (!strcmp(Token, "<TextureCoords0>:")) {
			nRead = (UINT)::fread(&nTextureCoord, sizeof(int), 1, InFile);
			if (nTextureCoord > 0) {
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_TextureCoord0 = new XMFLOAT2[nTextureCoord];
				nRead = (UINT)::fread(m_TextureCoord0, sizeof(XMFLOAT2), nTextureCoord, InFile);

				m_TextureCoord0Buffer = ::CreateBufferResource(Device, CommandList, m_TextureCoord0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_TextureCoord0UploadBuffer);

				m_TextureCoord0BufferView.BufferLocation = m_TextureCoord0Buffer->GetGPUVirtualAddress();
				m_TextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_TextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "<TextureCoords1>:")) {
			nRead = (UINT)::fread(&nTextureCoord, sizeof(int), 1, InFile);
			if (nTextureCoord > 0) {
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_TextureCoord1 = new XMFLOAT2[nTextureCoord];
				nRead = (UINT)::fread(m_TextureCoord1, sizeof(XMFLOAT2), nTextureCoord, InFile);

				m_TextureCoord1Buffer = ::CreateBufferResource(Device, CommandList, m_TextureCoord1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_TextureCoord1UploadBuffer);

				m_TextureCoord1BufferView.BufferLocation = m_TextureCoord1Buffer->GetGPUVirtualAddress();
				m_TextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_TextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "<Normals>:")) {
			nRead = (UINT)::fread(&nNormal, sizeof(int), 1, InFile);
			if (nNormal > 0) {
				m_nType |= VERTEXT_NORMAL;
				m_Normal = new XMFLOAT3[nNormal];
				nRead = (UINT)::fread(m_Normal, sizeof(XMFLOAT3), nNormal, InFile);

				m_NormalBuffer = ::CreateBufferResource(Device, CommandList, m_Normal, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_NormalUploadBuffer);

				m_NormalBufferView.BufferLocation = m_NormalBuffer->GetGPUVirtualAddress();
				m_NormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_NormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "<Tangents>:")) {
			nRead = (UINT)::fread(&nTangent, sizeof(int), 1, InFile);
			if (nTangent > 0) {
				m_nType |= VERTEXT_TANGENT;
				m_Tangent = new XMFLOAT3[nTangent];
				nRead = (UINT)::fread(m_Tangent, sizeof(XMFLOAT3), nTangent, InFile);

				m_TangentBuffer = ::CreateBufferResource(Device, CommandList, m_Tangent, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_TangentUploadBuffer);

				m_TangentBufferView.BufferLocation = m_TangentBuffer->GetGPUVirtualAddress();
				m_TangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_TangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "<BiTangents>:")) {
			nRead = (UINT)::fread(&nBiTangent, sizeof(int), 1, InFile);
			if (nBiTangent > 0) {
				m_BiTangent = new XMFLOAT3[nBiTangent];
				nRead = (UINT)::fread(m_BiTangent, sizeof(XMFLOAT3), nBiTangent, InFile);

				m_BiTangentBuffer = ::CreateBufferResource(Device, CommandList, m_BiTangent, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_BiTangentUploadBuffer);

				m_BiTangentBufferView.BufferLocation = m_BiTangentBuffer->GetGPUVirtualAddress();
				m_BiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_BiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "<SubMeshes>:")) {
			nRead = (UINT)::fread(&(m_nSubMesh), sizeof(int), 1, InFile);
			if (m_nSubMesh > 0) {
				m_pnSubSetIndices = new int[m_nSubMesh];
				m_ppnSubSetIndices = new UINT*[m_nSubMesh];

				m_SubSetIndexBuffer = new ID3D12Resource*[m_nSubMesh];
				m_SubSetIndexUploadBuffer = new ID3D12Resource*[m_nSubMesh];
				m_SubSetIndexBufferView = new D3D12_INDEX_BUFFER_VIEW[m_nSubMesh];

				for (int i = 0; i < m_nSubMesh; ++i) {
					nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, InFile);
					nRead = (UINT)::fread(Token, sizeof(char), nStrLength, InFile);
					Token[nStrLength] = '\0';
					if (!strcmp(Token, "<SubMesh>:")) {
						int nIndex = 0;
						nRead = (UINT)::fread(&nIndex, sizeof(int), 1, InFile);
						nRead = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, InFile);
						if (m_pnSubSetIndices[i] > 0) {
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nRead = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], InFile);

							m_SubSetIndexBuffer[i] = ::CreateBufferResource(Device, CommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_SubSetIndexUploadBuffer[i]);

							m_SubSetIndexBufferView[i].BufferLocation = m_SubSetIndexBuffer[i]->GetGPUVirtualAddress();
							m_SubSetIndexBufferView[i].Format = DXGI_FORMAT_R32_UINT;
							m_SubSetIndexBufferView[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(Token, "</Mesh>")) {
			break;
		}
	}
}

SkinnedMesh::SkinnedMesh(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList) : StandardMesh(Device, CommandList)
{

}

SkinnedMesh::~SkinnedMesh()
{
	
}

void SkinnedMesh::CreateShaderVariable(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList)
{
	UINT nElementByte = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255);
	m_BoneOffset = ::CreateBufferResource(Device, CommandList, NULL, nElementByte, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_BoneOffset->Map(0, NULL, (void **)&m_BoneOffsetPos);

	m_BoneTransform = ::CreateBufferResource(Device, CommandList, NULL, nElementByte, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_BoneTransform->Map(0, NULL, (void **)&m_BoneTransformPos);
}

void SkinnedMesh::LoadSkinInfoFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, FILE *InFile)
{
	char Token[64] = { '\0' };
	BYTE nStrLength = 0;

	UINT nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, InFile);
	nRead = (UINT)::fread(m_SkinnedMeshName, sizeof(char), nStrLength, InFile);
	m_SkinnedMeshName[nStrLength] = '\0';

	for (; ;) {
		nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, InFile);
		nRead = (UINT)::fread(Token, sizeof(char), nStrLength, InFile);
		Token[nStrLength] = '\0';

		if (!strcmp(Token, "<BonesPerVertex>:"))
			nRead = (UINT)::fread(&m_nBonePerVertex, sizeof(int), 1, InFile);
		else if (!strcmp(Token, "<Bounds>:")) {
			nRead = (UINT)::fread(&m_AABBCenter, sizeof(XMFLOAT3), 1, InFile);
			nRead = (UINT)::fread(&m_AABBExtent, sizeof(XMFLOAT3), 1, InFile);
		}
		else if (!strcmp(Token, "<BoneNames>:")) {
			nRead = (UINT)::fread(&m_nSkinningBone, sizeof(int), 1, InFile);
			if (m_nSkinningBone > 0) {
				m_SkinningBoneName = new char[m_nSkinningBone][64];
				m_SkinningBoneFrameCache = new GameObject*[m_nSkinningBone];

				for (int i = 0; i < m_nSkinningBone; ++i) {
					nRead = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, InFile);
					nRead = (UINT)::fread(m_SkinningBoneName[i], sizeof(char), nStrLength, InFile);
					m_SkinningBoneName[i][nStrLength] = '\0';

					m_SkinningBoneFrameCache[i] = NULL;
				}
			}
		}
		else if (!strcmp(Token, "<BoneOffsets>:")) {
			nRead = (UINT)::fread(&m_nSkinningBone, sizeof(int), 1, InFile);
			if (m_nSkinningBone > 0) {
				m_BindPoseBoneOffset = new XMFLOAT4X4[m_nSkinningBone];
				nRead = (UINT)::fread(m_BindPoseBoneOffset, sizeof(float), 16 * m_nSkinningBone, InFile);
			}
		}
		else if (!strcmp(Token, "<BoneWeights>:")) {
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			nRead = (UINT)::fread(&m_nVertices, sizeof(int), 1, InFile);
			if (m_nVertices > 0) {
				m_BoneIndice = new XMUINT4[m_nVertices];
				m_BoneWeight = new XMFLOAT4[m_nVertices];

				nRead = (UINT)::fread(m_BoneIndice, sizeof(XMUINT4), m_nVertices, InFile);
				m_BoneIndexBuffer = ::CreateBufferResource(Device, CommandList, m_BoneIndice, sizeof(XMUINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_BoneIndexUploadBuffer);

				m_BoneIndexBufferView.BufferLocation = m_BoneIndexBuffer->GetGPUVirtualAddress();
				m_BoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
				m_BoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * m_nVertices;

				nRead = (UINT)::fread(m_BoneWeight, sizeof(XMFLOAT4), m_nVertices, InFile);
				m_BoneWeightBuffer = ::CreateBufferResource(Device, CommandList, m_BoneWeight, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_BoneWeightUploadBuffer);

				m_BoneWeightBufferView.BufferLocation = m_BoneWeightBuffer->GetGPUVirtualAddress();
				m_BoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_BoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
			}
		}
		else if (!strcmp(Token, "</SkinningInfo>")) {
			break;
		}
	}
}