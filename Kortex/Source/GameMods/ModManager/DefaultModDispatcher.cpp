#include "stdafx.h"
#include "DefaultModDispatcher.h"
#include <Kortex/Application.hpp>
#include <Kortex/ModManager.hpp>
#include "Utility/Log.h"
#include "UI/ProgressOverlay.h"
#include <KxFramework/KxComparator.h>
#include <KxFramework/KxFileFinder.h>
#include <chrono>
#include <execution>

namespace
{
	using namespace Kortex;
	using namespace Kortex::ModManager;

	wxTimeSpan GetClockTime()
	{
		using namespace std::chrono;
		return wxTimeSpan::Milliseconds(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
	};

	struct FinderHashComparator
	{
		bool operator()(const kxf::String& lhs, const kxf::String& rhs) const
		{
			return KxComparator::IsEqual(lhs, rhs, true);
		}
	};
	struct FinderHashHasher
	{
		size_t operator()(const kxf::String& value) const
		{
			return FileTreeNode::HashFileName(value);
		}
	};
	using FinderHash = std::unordered_map<kxf::String, size_t, FinderHashHasher, FinderHashComparator>;

	void FindFilesInTree(FileTreeNode::CRefVector& nodes, const FileTreeNode& rootNode, const IModDispatcher::FilterFunctor& filter, bool recurse)
	{
		auto ScanSubTree = [&nodes, &filter, recurse](FileTreeNode::CRefVector& directories, const FileTreeNode& folderNode)
		{
			for (const FileTreeNode& node: folderNode.GetChildren())
			{
				// Add node if no filter is specified or filter accepts this node
				if (!filter || filter(node))
				{
					nodes.push_back(&node);
				}

				// Add directory to scan on next round
				if (recurse && node.IsDirectory())
				{
					directories.push_back(&node);
				}
			}
		};

		// Top level
		FileTreeNode::CRefVector directories;
		ScanSubTree(directories, rootNode);
		
		// Subdirectories
		while (!directories.empty())
		{
			FileTreeNode::CRefVector roundDirectories;
			for (const FileTreeNode* node: directories)
			{
				ScanSubTree(roundDirectories, *node);
			}
			directories = std::move(roundDirectories);
		}
	}
	size_t BuildTreeBranch(const IGameMod::RefVector& mods, FileTreeNode::Vector& children, const FileTreeNode* rootNode, FileTreeNode::RefVector& directories)
	{
		std::unordered_map<size_t, size_t> hash;
		hash.reserve(mods.size());
		const kxf::String rootPath = rootNode ? rootNode->GetRelativePath() : wxEmptyString;

		// Iterate manually, without using 'IterateOverModsEx'
		for (auto it = mods.rbegin(); it != mods.rend(); ++it)
		{
			const IGameMod& currentMod = **it;
			if (currentMod.IsInstalled())
			{
				// If we have root node, look for files in real file tree
				// Otherwise use mod's tree root
				const FileTreeNode* searchNode = &currentMod.GetFileTree();
				if (rootNode)
				{
					searchNode = FileTreeNode::NavigateToFolder(currentMod.GetFileTree(), rootPath);
				}

				if (searchNode)
				{
					// Not enough, but at least something
					children.reserve(searchNode->GetChildrenCount());

					for (const FileTreeNode& node: searchNode->GetChildren())
					{
						auto hashIt = hash.try_emplace(node.GetNameHash(), (size_t)-1);
						if (hashIt.second)
						{
							FileTreeNode& newNode = children.emplace_back(currentMod, node.GetItem(), rootNode);
							newNode.CopyBasicAttributes(node);

							// Save index to new node to add alternatives to it later.
							// I'd use pointer, but it can be invalidated on reallocation.
							hashIt.first->second = children.size() - 1;
						}
						else
						{
							const size_t index = hashIt.first->second;
							FileTreeNode& newAlternative = children[index].GetAlternatives().emplace_back(currentMod, node.GetItem(), rootNode);
							newAlternative.CopyBasicAttributes(node);
						}
					}
				}
			}
		}

		// Fill directories array
		size_t directoriesCount = 0;
		for (FileTreeNode& node: children)
		{
			if (node.IsDirectory())
			{
				directories.push_back(&node);
				directoriesCount++;
			}
		}
		return directoriesCount;
	}
}

namespace Kortex::ModManager
{
	kxf::String KDispatcherCollision::GetLocalizedCollisionName(KMMDispatcherCollisionType type)
	{
		switch (type)
		{
			case KMM_DCT_NONE:
			{
				return KTr(KxID_NONE);
			}
			case KMM_DCT_UNKNOWN:
			{
				return KTr("ModExplorer.Collision.Unknown");
			}
			case KMM_DCT_OVERWRITTEN:
			{
				return KTr("ModExplorer.Collision.Overwritten");
			}
			case KMM_DCT_OVERWRITES:
			{
				return KTr("ModExplorer.Collision.Owerwrites");
			}
		};
		return wxEmptyString;
	}
}

namespace Kortex::ModManager
{
	void DefaultModDispatcher::OnVirtualTreeInvalidated(BroadcastEvent& event)
	{
		InvalidateVirtualTree();
	}
	void DefaultModDispatcher::OnAccessVirtualTree() const
	{
		if (m_IsInvalidated)
		{
			const_cast<DefaultModDispatcher*>(this)->UpdateVirtualTree();
			m_IsInvalidated = false;
		}
	}
	
