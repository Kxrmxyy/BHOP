#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <thread>

namespace offsets
{
	// Client

	constexpr std::ptrdiff_t dwLocalPlayer = 0xDEB99C;
	constexpr std::ptrdiff_t dwForceJump = 0x52BCD88;

	// Player

	constexpr std::ptrdiff_t m_iHealth = 0x100;
	constexpr std::ptrdiff_t m_fFlags = 0x104;
}

// BHOP Func
void BunnyHop(const HMODULE instance) noexcept
{
	const auto client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));

	// BHOP Loop

	while (!GetAsyncKeyState(VK_END))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// Check for space press

		if (!GetAsyncKeyState(VK_SPACE))
			continue;

		// Get LocalPlr

		const auto localPlayer = *reinterpret_cast<std::uintptr_t*>(client + offsets::dwLocalPlayer);

		// Is Valid LP?
		if (!localPlayer)
			continue;

		const auto health = *reinterpret_cast<std::int32_t*>(localPlayer + offsets::m_iHealth);

		// Is Alive?

		if (!health)
			continue;

		const auto flags = *reinterpret_cast<std::int32_t*>(localPlayer + offsets::m_fFlags);

		// Onground

		(flags & (1 << 0))?
			*reinterpret_cast<std::uintptr_t*>(client + offsets::dwForceJump) = 6 : // Force Jump
			*reinterpret_cast<std::uintptr_t*>(client + offsets::dwForceJump) = 4;   // Reset
	}

	// Uninject

	FreeLibraryAndExitThread(instance, 0);
}


int __stdcall DLLMain(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)
{
	// Attach
	if (reason == 1)
	{
		DisableThreadLibraryCalls(instance);

		// Exploit Thread
		const auto thread = CreateThread(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(BunnyHop),
			instance,
			0,
			nullptr
		);

		if (thread)
			CloseHandle(thread);
	}


	return 1;
}
