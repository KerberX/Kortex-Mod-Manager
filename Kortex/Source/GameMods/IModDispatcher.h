#pragma once
#include <Kortex/Kortex.hpp>
#include "FileTreeNode.h"
#include "IGameMod.h"
#include "ModManager/Common.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex
{
	class IGameMod;

	class IModDispatcher: public kxf::SingletonPtr<IModDispatcher>
	{
		public:
			enum class IterationOrder
			{
				Forward,
				Backward
			};
			using IterationFunctor = std::function<bool(const IGameMod&)>;
			using FilterFunctor = std::function<bool(const FileTreeNode&)>;

		protected:
			IGameMod* DoIterateMods(const IGameMod::RefVector& mods, const IterationFunctor& functor, IterationOrder order) const;

		public:
			// Full rebuild of file tree. Invalidates all references to old tree nodes.
			virtual void UpdateVirtualTree() = 0;
			virtual void InvalidateVirtualTree() = 0;

			// Root node of virtual files tree.
			virtual const FileTreeNode& GetVirtualTree() const = 0;

			// Resolves provided relative file path to real file.
			virtual const FileTreeNode* ResolveLocation(const kxf::String& relativePath) const = 0;

			// A different variant of 'ResolveLocation'. If 'relativePath' is not found, returns it as relative to write target.
			// Returns absolute paths unchanged.
			virtual kxf::String ResolveLocationPath(const kxf::String& relativePath, const IGameMod** owningMod = nullptr) const = 0;

			// Searches virtual tree for specified file given its fill path.
			virtual const FileTreeNode* BackTrackFullPath(const kxf::String& fullPath) const = 0;
		
			// Searches files in virtual tree in specified directory.
			virtual FileTreeNode::CRefVector Find(const kxf::String& relativePath, const FilterFunctor& filter, bool recurse = false) const = 0;

			// Searches files in specified node. This can be 'BasicGameMod' tree or virtual tree.
			virtual FileTreeNode::CRefVector Find(const FileTreeNode& rootNode, const FilterFunctor& filter, bool recurse = false) const = 0;
		
			// Searches files in specified mod. It's basically short-circuit to previous function with mod's file tree.
			virtual FileTreeNode::CRefVector Find(const IGameMod& mod, const FilterFunctor& filter, bool recurse = false) const = 0;

			// Iterates over all mods in specified order calling provided functor.
			// Returns non-null if iteration stopped before reaching the end.
			// Return 'false' from functor to stop iteration.
			IGameMod* IterateModsForward(const IGameMod::RefVector& mods, IterationFunctor functor) const
			{
				return DoIterateMods(mods, functor, IterationOrder::Forward);
			}
			IGameMod* IterateModsBackward(const IGameMod::RefVector& mods, IterationFunctor functor) const
			{
				return DoIterateMods(mods, functor, IterationOrder::Backward);
			}
			IGameMod* IterateModsForward(IterationFunctor functor, ModManager::GetModsFlags flags = ModManager::GetModsFlags::None) const;
			IGameMod* IterateModsBackward(IterationFunctor functor, ModManager::GetModsFlags flags = ModManager::GetModsFlags::None) const;
	};
}
