#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/ConvertNif.h>
#include <commands/Geometry.h>
#include <core/games.h>
#include <core/bsa.h>
#include <core/NifFile.h>
#include <commands/NifScan.h>

#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\StiffSpring\hkpStiffSpringConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::Geometry;
using namespace ckcmd::NIF;
using namespace ckcmd::nifscan;

SkyrimHavokMaterial convert_havok_material(OblivionHavokMaterial material) {
	switch (material) {
	case OB_HAV_MAT_STONE: /*!< Stone */
		return SKY_HAV_MAT_STONE; // = 3741512247, /*!< Stone */
	case OB_HAV_MAT_CLOTH: /*!< Cloth */
		return SKY_HAV_MAT_CLOTH; // = 3839073443, /*!< Cloth */
	case OB_HAV_MAT_DIRT: /*!< Dirt */
		return SKY_HAV_MAT_DIRT; // = 3106094762, /*!< Dirt */
	case OB_HAV_MAT_GLASS: /*!< Glass */
		return SKY_HAV_MAT_GLASS; // = 3739830338, /*!< Glass */
	case OB_HAV_MAT_GRASS: /*!< Grass */
		return SKY_HAV_MAT_GRASS; //= 1848600814, /*!< Grass */
	case OB_HAV_MAT_METAL: /*!< Metal */
		return SKY_HAV_MAT_SOLID_METAL; // = 1288358971, /*!< Solid Metal */
	case OB_HAV_MAT_ORGANIC: /*!< Organic */
		return SKY_HAV_MAT_ORGANIC; // = 2974920155, /*!< Organic */
	case OB_HAV_MAT_SKIN: /*!< Skin */
		return SKY_HAV_MAT_SKIN; // = 591247106, /*!< Skin */
	case OB_HAV_MAT_WATER: /*!< Water */
		return SKY_HAV_MAT_WATER; // = 1024582599, /*!< Water */
	case OB_HAV_MAT_WOOD: /*!< Wood */
		return SKY_HAV_MAT_WOOD; // = 500811281, /*!< Wood */
	case OB_HAV_MAT_HEAVY_STONE: /*!< Heavy Stone */
		return SKY_HAV_MAT_HEAVY_STONE; // = 1570821952, /*!< Heavy Stone */
	case OB_HAV_MAT_HEAVY_METAL: /*!< Heavy Metal */
		return SKY_HAV_MAT_HEAVY_METAL; // = 2229413539, /*!< Heavy Metal */
	case OB_HAV_MAT_HEAVY_WOOD: /*!< Heavy Wood */
		return SKY_HAV_MAT_HEAVY_WOOD; // = 3070783559, /*!< Heavy Wood */
	case OB_HAV_MAT_CHAIN: /*!< Chain */
		return SKY_HAV_MAT_MATERIAL_CHAIN; // = 3074114406, /*!< Material Chain */ TODO: maybe SKY_HAV_MAT_MATERIAL_CHAIN_METAL?
	case OB_HAV_MAT_SNOW: /*!< Snow */
		return SKY_HAV_MAT_SNOW; // = 398949039, /*!< Snow */
								 //TODO: We do not have so much stairs		
	case OB_HAV_MAT_STONE_STAIRS: /*!< Stone Stairs */
		return SKY_HAV_MAT_STAIRS_STONE; // = 899511101, /*!< Stairs Stone */
	case OB_HAV_MAT_CLOTH_STAIRS: /*!< Cloth Stairs */
		return SKY_HAV_MAT_CLOTH; // = 3839073443, /*!< Cloth */
	case OB_HAV_MAT_DIRT_STAIRS: /*!< Dirt Stairs */
		return SKY_HAV_MAT_DIRT; // = 3106094762, /*!< Dirt */
	case OB_HAV_MAT_GLASS_STAIRS: /*!< Glass Stairs */
		return SKY_HAV_MAT_GLASS; // = 3739830338, /*!< Glass */
	case OB_HAV_MAT_GRASS_STAIRS: /*!< Grass Stairs */
		return SKY_HAV_MAT_GRASS; //= 1848600814, /*!< Grass */
	case OB_HAV_MAT_METAL_STAIRS: /*!< Metal Stairs */
		return SKY_HAV_MAT_SOLID_METAL; // = 1288358971, /*!< Solid Metal */
	case OB_HAV_MAT_ORGANIC_STAIRS: /*!< Organic Stairs */
		return SKY_HAV_MAT_ORGANIC; // = 2974920155, /*!< Organic */
	case OB_HAV_MAT_SKIN_STAIRS: /*!< Skin Stairs */
		return SKY_HAV_MAT_SKIN; // = 591247106, /*!< Skin */
	case OB_HAV_MAT_WATER_STAIRS: /*!< Water Stairs */
		return SKY_HAV_MAT_WATER; // = 1024582599, /*!< Water */
	case OB_HAV_MAT_WOOD_STAIRS: /*!< Wood Stairs */
		return SKY_HAV_MAT_STAIRS_WOOD; // = 1461712277, /*!< Stairs Wood */
	case OB_HAV_MAT_HEAVY_STONE_STAIRS: /*!< Heavy Stone Stairs */
		return SKY_HAV_MAT_HEAVY_STONE; // = 1570821952, /*!< Heavy Stone */
	case OB_HAV_MAT_HEAVY_METAL_STAIRS: /*!< Heavy Metal Stairs */
		return SKY_HAV_MAT_HEAVY_METAL; // = 2229413539, /*!< Heavy Metal */
	case OB_HAV_MAT_HEAVY_WOOD_STAIRS: /*!< Heavy Wood Stairs */
		return SKY_HAV_MAT_HEAVY_WOOD; // = 3070783559, /*!< Heavy Wood */
	case OB_HAV_MAT_CHAIN_STAIRS: /*!< Chain Stairs */
		return SKY_HAV_MAT_MATERIAL_CHAIN; // = 3074114406, /*!< Material Chain */ TODO: maybe SKY_HAV_MAT_MATERIAL_CHAIN_METAL?
	case OB_HAV_MAT_SNOW_STAIRS: /*!< Snow Stairs */
		return SKY_HAV_MAT_STAIRS_SNOW; // = 1560365355, /*!< Stairs Snow */
	case OB_HAV_MAT_ELEVATOR: /*!< Elevator */
		return SKY_HAV_MAT_STONE; // = 3741512247, /*!< Stone */ TODO: I really don't know'
	case OB_HAV_MAT_RUBBER: /*!< Rubber */
		return SKY_HAV_MAT_ORGANIC; // = 2974920155, /*!< Organic */ TODO: I really don't know'
	}
	//DEFAULT
	return SKY_HAV_MAT_STONE; // = 3741512247, /*!< Stone */
}

