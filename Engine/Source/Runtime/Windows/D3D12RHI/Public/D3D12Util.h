// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	D3D12Util.h: D3D RHI utility definitions.
	=============================================================================*/

#pragma once

#if WINVER == 0x0502
// Windows XP uses Win7 sdk, and in that one winerror.h doesn't include them
#ifndef DXGI_ERROR_INVALID_CALL
#define DXGI_ERROR_INVALID_CALL                 MAKE_DXGI_HRESULT(1)
#define DXGI_ERROR_NOT_FOUND                    MAKE_DXGI_HRESULT(2)
#define DXGI_ERROR_MORE_DATA                    MAKE_DXGI_HRESULT(3)
#define DXGI_ERROR_UNSUPPORTED                  MAKE_DXGI_HRESULT(4)
#define DXGI_ERROR_DEVICE_REMOVED               MAKE_DXGI_HRESULT(5)
#define DXGI_ERROR_DEVICE_HUNG                  MAKE_DXGI_HRESULT(6)
#define DXGI_ERROR_DEVICE_RESET                 MAKE_DXGI_HRESULT(7)
#define DXGI_ERROR_WAS_STILL_DRAWING            MAKE_DXGI_HRESULT(10)
#define DXGI_ERROR_FRAME_STATISTICS_DISJOINT    MAKE_DXGI_HRESULT(11)
#define DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE MAKE_DXGI_HRESULT(12)
#define DXGI_ERROR_DRIVER_INTERNAL_ERROR        MAKE_DXGI_HRESULT(32)
#define DXGI_ERROR_NONEXCLUSIVE                 MAKE_DXGI_HRESULT(33)
#define DXGI_ERROR_NOT_CURRENTLY_AVAILABLE      MAKE_DXGI_HRESULT(34)
#define DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED   MAKE_DXGI_HRESULT(35)
#define DXGI_ERROR_REMOTE_OUTOFMEMORY           MAKE_DXGI_HRESULT(36)
#endif

#endif

namespace D3D12RHI
{
	/**
	 * Checks that the given result isn't a failure.  If it is, the application exits with an appropriate error message.
	 * @param	Result - The result code to check
	 * @param	Code - The code which yielded the result.
	 * @param	Filename - The filename of the source file containing Code.
	 * @param	Line - The line number of Code within Filename.
	 */
	extern void VerifyD3D11Result(HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D12Device* Device);
}

namespace D3D12RHI
{
	/**
	* Checks that the given result isn't a failure.  If it is, the application exits with an appropriate error message.
	* @param	Result - The result code to check
	* @param	Code - The code which yielded the result.
	* @param	Filename - The filename of the source file containing Code.
	* @param	Line - The line number of Code within Filename.
	*/
	extern void VerifyD3D12CreateTextureResult(HRESULT D3DResult, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line,
		uint32 SizeX, uint32 SizeY, uint32 SizeZ, uint8 D3DFormat, uint32 NumMips, uint32 Flags);

	/**
	 * A macro for using VERIFYD3D11RESULT that automatically passes in the code and filename/line.
	 */
#define VERIFYD3D11RESULT_EX(x, Device)	{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, Device); }}
#define VERIFYD3D11RESULT(x)			{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, 0); }}
#define VERIFYD3D11CREATETEXTURERESULT(x,SizeX,SizeY,SizeZ,Format,NumMips,Flags) {HRESULT hr = x; if (FAILED(hr)) { VerifyD3D12CreateTextureResult(hr,#x,__FILE__,__LINE__,SizeX,SizeY,SizeZ,Format,NumMips,Flags); }}

	/**
	 * Checks that a COM object has the expected number of references.
	 */
	extern void VerifyComRefCount(IUnknown* Object, int32 ExpectedRefs, const TCHAR* Code, const TCHAR* Filename, int32 Line);
#define checkComRefCount(Obj,ExpectedRefs) VerifyComRefCount(Obj,ExpectedRefs,TEXT(#Obj),TEXT(__FILE__),__LINE__)

	/** Returns a string for the provided DXGI format. */
	const TCHAR* GetD3D11TextureFormatString(DXGI_FORMAT TextureFormat);
}

using namespace D3D12RHI;

