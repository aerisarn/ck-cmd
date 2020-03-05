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

			double bhkScaleFactor; // = 6.9969

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


			size_t getNumBlocks() {
				return blocks.size();
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
			double GetBhkScaleFactor() { return bhkScaleFactor; }
	
			static const char* shader_type_name(const BSLightingShaderPropertyShaderType& shader_type);
			static BSLightingShaderPropertyShaderType shader_type_value(const string& shader_type);
			static const char* material_name(const SkyrimHavokMaterial& material);
			static SkyrimHavokMaterial material_value(const string& name);
			static std::array<double, 3> material_color(const SkyrimHavokMaterial& material);
			static const char* layer_name(const SkyrimLayer& layer);
			static SkyrimLayer layer_value(const string& name);

			void Create(const NifInfo& version);
			void Clear();

			template<class T = NiObject>
			Ref<T> FindBlockByName(const std::string& name);
			NiObjectRef GetParentNode(NiObjectRef childBlock);

			NiObjectRef GetRoot() { return GetFirstRoot(blocks); }

			static bool hasExternalSkinnedMesh(vector<NiObjectRef>& blocks,  NiNode* root);
			
		};
	}
}