SkyrimLayer convert_havok_layer(OblivionLayer layer) {
	switch (layer) {
	case OL_UNIDENTIFIED:
		return SKYL_UNIDENTIFIED; /*!< Unidentified */
	case OL_STATIC: /*!< Static (red) */
		return SKYL_STATIC; /*!< Static */
	case OL_ANIM_STATIC: /*!< AnimStatic (magenta) */
		return SKYL_ANIMSTATIC; /*!< Anim Static */
	case OL_TRANSPARENT: /*!< Transparent (light pink) */
		return SKYL_TRANSPARENT; /*!< Transparent */
	case OL_CLUTTER: /*!< Clutter (light blue) */
		return SKYL_CLUTTER; /*!< Clutter. Object with this layer will float on water surface. */
	case OL_WEAPON: /*!< Weapon (orange) */
		return SKYL_WEAPON;  /*!< Weapon */
	case OL_PROJECTILE: /*!< Projectile (light orange) */
		return SKYL_PROJECTILE; /*!< Projectile */
	case OL_SPELL: /*!< Spell (cyan) */
		return SKYL_SPELL; /*!< Spell */
	case OL_BIPED: /*!< Biped (green) Seems to apply to all creatures/NPCs */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_TREES: /*!< Trees (light brown) */
		return SKYL_TREES; /*!< Trees */
	case OL_PROPS: /*!< Props (magenta) */
		return SKYL_PROPS; /*!< Props */
	case OL_WATER: /*!< Water (cyan) */
		return SKYL_WATER; /*!< Water */
	case OL_TRIGGER: /*!< Trigger (light grey) */
		return SKYL_TRIGGER; /*!< Trigger */
	case OL_TERRAIN: /*!< Terrain (light yellow) */
		return SKYL_TERRAIN; /*!< Terrain */
	case OL_TRAP: /*!< Trap (light grey) */
		return SKYL_TRAP; /*!< Trap */
	case OL_NONCOLLIDABLE: /*!< NonCollidable (white) */
		return SKYL_NONCOLLIDABLE; /*!< NonCollidable */
	case OL_CLOUD_TRAP: /*!< CloudTrap (greenish grey) */
		return SKYL_CLOUD_TRAP; /*!< CloudTrap */
	case OL_GROUND: /*!< Ground (none) */
		return SKYL_GROUND; /*!< Ground. It seems that produces no sound when collide. */
	case OL_PORTAL: /*!< Portal (green) */
		return SKYL_PORTAL; /*!< Portal */
	case OL_STAIRS: /*!< Stairs (white) */
		return SKYL_STAIRHELPER; /*!< = 31,  Stair Helper */
	case OL_CHAR_CONTROLLER: /*!< CharController (yellow) */
		return SKYL_CHARCONTROLLER; /*!<= 30, /*!< Char Controller */
	case OL_AVOID_BOX: /*!< AvoidBox (dark yellow) */
		return SKYL_AVOIDBOX; /*!< = 34,  Avoid Box */
	case OL_UNKNOWN1: /*!< ? (white) */
		return SKYL_DEBRIS_SMALL; /*!<= 19,  Debris Small */
	case OL_UNKNOWN2: /*!< ? (white) */
		return SKYL_DEBRIS_LARGE; /*!< = 20,  Debris Small */
	case OL_CAMERA_PICK: /*!< CameraPick (white) */
		return SKYL_CAMERAPICK;  /*!<= 39, Camera Pick */
	case OL_ITEM_PICK: /*!< ItemPick (white) */
		return SKYL_ITEMPICK;  /*!<= 40,  Item Pick */
	case OL_LINE_OF_SIGHT: /*!< LineOfSight (white) */
		return SKYL_LINEOFSIGHT;/*!<= 41, < Line of Sight */
	case OL_PATH_PICK: /*!< PathPick (white) */
		return SKYL_PATHPICK; /*!< = 42, Path Pick */
	case OL_CUSTOM_PICK_1: /*!< CustomPick1 (white) */
		return SKYL_CUSTOMPICK1; /*!< = 43, /*!< Custom Pick 1 */
	case OL_CUSTOM_PICK_2: /*!< CustomPick2 (white) */
		return SKYL_CUSTOMPICK2; /*!<= 44, /*!< Custom Pick 2 */
	case OL_SPELL_EXPLOSION: /*!< SpellExplosion (white) */
		return SKYL_SPELLEXPLOSION; /*!< = 45, Spell Explosion */
	case OL_DROPPING_PICK: /*!< DroppingPick (white) */
		return SKYL_DEADBIP; /*!<  = 32, Dead Bip */
	case OL_OTHER: /*!< Other (white) */
		return SKYL_STATIC; /*!< Static */
	case OL_HEAD: /*!< Head */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_BODY: /*!< Body */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_SPINE1: /*!< Spine1 */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_SPINE2: /*!< Spine2 */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_L_UPPER_ARM: /*!< LUpperArm */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_L_FOREARM: /*!< LForeArm */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_L_HAND: /*!< LHand */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_L_THIGH: /*!< LThigh */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_L_CALF: /*!< LCalf */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_L_FOOT:/*!< LFoot */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_R_UPPER_ARM: /*!< RUpperArm */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_R_FOREARM: /*!< RForeArm */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_R_HAND: /*!< RHand */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_R_THIGH: /*!< RThigh */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_R_CALF: /*!< RCalf */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_R_FOOT:/*!< RFoot */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_TAIL: /*!< Tail */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_SIDE_WEAPON: /*!< SideWeapon */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_SHIELD: /*!< Shield */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_QUIVER: /*!< Quiver */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_BACK_WEAPON: /*!< BackWeapon */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_BACK_WEAPON2: /*!< BackWeapon (?) */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_PONYTAIL: /*!< PonyTail */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_WING: /*!< Wing */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	case OL_NULL: /*!< Null */
		return SKYL_BIPED; /*!< Biped. Seems to apply to all creatures/NPCs */
	}
	//DEFAULT
	return SKYL_STATIC;
}

#define COLLISION_RATIO 0.1f
class bhkRigidBodyUpgrader {};


class CMSPacker {};