enum EShaderVisibility
{
	SV_Vertex,
	SV_Pixel,
	SV_Hull,
	SV_Domain,
	SV_Geometry,
	SV_All,
	SV_ShaderVisibilityCount
};

FORCEINLINE D3D12_SHADER_VISIBILITY GetD3D12ShaderVisibility(EShaderVisibility Visibility)
{
	switch (Visibility)
	{
	case SV_Vertex:
		return D3D12_SHADER_VISIBILITY_VERTEX;
	case SV_Hull:
		return D3D12_SHADER_VISIBILITY_HULL;
	case SV_Domain:
		return D3D12_SHADER_VISIBILITY_DOMAIN;
	case SV_Geometry:
		return D3D12_SHADER_VISIBILITY_GEOMETRY;
	case SV_Pixel:
		return D3D12_SHADER_VISIBILITY_PIXEL;
	case SV_All:
		return D3D12_SHADER_VISIBILITY_ALL;

	default:
		check(false);
		return static_cast<D3D12_SHADER_VISIBILITY>(-1);
	};
}

FORCEINLINE D3D12_ROOT_SIGNATURE_FLAGS GetD3D12RootSignatureDenyFlag(EShaderVisibility Visibility)
{
	switch (Visibility)
	{
	case SV_Vertex:
		return D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	case SV_Hull:
		return D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	case SV_Domain:
		return D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	case SV_Geometry:
		return D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	case SV_Pixel:
		return D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	case SV_All:
		return D3D12_ROOT_SIGNATURE_FLAG_NONE;

	default:
		check(false);
		return static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(-1);
	};
}

struct FShaderRegisterCounts
{
	uint8 SamplerCount;
	uint8 ConstantBufferCount;
	uint8 ShaderResourceCount;
	uint8 UnorderedAccessCount;
};

struct FD3D12QuantizedBoundShaderState
{
	FShaderRegisterCounts RegisterCounts[SV_ShaderVisibilityCount];
	bool bAllowIAInputLayout;

	inline bool operator==(const FD3D12QuantizedBoundShaderState& RHS) const
	{
		return 0 == FMemory::Memcmp(this, &RHS, sizeof(RHS));
	}

	friend uint32 GetTypeHash(const FD3D12QuantizedBoundShaderState& Key);

	static void InitShaderRegisterCounts(const D3D12_RESOURCE_BINDING_TIER& ResourceBindingTier, const FShaderCodePackedResourceCounts& Counts, FShaderRegisterCounts& Shader, bool bAllowUAVs = false);
};

/**
* Creates a discrete bound shader state object from a collection of graphics pipeline shaders.
*/

class FD3D12BoundShaderState;
extern void QuantizeBoundShaderState(
	const D3D12_RESOURCE_BINDING_TIER& ResourceBindingTier,
	const FD3D12BoundShaderState* const BSS,
	FD3D12QuantizedBoundShaderState &QBSS
	);

class FD3D12ComputeShader;
extern void QuantizeBoundShaderState(
	const D3D12_RESOURCE_BINDING_TIER& ResourceBindingTier,
	const FD3D12ComputeShader* const ComputeShader,
	FD3D12QuantizedBoundShaderState &QBSS);

