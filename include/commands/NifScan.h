#include <filesystem>

#include <niflib.h>
#include <obj\NiObject.h>
#include <obj\BSFadeNode.h>

#include <objDecl.cpp>
#include <field_visitor.h>
#include <interfaces\typed_visitor.h>

//hierarchy
#include <obj/NiTimeController.h>
#include <obj/NiExtraData.h>
#include <obj/NiCollisionObject.h>
#include <obj/NiProperty.h>
#include <obj/NiDynamicEffect.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#include <filesystem>

namespace fs = std::experimental::filesystem;

static const fs::path nif_in = ".\\resources\\in";
static const fs::path nif_err = ".\\resources\\err";

namespace ckcmd {
	namespace nifscan {

		using namespace Niflib;

		class SingleChunkFlagVerifier : public RecursiveFieldVisitor<SingleChunkFlagVerifier> {

			int n_collisions = 0;
			int n_phantoms = 0;
			int n_constraints = 0;
			bool hasBranches = false;
			bool branchesResult = true;
			set<pair<bhkEntity*, bhkEntity*>>& entitiesPair;
			const NifInfo& this_info;
			set<NiObject*>& alreadyVisitedNodes;

		public:
			bool singleChunkVerified = false;

			SingleChunkFlagVerifier(NiObject& data, const NifInfo& info) :
				RecursiveFieldVisitor(*this, info), this_info(info), alreadyVisitedNodes(set<NiObject*>()), entitiesPair(set<pair<bhkEntity*, bhkEntity*>>())
			{
				data.accept(*this, info);

				bool singlechain = false;
				if (n_collisions - n_constraints == 1) {
					singlechain = true;
					singleChunkVerified = true;
				}
				if (n_phantoms > 0 && (singlechain || n_collisions == 0)) {
					singleChunkVerified = true;
				}

				if (hasBranches) {
					if (n_collisions == 0 && n_phantoms == 0)
						singleChunkVerified = singleChunkVerified || branchesResult;
					else
						singleChunkVerified = singleChunkVerified && branchesResult;
				}
			}

			SingleChunkFlagVerifier(NiObject& data, const NifInfo& info, set<NiObject*>& alreadyVisitedNodes, set<pair<bhkEntity*, bhkEntity*>>& entitiesPair) :
				RecursiveFieldVisitor(*this, info), this_info(info), alreadyVisitedNodes(alreadyVisitedNodes), entitiesPair(entitiesPair)
			{
				data.accept(*this, info);

				bool singlechain = false;
				if (n_collisions - n_constraints == 1) {
					singlechain = true;
					singleChunkVerified = true;
				}
				if (n_phantoms > 0 && (singlechain || n_collisions == 0)) {
					singleChunkVerified = true;
				}
				if (hasBranches) {
					if (n_collisions == 0 && n_phantoms == 0)
						singleChunkVerified = singleChunkVerified || branchesResult;
					else
						singleChunkVerified = singleChunkVerified && branchesResult;
				}

				if (n_phantoms == 0 && n_collisions == 0)
					singleChunkVerified = true;
			}

			template<class T>
			inline void visit_object(T& obj) {
				NiObject* ptr = (NiObject*)&obj;
				if (alreadyVisitedNodes.insert(ptr).second) {
					NiObjectRef ref = DynamicCast<NiObject>(ptr);
					if (ref->IsSameType(NiSwitchNode::TYPE)) {
						branchesResult = false;
						hasBranches = true;
						bool singleResult = true;
						NiSwitchNodeRef ref = DynamicCast<NiSwitchNode>(ptr);
						for (NiAVObjectRef child : ref->GetChildren()) {
							bool result = SingleChunkFlagVerifier(*child, this_info, alreadyVisitedNodes, entitiesPair).singleChunkVerified;
							singleResult = singleResult && result;
						}
						branchesResult = branchesResult || singleResult;
					}

					if (ref->IsDerivedType(bhkSPCollisionObject::TYPE)) {
						n_phantoms++;
					}
					if (ref->IsDerivedType(bhkCollisionObject::TYPE)) {
						n_collisions++;
					}

					if (ref->IsDerivedType(bhkConstraint::TYPE)) {
						bhkConstraintRef cref = DynamicCast<bhkConstraint>(ref);
						std::pair<bhkEntity*, bhkEntity*> p;
						p.first = *cref->GetEntities().begin();
						p.second = *(++cref->GetEntities().begin());
						if (entitiesPair.insert(p).second)
							n_constraints++;
					}
				}
			}

			template<class T>
			inline void visit_compound(T& obj) {}

			template<class T>
			inline void visit_field(T& obj) {}

		};

	}
}
