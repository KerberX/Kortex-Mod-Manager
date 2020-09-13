#pragma once
#include <Kortex/Kortex.hpp>
#include "ListModel.h"
#include "KxFramework/KxDataViewModelExBase.h"

namespace Kortex::PackageDesigner
{
	template<class T>
	class VectorModel: public KxDataViewVectorListModelEx<T, ListModel>
	{
		private:
			void OnSetDataVectorInternal() override
			{
				KxDataViewVectorListModelEx::OnSetDataVectorInternal();
			}
	};
}
