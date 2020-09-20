#include "stdafx.h"
#include "LegacySerializer.h"
#include "PackageProject/ModPackageProject.h"
#include "PackageProject/ConfigSection.h"
#include "PackageProject/InfoSection.h"
#include "PackageProject/InterfaceSection.h"
#include "PackageProject/FileDataSection.h"
#include "PackageProject/RequirementsSection.h"
#include "PackageProject/ComponentsSection.h"
#include "ModPackages/IPackageManager.h"
#include "GameInstance/IGameInstance.h"
#include <Kortex/Application.hpp>
#include <Kortex/ModTagManager.hpp>
#include "Utility/Common.h"
#include <kxf::UI::Framework/KxString.h>
#include <kxf::UI::Framework/KxShell.h>

namespace Kortex::PackageProject
{
	namespace
	{
		void LoadStringArray(KxStringVector& array, const kxf::XMLNode& arrayNode)
		{
			for (kxf::XMLNode node = arrayNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				array.emplace_back(node.GetValue());
			}
		}
		void ReadConditionGroup(ConditionGroup& conditionGroup, const kxf::XMLNode& flagsNode, bool isRequired)
		{
			for (kxf::XMLNode node = flagsNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				conditionGroup.GetOrCreateFirstCondition().GetFlags().emplace_back(node.GetValue(), node.GetAttribute("Name"));
			}
		}
	}
}

