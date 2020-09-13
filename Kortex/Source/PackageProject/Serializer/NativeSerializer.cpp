#include "stdafx.h"
#include "NativeSerializer.h"
#include "PackageProject/ModPackageProject.h"
#include "PackageProject/ConfigSection.h"
#include "PackageProject/InfoSection.h"
#include "PackageProject/InterfaceSection.h"
#include "PackageProject/FileDataSection.h"
#include "PackageProject/RequirementsSection.h"
#include "PackageProject/ComponentsSection.h"
#include <Kortex/NetworkManager.hpp>
#include <Kortex/ModTagManager.hpp>
#include <Kortex/PackageManager.hpp>
#include "GameInstance/IGameInstance.h"
#include "Utility/LabeledValue.h"

namespace Kortex::PackageProject
{
	using WriteEmpty = KxIXDocumentNode::WriteEmpty;

	namespace
	{
		void SaveLabeledValueArray(const Utility::LabeledValue::Vector& array, kxf::XMLNode& arrayNode, const kxf::String& labelName = wxS("Label"))
		{
			arrayNode.ClearChildren();

			for (const Utility::LabeledValue& value: array)
			{
				kxf::XMLNode elementNode = arrayNode.NewElement("Entry");

				elementNode.SetValue(value.GetValue());
				elementNode.SetAttribute(labelName, value.GetLabel(), WriteEmpty::Never);
			}
		}
		void LoadLabeledValueArray(Utility::LabeledValue::Vector& array, const kxf::XMLNode& arrayNode, const kxf::String& labelName = wxS("Label"))
		{
			for (kxf::XMLNode node = arrayNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				array.emplace_back(Utility::LabeledValue(node.GetValue(), node.GetAttribute(labelName)));
			}
		}
	
		void SaveStringArray(const KxStringVector& array, kxf::XMLNode& arrayNode, const kxf::String& elementNodeName = wxS("Item"))
		{
			arrayNode.ClearChildren();
			for (const kxf::String& value: array)
			{
				arrayNode.NewElement(elementNodeName).SetValue(value, WriteEmpty::Never);
			}
		}
		void LoadStringArray(KxStringVector& array, const kxf::XMLNode& arrayNode)
		{
			for (kxf::XMLNode node = arrayNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				array.emplace_back(node.GetValue());
			}
		}
	}
	namespace
	{
		void WriteCondition(const Condition& condition, kxf::XMLNode& conditionNode, bool confitionFlags)
		{
			if (!confitionFlags)
			{
				conditionNode.SetAttribute("Operator", ModPackageProject::OperatorToString(condition.GetOperator()));
			}
			
			for (const FlagItem& flag: condition.GetFlags())
			{
				if (flag.HasName())
				{
					kxf::XMLNode flagNode = conditionNode.NewElement("Flag");
					flagNode.SetValue(flag.GetValue());
					flagNode.SetAttribute("Name", flag.GetName());
				}
			}
		}
		void WriteConditionGroup(const ConditionGroup& conditionGroup, kxf::XMLNode& groupNode)
		{
			groupNode.SetAttribute("Operator", ModPackageProject::OperatorToString(conditionGroup.GetOperator()));
			for (const Condition& condition: conditionGroup.GetConditions())
			{
				if (condition.HasFlags())
				{
					WriteCondition(condition, groupNode.NewElement("Condition"), false);
				}
			}
		}
		
		void ReadCondition(Condition& condition, const kxf::XMLNode& conditionNode)
		{
			condition.SetOperator(ModPackageProject::StringToOperator(conditionNode.GetAttribute("Operator"), false, Operator::And));
			for (kxf::XMLNode node = conditionNode.GetFirstChildElement(); node; node = node.GetNextSiblingElement())
			{
				condition.GetFlags().emplace_back(node.GetValue(), node.GetAttribute("Name"));
			}
		}
		void ReadConditionGroup(ConditionGroup& conditionGroup, const kxf::XMLNode& groupNode)
		{
			conditionGroup.SetOperator(ModPackageProject::StringToOperator(groupNode.GetAttribute("Operator"), false, Operator::And));
			for (kxf::XMLNode conditionNode = groupNode.GetFirstChildElement(); conditionNode.IsOK(); conditionNode = conditionNode.GetNextSiblingElement())
			{
				Condition& condition = conditionGroup.GetConditions().emplace_back();
				ReadCondition(condition, conditionNode);
				if (!condition.HasFlags())
				{
					conditionGroup.GetConditions().pop_back();
				}
			}
		}
	