template<>
class Accessor<CMSPacker> {
public:
	Accessor(hkpCompressedMeshShape* pCompMesh, bhkCompressedMeshShapeDataRef pData) 
	{
		short                                   chunkIdxNif(0);

		pData->SetBoundsMin(Vector4(pCompMesh->m_bounds.m_min(0), pCompMesh->m_bounds.m_min(1), pCompMesh->m_bounds.m_min(2), pCompMesh->m_bounds.m_min(3)));
		pData->SetBoundsMax(Vector4(pCompMesh->m_bounds.m_max(0), pCompMesh->m_bounds.m_max(1), pCompMesh->m_bounds.m_max(2), pCompMesh->m_bounds.m_max(3)));

		//  resize and copy bigVerts
		//vector<bhkCMSDBigTris > (pCompMesh->m_bigVertices.getSize())

		//pData->SetNumBigVerts(pCompMesh->m_bigVertices.getSize());

		//vector<Vector4 > bigVerts(pData->GetNumBigVerts())

		vector<Vector4 > tVec4Vec(pCompMesh->m_bigVertices.getSize());
		for (unsigned int idx(0); idx < pCompMesh->m_bigVertices.getSize(); ++idx)
		{
			tVec4Vec[idx].x = pCompMesh->m_bigVertices[idx](0);
			tVec4Vec[idx].y = pCompMesh->m_bigVertices[idx](1);
			tVec4Vec[idx].z = pCompMesh->m_bigVertices[idx](2);
			tVec4Vec[idx].w = pCompMesh->m_bigVertices[idx](3);
		}
		pData->SetBigVerts(tVec4Vec);

		//  resize and copy bigTris
		// pData->SetNumBigTris(pCompMesh->m_bigTriangles.getSize());
		vector<bhkCMSDBigTris > tBTriVec(pCompMesh->m_bigTriangles.getSize());
		//tBTriVec.resize(pData->GetNumBigTris());
		for (unsigned int idx(0); idx < pCompMesh->m_bigTriangles.getSize(); ++idx)
		{
			tBTriVec[idx].triangle1 = pCompMesh->m_bigTriangles[idx].m_a;
			tBTriVec[idx].triangle2 = pCompMesh->m_bigTriangles[idx].m_b;
			tBTriVec[idx].triangle3 = pCompMesh->m_bigTriangles[idx].m_c;
			tBTriVec[idx].material = pCompMesh->m_bigTriangles[idx].m_material;
			tBTriVec[idx].weldingInfo = pCompMesh->m_bigTriangles[idx].m_weldingInfo;
		}
		pData->SetBigTris(tBTriVec);

		//  resize and copy transform data
		//pData->SetNumTransforms(pCompMesh->m_transforms.getSize());
		vector<bhkCMSDTransform > tTranVec(pCompMesh->m_transforms.getSize());
		//tTranVec.resize(pData->GetNumTransforms());
		for (unsigned int idx(0); idx < pCompMesh->m_transforms.getSize(); ++idx)
		{
			tTranVec[idx].translation.x = pCompMesh->m_transforms[idx].m_translation(0);
			tTranVec[idx].translation.y = pCompMesh->m_transforms[idx].m_translation(1);
			tTranVec[idx].translation.z = pCompMesh->m_transforms[idx].m_translation(2);
			tTranVec[idx].translation.w = pCompMesh->m_transforms[idx].m_translation(3);
			tTranVec[idx].rotation.x = pCompMesh->m_transforms[idx].m_rotation(0);
			tTranVec[idx].rotation.y = pCompMesh->m_transforms[idx].m_rotation(1);
			tTranVec[idx].rotation.z = pCompMesh->m_transforms[idx].m_rotation(2);
			tTranVec[idx].rotation.w = pCompMesh->m_transforms[idx].m_rotation(3);
		}
		pData->chunkTransforms = tTranVec;

		vector<bhkCMSDMaterial > tMtrlVec(pCompMesh->m_materials.getSize());
		
		for (unsigned int idx(0); idx < pCompMesh->m_materials.getSize(); ++idx)
		{
			bhkCMSDMaterial& material = tMtrlVec[idx];
			material.material = (SkyrimHavokMaterial)pCompMesh->m_materials[idx];
			//TODO;
			material.filter.layer_sk = SKYL_STATIC;
		}

		//  set material list
		pData->chunkMaterials = tMtrlVec;

		vector<bhkCMSDChunk> chunkListNif(pCompMesh->m_chunks.getSize());

		//  for each chunk
		for (hkArray<hkpCompressedMeshShape::Chunk>::iterator pCIterHvk = pCompMesh->m_chunks.begin(); pCIterHvk != pCompMesh->m_chunks.end(); pCIterHvk++)
		{
			//  get nif chunk
			bhkCMSDChunk&	chunkNif = chunkListNif[chunkIdxNif];

			//  set offset => translation
			chunkNif.translation.x = pCIterHvk->m_offset(0);
			chunkNif.translation.y = pCIterHvk->m_offset(1);
			chunkNif.translation.z = pCIterHvk->m_offset(2);
			chunkNif.translation.w = pCIterHvk->m_offset(3);

			//  force flags to fixed values
			chunkNif.materialIndex = pCIterHvk->m_materialInfo;
			chunkNif.reference = 65535;
			chunkNif.transformIndex = pCIterHvk->m_transformIndex;

			//  vertices
			chunkNif.numVertices = pCIterHvk->m_vertices.getSize();
			chunkNif.vertices.resize(chunkNif.numVertices);
			for (unsigned int i(0); i < chunkNif.numVertices; ++i)
			{
				chunkNif.vertices[i] = pCIterHvk->m_vertices[i];
			}

			//  indices
			chunkNif.numIndices = pCIterHvk->m_indices.getSize();
			chunkNif.indices.resize(chunkNif.numIndices);
			for (unsigned int i(0); i < chunkNif.numIndices; ++i)
			{
				chunkNif.indices[i] = pCIterHvk->m_indices[i];
			}

			//  strips
			chunkNif.numStrips = pCIterHvk->m_stripLengths.getSize();
			chunkNif.strips.resize(chunkNif.numStrips);
			for (unsigned int i(0); i < chunkNif.numStrips; ++i)
			{
				chunkNif.strips[i] = pCIterHvk->m_stripLengths[i];
			}

			chunkNif.weldingInfo.resize(pCIterHvk->m_weldingInfo.getSize());
			for (int k = 0; k < pCIterHvk->m_weldingInfo.getSize(); k++) {
				chunkNif.weldingInfo[k] = pCIterHvk->m_weldingInfo[k];
			}

			++chunkIdxNif;

		}  //  for (hkArray<hkpCompressedMeshShape::Chunk>::iterator pCIterHvk = 

		   //  set modified chunk list to compressed mesh shape data
		pData->chunks = chunkListNif;
		//----  Merge  ----  END
	}
};

class CollisionShapeVisitor : public RecursiveFieldVisitor<CollisionShapeVisitor> {
	
	hkGeometry geometry;

	void calculate_collision()
	{	
		//----  Havok  ----  START
		hkpCompressedMeshShape*					pCompMesh(NULL);
		hkpMoppCode*							pMoppCode(NULL);
		hkpMoppBvTreeShape*						pMoppBvTree(NULL);
		hkpCompressedMeshShapeBuilder			shapeBuilder;
		hkpMoppCompilerInput					mci;
		vector<int>								geometryIdxVec;
		vector<bhkCMSDMaterial>					tMtrlVec;
		int										subPartId(0);
		int										tChunkSize(0);

		bhkCompressedMeshShapeDataRef pData = new bhkCompressedMeshShapeData();
	
		//  initialize shape Builder
		shapeBuilder.m_stripperPasses = 5000;
	
		//  create compressedMeshShape
		pCompMesh = shapeBuilder.createMeshShape(0.001f, hkpCompressedMeshShape::MATERIAL_SINGLE_VALUE_PER_CHUNK);
	
		//  add geometry to shape
		subPartId = shapeBuilder.beginSubpart(pCompMesh);
		shapeBuilder.addGeometry(geometry, hkMatrix4::getIdentity(), pCompMesh);
		shapeBuilder.endSubpart(pCompMesh);
		shapeBuilder.addInstance(subPartId, hkMatrix4::getIdentity(), pCompMesh);
	
	
		   //  create welding info
		mci.m_enableChunkSubdivision = false;  //  PC version
		pMoppCode = hkpMoppUtility::buildCode(pCompMesh, mci);
		pMoppBvTree = new hkpMoppBvTreeShape(pCompMesh, pMoppCode);
		hkpMeshWeldingUtility::computeWeldingInfo(pCompMesh, pMoppBvTree, hkpWeldingUtility::WELDING_TYPE_TWO_SIDED);
		//----  Havok  ----  END
	
		//----  Merge  ----  START
		hkArray<hkpCompressedMeshShape::Chunk>  chunkListHvk;
		//vector<bhkCMSDChunk>                    chunkListNif = pData->Get
		vector<Vector4>                         tVec4Vec;
		vector<bhkCMSDBigTris>                  tBTriVec;
		vector<bhkCMSDTransform>                tTranVec;
		map<unsigned int, bhkCMSDMaterial>		tMtrlMap;

	
		//  --- modify MoppBvTree ---
		// set origin
		pMoppShape->SetOrigin(Vector3(pMoppBvTree->getMoppCode()->m_info.m_offset(0), pMoppBvTree->getMoppCode()->m_info.m_offset(1), pMoppBvTree->getMoppCode()->m_info.m_offset(2)));
	
		// set scale
		pMoppShape->SetScale(pMoppBvTree->getMoppCode()->m_info.getScale());
	
		// set build Type
		pMoppShape->SetBuildType(MoppDataBuildType((Niflib::byte) pMoppCode->m_buildType));
	
		//  copy mopp data
		pMoppShape->SetMoppData(vector<Niflib::byte>(pMoppBvTree->m_moppData, pMoppBvTree->m_moppData + pMoppBvTree->m_moppDataSize));
	
		Accessor<CMSPacker> packer(pCompMesh, pData);

		bhkCompressedMeshShapeRef shape = new bhkCompressedMeshShape();
		shape->SetRadius(pCompMesh->m_radius);
		shape->SetRadiusCopy(pCompMesh->m_radius);
		shape->SetData(pData);

		pMoppShape->SetShape(DynamicCast<bhkShape>(shape));
	}

public:

	bhkMoppBvTreeShapeRef pMoppShape;

	CollisionShapeVisitor(bhkShapeRef shape, const NifInfo& info) :
		RecursiveFieldVisitor(*this, info) {
		shape->accept(*this, info);
	}

	template<class T>
	inline void visit_object(T& obj) {}

	template<class T>
	inline void visit_compound(T& obj) {}

	template<class T>
	inline void visit_field(T& obj) {}

	template<>
	inline void visit_object(bhkMoppBvTreeShape& obj) {
		pMoppShape = &obj;
	}

	template<>
	inline void visit_object(bhkPackedNiTriStripsShape& obj) {}

	template<>
	inline void visit_object(bhkNiTriStripsShape& obj) {}
};

