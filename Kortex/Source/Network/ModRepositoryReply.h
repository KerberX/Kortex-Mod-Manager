#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include <KxFramework/kxf::Version.h>
#include <KxFramework/KxURI.h>

namespace Kortex
{
	class ModEndorsement
	{
		public:
			static ModEndorsement Endorsed()
			{
				return ModEndorsement(State::Endorse);
			}
			static ModEndorsement Abstained()
			{
				return ModEndorsement(State::Abstain);
			}
			static ModEndorsement Undecided()
			{
				return ModEndorsement(State::Undecided);
			}

		private:
			enum class State
			{
				Undecided,
				Endorse,
				Abstain,
			};

		private:
			State m_State = State::Undecided;

		private:
			ModEndorsement(State value)
				:m_State(value)
			{
			}

		public:
			ModEndorsement(ModEndorsement&&) = default;
			ModEndorsement(const ModEndorsement&) = default;

		public:
			bool IsEndorsed() const
			{
				return m_State == State::Endorse;
			}
			bool IsAbstained() const
			{
				return m_State == State::Abstain;
			}
			bool IsUndecided() const
			{
				return m_State == State::Undecided;
			}

			bool ShouldRemindEndorse() const
			{
				return !IsEndorsed() && !IsAbstained();
			}
	
		public:
			ModEndorsement& operator=(ModEndorsement&&) = default;
			ModEndorsement& operator=(const ModEndorsement&) = default;
	};
}

namespace Kortex
{
	enum class ModFileCategory
	{
		Unknown = 0,
		Main,
		Misc,
		Optional,
		OldVersion,
	};
}

namespace Kortex
{
	struct ModFileReply
	{
		public:
			ModID ModID;
			ModFileID ID;
			int64_t Size = -1;
			kxf::String Name;
			kxf::String DisplayName;
			kxf::String ChangeLog;
			kxf::Version Version;
			wxDateTime UploadDate;
			ModFileCategory Category = ModFileCategory::Unknown;
			bool IsPrimary = false;

		public:
			bool IsOK() const
			{
				return ModID.HasValue() && ID.HasValue();
			}
			bool HasName() const
			{
				return !Name.IsEmpty();
			}
			bool HasSize() const
			{
				return Size > 0;
			}
	};
	struct ModInfoReply
	{
		public:
			ModID ID;

			kxf::String Name;
			kxf::String Summary;
			kxf::String Description;
			kxf::Version Version;
			kxf::String MainImage;

			kxf::String Author;
			kxf::String Uploader;
			kxf::String UploaderProfile;

			wxDateTime UploadDate;
			wxDateTime LastUpdateDate;

			bool ContainsAdultContent = false;
			ModEndorsement EndorsementState = ModEndorsement::Undecided();

			ModFileReply PrimaryFile;

		public:
			bool IsOK() const
			{
				return ID.HasValue() && !Name.IsEmpty();
			}
	};
	struct ModDownloadReply
	{
		public:
			kxf::String Name;
			kxf::String ShortName;
			KxURI URI;
			
		public:
			bool IsOK() const
			{
				return URI.IsOk();
			}
	};
	struct ModEndorsementReply
	{
		ModEndorsement Endorsement = ModEndorsement::Undecided();
		kxf::String Message;
	};
}
