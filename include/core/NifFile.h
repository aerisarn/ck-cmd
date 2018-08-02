/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius
See the included LICENSE file
*/

#pragma once

//#include "Factory.h"

#include <objDecl.cpp>
#include <field_visitor.h>
#include <interfaces\typed_visitor.h>

//hierarchy
#include <obj/NiTimeController.h>
#include <obj/NiExtraData.h>
#include <obj/NiCollisionObject.h>
#include <obj/NiProperty.h>
#include <obj/NiDynamicEffect.h>

#include <niflib.h>
#include <nif_basic_types.h>
#include <gen/SkinPartition.h>

#include <unordered_map>

namespace ckcmd {
namespace NIF {

using namespace Niflib;

		class NifFile {
		private:
			NifInfo hdr;
			vector<NiObjectRef> blocks;
			bool isValid = false;
			bool hasUnknown = false;
			bool hasExternalSkinning = false;
			bool isSkeleton = false;
			bool hasNiSequence = false;

			set<void*> skinned_bones;

			void PrepareData();

		public:
			string name;

			NifFile() {}

			NifFile(NifInfo& header, vector<NiObjectRef>& blocks) {
				this->blocks = blocks;
				this->hdr = header;
			}

			NifFile(const std::string& fileName) {
				Load(fileName);
			}

			NifFile(std::istream& stream) {
				Load(stream);
			}

			NiObjectRef getBlock(unsigned short index) {
				return blocks[index];
			}

			NifInfo& GetInfo() { return hdr; }
			//void CopyFrom(const NifFile& other);

			int Load(const string& fileName);
			int Load(istream& file);
			int Save(const string& fileName);
			int Save(std::ostream& file);

			bool IsValid() { return isValid; }
			bool HasUnknown() { return hasUnknown; }
			bool hasExternalSkin() { return hasExternalSkinning; }
			bool isSkeletonOnly() { return isSkeleton; }
			bool hasAnimation() { return hasNiSequence; }
	

			void Create(const NifInfo& version);
			void Clear();

			template<class T = NiObject>
			Ref<T> FindBlockByName(const std::string& name);
			NiObjectRef GetParentNode(NiObjectRef childBlock);

			NiObjectRef GetRoot() { return GetFirstRoot(blocks); }

			static bool hasExternalSkinnedMesh(vector<NiObjectRef>& blocks,  NiNode* root);
			void NifFile::UpdateSkinPartitions(NiTriShapeRef shape);

		};
	}
}