	void DefaultModDispatcher::InvalidateVirtualTree()
	{
		m_IsInvalidated = true;
		BroadcastProcessor::Get().ProcessEvent(ModEvent::EvtVirtualTreeInvalidated);
	}
	void DefaultModDispatcher::UpdateVirtualTree()
	{
		UI::ProgressOverlay status;

		const IGameMod::RefVector mods = IModManager::GetInstance()->GetMods(GetModsFlags::Everything|GetModsFlags::ActiveOnly);
		m_VirtualTree.ClearChildren();

		constexpr const bool useRecursive = false;
		const wxTimeSpan startTime = GetClockTime();

		if constexpr(useRecursive)
		{
			// Recursive (parallel)
			// Build top level
			FileTreeNode::RefVector directories;
			BuildTreeBranch(mods, m_VirtualTree.GetChildren(), &m_VirtualTree, directories);

			// Build subdirectories
			std::function<void(FileTreeNode::RefVector&&)> Execute;
			Execute = [this, &Execute, &mods](FileTreeNode::RefVector&& directories)
			{
				std::for_each(std::execution::par_unseq, directories.begin(), directories.end(), [this, &Execute, &mods](FileTreeNode* node)
				{
					FileTreeNode::RefVector directories;
					BuildTreeBranch(mods, node->GetChildren(), node, directories);
					Execute(std::move(directories));
				});
			};
			Execute(std::move(directories));
		}
		else
		{
			// Iterational (sequential)
			size_t processed = 0;
			size_t total = 0;
			auto NotifyProgress = [&](size_t processedInc, size_t totalInc = 0)
			{
				processed += processedInc;
				total += totalInc;

				status.UpdateProgress(processed, total);
			};

			// Build top level
			FileTreeNode::RefVector directories; 
			BuildTreeBranch(mods, m_VirtualTree.GetChildren(), &m_VirtualTree, directories);
			NotifyProgress(0, directories.size());

			// Build subdirectories
			while (!directories.empty())
			{
				FileTreeNode::RefVector roundDirectories;
				roundDirectories.reserve(directories.size());

				for (FileTreeNode* node: directories)
				{
					size_t directoriesCount = BuildTreeBranch(mods, node->GetChildren(), node, roundDirectories);
					NotifyProgress(1, directoriesCount);
				}
				directories = std::move(roundDirectories);
			}
		}

		Utility::Log::LogInfo("DefaultModDispatcher::UpdateVirtualTree: %1 ms", (GetClockTime() - startTime).GetMilliseconds().GetValue());
	}

	const FileTreeNode& DefaultModDispatcher::GetVirtualTree() const
	{
		OnAccessVirtualTree();

		return m_VirtualTree;
	}