class FD3D12RootSignatureDesc
{
public:
	explicit FD3D12RootSignatureDesc(const FD3D12QuantizedBoundShaderState& QBSS)
	{
		const EShaderVisibility RootSignaturePriorityOrder[] = { SV_Pixel, SV_Vertex, SV_Hull, SV_Domain, SV_Geometry, SV_All };
		D3D12_ROOT_SIGNATURE_FLAGS Flags = QBSS.bAllowIAInputLayout ? D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT : D3D12_ROOT_SIGNATURE_FLAG_NONE;
		uint32 RootParameterCount = 0;
		for (uint32 i = 0; i < _countof(RootSignaturePriorityOrder); i++)
		{
			const EShaderVisibility Visibility = RootSignaturePriorityOrder[i];
			bool bUsesShaderRegisters = false;
			if (QBSS.RegisterCounts[Visibility].ShaderResourceCount > 0)
			{
				check(RootParameterCount < MaxRootParameters);
				DescriptorRanges[RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, QBSS.RegisterCounts[Visibility].ShaderResourceCount, 0u);
				TableSlots[RootParameterCount].InitAsDescriptorTable(1, &DescriptorRanges[RootParameterCount], GetD3D12ShaderVisibility(Visibility));
				RootParameterCount++;
				bUsesShaderRegisters = true;
			}

			if (QBSS.RegisterCounts[Visibility].ConstantBufferCount > 0)
			{
				check(RootParameterCount < MaxRootParameters);
				DescriptorRanges[RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, QBSS.RegisterCounts[Visibility].ConstantBufferCount, 0u);
				TableSlots[RootParameterCount].InitAsDescriptorTable(1, &DescriptorRanges[RootParameterCount], GetD3D12ShaderVisibility(Visibility));
				RootParameterCount++;
				bUsesShaderRegisters = true;
			}

			if (QBSS.RegisterCounts[Visibility].SamplerCount > 0)
			{
				check(RootParameterCount < MaxRootParameters);
				DescriptorRanges[RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, QBSS.RegisterCounts[Visibility].SamplerCount, 0u);
				TableSlots[RootParameterCount].InitAsDescriptorTable(1, &DescriptorRanges[RootParameterCount], GetD3D12ShaderVisibility(Visibility));
				RootParameterCount++;
				bUsesShaderRegisters = true;
			}

			if (QBSS.RegisterCounts[Visibility].UnorderedAccessCount > 0)
			{
				check(RootParameterCount < MaxRootParameters);
				DescriptorRanges[RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, QBSS.RegisterCounts[Visibility].UnorderedAccessCount, 0u);
				TableSlots[RootParameterCount].InitAsDescriptorTable(1, &DescriptorRanges[RootParameterCount], GetD3D12ShaderVisibility(Visibility));
				RootParameterCount++;
				bUsesShaderRegisters = true;
			}

			if (!bUsesShaderRegisters)
			{
				// Deny access to the shader stage in the root signature.
				Flags = (Flags | GetD3D12RootSignatureDenyFlag(Visibility));
			}
		}

		// Init the desc.
		RootDesc.Init(RootParameterCount, TableSlots, 0, nullptr, Flags);
	}

	inline const D3D12_ROOT_SIGNATURE_DESC& GetDesc() const { return RootDesc; }

private:
	static const uint32 MaxRootParameters = 16;
	CD3DX12_ROOT_PARAMETER TableSlots[MaxRootParameters];
	CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[MaxRootParameters];
	CD3DX12_ROOT_SIGNATURE_DESC RootDesc;
};

/**
* Convert from ECubeFace to D3DCUBEMAP_FACES type
* @param Face - ECubeFace type to convert
* @return D3D cube face enum value
*/
FORCEINLINE uint32 GetD3D11CubeFace(ECubeFace Face)
{
	switch (Face)
	{
	case CubeFace_PosX:
	default:
		return 0;//D3DCUBEMAP_FACE_POSITIVE_X;
	case CubeFace_NegX:
		return 1;//D3DCUBEMAP_FACE_NEGATIVE_X;
	case CubeFace_PosY:
		return 2;//D3DCUBEMAP_FACE_POSITIVE_Y;
	case CubeFace_NegY:
		return 3;//D3DCUBEMAP_FACE_NEGATIVE_Y;
	case CubeFace_PosZ:
		return 4;//D3DCUBEMAP_FACE_POSITIVE_Z;
	case CubeFace_NegZ:
		return 5;//D3DCUBEMAP_FACE_NEGATIVE_Z;
	};
}

/**
* Calculate a subresource index for a texture
*/
FORCEINLINE uint32 CalcSubresource(uint32 MipSlice, uint32 ArraySlice, uint32 MipLevels)
{
	return MipSlice + ArraySlice * MipLevels;
}

class FD3D12Resource;

/**
 * Keeps track of Locks for D3D11 objects
 */