bhkShapeRef upgrade_shape(const bhkShapeRef& shape, const NifInfo& info) {	
	return CollisionShapeVisitor(shape,info).pMoppShape;
}

vector<bhkShapeRef> upgrade_shapes(const vector<bhkShapeRef>& shapes, const NifInfo& info) {
	vector<bhkShapeRef> out;
	for (bhkShapeRef shape : shapes) {
		if (shape->IsSameType(bhkMoppBvTreeShape::TYPE) ||
				shape->IsSameType(bhkNiTriStripsShape::TYPE) ||
				shape->IsSameType(bhkPackedNiTriStripsShape::TYPE))
			out.push_back(upgrade_shape(shape, info));
		else
			out.push_back(shape);
	}
	return out;
}


template<>
class Accessor<bhkRigidBodyUpgrader> {

	bhkBallAndSocketConstraintRef create_ball_socket(MalleableDescriptor& descriptor) {
		bhkBallAndSocketConstraintRef constraint = new bhkBallAndSocketConstraint();
		constraint->SetEntities({ descriptor.entityA, descriptor.entityB });
		constraint->SetBallAndSocket(descriptor.ballAndSocket);
		return constraint;
	}

	bhkHingeConstraintRef create_hinge(MalleableDescriptor& descriptor) {
		bhkHingeConstraintRef constraint = new bhkHingeConstraint();
		constraint->SetEntities({ descriptor.entityA, descriptor.entityB });
		constraint->SetHinge(descriptor.hinge);
		return constraint;
	}

	bhkLimitedHingeConstraintRef create_limited_hinge(MalleableDescriptor& descriptor) {
		bhkLimitedHingeConstraintRef constraint = new bhkLimitedHingeConstraint();
		constraint->SetEntities({ descriptor.entityA, descriptor.entityB });
		constraint->SetLimitedHinge(descriptor.limitedHinge);
		return constraint;
	}

	bhkPrismaticConstraintRef create_prismatic(MalleableDescriptor& descriptor) {
		bhkPrismaticConstraintRef constraint = new bhkPrismaticConstraint();
		constraint->SetEntities({ descriptor.entityA, descriptor.entityB });
		constraint->SetPrismatic(descriptor.prismatic);
		return constraint;
	}

	bhkRagdollConstraintRef create_ragdoll(MalleableDescriptor& descriptor) {
		bhkRagdollConstraintRef constraint = new bhkRagdollConstraint();
		constraint->SetEntities({ descriptor.entityA, descriptor.entityB });
		constraint->SetRagdoll(descriptor.ragdoll);
		return constraint;
	}

	bhkStiffSpringConstraintRef create_stiff_spring(MalleableDescriptor& descriptor) {
		bhkStiffSpringConstraintRef constraint = new bhkStiffSpringConstraint();
		constraint->SetEntities({ descriptor.entityA, descriptor.entityB });
		constraint->SetStiffSpring(descriptor.stiffSpring);
		return constraint;
	}


	bhkSerializableRef convert_malleable(bhkMalleableConstraintRef malleable) {
		//Malleables really don't suit skyrim afaik
		switch (malleable->GetMalleable().type) {
			case BALLANDSOCKET:
				return create_ball_socket(malleable->GetMalleable());
			case HINGE:
				return create_hinge(malleable->GetMalleable());
			case LIMITED_HINGE:
				return create_limited_hinge(malleable->GetMalleable());
			case PRISMATIC:
				return create_prismatic(malleable->GetMalleable());
			case RAGDOLL:
				return create_ragdoll(malleable->GetMalleable());
			case STIFFSPRING:
				return create_stiff_spring(malleable->GetMalleable());
			case MALLEABLE:
				throw runtime_error("Nested Malleable constraints!");
			default:
				throw runtime_error("Unknown malleable inner type!");
		}
		return NULL;
	}

	const NifInfo& this_info;

public:
	Accessor(bhkRigidBody& obj, const NifInfo& info) : this_info(info) {
		//zero out
		obj.unknownInt = 0;

		obj.unusedByte1 = (byte)0;
		obj.unknownInt1 = (unsigned int)0;
		obj.unknownInt2 = (unsigned int)0;
		obj.unusedByte2 = (byte)0;
		obj.timeFactor = (float)1.0;
		obj.gravityFactor = (float)1.0;
		obj.rollingFrictionMultiplier = (float)0.0;

		obj.enableDeactivation = obj.solverDeactivation != SOLVER_DEACTIVATION_OFF;
		obj.unknownFloat1 = (float)0.0;

		obj.unknownBytes1 = { 0,0,0,0,0,0,0,0,0,0,0,0 };
		obj.unknownBytes2 = { 0,0,0,0 };

		//convert
		obj.havokFilter.layer_sk = convert_havok_layer(obj.havokFilter.layer_ob);
		obj.havokFilterCopy = obj.havokFilter;

		obj.translation.x *= COLLISION_RATIO;
		obj.translation.y *= COLLISION_RATIO;
		obj.translation.z *= COLLISION_RATIO;

		obj.center.x *= COLLISION_RATIO;
		obj.center.y *= COLLISION_RATIO;
		obj.center.z *= COLLISION_RATIO;

		//bhkMalleableConstraints are no more supported I guess;
		for (int i = 0; i < obj.constraints.size(); i++) {
			if (obj.constraints[i]->IsDerivedType(bhkMalleableConstraint::TYPE)) {
				obj.constraints[i] = convert_malleable(DynamicCast<bhkMalleableConstraint>(obj.constraints[i]));
			}
		}

		//Seems like the old havok settings must be deactivated
		obj.motionSystem = MO_SYS_BOX_INERTIA;
		obj.qualityType = MO_QUAL_FIXED;

		//obsolete collisions
		if (obj.shape->IsSameType(bhkMoppBvTreeShape::TYPE) ||
			obj.shape->IsSameType(bhkNiTriStripsShape::TYPE) ||
			obj.shape->IsSameType(bhkPackedNiTriStripsShape::TYPE)) {
			obj.shape = upgrade_shape(obj.shape, this_info);
		}
			
	}
};

BSFadeNode* convert_root(NiObject* root)
{
	int numref = root->GetNumRefs();
	void* fadeNodeMem = (BSFadeNode*)malloc(sizeof(BSFadeNode));
	BSFadeNode* fadeNode = new (fadeNodeMem) BSFadeNode();

	//trick to overcome strong types inside refobjects;
	memcpy(root, fadeNode, sizeof(NiObject));
	for (int i = 0; i < numref; i++)
		root->AddRef();
	//root->AddRef();
	free(fadeNodeMem);

	return (BSFadeNode*)root;
}

