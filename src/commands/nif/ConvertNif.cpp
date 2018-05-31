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

template<>
class Accessor<bhkRigidBodyUpgrader> {
public:
	Accessor<bhkRigidBodyUpgrader>(bhkRigidBody& obj) {
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

	//void convert_wo(bhkWorldObjectRef wo)
	//{
	//	if (wo == NULL)
	//		throw runtime_error("NULL bhkWorldObjectRef!");

	//	int numref = wo->GetNumRefs();

	//	void* new_rb_mem = NULL;

	//	if (wo->IsSameType(bhkRigidBodyT::TYPE)) {
	//		new_rb_mem = malloc(sizeof(bhkRigidBodyT));
	//		new_rb_mem = new (new_rb_mem) bhkRigidBodyT();
	//		convert_rb((bhkRigidBody*)new_rb_mem, DynamicCast<bhkRigidBody>(wo));
	//		memcpy(&*wo, new_rb_mem, sizeof(bhkRigidBodyT));
	//	}
	//	else if (wo->IsSameType(bhkRigidBody::TYPE)) {
	//		new_rb_mem = malloc(sizeof(bhkRigidBody));
	//		new_rb_mem = new (new_rb_mem) bhkRigidBody();
	//		convert_rb((bhkRigidBody*)new_rb_mem, DynamicCast<bhkRigidBody>(wo));
	//		memcpy(&*wo, new_rb_mem, sizeof(bhkRigidBody));
	//	}
	//	else if (wo->IsSameType(bhkSimpleShapePhantom::TYPE)) {
	//		new_rb_mem = malloc(sizeof(bhkSimpleShapePhantom));
	//		new_rb_mem = new (new_rb_mem) bhkSimpleShapePhantom();
	//		convert_sp((bhkSimpleShapePhantom*)new_rb_mem, DynamicCast<bhkSimpleShapePhantom>(wo));
	//		memcpy(&*wo, new_rb_mem, sizeof(bhkSimpleShapePhantom));
	//	}
	//	else {
	//		throw runtime_error("Unknown bhkWorldObjectRef! " + wo->GetInternalType().GetTypeName());
	//	}

	//	//trick to overcome strong types inside refobjects;

	//	for (int i = 0; i < numref; i++)
	//		wo->AddRef();
	//	free(new_rb_mem);

	//}

	

	



	//void convert_rb(bhkRigidBody* rb_dest, const bhkRigidBodyRef rb_source)
	//{
	//	rb_dest->SetShape(convert_shape(rb_source->GetShape()));
	//	HavokFilter new_filter = rb_source->GetHavokFilter();
	//	new_filter.layer_sk = convert_havok_layer(new_filter.layer_ob);
	//	rb_dest->SetHavokFilter(new_filter);
	//	rb_dest->SetHavokFilterCopy(new_filter);
	//	rb_dest->SetBroadPhaseType(rb_source->GetBroadPhaseType());
	//	//CInfo is really an havok exported property and should probably not be changed;
	//	rb_dest->SetCollisionResponse(rb_source->GetCollisionResponse());
	//	rb_dest->SetProcessContactCallbackDelay(rb_source->GetProcessContactCallbackDelay());
	//	rb_dest->SetCollisionResponse2(rb_source->GetCollisionResponse2());
	//	rb_dest->SetProcessContactCallbackDelay2(rb_source->GetProcessContactCallbackDelay2());

	//	//Geometry
	//	Vector4 translation = rb_source->GetTranslation();
	//	translation.x *= COLLISION_RATIO;
	//	translation.y *= COLLISION_RATIO;
	//	translation.z *= COLLISION_RATIO;
	//	rb_dest->SetTranslation(translation);
	//	rb_dest->SetRotation(rb_source->GetRotation());
	//	//TODO: does velocity needs adjustments?
	//	rb_dest->SetLinearVelocity(rb_source->GetLinearVelocity());
	//	rb_dest->SetAngularVelocity(rb_source->GetAngularVelocity());
	//	//TODO: does inertia needs adjustments? Very tempted to calculate this
	//	rb_dest->SetInertiaTensor(rb_source->GetInertiaTensor());
	//	Vector4 center = rb_source->GetCenter();
	//	center.x *= COLLISION_RATIO;
	//	center.y *= COLLISION_RATIO;
	//	center.z *= COLLISION_RATIO;
	//	rb_dest->SetCenter(center);

	//	//Physics
	//	rb_dest->SetMass(rb_source->GetMass());
	//	rb_dest->SetLinearDamping(rb_source->GetLinearDamping());
	//	rb_dest->SetAngularDamping(rb_source->GetAngularDamping());
	//	//probably the rolling friction multiplier must be tuned to enable a simulation of static/dynamic friction, but havok sets it at 0 by default
	//	rb_dest->SetFriction(rb_source->GetFriction());
	//	rb_dest->SetRestitution(rb_source->GetRestitution());
	//	rb_dest->SetMaxLinearVelocity(rb_source->GetMaxLinearVelocity());
	//	rb_dest->SetMaxAngularVelocity(rb_source->GetMaxAngularVelocity());
	//	rb_dest->SetPenetrationDepth(rb_source->GetPenetrationDepth());

	//	//Physics Solver. There should be no difference between oblivion and skyrim
	//	rb_dest->SetMotionSystem(rb_source->GetMotionSystem());
	//	rb_dest->SetDeactivatorType(rb_source->GetDeactivatorType());
	//	//rb_dest->SetSolverDeactivation(rb_source->GetSolverDeactivation());
	//	rb_dest->SetEnableDeactivation(rb_source->GetSolverDeactivation() != SOLVER_DEACTIVATION_OFF);
	//	rb_dest->SetQualityType(rb_source->GetQualityType());

	//	//Constraints
	//	rb_dest->SetConstraints(rb_source->GetConstraints());

	//	//Gamebryo flags
	//	rb_dest->SetBodyFlags(rb_source->GetBodyFlags());

	//}

	void convert_sp(bhkSimpleShapePhantom* sp_dest, const bhkSimpleShapePhantomRef sp_source)
	{

	}

	void convert_constraint_descriptor(BallAndSocketDescriptor& descriptor) {
		descriptor.pivotA *= COLLISION_RATIO;
		descriptor.pivotB *= COLLISION_RATIO;
	}

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

	

	void convert_constraint_descriptor(PrismaticDescriptor& descriptor) {

	}

	void convert_constraint_descriptor(RagdollDescriptor& descriptor) {

	}

	void convert_constraint_descriptor(StiffSpringDescriptor& descriptor) {

	}

	bhkSerializableRef convert_constraint(bhkSerializableRef constraint) {
		//Malleables really don't suit skyrim afaik
		if (constraint->IsSameType(bhkMalleableConstraint::TYPE)) {
			//extract the inner constraint
			bhkMalleableConstraintRef malleable = DynamicCast<bhkMalleableConstraint>(constraint);
			switch (malleable->GetMalleable().type) {
			case BALLANDSOCKET:
			case HINGE:
			case LIMITED_HINGE:
			case PRISMATIC:
			case RAGDOLL:
			case STIFFSPRING:
			case MALLEABLE:
				throw runtime_error("Nested Malleable constraints!");
			}
		}

	}

	template<>
	inline void visit_object(bhkRigidBody& obj) {
		if (already_upgraded.insert(&obj).second)
			Accessor<bhkRigidBodyUpgrader> upgrader(obj);
	}

	template<>
	inline void visit_object(bhkRigidBodyT& obj) {
		if (already_upgraded.insert(&obj).second)
			Accessor<bhkRigidBodyUpgrader> upgrader(obj);
	}

	//Upgrade shapes

	template<typename T> void convertMaterialAndRadius(T& shape) {
		HavokMaterial material = shape.GetMaterial();
		material.material_sk = convert_havok_material(material.material_ob);
		shape.SetMaterial(material);
		shape.SetRadius(shape.GetRadius() * COLLISION_RATIO);
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