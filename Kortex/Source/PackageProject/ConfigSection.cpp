#include "stdafx.h"
#include "ConfigSection.h"
#include "ModPackageProject.h"
#include <Kortex/Application.hpp>

namespace Kortex::PackageProject
{
	const kxf::String ConfigSection::ms_DefaultCompressionMethod = "LZMA2";
	
	bool ConfigSection::IsCompressionMethodSupported(const kxf::String& value)
	{
		return value == "LZMA" || value == "LZMA2" || value == "BZip2" || value == "PPMd";
	}
	
	ConfigSection::ConfigSection(ModPackageProject& project)
		:ProjectSection(project)
	{
	}
	ConfigSection::~ConfigSection()
	{
	}
	
	void ConfigSection::SetCompressionMethod(const kxf::String& value)
	{
		if (IsCompressionMethodSupported(value))
		{
			m_CompressionMethod = value;
		}
		else
		{
			m_CompressionMethod = ms_DefaultCompressionMethod;
		}
	}
	void ConfigSection::SetCompressionLevel(int value)
	{
		if (value >= ms_MinCompressionLevel && value <= ms_MaxCompressionLevel)
		{
			m_CompressionLevel = value;
		}
		else
		{
			m_CompressionLevel = ms_DefaultCompressionLevel;
		}
	}
	void ConfigSection::SetCompressionDictionarySize(int value)
	{
		if (value >= ms_MinDictionarySize && value <= ms_MaxDictionarySize)
		{
			m_CompressionDictionarySize = value;
		}
		else
		{
			m_CompressionDictionarySize = ms_DefaultDictionarySize;
		}
	}
}
