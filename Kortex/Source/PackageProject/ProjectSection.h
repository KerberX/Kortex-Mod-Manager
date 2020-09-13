#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include <Kx/RTTI.hpp>

namespace Kortex
{
	class ModPackageProject;
}

namespace Kortex::PackageProject
{
	class ProjectSection
	{
		private:
			ModPackageProject& m_Project;
	
		public:
			ProjectSection(ModPackageProject& project)
				:m_Project(project)
			{
			}
			virtual ~ProjectSection() = default;
			
		public:
			ModPackageProject& GetProject()
			{
				return m_Project;
			}
			const ModPackageProject& GetProject() const
			{
				return m_Project;
			}
	};
}
