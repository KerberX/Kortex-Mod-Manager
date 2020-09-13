#pragma once
#include <Kortex/Kortex.hpp>
#include "GameMods/IModDispatcher.h"
#include "Application/BroadcastProcessor.h"
#include <KxFramework/KxFile.h>

namespace Kortex::ModManager
{
	enum KMMDispatcherCollisionType
	{
		KMM_DCT_NONE,
		KMM_DCT_UNKNOWN,
		KMM_DCT_OVERWRITTEN,
		KMM_DCT_OVERWRITES,
	};
	class KDispatcherCollision
	{
		public:
			using Vector = std::vector<KDispatcherCollision>;

		public:
			static kxf::String GetLocalizedCollisionName(KMMDispatcherCollisionType type);

		private:
			const IGameMod* m_Mod = nullptr;
			KMMDispatcherCollisionType m_Type = KMM_DCT_NONE;

		public:
			KDispatcherCollision(const IGameMod& mod, KMMDispatcherCollisionType type)
				:m_Mod(&mod), m_Type(type)
			{
			}

		public:
			const IGameMod& GetMod() const
			{
				return *m_Mod;
			}
			KMMDispatcherCollisionType GetType() const
			{
				return m_Type;
			}
	};
}

namespace Kortex::ModManager
{
	class DefaultModDispatcher: public IModDispatcher
	{
		private:
			BroadcastReciever m_BroadcastReciever;
			FileTreeNode m_VirtualTree;

			mutable bool m_IsInvalidated = true;

		private:
			void OnVirtualTreeInvalidated(BroadcastEvent& event);
			void OnAccessVirtualTree() const;

		public:
			void InvalidateVirtualTree() override;
			void UpdateVirtualTree() override;

			const FileTreeNode& GetVirtualTree() const override;

			const FileTreeNode* ResolveLocation(const kxf::String& relativePath) const override;
			kxf::String ResolveLocationPath(const kxf::String& relativePath, const IGameMod** owningMod = nullptr) const override;
			const FileTreeNode* BackTrackFullPath(const kxf::String& fullPath) const override;

			FileTreeNode::CRefVector Find(const kxf::String& relativePath, const FilterFunctor& filter, bool recurse = false) const override;
			FileTreeNode::CRefVector Find(const FileTreeNode& rootNode, const FilterFunctor& filter, bool recurse = false) const override;
			FileTreeNode::CRefVector Find(const IGameMod& mod, const FilterFunctor& filter, bool recurse = false) const override;

			// Searches for collisions of file specified by 'relativePath' for a mod.
			KDispatcherCollision::Vector FindCollisions(const IGameMod& scannedMod, const kxf::String& relativePath) const;

		public:
			DefaultModDispatcher();
	};
}