NiTriShapeRef convert_strip(NiTriStripsRef& stripsRef)
{
	//Convert NiTriStrips to NiTriShapes first of all.
	NiTriShapeRef shapeRef = new NiTriShape();
	shapeRef->SetName(stripsRef->GetName());
	shapeRef->SetExtraDataList(stripsRef->GetExtraDataList());
	shapeRef->SetTranslation(stripsRef->GetTranslation());
	shapeRef->SetRotation(stripsRef->GetRotation());
	shapeRef->SetScale(stripsRef->GetScale());
	shapeRef->SetFlags(524302);
	shapeRef->SetData(stripsRef->GetData());
	shapeRef->SetShaderProperty(stripsRef->GetShaderProperty());
	shapeRef->SetProperties(stripsRef->GetProperties());

	//Then do the data..
	bool hasAlpha = false;

	NiTriStripsDataRef stripsData = DynamicCast<NiTriStripsData>(stripsRef->GetData());
	NiTriShapeDataRef shapeData = new  NiTriShapeData();

	shapeData->SetHasVertices(stripsData->GetHasVertices());
	shapeData->SetVertices(stripsData->GetVertices());
	shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(stripsData->GetVectorFlags()));
	shapeData->SetUvSets(stripsData->GetUvSets());
	if (!shapeData->GetUvSets().empty())
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(shapeData->GetBsVectorFlags() | BSVF_HAS_UV));
	shapeData->SetCenter(stripsData->GetCenter());
	shapeData->SetRadius(stripsData->GetRadius());
	shapeData->SetHasVertexColors(stripsData->GetHasVertexColors());
	shapeData->SetVertexColors(stripsData->GetVertexColors());
	shapeData->SetConsistencyFlags(stripsData->GetConsistencyFlags());
	vector<Triangle> triangles = triangulate(stripsData->GetPoints());
	shapeData->SetNumTriangles(triangles.size());
	shapeData->SetNumTrianglePoints(triangles.size() * 3);
	shapeData->SetHasTriangles(1);
	shapeData->SetTriangles(triangles);

	shapeData->SetHasNormals(stripsData->GetHasNormals());
	shapeData->SetNormals(stripsData->GetNormals());

	vector<Vector3> vertices = shapeData->GetVertices();
	Vector3 COM;
	if (vertices.size() != 0)
		COM = (COM / 2) + (ckcmd::Geometry::centeroid(vertices) / 2);
	vector<Triangle> faces = shapeData->GetTriangles();
	vector<Vector3> normals = shapeData->GetNormals();
	if (vertices.size() != 0 && faces.size() != 0 && shapeData->GetUvSets().size() != 0) {
		vector<TexCoord> uvs = shapeData->GetUvSets()[0];
		TriGeometryContext g(vertices, COM, faces, uvs, normals);
		shapeData->SetHasNormals(1);
		//recalculate
		shapeData->SetNormals(g.normals);
		shapeData->SetTangents(g.tangents);
		shapeData->SetBitangents(g.bitangents);
		if (vertices.size() != g.normals.size() || vertices.size() != g.tangents.size() || vertices.size() != g.bitangents.size())
			throw runtime_error("Geometry mismatch!");
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(shapeData->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
	}
	else {
		shapeData->SetTangents(stripsData->GetTangents());
		shapeData->SetBitangents(stripsData->GetBitangents());
	}

	shapeRef->SetData(DynamicCast<NiGeometryData>(shapeData));

	//TODO: shared normals no more supported
	shapeData->SetMatchGroups(vector<MatchGroup>{});

	return shapeRef;
}

class ConverterVisitor : public RecursiveFieldVisitor<ConverterVisitor> {
	const NifInfo& this_info;
	set<void*> already_upgraded;
public:
	ConverterVisitor(const NifInfo& info) :
		RecursiveFieldVisitor(*this, info), this_info(info)
	{}
	template<class T>
	inline void visit_object(T& obj) {}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<class T>
	inline void visit_field(T& obj) {}

	template<>
	inline void visit_object(NiNode& obj) {
		vector<Ref<NiAVObject>> children = obj.GetChildren();
		int index = 0;
		for (NiAVObjectRef& block : children)
		{
			if (block == NULL) {
				children.erase(children.begin() + index);
				continue;
			}
			if (block->IsSameType(NiTriStrips::TYPE)) {
				NiTriStripsRef stripsRef = DynamicCast<NiTriStrips>(block);
				if (stripsRef->IsSameType(NiTriStrips::TYPE)) {
					NiTriShapeRef shape = convert_strip(stripsRef);
					children[index] = shape;
				}
			}

			index++;
		}
		//TODO
		//properties are deprecated
		obj.SetProperties(vector<NiPropertyRef>{});
		obj.SetChildren(children);
	}

	template<>
	inline void visit_object(NiBillboardNode& obj) {
		vector<Ref<NiAVObject>> children = obj.GetChildren();
		int index = 0;
		for (NiAVObjectRef& block : children)
		{
			if (block == NULL) {
				children.erase(children.begin() + index);
				continue;
			}
			if (block->IsSameType(NiTriStrips::TYPE)) {
				NiTriStripsRef stripsRef = DynamicCast<NiTriStrips>(block);
				if (stripsRef->IsSameType(NiTriStrips::TYPE)) {
					NiTriShapeRef shape = convert_strip(stripsRef);
					children[index] = shape;
				}
			}

			index++;
		}
		//TODO
		//properties are deprecated
		obj.SetProperties(vector<NiPropertyRef>{});
		obj.SetChildren(children);
	}

	template<>
	inline void visit_object(NiTriShapeData& obj) {
		VectorFlags vf = obj.GetVectorFlags();
		BSVectorFlags bvf = obj.GetBsVectorFlags();
		if (vf & VF_UV_2 || /*!< VF_UV_2 */
			vf & VF_UV_4 || /*!< VF_UV_4 */
			vf & VF_UV_8 || /*!< VF_UV_8 */
			vf & VF_UV_16 || /*!< VF_UV_16 */
			vf & VF_UV_32 /*!< VF_UV_32 */)
		{
			throw runtime_error("VF Unhandled");
		}

		if (vf != 0) {
			obj.SetBsVectorFlags(static_cast<BSVectorFlags>(obj.GetBsVectorFlags() | obj.GetVectorFlags()));
		}

		//TODO: shared normals no more supported
		obj.SetMatchGroups(vector<MatchGroup>{});
	}

