#pragma once

#include "Mesh.h"

class Shader;
class Texture;

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_SrvGpuDescriptorHandle;
};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_WorldPos;
};

class Texture
{
public:
	Texture(int nTextureResource, UINT ResourceType, int nSampler);
	virtual ~Texture();

private:
	int								m_nReference = 0;
	
	UINT							m_nTextureType = RESOURCE_TEXTURE2D;
	int								m_nTexture = 0;

	ID3D12Resource					**m_Texture = NULL;
	ID3D12Resource					**m_TextureUploadBuffer;

	SRVROOTARGUMENTINFO				*m_RootArgumentInfo = NULL;

	int								m_nSampler = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_SamplerGpuDescriptorHandle = NULL;

public:
	void AddRef() { ++m_nReference; }
	void Release() { if (--m_nReference <= 0) delete this; }

	void SetRootArgument(int Index, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE SrvGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList *CommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *CommandList, int Index);

	void LoadTextureFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, wchar_t *FileName, UINT Index);

	int GetTextureNum() { return m_nTexture; }
	UINT GetTextureType() { return m_nTextureType; }
	ID3D12Resource *GetTexture(int Index) { return m_Texture[Index]; }

};


struct MATERIALLOADINFO
{
	XMFLOAT4			m_AlbedoColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	XMFLOAT4			m_EmissiveColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT4			m_SpecularColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	float				m_Glossiness = 0.f;
	float				m_Smoothness = 0.f;
	float				m_SpecularHighlight = 0.f;
	float				m_Metallic = 0.f;
	float				m_GlossyReflection = 0.f;

	UINT				m_nType = 0x00;
};

struct MATERIALSLOADINFO
{
	int					m_nMaterial = 0;
	MATERIALLOADINFO	*m_Material = NULL;
};

class MaterialColor
{
public:
	MaterialColor(MATERIALLOADINFO *MaterialInfo);
	~MaterialColor();

private:
	int					m_nReference = 0;

public:
	XMFLOAT4			m_Ambient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT4			m_Diffuse = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT4			m_Specular = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT4			m_Emissive = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

public:
	void AddRef() { ++m_nReference; }
	void Release() { if (--m_nReference <= 0) delete this; }
};

class Material
{
public:
	Material(int nTexture);
	~Material();

private:
	MaterialColor	*m_MaterialColor = NULL;

	int				m_nTexture = 0;

	int				m_nType = 0x00;

	static Shader	*m_IlluminatedShader;
	static Shader	*m_StandardShader;
	static Shader	*m_SkinnedAnimationShader;

	int				m_nReference = 0;

public:
	Texture			**m_Texture = NULL;
	Shader			*m_Shader = NULL;
	_TCHAR			(*m_TextureName)[64] = NULL;

public:
	void AddRef() { ++m_nReference; }
	void Release() { if (--m_nReference <= 0) delete this; }

	void SetTexture(Texture *Texture, UINT nTexture = 0);
	void SetShader(Shader *Shader);
	void SetType(UINT nType) { m_nType |= nType; }
	void SetMaterialColor(MaterialColor *MaterialColor);
	
	void SetIlluminatedShader() { SetShader(m_IlluminatedShader); }
	void SetStandardShader() { SetShader(m_StandardShader); }
	void SetSkinnedAnimationShader() { Material::SetShader(m_SkinnedAnimationShader); }

	static void PrepareShader(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, ID3D12RootSignature *GraphicsRootSignature);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *CommandList);

	void LoadTexutreFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, UINT nType, UINT nRootParameter, _TCHAR *TextureName, Texture **Texture, FILE *InFile, GameObject *Parent, Shader *Shader);

	XMFLOAT4 m_Albedo = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT4 m_Emissive = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	XMFLOAT4 m_Specular = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	float	 m_Glossiness = 0.f;
	float	 m_Smoothness = 0.f;
	float	 m_Metallic = 0.f;
	float	 m_SpecularHighlight = 0.f;
	float	 m_GlossyReflection = 0.f;

	//Texture **GetTexture() { return m_Texture; }
	//Shader *GetShader() { return m_Shader; }
	int GetTextureNum() { return m_nTexture; }
};


