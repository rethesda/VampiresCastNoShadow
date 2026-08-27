#include "Hooks.h"

namespace VampiresCastNoShadow
{
	struct detail
	{
		static void stop_shadow_cast(RE::NiAVObject* a_object)
		{
			using FLAG = RE::BSShaderProperty::EShaderPropertyFlag8;

			RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
				const auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(a_geometry->shaderProperty.get());
				if (lightingShader) {
					lightingShader->SetFlags(FLAG::kCastShadows, false);
				}
				return RE::BSVisit::BSVisitControl::kContinue;
			});
		}
	};

	struct AttachBSFadeNode
	{
		static void thunk(RE::NiAVObject* a_object, RE::BSFadeNode* a_root3D)
		{
			func(a_object, a_root3D);

			const auto user = a_root3D && a_object ? a_root3D->GetUserData() : nullptr;
			const auto actor = user ? user->As<RE::Actor>() : nullptr;

			if (actor && actor->HasKeyword(vampireKYWD)) {
				detail::stop_shadow_cast(a_object);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct StoreHeadNodes
	{
		static void thunk(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::BSFaceGenNiNode* a_faceNode)
		{
			func(a_actor, a_root, a_faceNode);

			if (a_actor && a_faceNode && a_actor->HasKeyword(vampireKYWD)) {
				detail::stop_shadow_cast(a_faceNode);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		//B70 in 1.6.353
		//B60 in 1.6.629+
		//B70 in 1.7.99
		REL::Relocation<std::uintptr_t> attach_armor{ RELOCATION_ID(15501, 15678), OFFSET_3_VERSIONED(0xA13, 0xB60, 0xB70, 0xB0A) };
		stl::write_thunk_call<AttachBSFadeNode>(attach_armor.address());

		REX::INFO("Installed armor hook");

		//torches/weapons/anything with TESMODEL
		REL::Relocation<std::uintptr_t> attach_weapon{ RELOCATION_ID(15569, 15746), OFFSET_3(0x2DD, 0x2EC, 0x32E) };
		stl::write_thunk_call<AttachBSFadeNode>(attach_weapon.address());

		REX::INFO("Installed model hook");

		REL::Relocation<std::uintptr_t> attach_head{ RELOCATION_ID(24228, 24732), OFFSET(0x1CD, 0x15B) };
		stl::write_thunk_call<StoreHeadNodes>(attach_head.address());

		REX::INFO("Installed head hook");
	}
}
