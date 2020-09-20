#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxFileItem.h>

namespace Kortex
{
	class IGameMod;
	class IModDispatcher;

	class FileTreeNode final
	{
		friend class IModDispatcher;
		friend class IGameMod;

		public:
			using Vector = std::vector<FileTreeNode>;
			using RefVector = std::vector<FileTreeNode*>;
			using CRefVector = std::vector<const FileTreeNode*>;

			using TreeWalker = std::function<bool(const FileTreeNode&)>;

		private:
			enum class NavigateTo
			{
				File,
				Folder,
				Any
			};

		private:
			static const FileTreeNode* NavigateToElement(const FileTreeNode& rootNode, const kxf::String& relativePath, NavigateTo type);
			
			template<class T>
			static T* FindRootNode(T* thisNode)
			{
				T* node = thisNode;
				while (node && !node->IsRootNode() && node->IsOK())
				{
					node = node->GetParent();
				}
				return node;
			}
			
			template<class NodesVector, class NodesRefVector>
			static void RepackToRefVector(NodesVector& nodes, NodesRefVector& nodeRefs)
			{
				nodeRefs.reserve(nodes.size());
				for (auto& node: nodes)
				{
					nodeRefs.push_back(&node);
				}
			}

		public:
			static const FileTreeNode* NavigateToFolder(const FileTreeNode& rootNode, const kxf::String& relativePath)
			{
				return NavigateToElement(rootNode, relativePath, NavigateTo::Folder);
			}
			static const FileTreeNode* NavigateToFile(const FileTreeNode& rootNode, const kxf::String& relativePath)
			{
				return NavigateToElement(rootNode, relativePath, NavigateTo::File);
			}
			static const FileTreeNode* NavigateToAny(const FileTreeNode& rootNode, const kxf::String& relativePath)
			{
				return NavigateToElement(rootNode, relativePath, NavigateTo::Any);
			}

			static void ToRefVector(Vector& nodes, RefVector& refItems)
			{
				RepackToRefVector(nodes, refItems);
			}
			static void ToCRefVector(const Vector& nodes, CRefVector& refItems)
			{
				RepackToRefVector(nodes, refItems);
			}

			static size_t HashFileName(const std::wstring_view& name);
			static size_t HashFileName(const kxf::String& name)
			{
				return HashFileName(std::wstring_view(name.wc_str(), name.length()));
			}

		private:
			KxFileItem m_Item;
			Vector m_Children;
			Vector m_Alternatives;
			const IGameMod* m_Mod = nullptr;
			const FileTreeNode* m_Parent = nullptr;
			size_t m_NameHash = 0;

		private:
			void MakeNull()
			{
				*this = FileTreeNode();
			}
			void SetParent(const FileTreeNode* parent)
			{
				m_Parent = parent;
			}

		public:
			FileTreeNode() = default;
			FileTreeNode(const IGameMod& mod, const KxFileItem& item, const FileTreeNode* parent = nullptr);
			FileTreeNode(const FileTreeNode&) = delete;
			FileTreeNode(FileTreeNode&& other)
			{
				*this = std::move(other);
			}
			~FileTreeNode();

		public:
			bool IsOK() const
			{
				return m_Mod != nullptr;
			}
			bool IsRootNode() const
			{
				return m_Parent == nullptr && !m_Item.IsOK();
			}
			const IGameMod& GetMod() const
			{
				return *m_Mod;
			}
			void CopyBasicAttributes(const FileTreeNode& other)
			{
				m_NameHash = other.m_NameHash;
			}

			const FileTreeNode* WalkTree(const TreeWalker& functor) const;
			const FileTreeNode* WalkToRoot(const TreeWalker& functor) const;

			bool HasChildren() const
			{
				return !m_Children.empty();
			}
			size_t GetChildrenCount() const
			{
				return m_Children.size();
			}
			const Vector& GetChildren() const
			{
				return m_Children;
			}
			Vector& GetChildren()
			{
				return m_Children;
			}
			void ClearChildren()
			{
				m_Children.clear();
			}

			bool HasAlternatives() const
			{
				return !m_Alternatives.empty();
			}
			bool HasAlternativesFromActiveMods() const;
			size_t GetAlternativesCount() const
			{
				return m_Alternatives.size();
			}
			const Vector& GetAlternatives() const
			{
				return m_Alternatives;
			}
			Vector& GetAlternatives()
			{
				return m_Alternatives;
			}
			void ClearAlternatives()
			{
				m_Alternatives.clear();
			}

			const KxFileItem& GetItem() const
			{
				return m_Item;
			}
			KxFileItem& GetItem()
			{
				return m_Item;
			}

			bool HasParent() const
			{
				return m_Parent != nullptr;
			}
			const FileTreeNode* GetParent() const
			{
				return m_Parent;
			}
			const FileTreeNode* GetRootNode() const
			{
				return FindRootNode(this);
			}

			size_t GetNameHash() const
			{
				return m_NameHash;
			}
			void ComputeHash()
			{
				m_NameHash = HashFileName(GetName());
			}
		
			kxf::String GetName() const
			{
				return m_Item.GetName();
			}
			kxf::String GetSource() const
			{
				return m_Item.GetSource();
			}
			kxf::String GetFullPath() const
			{
				return m_Item.GetFullPath();
			}
			kxf::String GetRelativePath() const;
			kxf::String GetFileExtension() const
			{
				return m_Item.GetFileExtension();
			}

			bool IsDirectory() const
			{
				return m_Item.IsDirectory();
			}
			bool IsFile() const
			{
				return m_Item.IsFile();
			}
			int64_t GetFileSize() const
			{
				return m_Item.GetFileSize();
			}
			
		public:
			FileTreeNode& operator=(const FileTreeNode&) = delete;
			FileTreeNode& operator=(FileTreeNode&& other);
	};
}