class GameObject
{
public:
	GameObject();
	~GameObject();
	
protected:
	int								m_nReference = 0;

	Mesh							*m_Mesh = NULL;

	Material						**m_Material = NULL;
	int								m_nMaterial = 0;

	XMFLOAT4X4						m_TransformPos;
	XMFLOAT4X4						m_WorldPos;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_CbvGPUDescriptorHandle;

	char							m_FrameName[64];

	GameObject						*m_Child = NULL;
	GameObject						*m_Sibling = NULL;

	// 오브젝트의 Right, Up, Look, Position 정보
	XMFLOAT3						m_Right;
	XMFLOAT3						m_Up;
	XMFLOAT3						m_Look;
	XMFLOAT3						m_Position;
	// 오브젝트의 속도, 중력, 최대 속도, 저항력
	XMFLOAT3						m_Velocity;
	XMFLOAT3						m_Gravity;
	float							m_MaxVelocity;
	float							m_Friction;

	ID3D12Resource					*m_cbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_cbMappedGameObject = NULL;

public:
	GameObject						*m_Parent = NULL;

public:
	void AddRef();
	void Release();

	void SetMesh(Mesh *Mesh);
	void SetShader(int nMaterial, Shader *Shader);
	void SetMaterial(int nMaterial, Material *Material);

	void SetChild(GameObject *Child, bool ReferenceUpdate = false);

	void UpdateTransform(XMFLOAT4X4 *Parent);

	void SetPostion(XMFLOAT3 Position);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE CbvGPUDescriptorHandle) { m_CbvGPUDescriptorHandle = CbvGPUDescriptorHandle; }

	UINT GetMeshType() { return (m_Mesh) ? m_Mesh->GetType() : 0; }

	XMFLOAT3 GetRight() { return m_Right; }
	XMFLOAT3 GetUp() { return m_Up; }
	XMFLOAT3 GetLook() { return m_Look; }
	XMFLOAT3 GetPosition() { return m_Position; }

	static MeshLoadInfo *LoadMeshInfoFromFile(FILE *InFile);
	void LoadMaterialInfoFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, FILE *InFile, GameObject *Parent, Shader *Shader);

	static GameObject *LoadFrameHierarchyFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, ID3D12RootSignature *GraphicsRootSignature, FILE *InFile, GameObject *Parent, Shader *Shader);
	static GameObject *LoadGeometryAndAnimationFromFile(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, ID3D12RootSignature *GraphicsRootSignature, char *FileName, Shader *Shader, bool Animation);

	Texture *FindReplicatedTexture(_TCHAR *TextureName);

	virtual void CreateShaderVariable(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *CommandList, XMFLOAT4X4 *WorldPos);

	virtual void Animate(float ElapsedTime, XMFLOAT3 Position);
	virtual void Render(ID3D12GraphicsCommandList *CommandList);

	// 애니메이션
	void CacheSkinningBoneFrame(GameObject *RootFrame);
};

class UI : public GameObject
{
public:
	UI(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, ID3D12RootSignature *GraphicsRootSignature);
	~UI();

	virtual void Render(ID3D12GraphicsCommandList *CommandList);

};

class Trap : public GameObject
{
public:
	Trap() { }
	~Trap() { }
	
	virtual void Render(ID3D12GraphicsCommandList *CommandList);

};

class TrapCover : public GameObject
{
public:
	TrapCover(ID3D12Device *Device, ID3D12GraphicsCommandList *CommandList, ID3D12RootSignature *GraphicsRootSignature, int Type);
	~TrapCover();

	virtual void Render(ID3D12GraphicsCommandList *CommandList);
};