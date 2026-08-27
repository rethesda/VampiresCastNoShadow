#pragma once

#include "RE/Skyrim.h"
#include "REX/REX.h"
#include "SKSE/SKSE.h"

#include <spdlog/sinks/basic_file_sink.h>

using namespace std::literals;

namespace stl
{
	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = REL::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class T>
	void write_thunk_jump(std::uintptr_t a_src)
	{
		auto& trampoline = REL::GetTrampoline();
		T::func = trampoline.write_jmp<5>(a_src, T::thunk);
	}
}

namespace Runtime
{
	inline constexpr REL::Version SSE_1_7_99(1, 7, 99, 0);
	inline constexpr REL::Version MIN_ADDRESS_LIBRARY_V5 = SSE_1_7_99;

	[[nodiscard]] inline bool IsAtLeast1_7_99() noexcept
	{
		static bool result = REX::FModule::GetExecutingModule().GetFileVersion() >= Runtime::SSE_1_7_99;
		return result;
	}
}

#ifdef SKYRIM_AE
#	define OFFSET(se, ae) ae
#	define OFFSET_3(se, ae, vr) ae
#	define OFFSET_3_VERSIONED(se, ae, ae1799, vr) \
		(Runtime::IsAtLeast1_7_99() ? ae1799 : ae)
#elif SKYRIMVR
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) vr
#	define OFFSET_3_VERSIONED(se, ae, ae1799, vr) vr
#else
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) se
#	define OFFSET_3_VERSIONED(se, ae, ae1799, vr) se
#endif

#include "Version.h"
