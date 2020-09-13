#pragma once
#include <Kortex/Kortex.hpp>
#include "VariablesTable/VariableValue.h"

namespace Kortex
{
	class IVariableTable
	{
		public:
			using Visitor = std::function<bool(const kxf::String&, const VariableValue&)>;

		public:
			IVariableTable() = default;
			virtual ~IVariableTable() = default;

		public:
			virtual bool IsEmpty() const = 0;

			virtual bool HasVariable(const kxf::String& id) const = 0;
			virtual VariableValue GetVariable(const kxf::String& id) const = 0;
			virtual void SetVariable(const kxf::String& id, const VariableValue& value) = 0;

			virtual void Accept(const Visitor& visitor) const = 0;

			// This function supports $(...), $T(...) and $SHF(...) syntax. Variable name (...) can contain spaces.
			// $(...) normal variable like $(AppName).
			// $T(...) localization variable such as $T(Generic.Run) or $T(ID_OK).
			// $SHF(...) shell variable like $SHF(DESKTOP). Currently only supports expansion of constants
			// from 'KxShellFolderID' enum (without 'Kx' prefix).
			kxf::String Expand(const kxf::String& variables) const;
	};
}