	template<>
	inline void visit_object(NiTriShape& obj) {
		bool hasAlpha = false;

		BSLightingShaderPropertyRef lightingProperty = new BSLightingShaderProperty();
		BSShaderTextureSetRef textureSet = new BSShaderTextureSet();
		NiMaterialPropertyRef material = new NiMaterialProperty();
		NiTexturingPropertyRef texturing = new NiTexturingProperty();
		vector<Ref<NiProperty>> properties = obj.GetProperties();

		for (NiPropertyRef property : properties)
		{
			if (property->IsSameType(NiMaterialProperty::TYPE)) {
				material = DynamicCast<NiMaterialProperty>(property);
				lightingProperty->SetShaderType(BSShaderType::SHADER_DEFAULT);
				lightingProperty->SetEmissiveColor(material->GetEmissiveColor());
				lightingProperty->SetSpecularColor(material->GetSpecularColor());
				lightingProperty->SetEmissiveMultiple(1);
				lightingProperty->SetGlossiness(material->GetGlossiness());
				lightingProperty->SetAlpha(material->GetAlpha());
			}
			if (property->IsSameType(NiTexturingProperty::TYPE)) {
				texturing = DynamicCast<NiTexturingProperty>(property);
				string textureName;
				if (texturing->GetBaseTexture().source != NULL) {
					textureName += texturing->GetBaseTexture().source->GetFileName();
					//fix for orconebraid
					if (textureName == "Grey.dds")
						textureName = "textures\\characters\\hair\\Grey.dds";

					textureName.insert(9, "tes4\\");
					string textureNormal = textureName;
					textureNormal.erase(textureNormal.end() - 4, textureNormal.end());
					textureNormal += "_n.dds";

					//setup textureSet (TODO)
					std::vector<std::string> textures(9);
					textures[0] = textureName;
					textures[1] = textureNormal;

					//finally set them.
					textureSet->SetTextures(textures);
				}
			}
			if (property->IsSameType(NiStencilProperty::TYPE)) {
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() + SkyrimShaderPropertyFlags2::SLSF2_DOUBLE_SIDED));
			}
			if (property->IsSameType(NiAlphaProperty::TYPE)) {
				NiAlphaPropertyRef alpha = new NiAlphaProperty();
				alpha->SetFlags(DynamicCast<NiAlphaProperty>(property)->GetFlags());
				alpha->SetThreshold(DynamicCast<NiAlphaProperty>(property)->GetThreshold());
				obj.SetAlphaProperty(alpha);
			}
		}
		lightingProperty->SetTextureSet(textureSet);
		obj.SetShaderProperty(DynamicCast<BSShaderProperty>(lightingProperty));
		obj.SetExtraDataList(vector<Ref<NiExtraData>> {});
		obj.SetProperties(vector<Ref<NiProperty>> {});
	}

	template<>
	inline void visit_object(NiControllerManager& obj)
	{
		//obj.SetTarget(DynamicCast<NiAVObject>(rootNode));
	}

	template<>
	inline void visit_object(NiMultiTargetTransformController& obj)
	{
		//obj.SetTarget(DynamicCast<NiAVObject>(rootNode));
	}

	template<>
	inline void visit_object(NiPSysData& obj) {
		//TODO: how do we handle this geometry then?
		//NiPSysData no longer inherits geometry, so clear out
		obj.SetBsMaxVertices(obj.GetVertices().size());
		obj.SetVertices(vector<Vector3>{});
		obj.SetHasVertices(false);
		obj.SetVertexColors(vector<Color4>{});
		obj.SetHasVertexColors(false);
	}

	template<>
	inline void visit_object(NiParticleSystem& obj) {
		//TODO: I don't even know how particle systems work in skyrim
		obj.SetProperties(vector<NiPropertyRef>{});
	}

	template<>
	inline void visit_object(NiControllerSequence& obj)
	{
		vector<ControlledBlock> blocks = obj.GetControlledBlocks();
		vector<ControlledBlock> nblocks;

		//for some reason, oblivion's NIF blocks have empty NiTransforms, time to remove.
		for (int i = 0; i != blocks.size(); i++) {
			NiInterpolator* intp = blocks[i].interpolator;
			if (intp == NULL)
				continue;
			if (intp->IsDerivedType(NiTransformInterpolator::TYPE)) {
				NiTransformInterpolator* tintp = DynamicCast<NiTransformInterpolator>(intp);
				if (tintp->GetData() == NULL)
					continue;
			}
			//Deprecated. Maybe we can handle with tri facegens
			if (blocks[i].controller != NULL && blocks[i].controller->IsDerivedType(NiGeomMorpherController::TYPE))
				continue;

			if (blocks[i].stringPalette != NULL) {
				blocks[i].nodeName = blocks[i].stringPalette->GetPalette().palette.substr(blocks[i].nodeNameOffset);
				blocks[i].controllerType = blocks[i].stringPalette->GetPalette().palette.substr(blocks[i].controllerTypeOffset);
			}

			//set to default... if above doesn't work
			if (blocks[i].controllerType == "")
				blocks[i].controllerType = "NiTransformController";

			blocks[i].stringPalette = NULL;
			nblocks.push_back(blocks[i]);
		}
		obj.SetControlledBlocks(nblocks);
		obj.SetStringPalette(NULL);
	}

	template<>
	inline void visit_object(NiTransformController& obj)
	{
		//disable geomorph on ghost skeleton
		if (obj.GetNextController() != NULL && obj.GetNextController()->IsDerivedType(NiGeomMorpherController::TYPE))
			obj.SetNextController(NULL);

	}

	template<>
	inline void visit_object(NiTextKeyExtraData& obj)
	{
		vector<Key<IndexString>> textKeys = obj.GetTextKeys();

		for (int i = 0; i != textKeys.size(); i++) {
			if (std::strstr(textKeys[i].data.c_str(), "Sound:")) {
				textKeys[i].data.insert(7, "TES4");
			}
		}

		obj.SetTextKeys(textKeys);

	}

	/*
	bhkCollisionObject/bhkRigidBody/bhkBoxShape/
	bhkCollisionObject/bhkRigidBody/bhkCapsuleShape/
	bhkCollisionObject/bhkRigidBody/bhkConvexVerticesShape/
	bhkCollisionObject/bhkRigidBody/bhkHingeConstraint/
	bhkCollisionObject/bhkRigidBody/bhkLimitedHingeConstraint/
	bhkCollisionObject/bhkRigidBody/bhkListShape/bhkCapsuleShape/
	bhkCollisionObject/bhkRigidBody/bhkListShape/bhkConvexTransformShape/bhkBoxShape/
	bhkCollisionObject/bhkRigidBody/bhkListShape/bhkConvexTransformShape/bhkSphereShape/
	bhkCollisionObject/bhkRigidBody/bhkListShape/bhkConvexVerticesShape/
	bhkCollisionObject/bhkRigidBody/bhkListShape/bhkMultiSphereShape/
	bhkCollisionObject/bhkRigidBody/bhkMalleableConstraint/
	bhkCollisionObject/bhkRigidBody/bhkMoppBvTreeShape/bhkNiTriStripsShape/NiTriStripsData/
	bhkCollisionObject/bhkRigidBody/bhkMoppBvTreeShape/bhkPackedNiTriStripsShape/hkPackedNiTriStripsData/
	bhkCollisionObject/bhkRigidBody/bhkNiTriStripsShape/NiTriStripsData/
	bhkCollisionObject/bhkRigidBody/bhkPrismaticConstraint/
	bhkCollisionObject/bhkRigidBody/bhkRagdollConstraint/
	bhkCollisionObject/bhkRigidBody/bhkSphereShape/
	bhkCollisionObject/bhkRigidBody/bhkStiffSpringConstraint/
	bhkCollisionObject/bhkRigidBodyT/bhkBoxShape/
	bhkCollisionObject/bhkRigidBodyT/bhkCapsuleShape/
	bhkCollisionObject/bhkRigidBodyT/bhkConvexVerticesShape/
	bhkCollisionObject/bhkRigidBodyT/bhkLimitedHingeConstraint/
	bhkCollisionObject/bhkRigidBodyT/bhkListShape/bhkCapsuleShape/
	bhkCollisionObject/bhkRigidBodyT/bhkListShape/bhkConvexTransformShape/bhkBoxShape/
	bhkCollisionObject/bhkRigidBodyT/bhkListShape/bhkConvexTransformShape/bhkSphereShape/
	bhkCollisionObject/bhkRigidBodyT/bhkListShape/bhkConvexVerticesShape/
	bhkCollisionObject/bhkRigidBodyT/bhkListShape/bhkTransformShape/bhkConvexSweepShape/bhkBoxShape/
	bhkCollisionObject/bhkRigidBodyT/bhkMoppBvTreeShape/bhkMeshShape/NiTriStripsData/
	bhkCollisionObject/bhkRigidBodyT/bhkMoppBvTreeShape/bhkNiTriStripsShape/NiTriStripsData/
	bhkCollisionObject/bhkRigidBodyT/bhkMoppBvTreeShape/bhkPackedNiTriStripsShape/hkPackedNiTriStripsData/
	bhkCollisionObject/bhkRigidBodyT/bhkNiTriStripsShape/NiTriStripsData/
	bhkCollisionObject/bhkRigidBodyT/bhkPrismaticConstraint/
	bhkCollisionObject/bhkRigidBodyT/bhkRagdollConstraint/
	bhkCollisionObject/bhkRigidBodyT/bhkSphereShape/
	*/

	//from now on, we must switch from an old type to hkTransform, so it is useful to directly use havok data

	static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v) {
		return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
	}

	static inline Niflib::Vector4 TOVECTOR4(const hkVector4& v) {
		return Niflib::Vector4(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
	}

	static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v) {
		return hkVector4(v.x, v.y, v.z, v.w);
	}

	static inline Niflib::Quaternion TOQUAT(const ::hkQuaternion& q, bool inverse = false) {
		Niflib::Quaternion qt(q.m_vec.getSimdAt(3), q.m_vec.getSimdAt(0), q.m_vec.getSimdAt(1), q.m_vec.getSimdAt(2));
		return inverse ? qt.Inverse() : qt;
	}

	static inline ::hkQuaternion TOQUAT(const Niflib::Quaternion& q, bool inverse = false) {
		hkVector4 v(q.x, q.y, q.z, q.w);
		v.normalize4();
		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
		if (inverse) qt.setInverse(qt);
		return qt;
	}

	static inline ::hkQuaternion TOQUAT(const Niflib::hkQuaternion& q, bool inverse = false) {
		hkVector4 v(q.x, q.y, q.z, q.w);
		v.normalize4();
		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
		if (inverse) qt.setInverse(qt);
		return qt;
	}

	static inline hkMatrix3 TOMATRIX3(const Niflib::InertiaMatrix& q, bool inverse = false) {
		hkMatrix3 m3;
		m3.setCols(TOVECTOR4(q.rows[0]), TOVECTOR4(q.rows[1]), TOVECTOR4(q.rows[2]));
		if (inverse) m3.invert(0.001);
	}

	static inline Vector4 HKMATRIXROW(const hkTransform& q, const unsigned int row) {
		return Vector4(q(row,0),q(row,1),q(row,2),q(row,3));
	}



	template<>
	inline void visit_object(bhkRigidBody& obj) {
		if (already_upgraded.insert(&obj).second)
			Accessor<bhkRigidBodyUpgrader> upgrader(obj, this_info);
	}

	template<>
	inline void visit_object(bhkRigidBodyT& obj) {
		if (already_upgraded.insert(&obj).second)
			Accessor<bhkRigidBodyUpgrader> upgrader(obj, this_info);
	}

	//Upgrade shapes

	template<typename T> void convertMaterialAndRadius(T& shape) {
		HavokMaterial material = shape.GetMaterial();
		material.material_sk = convert_havok_material(material.material_ob);
		shape.SetMaterial(material);
		shape.SetRadius(shape.GetRadius() * COLLISION_RATIO);
	}

	//Containers
	template<>
	inline void visit_object(bhkConvexTransformShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
			Matrix44 transform = obj.GetTransform();
			transform[3][0] = transform[3][0] * COLLISION_RATIO;
			transform[3][1] = transform[3][1] * COLLISION_RATIO;
			transform[3][2] = transform[3][2] * COLLISION_RATIO;
			obj.SetTransform(transform);
			obj.SetShape(upgrade_shape(obj.GetShape(), this_info));
		}
	}

	template<>
	inline void visit_object(bhkTransformShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
			Matrix44 transform = obj.GetTransform();
			transform[3][0] = transform[3][0] * COLLISION_RATIO;
			transform[3][1] = transform[3][1] * COLLISION_RATIO;
			transform[3][2] = transform[3][2] * COLLISION_RATIO;
			obj.SetTransform(transform);
			obj.SetShape(upgrade_shape(obj.GetShape(), this_info));
		}
	}

	template<>
	inline void visit_object(bhkListShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			HavokMaterial material = obj.GetMaterial();
			material.material_sk = convert_havok_material(material.material_ob);
			obj.SetMaterial(material);
			obj.SetSubShapes(upgrade_shapes(obj.GetSubShapes(), this_info));
		}
	}

	//TODO: need to upgrade shapes into containers



	//Shapes
	template<>
	inline void visit_object(bhkSphereShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
		}
	}

	template<>
	inline void visit_object(bhkBoxShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
			Vector3 half_extent = obj.GetDimensions();
			half_extent.x *= COLLISION_RATIO;
			half_extent.y *= COLLISION_RATIO;
			half_extent.z *= COLLISION_RATIO;
			obj.SetDimensions(half_extent);
		}
	}

	template<>
	inline void visit_object(bhkCapsuleShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			HavokMaterial material = obj.GetMaterial();
			material.material_sk = convert_havok_material(material.material_ob);
			obj.SetMaterial(material);
			obj.SetRadius1(obj.GetRadius1() * COLLISION_RATIO);
			obj.SetRadius2(obj.GetRadius2() * COLLISION_RATIO);
			obj.SetFirstPoint(obj.GetFirstPoint() * COLLISION_RATIO);
			obj.SetSecondPoint(obj.GetSecondPoint() * COLLISION_RATIO);
		}
	}

	template<>
	inline void visit_object(bhkConvexVerticesShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
			vector<Vector4> vertices = obj.GetVertices();
			for (Vector4& v : vertices) {
				v.x *= COLLISION_RATIO;
				v.y *= COLLISION_RATIO;
				v.z *= COLLISION_RATIO;
			}
			obj.SetVertices(vertices);
			vector<Vector4> normals = obj.GetNormals();
			for (Vector4& n : normals) {
				n.w *= COLLISION_RATIO;
			}
			obj.SetNormals(normals);
		}
	}

	//Upgrade Constraints

	template<>
	void visit_compound(HingeDescriptor& descriptor) {
		if (already_upgraded.insert(&descriptor).second) {
			hkpHingeConstraintData data;
			data.setInBodySpace(
				TOVECTOR4(descriptor.parentSpace.pivot),
				TOVECTOR4(descriptor.childSpace.pivot),
				TOVECTOR4(descriptor.parentSpace.axis),
				TOVECTOR4(descriptor.childSpace.axis)
			);

			hkTransform& hkA = data.m_atoms.m_transforms.m_transformA;
			hkTransform& hkB = data.m_atoms.m_transforms.m_transformB;

			descriptor.axleA = HKMATRIXROW(hkA, 0);
			descriptor.perp2AxleInA1 = HKMATRIXROW(hkA, 1);
			descriptor.perp2AxleInA2 = HKMATRIXROW(hkA, 2);
			descriptor.pivotA = TOVECTOR4(hkA.getTranslation());

			descriptor.axleB = HKMATRIXROW(hkB, 0);
			descriptor.perp2AxleInB1 = HKMATRIXROW(hkB, 1);
			descriptor.perp2AxleInB2 = HKMATRIXROW(hkB, 2);
			descriptor.pivotB = TOVECTOR4(hkB.getTranslation());
		}
	}

	template<>
	void visit_compound(LimitedHingeDescriptor& descriptor) {
		if (already_upgraded.insert(&descriptor).second) {
			hkpLimitedHingeConstraintData data;
			data.setInBodySpace(
				TOVECTOR4(descriptor.parentSpace.pivot),
				TOVECTOR4(descriptor.childSpace.pivot),
				TOVECTOR4(descriptor.parentSpace.referenceSystem.xAxis),
				TOVECTOR4(descriptor.childSpace.referenceSystem.xAxis),
				TOVECTOR4(descriptor.parentSpace.referenceSystem.yAxis),
				TOVECTOR4(descriptor.childSpace.referenceSystem.yAxis)
			);

			hkTransform& hkA = data.m_atoms.m_transforms.m_transformA;
			hkTransform& hkB = data.m_atoms.m_transforms.m_transformB;

			descriptor.axleA = HKMATRIXROW(hkA, 0);
			descriptor.perp2AxleInA1 = HKMATRIXROW(hkA, 1);
			descriptor.perp2AxleInA2 = HKMATRIXROW(hkA, 2);
			descriptor.pivotA = TOVECTOR4(hkA.getTranslation());

			descriptor.axleB = HKMATRIXROW(hkB, 0);
			descriptor.perp2AxleInB1 = HKMATRIXROW(hkB, 1);
			descriptor.perp2AxleInB2 = HKMATRIXROW(hkB, 2);
			descriptor.pivotB = TOVECTOR4(hkB.getTranslation());
		}
	}

	template<>
	void visit_compound(BallAndSocketDescriptor& descriptor) {
		//Nothing to do;
	}

	template<>
	void visit_compound(PrismaticDescriptor& descriptor) {
		if (already_upgraded.insert(&descriptor).second) {
			hkpPrismaticConstraintData data;
			data.setInBodySpace(
				TOVECTOR4(descriptor.parentSpace.pivot),
				TOVECTOR4(descriptor.childSpace.pivot),
				TOVECTOR4(descriptor.parentSpace.referenceSystem.xAxis),
				TOVECTOR4(descriptor.childSpace.referenceSystem.xAxis),
				TOVECTOR4(descriptor.parentSpace.referenceSystem.yAxis),
				TOVECTOR4(descriptor.childSpace.referenceSystem.yAxis)
			);

			//TODO: Check if plane has to be used

			hkTransform& hkA = data.m_atoms.m_transforms.m_transformA;
			hkTransform& hkB = data.m_atoms.m_transforms.m_transformB;

			descriptor.slidingA = HKMATRIXROW(hkA, 0);
			descriptor.rotationA = HKMATRIXROW(hkA, 1);
			descriptor.planeA = HKMATRIXROW(hkA, 2);
			descriptor.pivotA = TOVECTOR4(hkA.getTranslation());

			descriptor.slidingB = HKMATRIXROW(hkB, 0);
			descriptor.rotationB = HKMATRIXROW(hkB, 1);
			descriptor.planeB = HKMATRIXROW(hkB, 2);
			descriptor.pivotB = TOVECTOR4(hkB.getTranslation());
		}
	}

	template<>
	void visit_compound(StiffSpringDescriptor& descriptor) {
		//Nothing to do;
	}

	template<>
	void visit_compound(RagdollDescriptor& descriptor) {
		if (already_upgraded.insert(&descriptor).second) {
			hkpRagdollConstraintData data;
			data.setInBodySpace(
				TOVECTOR4(descriptor.parentSpace.pivot),
				TOVECTOR4(descriptor.childSpace.pivot),
				TOVECTOR4(descriptor.parentSpace.referenceSystem.xAxis),
				TOVECTOR4(descriptor.childSpace.referenceSystem.xAxis),
				TOVECTOR4(descriptor.parentSpace.referenceSystem.yAxis),
				TOVECTOR4(descriptor.childSpace.referenceSystem.yAxis)
			);

			hkTransform& hkA = data.m_atoms.m_transforms.m_transformA;
			hkTransform& hkB = data.m_atoms.m_transforms.m_transformB;

			descriptor.twistA = HKMATRIXROW(hkA, 0);
			descriptor.planeA = HKMATRIXROW(hkA, 1);
			descriptor.motorA = HKMATRIXROW(hkA, 2);
			descriptor.pivotA = TOVECTOR4(hkA.getTranslation());

			descriptor.twistB = HKMATRIXROW(hkB, 0);
			descriptor.planeB = HKMATRIXROW(hkB, 1);
			descriptor.motorB = HKMATRIXROW(hkB, 2);
			descriptor.pivotB = TOVECTOR4(hkB.getTranslation());
		}
	}

	//COLLISIONS TODO:
	template<>
	inline void visit_object(bhkNiTriStripsShape& obj)
	{
		obj.SetStripsData(vector<NiTriStripsDataRef>{});
	}
};


