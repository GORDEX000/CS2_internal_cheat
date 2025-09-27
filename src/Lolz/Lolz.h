#ifndef __LOLZ_H__
#define __LOLZ_H__

#include <stdint.h>

#define LOLZ_VERSION "1.2.6"

#define LOLZ_INCLUDE_D3D9   0 // 1 if you need D3D9 hook
#define LOLZ_INCLUDE_D3D10  0 // 1 if you need D3D10 hook
#define LOLZ_INCLUDE_D3D11  1 // 1 if you need D3D11 hook
#define LOLZ_INCLUDE_D3D12  0 // 1 if you need D3D12 hook
#define LOLZ_INCLUDE_OPENGL 0 // 1 if you need OpenGL hook
#define LOLZ_INCLUDE_VULKAN 0 // 1 if you need Vulkan hook
#define LOLZ_USE_MINHOOK    1 // 1 if you will use Lolz::bind function

#define LOLZ_ARCH_X64 0
#define LOLZ_ARCH_X86 0

#if defined(_M_X64)	
# undef  LOLZ_ARCH_X64
# define LOLZ_ARCH_X64 1
#else
# undef  LOLZ_ARCH_X86
# define LOLZ_ARCH_X86 1
#endif

#if LOLZ_ARCH_X64
typedef uint64_t uint150_t;
#else
typedef uint32_t uint150_t;
#endif

namespace Lolz
{
	struct Status
	{
		enum Enum
		{
			UnknownError = -1,
			NotSupportedError = -2,
			ModuleNotFoundError = -3,

			AlreadyInitializedError = -4,
			NotInitializedError = -5,

			Success = 0,
		};
	};

	struct RenderType
	{
		enum Enum
		{
			None,

			D3D9,
			D3D10,
			D3D11,
			D3D12,

			OpenGL,
			Vulkan,

			Auto
		};
	};

	Status::Enum init(RenderType::Enum renderType);
	void shutdown();

	Status::Enum bind(uint16_t index, void** original, void* function);
	void unbind(uint16_t index);

	RenderType::Enum getRenderType();
	uint150_t* getMethodsTable();
}

#endif // __LOLZ_H__