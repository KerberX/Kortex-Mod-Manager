#include "stdafx.h"
#include "OptionStore.h"

namespace Kortex::NetworkManager::NXMHandler
{
	void OptionStore::Save(AppOption& option) const
	{
		kxf::XMLNode rootNode = option.GetNode();
		rootNode.ClearChildren();

		for (auto& [nexusID, anyValue]: m_Options)
		{
			kxf::XMLNode itemNode = rootNode.NewElement("Item");
			itemNode.SetAttribute("NexusID", nexusID);

			if (const Instance* value = std::get_if<Instance>(&anyValue))
			{
				itemNode.NewElement("Instance").SetAttribute("ID", value->ID);
			}
			else if (const Command* value = std::get_if<Command>(&anyValue))
			{
				kxf::XMLNode node = itemNode.NewElement("Command");
				node.NewElement("Executable").SetValue(value->Executable);
				node.NewElement("Arguments").SetValue(value->Arguments);
			}
		}
	}
	void OptionStore::Load(const AppOption& option)
	{
		kxf::XMLNode rootNode = option.GetNode();
		for (kxf::XMLNode itemNode = rootNode.GetFirstChildElement(); itemNode.IsOK(); itemNode = itemNode.GetNextSiblingElement())
		{
			kxf::String nexusID = itemNode.GetAttribute("NexusID");
			if (!nexusID.IsEmpty())
			{
				KxString::MakeLower(nexusID);
				if (kxf::XMLNode node = itemNode.GetFirstChildElement("Instance"); node.IsOK())
				{
					Instance instance{node.GetAttribute("ID")};
					if (instance)
					{
						m_Options.insert_or_assign(nexusID, std::move(instance));
					}
				}
				else if (kxf::XMLNode node = itemNode.GetFirstChildElement("Command"); node.IsOK())
				{
					Command command;
					command.Executable = node.GetFirstChildElement("Executable").GetValue();
					command.Arguments = node.GetFirstChildElement("Arguments").GetValue();

					if (command)
					{
						m_Options.insert_or_assign(nexusID, std::move(command));
					}
				}
			}
		}
	}
}