namespace Kortex::PackageProject
{
	kxf::String LegacySerializer::ConvertMultiLine(const kxf::String& source) const
	{
		// Convert terrible line separator used in 3.x versions to normal line separator
	
		kxf::String out = source;
		out.Replace("[NewLine]", "\r\n", true);
		return out;
	}
	kxf::String LegacySerializer::ConvertVariable(const kxf::String& sOldVariable) const
	{
		using namespace Kortex;
		using namespace Kortex::Variables;
	
		kxf::String oldVariableFixed = sOldVariable;
	
		// If variable is inside '%' symbols, remove them
		if (!oldVariableFixed.IsEmpty() && oldVariableFixed[0] == '%' && oldVariableFixed.Last() == '%')
		{
			oldVariableFixed.Remove(0, 1);
			oldVariableFixed.RemoveLast(1);
		}
	
		if (oldVariableFixed == "InstallPath" || oldVariableFixed == "Root")
		{
			return WrapAsInline(KVAR_VIRTUAL_GAME_DIR);
		}
	
		if (oldVariableFixed == "Data" || oldVariableFixed == "DataFilesPath")
		{
			const kxf::String& id = m_Project->GetTargetProfileID();
			if (id == "Morrowind")
			{
				return WrapAsInline(KVAR_VIRTUAL_GAME_DIR) + "\\Data Files";
			}
			else
			{
				return WrapAsInline(KVAR_VIRTUAL_GAME_DIR) + "\\Data";
			}
		}
	
		if (oldVariableFixed == "SettingsPath")
		{
			return WrapAsInline(KVAR_CONFIG_DIR);
		}
	
		auto AsShellVar = [](const kxf::String& name)
		{
			return WrapAsInline(name, NS::ShellFolder);
		};
	
		if (oldVariableFixed == "SavesPath")
		{
			return AsShellVar("SAVED_GAMES");
		}
		if (oldVariableFixed == "UserName")
		{
			return WrapAsInline("USERNAME", NS::Environment);
		}
		if (oldVariableFixed == "UserProfile")
		{
			return AsShellVar("USER_PROFILE");
		}
		if (oldVariableFixed == "WindowsFolder")
		{
			return AsShellVar("WINDOWS");
		}
		if (oldVariableFixed == "SystemDrive")
		{
			return AsShellVar("SYSTEMDRIVE");
		}
		if (oldVariableFixed == "Documents")
		{
			return AsShellVar("DOCUMENTS");
		}
		if (oldVariableFixed == "ProgramFiles")
		{
			return AsShellVar("PROGRAMFILES");
		}
		if (oldVariableFixed == "ProgramFilesX86")
		{
			return AsShellVar("PROGRAMFILES_X86");
		}
		if (oldVariableFixed == "ProgramFilesX64")
		{
			return AsShellVar("PROGRAMFILES_X64");
		}
	
		#undef SHVAR
		return wxEmptyString;
	}
	void LegacySerializer::AddSite(const kxf::String& url)
	{
		ModSourceStore& store = m_Project->GetInfo().GetModSourceStore();
	
		kxf::String siteName;
		ModSourceItem item = TryParseWebSite(url, &siteName);
		if (item.IsOK())
		{
			store.TryAddItem(std::move(item));
		}
		else
		{
			store.TryAddWith(siteName, url);
		}
	}
	void LegacySerializer::FixRequirementID(RequirementItem* entry) const
	{
		if (m_Project->GetTargetProfileID() == "Skyrim" || m_Project->GetTargetProfileID() == "SkyrimSE")
		{
			if (entry->GetID() == "DG" || entry->GetID() == "DB" || entry->GetID() == "HF" || entry->GetID() == "HRTP")
			{
				entry->SetID("DLC-" + entry->GetID());
			}
			else if (entry->GetID() == "DGD")
			{
				entry->SetID("DLC-DG");
			}
			else if (entry->GetID() == "SD")
			{
				entry->SetID("ScriptDragon");
			}
			else if (entry->GetID() == "UKSP")
			{
				entry->SetID("USKP");
			}
			else if (entry->GetID() == "SSL")
			{
				entry->SetID("SexLab");
			}
			else if (entry->GetID() == "SLA" || entry->GetID() == "SSL-Aroused")
			{
				entry->SetID("SexLab Aroused");
			}
			else if (entry->GetID() == "SLAL")
			{
				entry->SetID("SexLab Animation Loader");
			}
			else if (entry->GetID() == "ASH")
			{
				entry->SetID("Apachii SkyHair");
			}
			else if (entry->GetID() == "RM")
			{
				entry->SetID("RaceMenu");
			}
			else if (entry->GetID() == "AS-LAL")
			{
				entry->SetID("Alternate Start - Live Another Life");
			}
			else if (entry->GetID() == "NiO")
			{
				entry->SetID("NetImmerse Override");
			}
			else if (entry->GetID() == "HDT")
			{
				entry->SetID("HDT-HHS");
			}
			else if (entry->GetID() == "HHS")
			{
				entry->SetID("HDT-HHS");
			}
			else if (entry->GetID() == "ZaZ")
			{
				entry->SetID("ZAP");
			}
			else if (entry->GetID() == "ZaX")
			{
				entry->SetID("ZAX");
			}
			else if (entry->GetID() == "DD-A" || entry->GetID() == "Devious Devices - Assets")
			{
				entry->SetID("DDa");
			}
			else if (entry->GetID() == "DD-I" || entry->GetID() == "Devious Devices - Integration")
			{
				entry->SetID("DDi");
			}
			else if (entry->GetID() == "DD-X" || entry->GetID() == "Devious Devices - Extension")
			{
				entry->SetID("DDx");
			}
			else if (entry->GetID() == "DD-CDS")
			{
				entry->SetID("Captured Dreams Shop");
			}
		}
	}
	bool LegacySerializer::IsComponentsUsed() const
	{
		return m_XML.QueryElement("SetupInfo/Installer/Settings/ComponentsEnabled").GetValueBool() ||
			m_XML.QueryElement("SetupInfo/Installer/Settings/Components").GetValueBool() ||
			m_XML.QueryElement("SetupInfo/Installer/Settings/UseComponents").GetValueBool();
	}
	void LegacySerializer::ReadInterface3x4x5x(const kxf::String& sLogoNodeName)
	{
		InterfaceSection& interfaceConfig = m_Project->GetInterface();
	
		kxf::XMLNode interfaceNode = m_XML.QueryElement("SetupInfo/Installer/Interface");
		if (interfaceNode.IsOK())
		{
			// Attributes 'WindowTitle', 'WindowSubtitle', background image and colors no longer supported since 5.0.
	
			// SMI stores only file names. KMP stores full paths.
			const char* path = "SetupInfo\\Images\\";
	
			for (kxf::XMLNode node = interfaceNode.GetFirstChildElement("Images").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				interfaceConfig.GetImages().emplace_back(ImageItem(path + node.GetValue(), wxEmptyString, true));
			}
	
			// Set main image or if main image not set in this installer choose first available
			kxf::String sLogo = interfaceNode.GetFirstChildElement(sLogoNodeName).GetValue();
			if (!sLogo.IsEmpty())
			{
				interfaceConfig.SetMainImage(path + sLogo);
			}
			else if (!interfaceConfig.GetImages().empty())
			{
				interfaceConfig.SetMainImage(interfaceConfig.GetImages().front().GetPath());
			}
	
			// If main image not in the images list for some reason, add it there
			if (!interfaceConfig.GetMainItem())
			{
				interfaceConfig.GetImages().emplace_back(interfaceConfig.GetMainImage());
			}
		}
	}
	void LegacySerializer::ReadFiles3x4x()
	{
		kxf::XMLNode fileDataNode = m_XML.QueryElement("SetupInfo/Installer/Files");
		if (fileDataNode.IsOK())
		{
			bool bNestedStructure = fileDataNode.GetAttributeInt("StructureType", 0) == 0;
			FileDataSection& fileData = m_Project->GetFileData();
	
			for (kxf::XMLNode folderNode = fileDataNode.GetFirstChildElement(); folderNode.IsOK(); folderNode = folderNode.GetNextSiblingElement())
			{
				FolderItem& folderEntry = fileData.AddFolder(std::make_unique<FolderItem>());
				folderEntry.SetID(folderNode.GetAttribute("ID", folderNode.GetAttribute("Source")));
	
				// Source
				kxf::String source = folderNode.GetAttribute("Source", folderEntry.GetID());
				folderEntry.SetSource(bNestedStructure ? "SetupData\\" + source : source);
				
				// ExtractingPath == 0 -> Game root, ExtractingPath == 1 -> Data folder (0 is the default).
				// Boolean attribute 'Install' and enum 'OverwriteMode' are ignored as they was removed in KMP.
				int extractingPathType = folderNode.GetAttributeInt("ExtractingPath", 0);
				folderEntry.SetDestination(extractingPathType == 1 ? wxS("Data") : wxEmptyString);
	
				// Files list also ignored
			}
		}
	}
	