class FD3D12LockedKey
{
public:
	void* SourceObject;
	uint32 Subresource;

public:
	FD3D12LockedKey() : SourceObject(NULL)
		, Subresource(0)
	{}
	FD3D12LockedKey(FD3D12Resource* source, uint32 subres = 0) : SourceObject((void*)source)
		, Subresource(subres)
	{}
	FD3D12LockedKey(class FD3D12ResourceLocation* source, uint32 subres = 0) : SourceObject((void*)source)
		, Subresource(subres)
	{}
	bool operator==(const FD3D12LockedKey& Other) const
	{
		return SourceObject == Other.SourceObject && Subresource == Other.Subresource;
	}
	bool operator!=(const FD3D12LockedKey& Other) const
	{
		return SourceObject != Other.SourceObject || Subresource != Other.Subresource;
	}
	FD3D12LockedKey& operator=(const FD3D12LockedKey& Other)
	{
		SourceObject = Other.SourceObject;
		Subresource = Other.Subresource;
		return *this;
	}
	uint32 GetHash() const
	{
		return PointerHash(SourceObject);
	}

	/** Hashing function. */
	friend uint32 GetTypeHash(const FD3D12LockedKey& K)
	{
		return K.GetHash();
	}
};

/** Information about a D3D resource that is currently locked. */
struct FD3D12LockedData
{
	TRefCountPtr<FD3D12Resource> StagingResource;
	TRefCountPtr<FD3D12Resource> UploadHeapResource;
	TRefCountPtr<FD3D12ResourceLocation> UploadHeapLocation;
	uint32 Pitch;
	uint32 DepthPitch;

	// constructor
	FD3D12LockedData()
		: bAllocDataWasUsed(false)
	{
	}

	// 16 byte alignment for best performance  (can be 30x faster than unaligned)
	void AllocData(uint32 Size)
	{
		Data = (uint8*)FMemory::Malloc(Size, 16);
		bAllocDataWasUsed = true;
	}

	// Some driver might return aligned memory so we don't enforce the alignment
	void SetData(void* InData)
	{
		check(!bAllocDataWasUsed); Data = (uint8*)InData;
	}

	uint8* GetData() const
	{
		return Data;
	}

	// only call if AllocData() was used
	void FreeData()
	{
		check(bAllocDataWasUsed);
		FMemory::Free(Data);
		Data = 0;
	}

private:
	uint8* Data;
	// then FreeData
	bool bAllocDataWasUsed;
};

class FD3D12RenderTargetView;
class FD3D12DepthStencilView;

/**
 * Class for retrieving render targets currently bound to the device context.
 */
class FD3D12BoundRenderTargets
{
public:
	/** Initialization constructor: requires the state cache. */
	explicit FD3D12BoundRenderTargets(FD3D12RenderTargetView** RTArray, uint32 NumActiveRTs, FD3D12DepthStencilView* DSView);

	/** Destructor. */
	~FD3D12BoundRenderTargets();

	/** Accessors. */
	FORCEINLINE int32 GetNumActiveTargets() const { return NumActiveTargets; }
	FORCEINLINE FD3D12RenderTargetView* GetRenderTargetView(int32 TargetIndex) { return RenderTargetViews[TargetIndex]; }
	FORCEINLINE FD3D12DepthStencilView* GetDepthStencilView() { return DepthStencilView; }

private:
	/** Active render target views. */
	FD3D12RenderTargetView* RenderTargetViews[MaxSimultaneousRenderTargets];

	/** Active depth stencil view. */
	FD3D12DepthStencilView* DepthStencilView;

	/** The number of active render targets. */
	int32 NumActiveTargets;
};

/**
 * Class for managing dynamic buffers.
 */
class FD3D12DynamicBuffer : public FRenderResource, public FRHIResource, public FD3D12DeviceChild
{
public:
	/** Initialization constructor. */
	FD3D12DynamicBuffer(FD3D12Device* InParent, class FD3D12DynamicHeapAllocator& UploadHeap);
	/** Destructor. */
	~FD3D12DynamicBuffer();

	/** Locks the buffer returning at least Size bytes. */
	void* Lock(uint32 Size);
	/** Unlocks the buffer returning the underlying D3D12 buffer to use as a resource. */
	FD3D12ResourceLocation* Unlock();

	// Begin FRenderResource interface.
	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;
	// End FRenderResource interface.

private:
	TRefCountPtr<FD3D12ResourceLocation> ResourceLocation;
	class FD3D12DynamicHeapAllocator& UploadHeapAllocator;
};

