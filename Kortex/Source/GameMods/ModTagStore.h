#pragma once
#include <Kortex/Kortex.hpp>

namespace Kortex
{
	class IModTag;

	class ModTagStore
	{
		private:
			std::unordered_set<kxf::String> m_TagIDs;
			kxf::String m_PrimaryTag;

		public:
			using Visitor = std::function<bool(IModTag&)>;
			using CVisitor = std::function<bool(const IModTag&)>;

		public:
			bool HasTag(const kxf::String& tagID) const;
			bool HasTag(const IModTag& tag) const;

			void AddTag(const kxf::String& tagID);
			void AddTag(const IModTag& tag);

			void RemoveTag(const kxf::String& tagID);
			void RemoveTag(const IModTag& tag);

			void ToggleTag(const kxf::String& tagID, bool addTag);
			void ToggleTag(const IModTag& tagID, bool addTag);
			
			size_t GetSize() const
			{
				return m_TagIDs.size();
			}
			size_t IsEmpty() const
			{
				return m_TagIDs.empty();
			}
			void Clear();

			void Visit(const Visitor& visitor);
			void Visit(const CVisitor& visitor) const;

			IModTag* GetPrimaryTag() const;
			kxf::String GetPrimaryTagID() const;
			bool SetPrimaryTag(const IModTag& tag);
			bool SetPrimaryTag(const kxf::String& tagID);
			bool ClearPrimaryTag();

			KxStringVector GetIDs() const;
			KxStringVector GetNames() const;
	};
}
