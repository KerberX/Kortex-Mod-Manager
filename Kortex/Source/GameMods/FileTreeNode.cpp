#include "stdafx.h"
#include <Kortex/ModManager.hpp>
#include "Utility/String.h"
#include <kxf::UI::Framework/KxComparator.h>

namespace
{
	bool IsRequestToRootNode(const kxf::String& relativePath)
	{
		return relativePath.IsEmpty() || relativePath == wxS('\\') || relativePath == wxS('/') || relativePath == wxS('.') || relativePath == wxS("..");
	}
	struct FileNameHasher
	{
		// From Boost
		template<class T> static void hash_combine(size_t& seed, const T& v)
		{
			std::hash<T> hasher;
			seed ^= hasher(v) + size_t(0x9e3779b9u) + (seed << 6) + (seed >> 2);
		}
		
		size_t operator()(std::wstring_view value) const
		{
			size_t hashValue = 0;
			for (wchar_t c: value)
			{
				hash_combine(hashValue, KxString::CharToLower(c));
			}
			return hashValue;
		}
	};
}

namespace Kortex
{
	const FileTreeNode* FileTreeNode::NavigateToElement(const FileTreeNode& rootNode, const kxf::String& relativePath, NavigateTo type)
	{
		if (type == NavigateTo::Folder && IsRequestToRootNode(relativePath))
		{
			return &rootNode;
		}

		if (rootNode.HasChildren())
		{
			auto ScanChildren = [](const FileTreeNode& rootNode, std::wstring_view folderName) -> const FileTreeNode*
			{
				const size_t hash = FileNameHasher()(folderName);
				for (const FileTreeNode& node: rootNode.GetChildren())
				{
					if (hash == node.GetNameHash())
					{
						return &node;
					}
				}
				return nullptr;
			};

			const FileTreeNode* finalNode = nullptr;
			Utility::String::SplitBySeparator(relativePath, wxS('\\'), [&ScanChildren, &finalNode, &rootNode](std::wstring_view folderName)
			{
				finalNode = ScanChildren(finalNode ? *finalNode : rootNode, folderName);
				return finalNode != nullptr;
			});

			if (finalNode == nullptr || (type == NavigateTo::Folder && !finalNode->IsDirectory()) || (type == NavigateTo::File && !finalNode->IsFile()))
			{
				return nullptr;
			}
			return finalNode;
		}
		return nullptr;
	}
	size_t FileTreeNode::HashFileName(const std::wstring_view& name)
	{
		return FileNameHasher()(name);
	}

	FileTreeNode::FileTreeNode(const IGameMod& mod, const KxFileItem& item, const FileTreeNode* parent)
		:m_Mod(&mod), m_Item(item), m_Parent(parent)
	{
	}
	FileTreeNode::~FileTreeNode()
	{
		m_Mod = nullptr;
		m_Parent = nullptr;
		m_NameHash = 0;
	}

	const FileTreeNode* FileTreeNode::WalkTree(const TreeWalker& functor) const
	{
		std::function<const FileTreeNode*(const FileTreeNode::Vector&)> Recurse;
		Recurse = [&Recurse, &functor](const FileTreeNode::Vector& children) -> const FileTreeNode*
		{
			for (const FileTreeNode& node: children)
			{
				if (!functor(node))
				{
					return &node;
				}

				if (node.HasChildren())
				{
					Recurse(node.GetChildren());
				}
			}
			return nullptr;
		};
		return Recurse(m_Children);
	}
	const FileTreeNode* FileTreeNode::WalkToRoot(const TreeWalker& functor) const
	{
		const FileTreeNode* node = this;
		while (node && !node->IsRootNode() && node->IsOK())
		{
			if (functor(*node))
			{
				node = node->GetParent();
			}
			else
			{
				break;
			}
		}
		return node;
	}

	bool FileTreeNode::HasAlternativesFromActiveMods() const
	{
		if (!m_Alternatives.empty())
		{
			for (const FileTreeNode& node: m_Alternatives)
			{
				if (!node.GetMod().IsActive())
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
	kxf::String FileTreeNode::GetRelativePath() const
	{
		if (m_Mod)
		{
			kxf::String path = m_Item.GetFullPath();
			if (path.Replace(m_Mod->GetModFilesDir(), wxEmptyString, false) == 1 && !path.IsEmpty() && path[0] == wxS('\\'))
			{
				// Remove leading slash
				path.Remove(0, 1);
			}
			return path;
		}
		return {};
	}

	FileTreeNode& FileTreeNode::operator=(FileTreeNode&& other)
	{
		m_Item = std::move(other.m_Item);
		m_Children = std::move(other.m_Children);
		m_Alternatives = std::move(other.m_Alternatives);

		m_Mod = other.m_Mod;
		other.m_Mod = nullptr;

		m_Parent = other.m_Parent;
		other.m_Parent = nullptr;

		m_NameHash = other.m_NameHash;
		other.m_NameHash = 0;

		return *this;
	}
}