	const FileTreeNode* DefaultModDispatcher::ResolveLocation(const kxf::String& relativePath) const
	{
		OnAccessVirtualTree();

		return FileTreeNode::NavigateToAny(m_VirtualTree, relativePath);
	}
	kxf::String DefaultModDispatcher::ResolveLocationPath(const kxf::String& relativePath, const IGameMod** owningMod) const
	{
		// This is an absolute path, return it as is.
		if (relativePath.Length() >= 2 && relativePath[1] == wxT(':'))
		{
			KxUtility::SetIfNotNull(owningMod, nullptr);
			return relativePath;
		}

		if (const FileTreeNode* node = ResolveLocation(relativePath))
		{
			KxUtility::SetIfNotNull(owningMod, &node->GetMod());
			return node->GetFullPath();
		}

		// Fallback to write target
		KxUtility::SetIfNotNull(owningMod, nullptr);
		return IModManager::GetInstance()->GetWriteTarget().GetModFilesDir() + wxS('\\') + relativePath;
	}
	const FileTreeNode* DefaultModDispatcher::BackTrackFullPath(const kxf::String& fullPath) const
	{
		OnAccessVirtualTree();

		return m_VirtualTree.WalkTree([&fullPath](const FileTreeNode& node)
		{
			return KxComparator::IsEqual(node.GetFullPath(), fullPath, true);
		});
	}

	FileTreeNode::CRefVector DefaultModDispatcher::Find(const kxf::String& relativePath, const FilterFunctor& filter, bool recurse) const
	{
		OnAccessVirtualTree();

		FileTreeNode::CRefVector nodes;
		const FileTreeNode* folderNode = FileTreeNode::NavigateToFolder(m_VirtualTree, relativePath);
		if (folderNode)
		{
			FindFilesInTree(nodes, *folderNode, filter, recurse);
		}
		return nodes;
	}
	FileTreeNode::CRefVector DefaultModDispatcher::Find(const FileTreeNode& rootNode, const FilterFunctor& filter, bool recurse) const
	{
		OnAccessVirtualTree();

		FileTreeNode::CRefVector nodes;
		FindFilesInTree(nodes, rootNode, filter, recurse);

		return nodes;
	}
	FileTreeNode::CRefVector DefaultModDispatcher::Find(const IGameMod& mod, const FilterFunctor& filter, bool recurse) const
	{
		OnAccessVirtualTree();

		FileTreeNode::CRefVector nodes;
		FindFilesInTree(nodes, mod.GetFileTree(), filter, recurse);

		return nodes;
	}

	KDispatcherCollision::Vector DefaultModDispatcher::FindCollisions(const IGameMod& scannedMod, const kxf::String& relativePath) const
	{
		OnAccessVirtualTree();

		KDispatcherCollision::Vector collisions;
		KMMDispatcherCollisionType type = KMM_DCT_OVERWRITTEN;
		auto CheckMod = [&scannedMod, &collisions, &type, &relativePath](const IGameMod& currentMod)
		{
			// Flip collision type after this mod is found
			if (&currentMod == &scannedMod)
			{
				type = KMM_DCT_OVERWRITES;

				// Don't count scanned mod itself
				return true;
			}

			// If this mod is enabled and it have such file - add collision info for it.
			if (currentMod.IsActive())
			{
				const FileTreeNode* node = FileTreeNode::NavigateToAny(currentMod.GetFileTree(), relativePath);
				if (node && node->IsFile())
				{
					collisions.emplace_back(currentMod, type);
				}
			}
			return true;
		};

		IterateModsBackward(CheckMod, GetModsFlags::Everything|GetModsFlags::ActiveOnly);
		std::reverse(collisions.begin(), collisions.end());
		return collisions;
	}

	DefaultModDispatcher::DefaultModDispatcher()
	{
		m_BroadcastReciever.Bind(ModEvent::EvtFilesChanged, &DefaultModDispatcher::OnVirtualTreeInvalidated, this);
		m_BroadcastReciever.Bind(ModEvent::EvtInstalled, &DefaultModDispatcher::OnVirtualTreeInvalidated, this);
		m_BroadcastReciever.Bind(ModEvent::EvtUninstalled, &DefaultModDispatcher::OnVirtualTreeInvalidated, this);
		m_BroadcastReciever.Bind(ModEvent::EvtToggled, &DefaultModDispatcher::OnVirtualTreeInvalidated, this);
		m_BroadcastReciever.Bind(ModEvent::EvtReordered, &DefaultModDispatcher::OnVirtualTreeInvalidated, this);
	}
}