		template<class T>
		void WriteLabeledValueArray(const Utility::LabeledValue::Vector& array, kxf::XMLNode& arrayNode, const T& Func, bool isCDATA = false)
		{
			for (const Utility::LabeledValue& value: array)
			{
				kxf::XMLNode elementNode = arrayNode.NewElement("Item");
	
				elementNode.SetValue(Func(value));
				elementNode.SetAttribute("Name", value.GetLabel(), WriteEmpty::Never);
			}
		}
	}
}

namespace Kortex::PackageProject
{
	void NativeSerializer::ReadBase()
	{
		if (kxf::XMLNode baseNode = m_XML.QueryElement("Package"))
		{
			m_ProjectLoad->SetFormatVersion(baseNode.GetAttribute("FormatVersion"));
			m_ProjectLoad->SetModID(baseNode.GetAttribute("ID"));
			
			kxf::XMLNode targetProfileNode = baseNode.GetFirstChildElement("TargetProfile");
			m_ProjectLoad->SetTargetProfileID(targetProfileNode.GetAttribute("ID"));
		}
	}
	void NativeSerializer::ReadConfig()
	{
		if (kxf::XMLNode configNode = m_XML.QueryElement("Package/PackageConfig"))
		{
			ConfigSection& config = m_ProjectLoad->GetConfig();
	
			config.SetInstallPackageFile(configNode.GetFirstChildElement("InstallPackageFile").GetValue());
			config.SetCompressionMethod(configNode.GetFirstChildElement("CompressionMethod").GetValue());
			config.SetCompressionLevel(configNode.GetFirstChildElement("CompressionLevel").GetValueInt());
			config.SetCompressionDictionarySize(configNode.GetFirstChildElement("CompressionDictionarySize").GetValueInt());
			config.SetUseMultithreading(configNode.GetFirstChildElement("CompressionUseMultithreading").GetValueBool());
			config.SetSolidArchive(configNode.GetFirstChildElement("CompressionSolidArchive").GetValueBool());
		}
	}
	void NativeSerializer::ReadInfo()
	{
		if (kxf::XMLNode infoNode = m_XML.QueryElement("Package/Info"))
		{
			InfoSection& info = m_ProjectLoad->GetInfo();
			
			// Basic info
			info.SetName(infoNode.GetFirstChildElement("Name").GetValue());
			info.SetTranslatedName(infoNode.GetFirstChildElement("TranslatedName").GetValue());
			info.SetVersion(infoNode.GetFirstChildElement("Version").GetValue());
			info.SetAuthor(infoNode.GetFirstChildElement("Author").GetValue());
			info.SetTranslator(infoNode.GetFirstChildElement("Translator").GetValue());
			info.SetDescription(infoNode.GetFirstChildElement("Description").GetValue());

			if (info.GetName() == m_ProjectLoad->GetModID())
			{
				m_ProjectLoad->SetModID(wxEmptyString);
			}
			
			// Custom info
			LoadLabeledValueArray(info.GetCustomFields(), infoNode.GetFirstChildElement("Custom"));
			
			// Source
			using namespace NetworkManager;
			ModSourceStore& store = info.GetModSourceStore();
			store.LoadAssign(infoNode.GetFirstChildElement("Source"));
			
			// Documents
			LoadLabeledValueArray(info.GetDocuments(), infoNode.GetFirstChildElement("Documents"), "Name");
			
			// Tags
			ModTagStore& tagStore = info.GetTagStore();
			for (kxf::XMLNode node = infoNode.GetFirstChildElement("Tags").GetFirstChildElement(); node; node = node.GetNextSiblingElement())
			{
				auto tag = IModTagManager::GetInstance()->NewTag();
				tag->SetID(node.GetValue());
				tagStore.AddTag(*tag);
			}
		}
	}
	void NativeSerializer::ReadInterface()
	{
		if (kxf::XMLNode interfaceNode = m_XML.QueryElement("Package/Interface"))
		{
			InterfaceSection& interfaceConfig = m_ProjectLoad->GetInterface();
			TitleConfig& titleConfig = interfaceConfig.GetTitleConfig();
	
			// Read customization
			if (kxf::XMLNode titleConfigNode = interfaceNode.GetFirstChildElement("Caption"))
			{
				titleConfig.SetAlignment((wxAlignment)titleConfigNode.GetAttributeInt("Alignment", TitleConfig::ms_InvalidAlignment));
				
				int64_t colorValue = titleConfigNode.GetAttributeInt("Color", -1);
				if (colorValue != -1)
				{
					titleConfig.SetColor(KxColor::FromRGBA(colorValue));
				}
			}
	
			// Read special images config
			auto ReadImageConfig = [](const kxf::XMLNode& node) -> ImageItem
			{
				ImageItem item;
				if (node.IsOK())
				{
					item.SetPath(node.GetAttribute("Path"));
					item.SetVisible(node.GetAttributeBool("Visible", true));
					item.SetSize(wxSize(node.GetAttributeInt("Width", wxDefaultCoord), node.GetAttributeInt("Height", wxDefaultCoord)));
					item.SetDescription(node.GetFirstChildElement("Description").GetValue());
				}
				return item;
			};
			interfaceConfig.SetMainImage(ReadImageConfig(interfaceNode.GetFirstChildElement("MainImage")).GetPath());
			interfaceConfig.SetHeaderImage(ReadImageConfig(interfaceNode.GetFirstChildElement("HeaderImage")).GetPath());
			
			// Read images list
			kxf::XMLNode imagesNode = interfaceNode.GetFirstChildElement("Images");
			for (kxf::XMLNode node = imagesNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				interfaceConfig.GetImages().emplace_back(ReadImageConfig(node));
			}
		}
	}
	void NativeSerializer::ReadFiles()
	{
		if (kxf::XMLNode fileDataNode = m_XML.QueryElement("Package/Files"))
		{
			FileDataSection& fileData = m_ProjectLoad->GetFileData();
			
			// Folder
			for (kxf::XMLNode folderNode = fileDataNode.GetFirstChildElement(); folderNode.IsOK(); folderNode = folderNode.GetNextSiblingElement())
			{
				FileItem& item = folderNode.GetName() == "Folder" ? fileData.AddFolder(std::make_unique<FolderItem>()) : fileData.AddFile(std::make_unique<FileItem>());
				item.SetID(folderNode.GetAttribute("ID"));
				item.SetSource(folderNode.GetAttribute("Source"));
				item.SetDestination(folderNode.GetAttribute("Destination"));
				item.SetPriority(folderNode.GetAttributeInt("Priority", FileDataSection::ms_DefaultPriority));
				
				if (FolderItem* folderItem; m_AsProject && item.QueryInterface(folderItem))
				{
					for (kxf::XMLNode fileNode = folderNode.GetFirstChildElement(); fileNode.IsOK(); fileNode = fileNode.GetNextSiblingElement())
					{
						FileItem& fileItem = folderItem->AddFile();
						fileItem.SetDestination(fileNode.GetValue());
						fileItem.SetSource(fileNode.GetAttribute("Source"));
					}
				}
			}
		}
	}
	void NativeSerializer::ReadRequirements()
	{
		RequirementsSection& requirements = m_ProjectLoad->GetRequirements();
		
		if (kxf::XMLNode requirementsNode = m_XML.QueryElement("Package/Requirements"))
		{
			LoadStringArray(requirements.GetDefaultGroup(), requirementsNode.GetFirstChildElement("DefaultGroups"));
			
			for (kxf::XMLNode groupNode = requirementsNode.GetFirstChildElement("Groups").GetFirstChildElement(); groupNode.IsOK(); groupNode = groupNode.GetNextSiblingElement())
			{
				RequirementGroup* requirementGroup = requirements.GetGroups().emplace_back(std::make_unique<RequirementGroup>()).get();
				requirementGroup->SetID(groupNode.GetAttribute("ID"));
				requirementGroup->SetOperator(ModPackageProject::StringToOperator(groupNode.GetAttribute("Operator"), false, requirements.ms_DefaultGroupOperator));
				
				for (kxf::XMLNode itemNode = groupNode.GetFirstChildElement(); itemNode.IsOK(); itemNode = itemNode.GetNextSiblingElement())
				{
					ReqType type = requirements.StringToTypeDescriptor(itemNode.GetAttribute("Type"));
					
					auto& item = requirementGroup->GetItems().emplace_back(std::make_unique<RequirementItem>(type));
					item->SetID(itemNode.GetAttribute("ID"));
					item->SetName(itemNode.GetFirstChildElement("Name").GetValue());
					
					// Object
					kxf::XMLNode objectNode = itemNode.GetFirstChildElement("Object");
					item->SetObject(objectNode.GetValue());
					item->SetObjectFunction(requirements.StringToObjectFunction(objectNode.GetAttribute("Function")));
					
					// Version
					kxf::XMLNode versionNode = itemNode.GetFirstChildElement("Version");
					item->SetRequiredVersion(versionNode.GetValue());

					kxf::String operatorString = versionNode.GetAttribute("Operator");
					if (operatorString.IsEmpty())
					{
						// There was an error with required version operator were written as 'Function'.
						operatorString = versionNode.GetAttribute("Function");
					}
					item->SetRequiredVersionOperator(ModPackageProject::StringToOperator(operatorString, false, requirements.ms_DefaultVersionOperator));
					
					// Description
					item->SetDescription(itemNode.GetFirstChildElement("Description").GetValue());
					
					// Conform
					item->ConformToType();
				}
			}
		}
	}
	void NativeSerializer::ReadComponents()
	{
		ComponentsSection& components = m_ProjectLoad->GetComponents();
	
		if (kxf::XMLNode componentsNode = m_XML.QueryElement("Package/Components"))
		{
			// Read required files
			LoadStringArray(components.GetRequiredFileData(), componentsNode.GetFirstChildElement("RequiredFiles"));
			
			// Read steps
			for (kxf::XMLNode stepNode = componentsNode.GetFirstChildElement("Steps").GetFirstChildElement(); stepNode.IsOK(); stepNode = stepNode.GetNextSiblingElement())
			{
				auto& step = components.GetSteps().emplace_back(std::make_unique<ComponentStep>());
				step->SetName(stepNode.GetAttribute("Name"));

				kxf::XMLNode stepConditionGroupNode = stepNode.GetFirstChildElement("ConditionGroup");
				if (!stepConditionGroupNode)
				{
					stepConditionGroupNode = stepNode.GetFirstChildElement("Conditions");
				}
				ReadConditionGroup(step->GetConditionGroup(), stepConditionGroupNode);
				
				for (kxf::XMLNode groupNode = stepNode.GetFirstChildElement("Groups").GetFirstChildElement(); groupNode.IsOK(); groupNode = groupNode.GetNextSiblingElement())
				{
					auto& group = step->GetGroups().emplace_back(std::make_unique<ComponentGroup>());
					group->SetName(groupNode.GetAttribute("Name"));
					group->SetSelectionMode(components.StringToSelectionMode(groupNode.GetAttribute("SelectionMode")));
	
					kxf::XMLNode groupArrayNode = groupNode.GetFirstChildElement("Items");
					if (!groupArrayNode.IsOK())
					{
						// v1.3.1-
						groupArrayNode = groupNode.GetFirstChildElement("Entries");
					}
					for (kxf::XMLNode itemNode = groupArrayNode.GetFirstChildElement(); itemNode.IsOK(); itemNode = itemNode.GetNextSiblingElement())
					{
						auto& item = group->GetItems().emplace_back(std::make_unique<ComponentItem>());
						item->SetName(itemNode.GetFirstChildElement("Name").GetValue());
						item->SetImage(itemNode.GetFirstChildElement("Image").GetAttribute("Path"));
						item->SetDescription(itemNode.GetFirstChildElement("Description").GetValue());
	
						// Type descriptor
						kxf::XMLNode typeDescriptorNode = itemNode.GetFirstChildElement("TypeDescriptor");
						item->SetTDDefaultValue(components.StringToTypeDescriptor(typeDescriptorNode.GetAttribute("DefaultValue")));
						item->SetTDConditionalValue(components.StringToTypeDescriptor(typeDescriptorNode.GetAttribute("ConditionalValue"), TypeDescriptor::Invalid));
						
						if (m_ProjectLoad->GetFormatVersion() < kxf::Version("1.3"))
						{
							if (kxf::XMLNode conditionsNode = typeDescriptorNode.GetFirstChildElement("Conditions"))
							{
								ConditionGroup& conditionGroup = item->GetTDConditionGroup();
								Condition& condition = conditionGroup.GetOrCreateFirstCondition();
								ReadCondition(condition, conditionsNode);
								
								conditionGroup.SetOperator(Operator::And);
								condition.SetOperator(Operator::And);
							}
						}
						else
						{
							kxf::XMLNode itemConditionGroupNode = typeDescriptorNode.GetFirstChildElement("ConditionGroup");
							if (!itemConditionGroupNode)
							{
								itemConditionGroupNode = typeDescriptorNode.GetFirstChildElement("Conditions");
							}
							ReadConditionGroup(item->GetTDConditionGroup(), itemConditionGroupNode);
						}
						
						// If condition list is empty and type descriptor values are equal, clear 'ConditionalValue'
						if (!item->GetTDConditionGroup().HasConditions() && item->GetTDDefaultValue() == item->GetTDConditionalValue())
						{
							item->SetTDConditionalValue(TypeDescriptor::Invalid);
						}
	
						LoadStringArray(item->GetFileData(), itemNode.GetFirstChildElement("Files"));
						LoadStringArray(item->GetRequirements(), itemNode.GetFirstChildElement("Requirements"));
						
						// Conditional flags
						kxf::XMLNode conditionFlagsNode = itemNode.GetFirstChildElement("ConditionFlags");
						if (!conditionFlagsNode)
						{
							conditionFlagsNode = itemNode.GetFirstChildElement("ConditionalFlags");
						}
						if (!conditionFlagsNode)
						{
							conditionFlagsNode = itemNode.GetFirstChildElement("AssignedFlags");
						}
						ReadCondition(item->GetConditionFlags(), conditionFlagsNode);
					}
				}
			}
	
			auto ReadConditionalSteps = [&componentsNode, &components](const kxf::String& rootNodeName, const kxf::String& nodeName)
			{
				for (kxf::XMLNode stepNode = componentsNode.GetFirstChildElement(rootNodeName).GetFirstChildElement(); stepNode.IsOK(); stepNode = stepNode.GetNextSiblingElement())
				{
					auto& step = components.GetConditionalSteps().emplace_back(std::make_unique<ConditionalComponentStep>());

					kxf::XMLNode stepConditionGroupNode = stepNode.GetFirstChildElement("ConditionGroup");
					if (!stepConditionGroupNode)
					{
						stepConditionGroupNode = stepNode.GetFirstChildElement("Conditions");
					}
					ReadConditionGroup(step->GetConditionGroup(), stepConditionGroupNode);

					LoadStringArray(step->GetItems(), stepNode.GetFirstChildElement(nodeName));
				}
			};
			ReadConditionalSteps("ConditionalSteps", "Files");
		}
	}
	