static D3D12_DESCRIPTOR_HEAP_DESC CreateDHD(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32 NumDescriptorsPerHeap, D3D12_DESCRIPTOR_HEAP_FLAGS Flags)
{
	D3D12_DESCRIPTOR_HEAP_DESC DHD ={Type, NumDescriptorsPerHeap, Flags};

	return DHD;
}

class FDescriptorHeapManager
{
public: // Types
	typedef D3D12_CPU_DESCRIPTOR_HANDLE HeapOffset;
	typedef decltype(HeapOffset::ptr) HeapOffsetRaw;
	typedef uint32 HeapIndex;

private: // Types
	struct SFreeRange { HeapOffsetRaw Start; HeapOffsetRaw End; };
	struct SHeapEntry
	{
		TRefCountPtr<ID3D12DescriptorHeap> m_Heap;
		TDoubleLinkedList<SFreeRange> m_FreeList;

		SHeapEntry() { }
	};
	typedef TArray<SHeapEntry> THeapMap;

public: // Methods
	FDescriptorHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE Type,
		uint32 NumDescriptorsPerHeap)
		: m_Desc(CreateDHD(Type, NumDescriptorsPerHeap, D3D12_DESCRIPTOR_HEAP_FLAG_NONE))
		, m_DescriptorSize(0)
		, m_pDevice(nullptr)
	{
	}

	void Init(ID3D12Device* pDevice)
	{
		m_pDevice = pDevice;
		m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(m_Desc.Type);
	}

	HeapOffset AllocateHeapSlot(HeapIndex &outIndex)
	{
		FScopeLock Lock(&CritSect);
		if (0 == m_FreeHeaps.Num())
		{
			AllocateHeap();
		}
		check(0 != m_FreeHeaps.Num());
		auto Head = m_FreeHeaps.GetHead();
		outIndex = Head->GetValue();
		SHeapEntry &HeapEntry = m_Heaps[outIndex];
		check(0 != HeapEntry.m_FreeList.Num());
		SFreeRange &Range = HeapEntry.m_FreeList.GetHead()->GetValue();
		HeapOffset Ret = { Range.Start };
		Range.Start += m_DescriptorSize;

		if (Range.Start == Range.End)
		{
			HeapEntry.m_FreeList.RemoveNode(HeapEntry.m_FreeList.GetHead());
			if (0 == HeapEntry.m_FreeList.Num())
			{
				m_FreeHeaps.RemoveNode(Head);
			}
		}
		return Ret;
	}

	void FreeHeapSlot(HeapOffset Offset, HeapIndex index)
	{
		FScopeLock Lock(&CritSect);
		SHeapEntry &HeapEntry = m_Heaps[index];

		SFreeRange NewRange =
		{
			Offset.ptr,
			Offset.ptr + m_DescriptorSize
		};

		bool bFound = false;
		for (auto Node = HeapEntry.m_FreeList.GetHead();
				Node != nullptr && !bFound;
				Node = Node->GetNextNode())
		{
			SFreeRange &Range = Node->GetValue();
			check(Range.Start < Range.End);
			if (Range.Start == Offset.ptr + m_DescriptorSize)
			{
				Range.Start = Offset.ptr;
				bFound = true;
			}
			else if (Range.End == Offset.ptr)
			{
				Range.End += m_DescriptorSize;
				bFound = true;
			}
			else
			{
				check(Range.End < Offset.ptr || Range.Start > Offset.ptr);
				if (Range.Start > Offset.ptr)
				{
					HeapEntry.m_FreeList.InsertNode(NewRange, Node);
					bFound = true;
				}
			}
		}

		if (!bFound)
		{
			if (0 == HeapEntry.m_FreeList.Num())
			{
				m_FreeHeaps.AddTail(index);
			}
			HeapEntry.m_FreeList.AddTail(NewRange);
		}
	}

private: // Methods
	void AllocateHeap()
	{
		TRefCountPtr<ID3D12DescriptorHeap> Heap;
		VERIFYD3D11RESULT(m_pDevice->CreateDescriptorHeap(&m_Desc, IID_PPV_ARGS(Heap.GetInitReference())));
		HeapOffset HeapBase = Heap->GetCPUDescriptorHandleForHeapStart();
		check(HeapBase.ptr != 0);

		// Allocate and initialize a single new entry in the map
		m_Heaps.SetNum(m_Heaps.Num() + 1);
		SHeapEntry& HeapEntry = m_Heaps.Last();
		HeapEntry.m_FreeList.AddTail({HeapBase.ptr,
			HeapBase.ptr + m_Desc.NumDescriptors * m_DescriptorSize});
		HeapEntry.m_Heap = Heap;
		m_FreeHeaps.AddTail(m_Heaps.Num() - 1);
	}