	kxf::Version LegacySerializer::ReadBase()
	{
		kxf::XMLNode baseNode = m_XML.QueryElement("SetupInfo");
		if (baseNode.IsOK())
		{
			kxf::XMLNode installerNode = baseNode.GetFirstChildElement("Installer");
			m_Project->SetModID(installerNode.GetAttribute("ID"));
	
			// Defaults to 'Skyrim' for compatibility with SKSM
			m_Project->SetTargetProfileID(installerNode.GetAttribute("Profile", "Skyrim"));
	
			return baseNode.GetAttribute("Version");
		}
		return KxNullVersion;
	}
	void LegacySerializer::ReadConfig()
	{
		kxf::XMLNode configNode = m_XML.QueryElement("SetupInfo/Installer/Settings");
		if (configNode.IsOK())
		{
			ConfigSection& config = m_Project->GetConfig();
	
			config.SetInstallPackageFile(configNode.GetFirstChildElement("InstallerPath").GetValue());
			config.SetCompressionMethod(configNode.GetFirstChildElement("CompressionMethod").GetValue());
			config.SetCompressionLevel(configNode.GetFirstChildElement("CompressionLevel").GetValueInt());
			config.SetCompressionDictionarySize(configNode.GetFirstChildElement("DictionarySize").GetValueInt());
			config.SetUseMultithreading(configNode.GetFirstChildElement("UseMultiThreading").GetValueBool());
			config.SetSolidArchive(configNode.GetFirstChildElement("Solid").GetValueBool());
		}
	}
	