	kxf::XMLNode NativeSerializer::WriteBase()
	{
		kxf::XMLNode baseNode = m_XML.NewElement("Package");
		baseNode.SetAttribute("FormatVersion", ModPackagesModule::GetInstance()->GetModuleInfo().GetVersion());

		if (m_ProjectSave->GetModID() != m_ProjectSave->GetInfo().GetName())
		{
			baseNode.SetAttribute("ID", m_ProjectSave->GetModID(), WriteEmpty::Never);
		}
		
		// Install engine version
		kxf::XMLNode targetProfileNode = baseNode.NewElement("TargetProfile");
		targetProfileNode.SetAttribute("ID", IGameInstance::GetActive()->GetGameID());
	
		return baseNode;
	}
	void NativeSerializer::WriteConfig(kxf::XMLNode& baseNode)
	{
		if (m_AsProject)
		{
			kxf::XMLNode configNode = baseNode.NewElement("PackageConfig");
			const ConfigSection& config = m_ProjectSave->GetConfig();
	
			configNode.NewElement("InstallPackageFile").SetValue(config.GetInstallPackageFile(), WriteEmpty::Never);
			configNode.NewElement("CompressionMethod").SetValue(config.GetCompressionMethod(), WriteEmpty::Never);
			configNode.NewElement("CompressionLevel").SetValue(config.GetCompressionLevel());
			configNode.NewElement("CompressionDictionarySize").SetValue(config.GetCompressionDictionarySize());
			configNode.NewElement("CompressionUseMultithreading").SetValue(config.IsMultithreadingUsed());
			configNode.NewElement("CompressionSolidArchive").SetValue(config.IsSolidArchive());
		}
	}
	void NativeSerializer::WriteInfo(kxf::XMLNode& baseNode)
	{
		kxf::XMLNode infoNode = baseNode.NewElement("Info");
		const InfoSection& info = m_ProjectSave->GetInfo();
	
		// Basic info
		infoNode.NewElement("Name").SetValue(info.GetName());
		infoNode.NewElement("Version").SetValue(info.GetVersion());
		infoNode.NewElement("Author").SetValue(info.GetAuthor());

		if (!info.GetTranslator().IsEmpty())
		{
			infoNode.NewElement("Translator").SetValue(info.GetTranslator());
		}

		if (!info.GetTranslatedName().IsEmpty())
		{
			infoNode.NewElement("TranslatedName").SetValue(info.GetTranslatedName());
		}

		infoNode.NewElement("Description").SetValue(info.GetDescription());

		// Custom info
		if (!info.GetCustomFields().empty())
		{
			SaveLabeledValueArray(info.GetCustomFields(), infoNode.NewElement("Custom"));
		}
	
		// Source
		kxf::XMLNode providerNode = infoNode.NewElement("Source");
		info.GetModSourceStore().Save(providerNode);
	
		// Documents
		if (!info.GetDocuments().empty())
		{
			WriteLabeledValueArray(info.GetDocuments(), infoNode.NewElement("Documents"), [this](const Utility::LabeledValue& value)
			{
				return m_AsProject ? value.GetValue() : PathNameToPackage(value.GetValue(), ContentType::Documents);
			});
		}
	
		// Tags
		const ModTagStore& tagStore = info.GetTagStore();
		if (!tagStore.IsEmpty())
		{
			kxf::XMLNode tagsNode = infoNode.NewElement("Tags");
			tagStore.Visit([&tagsNode](const IModTag& tag)
			{
				tagsNode.NewElement("Item").SetValue(tag.GetID());
				return true;
			});
		}
	}
	void NativeSerializer::WriteInterface(kxf::XMLNode& baseNode)
	{
		const InterfaceSection& interfaceConfig = m_ProjectSave->GetInterface();
		const TitleConfig& titleConfig = interfaceConfig.GetTitleConfig();
		kxf::XMLNode interfaceNode = baseNode.NewElement("Interface");
	
		// Write customization
		if (titleConfig.IsOK())
		{
			kxf::XMLNode node = interfaceNode.NewElement("Caption");
			if (titleConfig.HasAlignment())
			{
				node.SetAttribute("Alignment", (int)titleConfig.GetAlignment());
			}
			if (titleConfig.HasColor())
			{
				node.SetAttribute("Color", (int)titleConfig.GetColor().GetRGBA());
			}
		}
	
		// Write special images config
		auto WriteImageConfig = [this](kxf::XMLNode& rootNode, const kxf::String& name, const ImageItem* item, bool isListItem)
		{
			if (item)
			{
				if (!isListItem || item->HasPath())
				{
					kxf::XMLNode node = rootNode.NewElement(name);
					node.SetAttribute("Path", m_AsProject ? item->GetPath() : PathNameToPackage(item->GetPath(), ContentType::Images));
					node.SetAttribute("Visible", item->IsVisible());
	
					if (isListItem)
					{
						if (item->HasDescription())
						{
							node.NewElement("Description").SetValue(item->GetDescriptionRaw());
						}
					}
					else
					{
						// What I wanted to do with that?
						//node.SetAttribute("Width", item->GetSize().GetWidth());
						//node.SetAttribute("Height", item->GetSize().GetHeight());
					}
				}
			}
		};
		WriteImageConfig(interfaceNode, "MainImage", interfaceConfig.GetMainItem(), false);
		WriteImageConfig(interfaceNode, "HeaderImage", interfaceConfig.GetHeaderItem(), false);
	
		// Write images list
		if (!interfaceConfig.GetImages().empty())
		{
			kxf::XMLNode imagesNode = interfaceNode.NewElement("Images");
			for (const ImageItem& item: interfaceConfig.GetImages())
			{
				WriteImageConfig(imagesNode, "Item", &item, true);
			}
		}
	}
	void NativeSerializer::WriteFiles(kxf::XMLNode& baseNode)
	{
		kxf::XMLNode fileDataNode = baseNode.NewElement("Files");
		const FileDataSection& fileData = m_ProjectSave->GetFileData();
	
		for (const auto& item: fileData.GetItems())
		{
			const FolderItem* folderItem = item->QueryInterface<FolderItem>();
			kxf::XMLNode itemNode = fileDataNode.NewElement(folderItem ? "Folder" : "File");

			if (m_AsProject)
			{
				if (item->GetID() != item->GetSource())
				{
					itemNode.SetAttribute("ID", item->GetID());
				}
				itemNode.SetAttribute("Source", PathNameToPackage(item->GetSource(), ContentType::FileData));
			}
			else
			{
				itemNode.SetAttribute("Source", item->GetID());
			}
			itemNode.SetAttribute("Destination", item->GetDestination());

			if (!item->IsDefaultPriority())
			{
				itemNode.SetAttribute("Priority", item->GetPriority());
			}

			if (m_AsProject && folderItem && !folderItem->GetFiles().empty())
			{
				for (const FileItem& folderFile: folderItem->GetFiles())
				{
					kxf::XMLNode node = itemNode.NewElement("Item");
					node.SetValue(folderFile.GetDestination());
					node.SetAttribute("Source", folderFile.GetSource());
				}
			}
		}
	}
	void NativeSerializer::WriteRequirements(kxf::XMLNode& baseNode)
	{
		const RequirementsSection& requirements = m_ProjectSave->GetRequirements();

		kxf::XMLNode requirementsNode = baseNode.NewElement("Requirements");
		if (!requirements.IsDefaultGroupEmpty())
		{
			SaveStringArray(requirements.GetDefaultGroup(), requirementsNode.NewElement("DefaultGroups"));
		}

		kxf::XMLNode groupsArrayNode = groupsArrayNode = requirementsNode.NewElement("Groups");
		for (const auto& group: requirements.GetGroups())
		{
			kxf::XMLNode groupNode = groupsArrayNode.NewElement("Group");
			groupNode.SetAttribute("ID", group->GetID());
			groupNode.SetAttribute("Operator", ModPackageProject::OperatorToString(group->GetOperator()));

			for (const auto& item: group->GetItems())
			{
				kxf::XMLNode itemNode = groupNode.NewElement("Item");

				// ID and type
				itemNode.SetAttribute("ID", item->RawGetID(), WriteEmpty::Never);
				itemNode.SetAttribute("Type", requirements.TypeDescriptorToString(item->GetType()));

				// Name
				itemNode.NewElement("Name").SetValue(item->RawGetName());

				// Object
				kxf::XMLNode objectNode = itemNode.NewElement("Object");
				objectNode.SetValue(item->GetObject());
				objectNode.SetAttribute("Function", requirements.ObjectFunctionToString(item->GetObjectFunction()));

				// Version
				kxf::XMLNode versionNode = itemNode.NewElement("Version");
				versionNode.SetValue(item->GetRequiredVersion());
				versionNode.SetAttribute("Operator", ModPackageProject::OperatorToString(item->GetRequiredVersionOperator()));

				// Description
				if (kxf::String value = item->GetDescription(); !value.IsEmpty())
				{
					itemNode.NewElement("Description").SetValue(value);
				}
			}
		}
	}
	void NativeSerializer::WriteComponents(kxf::XMLNode& baseNode)
	{
		kxf::XMLNode componentsNode = baseNode.NewElement("Components");
		const ComponentsSection& components = m_ProjectSave->GetComponents();
	
		// Write required files
		if (!components.GetRequiredFileData().empty())
		{
			SaveStringArray(components.GetRequiredFileData(), componentsNode.NewElement("RequiredFiles"));
		}
	
		// Write steps
		kxf::XMLNode stepsArrayNode = componentsNode.NewElement("Steps");
		for (const auto& step: components.GetSteps())
		{
			// Header
			kxf::XMLNode stepNode = stepsArrayNode.NewElement("Step");
			stepNode.SetAttribute("Name", step->GetName(), WriteEmpty::Never);

			// Step conditions
			if (step->GetConditionGroup().HasConditions())
			{
				WriteConditionGroup(step->GetConditionGroup(), stepNode.NewElement("ConditionGroup"));
			}

			// Groups
			if (!step->GetGroups().empty())
			{
				kxf::XMLNode groupsArrayNode = stepNode.NewElement("Groups");
				for (const auto& group: step->GetGroups())
				{
					kxf::XMLNode groupNode = groupsArrayNode.NewElement("Group");
					groupNode.SetAttribute("Name", group->GetName(), WriteEmpty::Never);
					groupNode.SetAttribute("SelectionMode", components.SelectionModeToString(group->GetSelectionMode()));

					// Group entries
					if (!group->GetItems().empty())
					{
						kxf::XMLNode itemArrayNode = groupNode.NewElement("Items");
						for (const auto& item: group->GetItems())
						{
							kxf::XMLNode itemNode = itemArrayNode.NewElement("Item");

							// Name is required
							itemNode.NewElement("Name").SetValue(item->GetName());

							// Image
							if (!item->GetImage().IsEmpty())
							{
								kxf::String image = m_AsProject ? item->GetImage() : PathNameToPackage(item->GetImage(), ContentType::Images);
								itemNode.NewElement("Image").SetAttribute("Path", image);
							}

							// Description
							if (!item->GetDescription().IsEmpty())
							{
								itemNode.NewElement("Description").SetValue(item->GetDescription());
							}

							// Type descriptor
							kxf::XMLNode typeDescriptorNode = itemNode.NewElement("TypeDescriptor");
							typeDescriptorNode.SetAttribute("DefaultValue", components.TypeDescriptorToString(item->GetTDDefaultValue()));
							if (item->GetTDConditionalValue() != TypeDescriptor::Invalid)
							{
								typeDescriptorNode.SetAttribute("ConditionalValue", components.TypeDescriptorToString(item->GetTDConditionalValue()));
							}

							if (item->GetTDConditionGroup().HasConditions())
							{
								WriteConditionGroup(item->GetTDConditionGroup(), typeDescriptorNode.NewElement("ConditionGroup"));
							}

							if (!item->GetFileData().empty())
							{
								SaveStringArray(item->GetFileData(), itemNode.NewElement("Files"));
							}

							if (!item->GetRequirements().empty())
							{
								SaveStringArray(item->GetRequirements(), itemNode.NewElement("Requirements"));
							}

							if (item->GetConditionFlags().HasFlags())
							{
								WriteCondition(item->GetConditionFlags(), itemNode.NewElement("ConditionFlags"), true);
							}
						}
					}
				}
			}
		}
	
		[&componentsNode](const ConditionalComponentStep::Vector& steps, const kxf::String& rootNodeName, const kxf::String& nodeName)
		{
			if (!steps.empty())
			{
				kxf::XMLNode stepArrayNode = componentsNode.NewElement(rootNodeName);
				for (const auto& step: steps)
				{
					// Header
					kxf::XMLNode setNode = stepArrayNode.NewElement("Step");
					if (step->GetConditionGroup().HasConditions())
					{
						WriteConditionGroup(step->GetConditionGroup(), setNode.NewElement("ConditionGroup"));
					}
	
					// Entries
					if (!step->GetItems().empty())
					{
						SaveStringArray(step->GetItems(), setNode.NewElement(nodeName));
					}
				}
			}
		}(components.GetConditionalSteps(), "ConditionalSteps", "Files");
	}
	
	void NativeSerializer::Serialize(const ModPackageProject& project)
	{
		m_ProjectSave = &project;
		m_XML = {};
	
		kxf::XMLNode baseNode = WriteBase();
		WriteConfig(baseNode);
		WriteInfo(baseNode);
		WriteFiles(baseNode);
		WriteInterface(baseNode);
		WriteRequirements(baseNode);
		WriteComponents(baseNode);
	
		m_Data = m_XML.Save();
	}
	void NativeSerializer::Structurize(ModPackageProject& project)
	{
		m_ProjectLoad = &project;
		m_XML.Load(m_Data);
	
		ReadBase();
		ReadConfig();
		ReadInfo();
		ReadFiles();
		ReadInterface();
		ReadRequirements();
		ReadComponents();
	}
}