private: // Members
	const D3D12_DESCRIPTOR_HEAP_DESC m_Desc;
	uint32 m_DescriptorSize;
	ID3D12Device* m_pDevice; // weak-ref

	THeapMap m_Heaps;
	TDoubleLinkedList<HeapIndex> m_FreeHeaps;
	FCriticalSection CritSect;
};

void LogExecuteCommandLists(uint32 NumCommandLists, ID3D12CommandList *const *ppCommandLists);
FString ConvertToResourceStateString(uint32 ResourceState);
void LogResourceBarriers(uint32 NumBarriers, D3D12_RESOURCE_BARRIER *pBarriers, ID3D12CommandList *const pCommandList);


// Custom resource states
// To Be Determined (TBD) means we need to fill out a resource barrier before the command list is executed.
#define D3D12_RESOURCE_STATE_TBD (D3D12_RESOURCE_STATES)-1
#define D3D12_RESOURCE_STATE_CORRUPT (D3D12_RESOURCE_STATES)-2

//==================================================================================================================================
// CResourceState
// Tracking of per-resource or per-subresource state
//==================================================================================================================================
class CResourceState
{
public:
	void Initialize(uint32 SubresourceCount);

	bool AreAllSubresourcesSame() const;
	bool CheckResourceState(D3D12_RESOURCE_STATES State) const;
	bool CheckResourceStateInitalized() const;
	D3D12_RESOURCE_STATES GetSubresourceState(uint32 SubresourceIndex) const;
	void SetResourceState(D3D12_RESOURCE_STATES State);
	void SetSubresourceState(uint32 SubresourceIndex, D3D12_RESOURCE_STATES State);

private:
	// Only used if m_AllSubresourcesSame is 1.
	// Bits defining the state of the full resource, bits are from D3D12_RESOURCE_STATES
	D3D12_RESOURCE_STATES m_ResourceState : 31;

	// Set to 1 if m_ResourceState is valid.  In this case, all subresources have the same state
	// Set to 0 if m_SubresourceState is valid.  In this case, each subresources may have a different state (or may be unknown)
	uint32 m_AllSubresourcesSame : 1;

	// Only used if m_AllSubresourcesSame is 0.
	// The state of each subresources.  Bits are from D3D12_RESOURCE_STATES.
	TArray<D3D12_RESOURCE_STATES> m_SubresourceState;
};

//==================================================================================================================================
// FD3D12ShaderBytecode
// Encapsulates D3D12 shader bytecode and creates a hash for the shader bytecode
//==================================================================================================================================
struct ShaderBytecodeHash
{
	// 160 bit strong SHA1 hash
	uint32 SHA1Hash[5];

	bool operator ==(const ShaderBytecodeHash &b) const
	{
		return (SHA1Hash[0] == b.SHA1Hash[0] &&
			SHA1Hash[1] == b.SHA1Hash[1] &&
			SHA1Hash[2] == b.SHA1Hash[2] &&
			SHA1Hash[3] == b.SHA1Hash[3] &&
			SHA1Hash[4] == b.SHA1Hash[4]);
	}

	bool operator !=(const ShaderBytecodeHash &b) const
	{
		return (SHA1Hash[0] != b.SHA1Hash[0] ||
			SHA1Hash[1] != b.SHA1Hash[1] ||
			SHA1Hash[2] != b.SHA1Hash[2] ||
			SHA1Hash[3] != b.SHA1Hash[3] ||
			SHA1Hash[4] != b.SHA1Hash[4]);
	}
};

class FD3D12ShaderBytecode
{
public:
	FD3D12ShaderBytecode()
	{
		FMemory::Memzero(&Shader, sizeof(Shader));
		FMemory::Memset(&Hash, 0, sizeof(Hash));
	}

