#include "stdafx.h"
#include "Definition.h"
#include "ITypeDetector.h"
#include <kxf::UI::Framework/KxFileStream.h>
#include <kxf::UI::Framework/KxXML.h>

namespace Kortex::GameConfig
{
	void Definition::LoadGroups(const kxf::XMLNode& groupsNode)
	{
		for (kxf::XMLNode node = groupsNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			kxf::String id = node.GetAttribute(wxS("ID"));
			m_Groups.insert_or_assign(id, std::make_unique<ItemGroup>(*this, id, node, m_Options));
		}
	}

	Definition::~Definition()
	{
	}

	bool Definition::Load()
	{
		KxFileStream stream(m_FilePath, KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
		if (kxf::XMLDocument xml(stream); xml.IsOK())
		{
			const kxf::XMLNode rootNode = xml.GetFirstChildElement("Definition");

			// Load data types
			for (kxf::XMLNode node = rootNode.GetFirstChildElement("DataTypes").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				DataType type(node);
				if (type.IsOK())
				{
					m_DataTypes.insert_or_assign(type.GetID(), std::move(type));
				}
			}

			// Load type detectors config
			for (kxf::XMLNode node = rootNode.GetFirstChildElement("TypeDetection").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				switch (TypeDetectorDef::FromString(node.GetName(), TypeDetectorID::None))
				{
					case TypeDetectorID::HungarianNotation:
					{
						m_TypeDetectors.emplace_back(std::make_unique<HungarianNotationTypeDetector>(node));
						break;
					}
					case TypeDetectorID::DataAnalysis:
					{
						m_TypeDetectors.emplace_back(std::make_unique<DataAnalysisTypeDetector>());
						break;
					}
				};
			}

			// Load options
			m_Options.Load(rootNode.GetFirstChildElement(wxS("Options")));
			m_Options.CopyPrecisionIfNotSpecified(GetDataType(DataTypeID::Float32));
			m_Options.CopyPrecisionIfNotSpecified(GetDataType(DataTypeID::Float64));

			// Load groups
			LoadGroups(rootNode.GetFirstChildElement(wxS("Groups")));

			return true;
		}
		return false;
	}
	void Definition::RemoveInvalidGroups()
	{
		for (auto it = m_Groups.begin(); it != m_Groups.end();)
		{
			if (!it->second->IsOK())
			{
				m_Groups.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	DataType Definition::GetDataType(TypeID id) const
	{
		auto it = m_DataTypes.find(id);
		if (it != m_DataTypes.end())
		{
			return it->second;
		}
		return DataType::CreateGeneric(id);
	}
}