	void LegacySerializer::ReadInfo3x()
	{
		InfoSection& info = m_Project->GetInfo();
	
		// Basic info
		kxf::XMLNode basicInfoNode = m_XML.QueryElement("SetupInfo/Installer/Info/Plugin");
		if (basicInfoNode.IsOK())
		{
			kxf::String name = basicInfoNode.GetFirstChildElement("Name").GetValue();
			kxf::String originalName = basicInfoNode.GetFirstChildElement("OriginalName").GetValue();
	
			if (originalName.IsEmpty())
			{
				info.SetName(name);
			}
			else
			{
				info.SetName(originalName);
				info.SetTranslatedName(name);
			}
	
			info.SetVersion(basicInfoNode.GetFirstChildElement("Version").GetValue());
			info.SetAuthor(basicInfoNode.GetFirstChildElement("Author").GetValue());
			info.SetTranslator(basicInfoNode.GetFirstChildElement("Localizer").GetValue());
			info.SetDescription(ConvertMultiLine(basicInfoNode.GetFirstChildElement("Description").GetValue()));
	
			// Copyrights field no longer supported, but it still can be saved
			kxf::String copyrights = ConvertMultiLine(basicInfoNode.GetFirstChildElement("Copyrights").GetValue());
			if (!copyrights.IsEmpty())
			{
				copyrights.Replace("\r\n", "; ", true);
				info.GetCustomFields().emplace_back(copyrights, "Copyrights");
			}
	
			// This version support inclusion of one PDF file
			// There are attribute 'Included' in 'PDFDocument' node, but it was redundant even then.
			kxf::String documentPDF = basicInfoNode.GetFirstChildElement("PDFDocument").GetValue();
			if (!documentPDF.IsEmpty())
			{
				info.GetDocuments().emplace_back("SetupInfo\\" + documentPDF, documentPDF.AfterLast('.'));
			}
	
			// Main site
			AddSite(basicInfoNode.GetFirstChildElement("URL").GetValue());
	
			// URL for discussion site (almost always empty)
			kxf::String discussion = basicInfoNode.GetFirstChildElement("Discussion").GetValue();
			if (!discussion.IsEmpty())
			{
				info.GetModSourceStore().AssignWith("Discussion", discussion);
			}
		}
	
		// Custom info have slightly different format
		for (kxf::XMLNode node = m_XML.QueryElement("SetupInfo/Installer/Info/CustomFields").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			info.GetCustomFields().emplace_back(node.GetAttribute("Value"), node.GetAttribute("Name"));
		}
	}
	void LegacySerializer::ReadInterface3x()
	{
		ReadInterface3x4x5x("InstallerLogo");
	}
	void LegacySerializer::ReadFiles3x()
	{
		ReadFiles3x4x();
	}
	void LegacySerializer::ReadRequirements3x()
	{
		RequirementsSection& requirements = m_Project->GetRequirements();
	
		kxf::XMLNode requirementsNode = m_XML.QueryElement("SetupInfo/Installer/Dependencies");
		if (requirementsNode.IsOK())
		{
			RequirementGroup* requirementGroup = requirements.GetGroups().emplace_back(std::make_unique<RequirementGroup>()).get();
			requirementGroup->SetID("Main");
			requirements.GetDefaultGroup().push_back(requirementGroup->GetID());
	
			// In this version there is only one group with multiple requirements
			for (kxf::XMLNode node = requirementsNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				if (node.HasChildren())
				{
					RequirementItem* entry = requirementGroup->GetItems().emplace_back(std::make_unique<RequirementItem>()).get();
					entry->SetID(node.GetValue());
					entry->SetObjectFunction(ObjectFunction::FileExist);
					entry->SetRequiredVersion(node.GetAttribute("RequiredVersion"));
					entry->SetDescription(node.GetAttribute("Comment"));
	
					// I may be able to fix some IDs
					FixRequirementID(entry);
	
					// This will likely work, since 3.x not support user requirements,
					// but since then system ID's may have changed. 
					entry->SetType(ReqType::System);
	
					// Bool attribute 'Necessary' is ignored
				}
			}
		}
	}
	void LegacySerializer::ReadComponents3x()
	{
		ComponentsSection& components = m_Project->GetComponents();
	
		kxf::XMLNode componentsNode = m_XML.QueryElement("SetupInfo/Installer/Components");
		if (componentsNode.IsOK() && componentsNode.HasChildren())
		{
			// Ignore bool attribute 'Use' as it almost always just reflect existence of components array
	
			// Version 3.x supports only one step and one group
			ComponentStep* step = components.GetSteps().emplace_back(std::make_unique<ComponentStep>()).get();
			step->SetName("Select options");
	
			ComponentGroup* group = step->GetGroups().emplace_back(std::make_unique<ComponentGroup>()).get();
			group->SetName("Options");
			group->SetSelectionMode(SelectionMode::Any);
	
			for (kxf::XMLNode entryNode = componentsNode.GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
			{
				ComponentItem* entry = group->GetItems().emplace_back(std::make_unique<ComponentItem>()).get();
				entry->SetName(entryNode.GetAttribute("Name"));
				entry->SetDescription(entryNode.GetAttribute("Description"));
				entry->SetTDDefaultValue(entryNode.GetAttributeBool("Main") ? TypeDescriptor::Recommended : TypeDescriptor::Optional);
				
				kxf::String folder = entryNode.GetAttribute("Folder");
				if (!folder.IsEmpty())
				{
					entry->GetFileData().emplace_back();
				}
	
				kxf::String image = entryNode.GetAttribute("Image");
				if (!image.IsEmpty() && image != "---")
				{
					entry->SetImage("SetupInfo\\Images\\" + image);
				}
	
				// Ignore 'Index' attribute. Starting from 5.0 entries displayed as list, not as tree.
	
				// It's possible to analyze attributes 'Required' and 'Incompatible' (semicolon separated array of entries ID),
				// to get selection mode for this group, but it just not worth it. Just use 'SelectAny' mode.
			}
		}
	}
	
	void LegacySerializer::ReadInfo4x()
	{
		InfoSection& info = m_Project->GetInfo();
	
		// Basic info
		// Word 'Standart' is not a typo here, but typo in 4.x versions
		kxf::XMLNode basicInfoNode = m_XML.QueryElement("SetupInfo/Installer/Info/Standart");
		if (basicInfoNode.IsOK())
		{
			info.SetName(basicInfoNode.GetFirstChildElement("Name").GetValue());
			info.SetTranslatedName(basicInfoNode.GetFirstChildElement("LocalName").GetValue());
			info.SetVersion(basicInfoNode.GetFirstChildElement("Version").GetValue());
			info.SetAuthor(basicInfoNode.GetFirstChildElement("Author").GetValue());
			info.SetTranslator(basicInfoNode.GetFirstChildElement("Localizer").GetValue());
			info.SetDescription(basicInfoNode.GetFirstChildElement("Description").GetValue());
	
			// Copyrights field no longer supported, but it still can be saved
			kxf::String copyrights = basicInfoNode.GetFirstChildElement("Copyrights").GetValue();
			if (!copyrights.IsEmpty())
			{
				copyrights.Replace("\r\n", "; ", true);
				info.GetCustomFields().emplace_back(copyrights, "Copyrights");
			}
	
			// Main site
			AddSite(basicInfoNode.GetFirstChildElement("URL").GetValue());
	
			// URL for discussion site (almost always empty)
			kxf::String discussion = basicInfoNode.GetFirstChildElement("Discussion").GetValue();
			if (!discussion.IsEmpty())
			{
				#if 0
				info.GetWebSites().emplace_back(discussion, "Discussion");
				#endif
			}
	
			// An ID of '---' means no category
			kxf::String category = basicInfoNode.GetFirstChildElement("Category").GetValue();
			if (!category.IsEmpty() && category != "---" && CheckTag(category))
			{
				info.GetTagStore().AddTag(ModTagManager::DefaultTag(category));
			}
		}
	
		// Custom info have very different (but perfectly compatible) format
		for (kxf::XMLNode node = m_XML.QueryElement("SetupInfo/Installer/Info/Custom").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			info.GetCustomFields().emplace_back(node.GetFirstChildElement("Value").GetValue(), node.GetFirstChildElement("Name").GetValue());
		}
	
		// Documents
		for (kxf::XMLNode node = m_XML.QueryElement("SetupInfo/Installer/Info/Documents/Files").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			// Attributes 'Index' and 'Show' are ignored.
			info.GetDocuments().emplace_back("SetupInfo\\Documents\\" + node.GetValue(), node.GetAttribute("Name"));
		}
	}
	void LegacySerializer::ReadInterface4x()
	{
		ReadInterface3x4x5x("Logo");
	}
	void LegacySerializer::ReadFiles4x()
	{
		ReadFiles3x4x();
	}
	void LegacySerializer::ReadRequirements4x()
	{
		RequirementsSection& requirements = m_Project->GetRequirements();
	
		kxf::XMLNode requirementsNode = m_XML.QueryElement("SetupInfo/Installer/Requirements");
		if (requirementsNode.IsOK())
		{
			// This means use set with ID '---' as main requirements group.
			// Introduced in version 4.3
			bool setMainGroup = requirementsNode.GetAttributeBool("CommonRequirements", true) || requirementsNode.GetAttributeBool("ShowCommonRequirements", true);
	
			for (kxf::XMLNode setNode = requirementsNode.GetFirstChildElement(); setNode.IsOK(); setNode = setNode.GetNextSiblingElement())
			{
				if (setNode.HasChildren())
				{
					RequirementGroup* requirementGroup = requirements.GetGroups().emplace_back(std::make_unique<RequirementGroup>()).get();
					requirementGroup->SetID(setNode.GetAttribute("ID"));
	
					// ID of '---' denotes main set
					if (setMainGroup && (requirementGroup->GetID() == "---" || requirementGroup->GetID() == "Default"|| requirementGroup->GetID() == "Main"))
					{
						requirements.GetDefaultGroup().push_back(requirementGroup->GetID());
					}
	
					for (kxf::XMLNode entryNode = setNode.GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
					{
						if (entryNode.HasChildren())
						{
							RequirementItem* entry = requirementGroup->GetItems().emplace_back(std::make_unique<RequirementItem>()).get();
							entry->SetID(entryNode.GetAttribute("ID"));
							entry->SetName(entryNode.GetFirstChildElement("Name").GetValue());
							entry->SetRequiredVersion(entryNode.GetFirstChildElement("RequiredVersion").GetValue());
							entry->SetDescription(entryNode.GetFirstChildElement("Comment").GetValue());
	
							// Version 4.x supports custom requirements only with this type (checking mod ID from install log)
							entry->SetObjectFunction(ObjectFunction::ModActive);
	
							// I may be able to fix some IDs
							FixRequirementID(entry);
	
							// Version 4.x supports user requirements. They are denoted by bool attribute 'Standart' (typo in this version again).
							// But it's better to ignore this flag, as system ID's have changed and list of system requirements have been extended.
							entry->SetType(ReqType::System);
						}
					}
				}
			}
		}
	}
	void LegacySerializer::ReadComponents4x()
	{
		if (IsComponentsUsed())
		{
			// For additional info see 'ReadComponents3x' function
			ComponentsSection& components = m_Project->GetComponents();
	
			kxf::XMLNode componentsNode = m_XML.QueryElement("SetupInfo/Installer/Components");
			if (componentsNode.IsOK())
			{
				auto ReadEntriesArray = [&components](ComponentGroup* group, const kxf::XMLNode& groupNode)
				{
					for (kxf::XMLNode entryNode = groupNode.GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
					{
						ComponentItem* entry = group->GetItems().emplace_back(std::make_unique<ComponentItem>()).get();
						entry->SetName(Utility::String::StrOr(entryNode.GetFirstChildElement("Name").GetValue(), entryNode.GetAttribute("ID")));
						if (entry->GetName() == "---")
						{
							entry->SetName("Options");
						}
	
						entry->SetDescription(entryNode.GetFirstChildElement("Description").GetValue());
						entry->SetTDDefaultValue(entryNode.GetAttributeBool("Checked") ? TypeDescriptor::Recommended : TypeDescriptor::Optional);
	
						kxf::String folder = entryNode.GetFirstChildElement("Folder").GetValue();
						if (!folder.IsEmpty())
						{
							entry->GetFileData().emplace_back(folder);
						}
	
						kxf::String reqSet = entryNode.GetFirstChildElement("RequirementsSet").GetValue();
						if (!reqSet.IsEmpty())
						{
							entry->GetRequirements().emplace_back(reqSet);
						}
	
						kxf::String image = entryNode.GetFirstChildElement("Image").GetValue();
						if (!image.IsEmpty() && image != "---")
						{
							entry->SetImage("SetupInfo\\Images\\" + image);
						}
					}
				};
	
				// Versions before 4.3 supports only one group
				if (m_ProjectVersion < kxf::Version("4.3"))
				{
					ComponentStep* step = components.GetSteps().emplace_back(std::make_unique<ComponentStep>()).get();
					step->SetName("Select options");
	
					ComponentGroup* group = step->GetGroups().emplace_back(std::make_unique<ComponentGroup>()).get();
					group->SetName("Options");
	
					ReadEntriesArray(group, componentsNode);
				}
				else
				{
					// Required files
					components.GetRequiredFileData() = KxString::Split(componentsNode.GetAttribute("PreDefined"), ";");
	
					if (componentsNode.HasChildren())
					{
						ComponentStep* step = components.GetSteps().emplace_back(std::make_unique<ComponentStep>()).get();
						step->SetName("Select options");
	
						for (kxf::XMLNode groupNode = componentsNode.GetFirstChildElement(); groupNode.IsOK(); groupNode = groupNode.GetNextSiblingElement())
						{
							ComponentGroup* group = step->GetGroups().emplace_back(std::make_unique<ComponentGroup>()).get();
							group->SetName(Utility::String::StrOr(groupNode.GetFirstChildElement("Name").GetValue(), groupNode.GetAttribute("ID")));
							group->SetSelectionMode(components.StringToSelectionMode(groupNode.GetAttribute("SelectionMode")));
	
							ReadEntriesArray(group, groupNode);
						}
					}
				}
			}
		}
	}
	
	void LegacySerializer::ReadInfo5x()
	{
		InfoSection& info = m_Project->GetInfo();
	
		// Basic info
		// Seems like this variant somehow get into 5.x version
		kxf::XMLNode basicInfoNode = m_XML.QueryElement("SetupInfo/Installer/Info/Standart");
		if (!basicInfoNode.IsOK())
		{
			basicInfoNode = m_XML.QueryElement("SetupInfo/Installer/Info");
		}
	
		if (basicInfoNode.IsOK())
		{
			info.SetName(basicInfoNode.GetFirstChildElement("Name").GetValue());
			info.SetTranslatedName(basicInfoNode.GetFirstChildElement("LocalName").GetValue());
			info.SetVersion(basicInfoNode.GetFirstChildElement("Version").GetValue());
			info.SetAuthor(basicInfoNode.GetFirstChildElement("Author").GetValue());
			info.SetTranslator(basicInfoNode.GetFirstChildElement("Localizer").GetValue());
			info.SetDescription(basicInfoNode.GetFirstChildElement("Description").GetValue());
	
			// Copyrights field no longer supported, but it still can be saved
			kxf::String copyrights = basicInfoNode.GetFirstChildElement("Copyrights").GetValue();
			if (!copyrights.IsEmpty())
			{
				copyrights.Replace("\r\n", "; ", true);
				info.GetCustomFields().emplace_back(copyrights, "Copyrights");
			}
	
			// Main site
			AddSite(basicInfoNode.GetFirstChildElement("URL").GetValue());
			AddSite(basicInfoNode.GetFirstChildElement("OriginalURL").GetValue());
	
			// URL for discussion site (almost always empty)
			kxf::String discussion = basicInfoNode.GetFirstChildElement("Discussion").GetValue();
			if (!discussion.IsEmpty())
			{
				#if 0
				info.GetWebSites().emplace_back(discussion, "Discussion");
				#endif
			}
	
			// An ID of '---' means no category
			kxf::String category = basicInfoNode.GetFirstChildElement("Category").GetValue();
			if (!category.IsEmpty() && category != "---" && CheckTag(category))
			{
				info.GetTagStore().AddTag(ModTagManager::DefaultTag(category));
			}
		}
	
		// Custom info have very different (but perfectly compatible) format
		for (kxf::XMLNode node = m_XML.QueryElement("SetupInfo/Installer/Info/Custom").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			info.GetCustomFields().emplace_back(node.GetFirstChildElement("Value").GetValue(), node.GetFirstChildElement("Name").GetValue());
		}
	
		// Documents
		for (kxf::XMLNode node = m_XML.QueryElement("SetupInfo/Installer/Info/Documents").GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			// Attributes 'Index' and 'Show' are ignored.
			info.GetDocuments().emplace_back("SetupInfo\\Documents\\" + node.GetValue(), node.GetAttribute("Name"));
		}
	}
	void LegacySerializer::ReadInterface5x()
	{
		ReadInterface3x4x5x("Logo");
	}
	void LegacySerializer::ReadFiles5x()
	{
		kxf::XMLNode fileDataNode = m_XML.QueryElement("SetupInfo/Installer/Files");
		if (fileDataNode.IsOK())
		{
			FileDataSection& fileData = m_Project->GetFileData();
			bool isNestedStructure = fileDataNode.GetAttribute("StructureType") == "Nested";
	
			// Folder
			for (kxf::XMLNode entryNode = fileDataNode.GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
			{
				FileItem& item = entryNode.GetName() == "Folder" ? fileData.AddFolder(std::make_unique<FolderItem>()) : fileData.AddFile(std::make_unique<FileItem>());

				item.SetID(entryNode.GetAttribute("ID", entryNode.GetAttribute("Source")));
				item.SetDestination(entryNode.GetAttribute("Destination"));
				
				kxf::String source = entryNode.GetAttribute("Source", item.GetID());
				item.SetSource(isNestedStructure ? "SetupData\\" + source : source);
			}
	
			// Versions 5.x supports file entries inside 'Files' list,
			// but KMM 1.x doesn't expose any interface to create them.
			// So there is no need to convert them form SMI as they can only get to AMI from FOMod converter
			// or if XML has been modified manually (but no one did this to SKSM and KMM 1.x formats).
		}
	}
	void LegacySerializer::ReadRequirements5x()
	{
		RequirementsSection& requirements = m_Project->GetRequirements();
	
		kxf::XMLNode requirementsNode = m_XML.QueryElement("SetupInfo/Installer/Requirements");
		if (requirementsNode.IsOK())
		{
			bool showCommonRequirements = requirementsNode.GetAttributeBool("CommonRequirements", true) || requirementsNode.GetAttributeBool("ShowCommonRequirements", true);
	
			for (kxf::XMLNode setsNode = requirementsNode.GetFirstChildElement(); setsNode.IsOK(); setsNode = setsNode.GetNextSiblingElement())
			{
				if (setsNode.HasChildren())
				{
					RequirementGroup* group = requirements.GetGroups().emplace_back(std::make_unique<RequirementGroup>()).get();
					group->SetID(setsNode.GetAttribute("ID"));
					group->SetOperator(setsNode.GetAttribute("Operator") == "Or" ? Operator::Or : Operator::And);
	
					// In 5.x default set have ID 'Default'. What a coincidence.
					if (showCommonRequirements && (group->GetID() == "---" || group->GetID() == "Default"|| group->GetID() == "Main"))
					{
						requirements.GetDefaultGroup().push_back(group->GetID());
					}
	
					for (kxf::XMLNode entryNode = setsNode.GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
					{
						if (entryNode.HasChildren())
						{
							RequirementItem* entry = group->GetItems().emplace_back(std::make_unique<RequirementItem>()).get();
							entry->SetID(entryNode.GetFirstChildElement("ID").GetValue());
							entry->SetName(entryNode.GetFirstChildElement("Name").GetValue());
							entry->SetRequiredVersion(entryNode.GetFirstChildElement("RequiredVersion").GetValue());
							entry->SetDescription(entryNode.GetFirstChildElement("Comment").GetValue());
	
							// File path
							kxf::String path = entryNode.GetFirstChildElement("Path").GetValue();
							if (Utility::SingleFileExtensionMatches(path, "esp") || Utility::SingleFileExtensionMatches(path, "esm"))
							{
								entry->SetObject(path);
							}
							else if (!path.IsEmpty())
							{
								kxf::String sVariable = ConvertVariable(entryNode.GetFirstChildElement("Variable").GetValue());
								entry->SetObject(sVariable + '\\' + path);
							}
	
							if (entry->IsEmptyName())
							{
								entry->SetName(path.AfterLast('\\').BeforeLast('.'));
							}
	
							// Required state
							kxf::String state = entryNode.GetFirstChildElement("State").GetValue();
							ObjectFunction objectFunction = ObjectFunction::Invalid;
							if (state == "Active")
							{
								objectFunction = ObjectFunction::PluginActive;
							}
							else if (state == "Inactive")
							{
								objectFunction = ObjectFunction::PluginInactive;
							}
							else if (state == "Present")
							{
								objectFunction = ObjectFunction::FileExist;
							}
							else if (state == "Missing")
							{
								objectFunction = ObjectFunction::FileNotExist;
							}
							else if (state == "Installed")
							{
								objectFunction = ObjectFunction::ModActive;
							}
							// Version 5.x have no 'NotInstalled' state to match to 'KPPR_STATE_MOD_INACTIVE' from KMP.
	
							entry->SetObjectFunction(objectFunction != ObjectFunction::Invalid ? objectFunction : ObjectFunction::ModActive);
	
							// Operator
							kxf::String operatorRVName = entryNode.GetFirstChildElement("Operator").GetValue();
							Operator operatorRVType = Operator::Invalid;
							if (operatorRVName == "==")
							{
								operatorRVType = Operator::Equal;
							}
							else if (operatorRVName == "!=")
							{
								operatorRVType = Operator::NotEqual;
							}
							else if (operatorRVName == ">=")
							{
								operatorRVType = Operator::GreaterThanOrEqual;
							}
							else if (operatorRVName == ">")
							{
								operatorRVType = Operator::GreaterThan;
							}
							else if (operatorRVName == "<=")
							{
								operatorRVType = Operator::LessThanOrEqual;
							}
							else if (operatorRVName == "<")
							{
								operatorRVType = Operator::LessThan;
							}
	
							if (operatorRVType != Operator::Invalid)
							{
								entry->SetRequiredVersionOperator(operatorRVType);
							}
	
							FixRequirementID(entry);
							entry->SetType(ReqType::System);
						}
					}
				}
			}
		}
	}
	void LegacySerializer::ReadComponents5x()
	{
		if (IsComponentsUsed())
		{
			ComponentsSection& components = m_Project->GetComponents();
	
			auto ReadFlagsArray = [](Condition& conditions, const kxf::XMLNode& flagsNode)
			{
				for (kxf::XMLNode node = flagsNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
				{
					conditions.GetFlags().emplace_back(node.GetAttribute("Value"), node.GetValue());
				}
			};
	
			kxf::XMLNode componentsNode = m_XML.QueryElement("SetupInfo/Installer/Components");
			if (componentsNode.IsOK())
			{
				// Read required files
				LoadStringArray(components.GetRequiredFileData(), componentsNode.GetFirstChildElement("RequiredFiles"));
	
				// Read sets
				std::vector<std::pair<kxf::String, kxf::XMLNode>> groupsIDArray;
	
				for (kxf::XMLNode groupNode = componentsNode.GetFirstChildElement("Sets").GetFirstChildElement(); groupNode.IsOK(); groupNode = groupNode.GetNextSiblingElement())
				{
					groupsIDArray.emplace_back(groupNode.GetAttribute("ID"), groupNode);
				}
	
				auto ReadGroup = [&components, &ReadFlagsArray](const kxf::XMLNode& groupNode) -> ComponentGroup*
				{
					ComponentGroup* group = new ComponentGroup();
	
					/* Attributes. ID not used now */
					group->SetName(groupNode.GetAttribute("Name"));
					group->SetSelectionMode(components.StringToSelectionMode(groupNode.GetAttribute("SelectionMode")));
	
					// Required flags for group no longer supported, so skip it
	
					/* Entries */
					for (kxf::XMLNode entryNode = groupNode.GetFirstChildElement("Data").GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
					{
						ComponentItem* entry = group->GetItems().emplace_back(std::make_unique<ComponentItem>()).get();
						entry->SetName(entryNode.GetFirstChildElement("Name").GetValue());
						entry->SetDescription(entryNode.GetFirstChildElement("Description").GetValue());
	
						kxf::String image = entryNode.GetFirstChildElement("Image").GetValue();
						if (!image.IsEmpty() && image != "---")
						{
							entry->SetImage("SetupInfo\\Images\\" + image);
						}
	
						kxf::String reqSet = entryNode.GetFirstChildElement("RequirementsSet").GetValue();
						if (!reqSet.IsEmpty() && reqSet != "---")
						{
							entry->GetRequirements().emplace_back(reqSet);
						}
	
						// Files
						LoadStringArray(entry->GetFileData(), entryNode.GetFirstChildElement("Files"));
	
						/* Required flags and type descriptor*/
						// In version 5.x entry is shown if it's required flags checking succeed or required flags list is empty and hidden otherwise.
						// In KMP required flags (now TDConditions) changes type descriptor if check is successful.
						TypeDescriptor typeDescriptor = components.StringToTypeDescriptor(entryNode.GetFirstChildElement("TypeDescriptor").GetValue());
	
						ReadFlagsArray(entry->GetTDConditionGroup().GetOrCreateFirstCondition(), entryNode.GetFirstChildElement("RequiredFlags"));
						if (entry->GetTDConditionGroup().HasConditions())
						{
							entry->SetTDDefaultValue(TypeDescriptor::NotUsable);
							entry->SetTDConditionalValue(typeDescriptor);
						}
						else
						{
							entry->SetTDDefaultValue(typeDescriptor);
						}
	
						/* Assigned flags */
						ReadFlagsArray(entry->GetConditionFlags(), entryNode.GetFirstChildElement("SetFlags"));
					}
	
					return group;
				};
	
				// Read steps
				for (kxf::XMLNode stepNode = componentsNode.GetFirstChildElement("Steps").GetFirstChildElement(); stepNode.IsOK(); stepNode = stepNode.GetNextSiblingElement())
				{
					ComponentStep* step = components.GetSteps().emplace_back(std::make_unique<ComponentStep>()).get();
					step->SetName(stepNode.GetAttribute("Name"));
					ReadFlagsArray(step->GetConditionGroup().GetOrCreateFirstCondition(), stepNode.GetFirstChildElement("RequiredFlags"));
	
					// KMP stores groups inside steps.
					// Version 5.x stores groups separately and links them by IDs.
					KxStringVector groups;
					LoadStringArray(groups, stepNode.GetFirstChildElement("Data"));
					for (const kxf::String& groupID: groups)
					{
						auto it = std::find_if(groupsIDArray.begin(), groupsIDArray.end(), [&groupID](const auto& v)
						{
							return v.first == groupID;
						});
	
						if (it != groupsIDArray.end())
						{
							step->GetGroups().emplace_back(ReadGroup(it->second));
						}
					}
				}
	
				// Conditional steps
				auto ReadConditionalSteps = [&componentsNode, &components, &ReadFlagsArray](const kxf::String& sRootNodeName, const kxf::String& sNodeName)
				{
					for (kxf::XMLNode stepNode = componentsNode.GetFirstChildElement(sRootNodeName).GetFirstChildElement(); stepNode.IsOK(); stepNode = stepNode.GetNextSiblingElement())
					{
						auto& step = components.GetConditionalSteps().emplace_back(std::make_unique<ConditionalComponentStep>());
						ReadFlagsArray(step->GetConditionGroup().GetOrCreateFirstCondition(), stepNode.GetFirstChildElement("RequiredFlags"));
						LoadStringArray(step->GetItems(), stepNode.GetFirstChildElement(sNodeName));
					}
				};
				ReadConditionalSteps("ConditionalInstall", "Data");
			}
		}
	}
	void LegacySerializer::ReadINI5x()
	{
		InfoSection& info = m_Project->GetInfo();
	
		// Current version of package format don't support storing game config edits
		// as ConfigManager doesn't currently support external modifying requests.
		kxf::XMLNode iniNode = m_XML.QueryElement("SetupInfo/Installer/INI");
		if (!iniNode.IsOK())
		{
			m_XML.QueryElement("SetupInfo/Installer/INI-Files");
		}
	
		if (iniNode.IsOK())
		{
			for (kxf::XMLNode entryNode = iniNode.GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
			{
				// Maybe I find something someday
				if (m_ProjectVersion < kxf::Version("5.0"))
				{
					wxMessageBox(m_ProjectVersion);
				}
	
				kxf::String id = entryNode.GetAttribute("ID");
				bool isAuto = entryNode.GetAttributeBool("AutoApply");
				kxf::String path = kxf::String::Format("$(%s)\\%s", entryNode.GetFirstChildElement("Variable").GetValue(), entryNode.GetFirstChildElement("Path").GetValue());
				kxf::String section = entryNode.GetFirstChildElement("Section").GetValue();
				kxf::String key = entryNode.GetFirstChildElement("Key").GetValue();
				kxf::String value = entryNode.GetFirstChildElement("Value").GetValue();
	
				kxf::String serializedName = kxf::String::Format("INI<string id = %s, bool bAutoApply = %d>", id, (int)isAuto);
				kxf::String serializedValue = kxf::String::Format("INI(\"%s\").SetValue(string section = \"%s\", string key = \"%s\", string value = \"%s\");", path, section, key, value);
				info.GetCustomFields().emplace_back(serializedValue, serializedName);
			}
		}
	}
	
	void LegacySerializer::Structurize(ModPackageProject& project)
	{
		m_Project = &project;
		m_XML.Load(m_Data);
	
		m_ProjectVersion = ReadBase();
	
		// Config can be read from any version
		ReadConfig();
		if (m_ProjectVersion >= kxf::Version("5.0"))
		{
			// AMI variant (SMI 5.0+)
			ReadInfo5x();
			ReadFiles5x();
			ReadInterface5x();
			ReadRequirements5x();
			ReadComponents5x();
			ReadINI5x();
		}
		else if (m_ProjectVersion >= kxf::Version("4.0"))
		{
			// SMI 4.0+
			ReadInfo4x();
			ReadFiles4x();
			ReadInterface4x();
			ReadRequirements4x();
			ReadComponents4x();
			ReadINI5x(); // Can't find example for 4.0, but this may work
		}
		else if (m_ProjectVersion >= kxf::Version("3.0"))
		{
			// SMI 3.0+
			ReadInfo3x();
			ReadFiles3x();
			ReadInterface3x();
			ReadRequirements3x();
			ReadComponents3x();
		}
		else
		{
			// Not supported as I don't have config files for these versions
		}
	}
}