	FD3D12ShaderBytecode(const D3D12_SHADER_BYTECODE &InShader) :
		Shader(InShader)
	{
		HashShader();
	}

	void SetShaderBytecode(const D3D12_SHADER_BYTECODE &InShader)
	{
		Shader = InShader;
		HashShader();
	}

	const D3D12_SHADER_BYTECODE& GetShaderBytecode() const { return Shader; }
	const ShaderBytecodeHash& GetHash() const { return Hash; }

private:
	void HashShader()
	{
		FMemory::Memset(&Hash, 0, sizeof(Hash));
		if (Shader.pShaderBytecode && Shader.BytecodeLength > 0)
		{
			FSHA1::HashBuffer(Shader.pShaderBytecode, Shader.BytecodeLength, (uint8*)Hash.SHA1Hash);
		}
	}

private:
	ShaderBytecodeHash Hash;
	D3D12_SHADER_BYTECODE Shader;
};

class FD3D12RootSignature;
struct FD3D12LowLevelGraphicsPipelineStateDesc
{
	FD3D12RootSignature *pRootSignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc;
	ShaderBytecodeHash VSHash;
	ShaderBytecodeHash HSHash;
	ShaderBytecodeHash DSHash;
	ShaderBytecodeHash GSHash;
	ShaderBytecodeHash PSHash;

	SIZE_T CombinedHash;
};

class FD3D12BoundShaderState; // forward-declare

struct FD3D12HighLevelGraphicsPipelineStateDesc
{
	FD3D12BoundShaderState* BoundShaderState;
	D3D12_BLEND_DESC* BlendState;
	D3D12_DEPTH_STENCIL_DESC* DepthStencilState;
	D3D12_RASTERIZER_DESC* RasterizerState;
	// IBStripCutValue unused
	uint32 SampleMask;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
	uint32 NumRenderTargets;
	DXGI_FORMAT RTVFormats[8];
	DXGI_FORMAT DSVFormat;
	DXGI_SAMPLE_DESC SampleDesc;

	SIZE_T CombinedHash; // Pre-computed hash

	void GetLowLevelDesc(FD3D12LowLevelGraphicsPipelineStateDesc& psoDesc);
};

struct FD3D12ComputePipelineStateDesc
{
	FD3D12RootSignature* pRootSignature;
	D3D12_COMPUTE_PIPELINE_STATE_DESC Desc;
	ShaderBytecodeHash CSHash;

	SIZE_T CombinedHash;
};


/**
 * The base class of threadsafe reference counted objects.
 */
template <class Type>
struct FThreadsafeQueue
{
private:
	mutable FCriticalSection	SynchronizationObject; // made this mutable so this class can have const functions and still be thread safe
	TQueue<Type>				Items;
public:
	void Enqueue(const Type& Item)
	{
		FScopeLock ScopeLock(&SynchronizationObject);
		Items.Enqueue(Item);
	}

	bool Dequeue(Type& Result)
	{
		FScopeLock ScopeLock(&SynchronizationObject);

		return Items.Dequeue(Result);
	}

	template <typename CompareFunc>
	bool Dequeue(Type& Result, CompareFunc Func)
	{
		FScopeLock ScopeLock(&SynchronizationObject);

		if (Items.Peek(Result))
		{
			if (Func(Result))
			{
				Items.Dequeue(Result);

				return true;
			}
		}

		return false;
	}

	bool Peek(Type& Result)
	{
		FScopeLock ScopeLock(&SynchronizationObject);
		return Items.Peek(Result);
	}

	bool IsEmpty()
	{
		FScopeLock ScopeLock(&SynchronizationObject);
		return Items.IsEmpty();
	}

	void Empty()
	{
		FScopeLock ScopeLock(&SynchronizationObject);

		Type Result;
		while (Items.Dequeue(Result)) {}
	}
};
class FD3D12Fence;
class FD3D12SyncPoint
{
public:
	FD3D12SyncPoint()
		: Fence(nullptr)
		, Value(0)
	{
	}

	FD3D12SyncPoint(FD3D12Fence* InFence, uint64 InValue)
		: Fence(InFence)
		, Value(InValue)
	{
	}

	bool IsComplete() const;
	void WaitForCompletion() const;

private:
	FD3D12Fence* Fence;
	uint64 Value;
};