class RebuildVisitor : public RecursiveFieldVisitor<RebuildVisitor> {
	set<NiObject*> objects;
public:
	vector<NiObjectRef> blocks;

	RebuildVisitor(NiObject* root, const NifInfo& info) :
		RecursiveFieldVisitor(*this, info) {
		root->accept(*this, info);
		
		for (NiObject* ptr : objects) {
			blocks.push_back(ptr);
		}
	}


	template<class T>
	inline void visit_object(T& obj) {
		objects.insert(&obj);
	}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<class T>
	inline void visit_field(T& obj) {}
};

void findFiles(fs::path startingDir, string extension, vector<fs::path>& results) {
	if (!exists(startingDir) || !is_directory(startingDir)) return;
	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(startingDir))
	{
		if (is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == extension) {
			results.push_back(dirEntry.path().string());
		}
	}
}


bool BeginConversion() {
	char fullName[MAX_PATH], exeName[MAX_PATH];
	GetModuleFileName(NULL, fullName, MAX_PATH);
	_splitpath(fullName, NULL, NULL, exeName, NULL);

	NifInfo info;
	vector<fs::path> nifs;

	findFiles(nif_in, ".nif", nifs);

	if (nifs.empty()) {
		Log::Info("No NIFs found.. trying BSAs");

		Games& games = Games::Instance();
		const Games::GamesPathMapT& installations = games.getGames();

		for (const auto& bsa : games.bsas(Games::TES4)) {
			std::cout << "Checking: " << bsa.filename() << std::endl;
			BSAFile bsa_file(bsa);
			for (const auto& nif : bsa_file.assets(".*\.nif")) {
				Log::Info("Current File: %s", nif.c_str());

				if (nif.find("meshes\\landscape\\lod") != string::npos) {
					Log::Warn("Ignored LOD file: %s", nif.c_str());
					continue;
				}
				if (nif.find("\\marker_") != string::npos) {
					Log::Warn("Ignored marker file: %s", nif.c_str());
					continue;
				}
				if (nif.find("\\minotaurold") != string::npos) {
					Log::Warn("Ignored malformed file: %s", nif.c_str());
					continue;
				}
				if (nif.find("\\sky\\") != string::npos) {
					Log::Warn("Ignored obsolete sky nifs: %s", nif.c_str());
					continue;
				}

				size_t size = -1;
				const uint8_t* data = bsa_file.extract(nif, size);

				std::string sdata((char*)data, size);
				std::istringstream iss(sdata);

				vector<NiObjectRef> blocks = ReadNifList(iss, &info);
				NiObjectRef root = GetFirstRoot(blocks);
				NiNode* rootn = DynamicCast<NiNode>(root);

				ConverterVisitor fimpl(info);
				root->accept(fimpl, info);
				if (!NifFile::hasExternalSkinnedMesh(blocks, rootn)) {
					root = convert_root(root);
				}

				info.userVersion = 12;
				info.userVersion2 = 83;
				info.version = Niflib::VER_20_2_0_7;

				fs::path out_path = nif_out / nif;
				fs::create_directories(out_path.parent_path());
				WriteNifTree(out_path.string(), root, info);
				// Ensure valid
				NifFile check(out_path.string());
				NiObject* lroot = check.GetRoot();
				if (lroot == NULL)
					throw runtime_error("Error converting");
				delete data;
			}
		}
	}
	else {

		for (size_t i = 0; i < nifs.size(); i++) {
			Log::Info("Current File: %s", nifs[i].string().c_str());

			vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
			NiObjectRef root = GetFirstRoot(blocks);
			NiNode* rootn = DynamicCast<NiNode>(root);

			ConverterVisitor fimpl(info);
			root->accept(fimpl, info);

			info.userVersion = 12;
			info.userVersion2 = 83;
			info.version = Niflib::VER_20_2_0_7;

			if (!NifFile::hasExternalSkinnedMesh(blocks, rootn)) {			
				root = convert_root(root);

				//to calculate the right flags, we need to rebuild the blocks
				vector<NiObjectRef> new_blocks = RebuildVisitor(root, info).blocks;

				bsx_flags_t calculated_flags = calculateSkyrimBSXFlags(new_blocks, info);
				//if (calculated_flags[7] == true) {
				//	//has a single chain, let's promote it. EXPERIMENTAL
				//	BSFadeNodeRef bsroot = DynamicCast<BSFadeNode>(root);
				//	for (NiObjectRef ref : new_blocks) {
				//		if (ref->IsDerivedType(bhkCollisionObject::TYPE)) {
				//			bhkCollisionObjectRef bsc = DynamicCast<bhkCollisionObject>(ref);
				//			if (bsc->GetBody() != NULL && bsc->GetBody()->IsDerivedType(bhkRigidBody::TYPE)) {
				//				bhkRigidBodyRef bsbody = DynamicCast<bhkRigidBody>(bsc->GetBody());
				//				if (bsbody->GetConstraints().empty()) {
				//					bsroot->SetCollisionObject(DynamicCast<NiCollisionObject>(bsc));
				//					calculated_flags[3] = false;
				//				}
				//			}
				//		}
				//	}
				//}


				for (NiObjectRef ref : blocks) {
					if (ref->IsDerivedType(BSXFlags::TYPE)) {
						BSXFlagsRef bref = DynamicCast<BSXFlags>(ref);
						bref->SetIntegerData(calculated_flags.to_ulong());
					}
				}
			}



			fs::path out_path = nif_out / nifs[i].filename();
			fs::create_directories(out_path.parent_path());
			WriteNifTree(out_path.string(), root, info);

		}
	}
	Log::Info("Done");
	return true;
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("About - Help about this program."); break;
	case hkxcmd::htLong: {
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s about", exeName);
		Log::Info("  Prints additional information about this program.");
	}
						 break;
	}
}

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

static bool ExecuteCmd(hkxcmdLine &cmdLine) {
	BeginConversion();
	return true;
}

REGISTER_COMMAND(ConvertNif, HelpString, ExecuteCmd);