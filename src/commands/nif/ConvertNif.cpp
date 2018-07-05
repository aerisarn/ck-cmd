#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <core/HKXWrangler.h>
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

#include <Physics\Collide\Util\hkpTriangleUtil.h>

#include <limits>
#include <array>
#include <unordered_map>

static bool BeginConversion();
static void InitializeHavok();
static void CloseHavok();

REGISTER_COMMAND_CPP(ConvertNif)

ConvertNif::ConvertNif()
{
}

ConvertNif::~ConvertNif()
{
}

string ConvertNif::GetName() const
{
	return "ConvertNif";
}

string ConvertNif::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd convertnif
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + "\r\n";

	const char help[] = "TODO: Short description for ConvertNif";

	return usage + help;
}

string ConvertNif::GetHelpShort() const
{
	return "TODO: Short help message for ConvertNif";
}

bool ConvertNif::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	InitializeHavok();
	BeginConversion();
	CloseHavok();
	return true;
}

using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::Geometry;
using namespace ckcmd::NIF;
using namespace ckcmd::HKX;
using namespace ckcmd::nifscan;

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
	return Vector4(q(row, 0), q(row, 1), q(row, 2), q(row, 3));
}


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
		return SKY_HAV_MAT_STAIRS_WOOD;
	case OB_HAV_MAT_DIRT_STAIRS: /*!< Dirt Stairs */
		return SKY_HAV_MAT_STAIRS_BROKEN_STONE;
	case OB_HAV_MAT_GLASS_STAIRS: /*!< Glass Stairs */
		return SKY_HAV_MAT_STAIRS_STONE; // = 3739830338, /*!< Glass */
	case OB_HAV_MAT_GRASS_STAIRS: /*!< Grass Stairs */
		return SKY_HAV_MAT_STAIRS_WOOD; //= 1848600814, /*!< Grass */
	case OB_HAV_MAT_METAL_STAIRS: /*!< Metal Stairs */
		return SKY_HAV_MAT_STAIRS_STONE; // = 1288358971, /*!< Solid Metal */
	case OB_HAV_MAT_ORGANIC_STAIRS: /*!< Organic Stairs */
		return SKY_HAV_MAT_STAIRS_WOOD; // = 2974920155, /*!< Organic */
	case OB_HAV_MAT_SKIN_STAIRS: /*!< Skin Stairs */
		return SKY_HAV_MAT_STAIRS_WOOD; // = 591247106, /*!< Skin */
	case OB_HAV_MAT_WATER_STAIRS: /*!< Water Stairs */
		return SKY_HAV_MAT_STAIRS_SNOW; // = 1024582599, /*!< Water */
	case OB_HAV_MAT_WOOD_STAIRS: /*!< Wood Stairs */
		return SKY_HAV_MAT_STAIRS_WOOD; // = 1461712277, /*!< Stairs Wood */
	case OB_HAV_MAT_HEAVY_STONE_STAIRS: /*!< Heavy Stone Stairs */
		return SKY_HAV_MAT_STAIRS_STONE; // = 1570821952, /*!< Heavy Stone */
	case OB_HAV_MAT_HEAVY_METAL_STAIRS: /*!< Heavy Metal Stairs */
		return SKY_HAV_MAT_STAIRS_STONE; // = 2229413539, /*!< Heavy Metal */
	case OB_HAV_MAT_HEAVY_WOOD_STAIRS: /*!< Heavy Wood Stairs */
		return SKY_HAV_MAT_STAIRS_WOOD; // = 3070783559, /*!< Heavy Wood */
	case OB_HAV_MAT_CHAIN_STAIRS: /*!< Chain Stairs */
		return SKY_HAV_MAT_STAIRS_STONE; // = 3074114406, /*!< Material Chain */ TODO: maybe SKY_HAV_MAT_MATERIAL_CHAIN_METAL?
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

SkyrimHavokMaterial convert_material_to_stairs(SkyrimHavokMaterial to_convert) {
	switch (to_convert) {
	case SKY_HAV_MAT_MATERIAL_BOULDER_LARGE: // = 1885326971, /*!< Material Boulder Large */
	case SKY_HAV_MAT_MATERIAL_STONE_AS_STAIRS: // = 1886078335, /*!< Material Stone As Stairs */
	case SKY_HAV_MAT_MATERIAL_BLADE_2HAND: // = 2022742644, /*!< Material Blade 2Hand */
	case SKY_HAV_MAT_MATERIAL_BOTTLE_SMALL: // = 2025794648, /*!< Material Bottle Small */
	case SKY_HAV_MAT_SAND: // = 2168343821, /*!< Sand */
	case SKY_HAV_MAT_HEAVY_METAL: // = 2229413539, /*!< Heavy Metal */
	case SKY_HAV_MAT_UNKNOWN_2290050264: // = 2290050264, /*!< Unknown in Creation Kit v1.9.32.0. Found in Dawnguard DLC in meshes\dlc01\clutter\dlc01sabrecatpelt.nif. */
	case SKY_HAV_MAT_DRAGON: // = 2518321175, /*!< Dragon */
	case SKY_HAV_MAT_MATERIAL_BLADE_1HAND_SMALL: // = 2617944780, /*!< Material Blade 1Hand Small */
	case SKY_HAV_MAT_MATERIAL_SKIN_SMALL: // = 2632367422, /*!< Material Skin Small */
	case SKY_HAV_MAT_MATERIAL_SKIN_LARGE: // = 2965929619, /*!< Material Skin Large */
	case SKY_HAV_MAT_ORGANIC: // = 2974920155, /*!< Organic */
	case SKY_HAV_MAT_MATERIAL_BONE: // = 3049421844, /*!< Material Bone */
	case SKY_HAV_MAT_HEAVY_WOOD: // = 3070783559, /*!< Heavy Wood */
	case SKY_HAV_MAT_MATERIAL_CHAIN: // = 3074114406, /*!< Material Chain */
	case SKY_HAV_MAT_DIRT: // = 3106094762, /*!< Dirt */
	case SKY_HAV_MAT_MATERIAL_ARMOR_LIGHT: // = 3424720541, /*!< Material Armor Light */
	case SKY_HAV_MAT_MATERIAL_SHIELD_LIGHT: // = 3448167928, /*!< Material Shield Light */
	case SKY_HAV_MAT_MATERIAL_COIN: // = 3589100606, /*!< Material Coin */
	case SKY_HAV_MAT_MATERIAL_SHIELD_HEAVY: // = 3702389584, /*!< Material Shield Heavy */
	case SKY_HAV_MAT_MATERIAL_ARMOR_HEAVY: // = 3708432437, /*!< Material Armor Heavy */
	case SKY_HAV_MAT_MATERIAL_ARROW: // = 3725505938, /*!< Material Arrow */
	case SKY_HAV_MAT_GLASS: // = 3739830338, /*!< Glass */
	case SKY_HAV_MAT_STONE: // = 3741512247, /*!< Stone */
	case SKY_HAV_MAT_CLOTH: // = 3839073443, /*!< Cloth */
	case SKY_HAV_MAT_MATERIAL_BLUNT_2HAND: // = 3969592277, /*!< Material Blunt 2Hand */
	case SKY_HAV_MAT_UNKNOWN_4239621792: // = 4239621792, /*!< Unknown in Creation Kit v1.9.32.0. Found in Dawnguard DLC in meshes\dlc01\prototype\dlc1protoswingingbridge.nif. */
	case SKY_HAV_MAT_MATERIAL_BOULDER_MEDIUM: // = 4283869410, /*!< Material Boulder Medium */
	case SKY_HAV_MAT_HEAVY_STONE: // = 1570821952, /*!< Heavy Stone */
	case SKY_HAV_MAT_UNKNOWN_1574477864: // = 1574477864, /*!< Unknown in Creation Kit v1.6.89.0. Found in actors\dragon\character assets\skeleton.nif. */
	case SKY_HAV_MAT_UNKNOWN_1591009235: // = 1591009235, /*!< Unknown in Creation Kit v1.6.89.0. Found in trap objects or clutter\displaycases\displaycaselgangled01.nif or actors\deer\character assets\skeleton.nif. */
	case SKY_HAV_MAT_MATERIAL_BOWS_STAVES: // = 1607128641, /*!< Material Bows Staves */
	case SKY_HAV_MAT_MATERIAL_WOOD_AS_STAIRS: // = 1803571212, /*!< Material Wood As Stairs */
	case SKY_HAV_MAT_WATER: // = 1024582599, /*!< Water */
	case SKY_HAV_MAT_UNKNOWN_1028101969: // = 1028101969, /*!< Unknown in Creation Kit v1.6.89.0. Found in actors\draugr\character assets\skeletons.nif. */
	case SKY_HAV_MAT_MATERIAL_BLADE_1HAND: // = 1060167844, /*!< Material Blade 1 Hand */
	case SKY_HAV_MAT_MATERIAL_BOOK: // = 1264672850, /*!< Material Book */
	case SKY_HAV_MAT_MATERIAL_CARPET: // = 1286705471, /*!< Material Carpet */
	case SKY_HAV_MAT_SOLID_METAL: // = 1288358971, /*!< Solid Metal */
	case SKY_HAV_MAT_MATERIAL_AXE_1HAND: // = 1305674443, /*!< Material Axe 1Hand */
	case SKY_HAV_MAT_UNKNOWN_1440721808: // = 1440721808, /*!< Unknown in Creation Kit v1.6.89.0. Found in armor\draugr\draugrbootsfemale_go.nif or armor\amuletsandrings\amuletgnd.nif. */
	case SKY_HAV_MAT_GRAVEL: // = 428587608, /*!< Gravel */
	case SKY_HAV_MAT_MATERIAL_CHAIN_METAL: // = 438912228, /*!< Material Chain Metal */
	case SKY_HAV_MAT_BOTTLE: // = 493553910, /*!< Bottle */
		return SKY_HAV_MAT_STAIRS_STONE;
	case SKY_HAV_MAT_MUD: // = 1486385281, /*!< Mud */
	case SKY_HAV_MAT_MATERIAL_BOULDER_SMALL: // = 1550912982, /*!< Material Boulder Small */
	case SKY_HAV_MAT_WOOD: // = 500811281, /*!< Wood */
	case SKY_HAV_MAT_SKIN: // = 591247106, /*!< Skin */
	case SKY_HAV_MAT_UNKNOWN_617099282: // = 617099282, /*!< Unknown in Creation Kit v1.9.32.0. Found in Dawnguard DLC in meshes\dlc01\clutter\dlc01deerskin.nif. */
	case SKY_HAV_MAT_BARREL: // = 732141076, /*!< Barrel */
	case SKY_HAV_MAT_MATERIAL_CERAMIC_MEDIUM: // = 781661019, /*!< Material Ceramic Medium */
	case SKY_HAV_MAT_MATERIAL_BASKET: // = 790784366, /*!< Material Basket */
	case SKY_HAV_MAT_LIGHT_WOOD: // = 365420259, /*!< Light Wood */
		return SKY_HAV_MAT_STAIRS_WOOD;
	case SKY_HAV_MAT_BROKEN_STONE: // = 131151687, /*!< Broken Stone */
		return SKY_HAV_MAT_STAIRS_BROKEN_STONE;
	case SKY_HAV_MAT_ICE: // = 873356572, /*!< Ice */
	case SKY_HAV_MAT_SNOW: // = 398949039, /*!< Snow */
		return SKY_HAV_MAT_STAIRS_SNOW;
	}
	return SKY_HAV_MAT_STAIRS_STONE;
}

bool is_stairs_material(SkyrimHavokMaterial material) {
	return material == SKY_HAV_MAT_STAIRS_STONE ||
		material == SKY_HAV_MAT_STAIRS_WOOD ||
		material == SKY_HAV_MAT_STAIRS_SNOW ||
		material == SKY_HAV_MAT_STAIRS_BROKEN_STONE;
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

#define COLLISION_RATIO 0.0999682116260354643752272923431f

#define MATERIAL_MASK 1984

#define STEP_SIZE 0.3
#define STEP_SIZE_TOLERANCE 0.15
#define ALMOST_DISTANCE 0.075
#define ALMOST_DISTANCE_TOUCH 0.1
#define PLANAR_MAX_ANGLE_COS 0.3
#define STEP_MAX_ANGLE_COS 0.1

class bhkRigidBodyUpgrader {};


class CMSPacker {};

template<>
class Accessor<CMSPacker> {
public:
	Accessor(hkpCompressedMeshShape* pCompMesh, bhkCompressedMeshShapeDataRef pData, const vector<SkyrimHavokMaterial>& materials)
	{
		short                                   chunkIdxNif(0);

		pData->SetBoundsMin(Vector4(pCompMesh->m_bounds.m_min(0), pCompMesh->m_bounds.m_min(1), pCompMesh->m_bounds.m_min(2), pCompMesh->m_bounds.m_min(3)));
		pData->SetBoundsMax(Vector4(pCompMesh->m_bounds.m_max(0), pCompMesh->m_bounds.m_max(1), pCompMesh->m_bounds.m_max(2), pCompMesh->m_bounds.m_max(3)));

		pData->SetBitsPerIndex(pCompMesh->m_bitsPerIndex);
		pData->SetBitsPerWIndex(pCompMesh->m_bitsPerWIndex);
		pData->SetMaskIndex(pCompMesh->m_indexMask);
		pData->SetMaskWIndex(pCompMesh->m_wIndexMask);
		pData->SetWeldingType(0); //seems to be fixed for skyrim pData->SetWeldingType(pCompMesh->m_weldingType);
		pData->SetMaterialType(1); //seems to be fixed for skyrim pData->SetMaterialType(pCompMesh->m_materialType);
		pData->SetError(pCompMesh->m_error);

		//  resize and copy bigVerts
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
		vector<bhkCMSDBigTris > tBTriVec(pCompMesh->m_bigTriangles.getSize());
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
		vector<bhkCMSDTransform > tTranVec(pCompMesh->m_transforms.getSize());
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
			material.material = materials[pCompMesh->m_materials[idx]];
			//TODO: may be unnecessary due to the fact that MOPP shouldn't be used for anything else;
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

		}

		//  set modified chunk list to compressed mesh shape data
		pData->chunks = chunkListNif;
		//----  Merge  ----  END
	}
};

struct vertexInfo {
	hkVector4 vertex;
	unsigned int index;

	vertexInfo(hkVector4 v, unsigned int i) : vertex(v), index(i) {}

	vertexInfo & operator=(vertexInfo & source) {
		vertex = source.vertex;
		index = source.index;
		return *this;
	}

	vertexInfo & operator=(const vertexInfo & source) {
		vertex = source.vertex;
		index = source.index;
		return *this;
	}

};

bool is_equal(const hkGeometry::Triangle& t1, const hkGeometry::Triangle& other) {
	//return t1.m_a == t2.m_a &&
	//	t2.m_b == t2.m_b &&
	//	t2.m_c == t2.m_c;

	return
		(t1.m_a == other.m_a && t1.m_b == other.m_b && t1.m_c == other.m_c) ||
		(t1.m_a == other.m_a && t1.m_b == other.m_c && t1.m_c == other.m_b) ||
		(t1.m_a == other.m_b && t1.m_b == other.m_a && t1.m_c == other.m_c) ||
		(t1.m_a == other.m_b && t1.m_b == other.m_c && t1.m_c == other.m_a) ||
		(t1.m_a == other.m_c && t1.m_b == other.m_a && t1.m_c == other.m_b) ||
		(t1.m_a == other.m_c && t1.m_b == other.m_b && t1.m_c == other.m_a);
}

bool is_equal(const hkGeometry::Triangle& t1, const std::array<int, 3 >& other) {
	//return t1.m_a == t2.m_a &&
	//	t2.m_b == t2.m_b &&
	//	t2.m_c == t2.m_c;

	return
		(t1.m_a == other[0] && t1.m_b == other[1] && t1.m_c == other[2]) ||
		(t1.m_a == other[0] && t1.m_b == other[2] && t1.m_c == other[1]) ||
		(t1.m_a == other[1] && t1.m_b == other[0] && t1.m_c == other[2]) ||
		(t1.m_a == other[1] && t1.m_b == other[2] && t1.m_c == other[0]) ||
		(t1.m_a == other[2] && t1.m_b == other[0] && t1.m_c == other[1]) ||
		(t1.m_a == other[2] && t1.m_b == other[1] && t1.m_c == other[0]);
}

class CollisionShapeVisitor : public RecursiveFieldVisitor<CollisionShapeVisitor> {

	hkGeometry					geometry; //vetrices and triangles with materials indices
	vector<SkyrimHavokMaterial> materials;
	vector<SkyrimLayer>			layers; //one per triangle

	NiAVObject*					target = NULL;

	bool isGeometryDegenerate() {
		for (hkGeometry::Triangle t : geometry.m_triangles) {
			if (!hkpTriangleUtil::isDegenerate(
				geometry.m_vertices[t.m_a],
				geometry.m_vertices[t.m_b],
				geometry.m_vertices[t.m_c]
			)) {
				return false;
			}

		}
		return true;
	}

	int convert_to_stairs(int material) {
		int material_index = -1;
		SkyrimHavokMaterial stair_material = convert_material_to_stairs(materials[material]);
		auto material_it = find(materials.begin(), materials.end(), stair_material);
		if (material_it != materials.end())
			return distance(materials.begin(), material_it);
		material_index = materials.size();
		materials.push_back(stair_material);
		return material_index;
	}

	typedef hkGeometry::Triangle hkTriangle;

	class walker {

		struct side_comparator {
			bool operator()(const std::array<int, 2>& c1, const std::array<int, 2>& c2) const {
				return c1[0] == c2[0] && c1[1] == c2[1];
			}

			size_t operator()(const std::array<int, 2>& c1) const
			{	// hash _Keyval to size_t value
				return hash<size_t>()((uint64_t)c1[0] << 32 | c1[1]);
			}
		};

		typedef unordered_map< std::array<int, 2>, int, side_comparator> adjacent_map_t;

		hkGeometry& geometry;
		vector < std::array<int, 3 >> planes;
		vector < std::array<int, 3 >> normal_planes;
		adjacent_map_t adjacent_map;
		set < std::array<int, 2 >> visited;
		set<std::array<int, 3 >> current_stair;

		struct order_by_plane_up {

			const hkGeometry& geometry;

			order_by_plane_up(const hkGeometry& geometry) : geometry(geometry) {}

			bool operator() (const std::array<int, 3>& v1, const std::array<int, 3>& v2) const {
				return geometry.m_vertices[v1[2]](2) < geometry.m_vertices[v2[2]](2);
			}
		};

		float calculate_angle_with_z(const hkGeometry::Triangle& v1) {
			hkVector4 v1ab = geometry.m_vertices[v1.m_b];
			v1ab.sub4(geometry.m_vertices[v1.m_a]);
			hkVector4 v1ac = geometry.m_vertices[v1.m_c];
			v1ac.sub4(geometry.m_vertices[v1.m_a]);
			hkVector4 z(0.0, 0.0, 1.0);
			hkVector4 n; n.setCross(v1ab, v1ac); n.normalize3();
			return n.dot3(z);
		}

		void get_planar_z_triangles() {
			for (int t = 0; t < geometry.m_triangles.getSize(); t++) {
				float z_angle = calculate_angle_with_z(geometry.m_triangles[t]);
				if (abs(z_angle) > 1 - PLANAR_MAX_ANGLE_COS)
					planes.push_back({ geometry.m_triangles[t].m_a,geometry.m_triangles[t].m_b, geometry.m_triangles[t].m_c });
			}
			sort(planes.begin(), planes.end(), [this](const std::array<int, 3>& v1, const std::array<int, 3>& v2) -> bool {
				return this->geometry.m_vertices[v1[0]](2) < this->geometry.m_vertices[v2[0]](2);
			});
		}

		void get_normal_z_triangles() {
			for (int t = 0; t < geometry.m_triangles.getSize(); t++) {
				float z_angle = calculate_angle_with_z(geometry.m_triangles[t]);
				if (abs(z_angle) < PLANAR_MAX_ANGLE_COS)
					normal_planes.push_back({ geometry.m_triangles[t].m_a,geometry.m_triangles[t].m_b, geometry.m_triangles[t].m_c });
			}
			sort(normal_planes.begin(), normal_planes.end(), [this](const std::array<int, 3>& v1, const std::array<int, 3>& v2) -> bool {
				return this->geometry.m_vertices[v1[0]](2) < this->geometry.m_vertices[v2[0]](2) &&
					this->geometry.m_vertices[v1[1]](2) < this->geometry.m_vertices[v2[1]](2) &&
					this->geometry.m_vertices[v1[2]](2) < this->geometry.m_vertices[v2[2]](2);
			});
		}

		void split(const std::array<int, 2>& side, int c, int opposite_c) {
			//current_stair.insert({ side[0], side[1],  c });
			//current_stair.insert({ side[1], side[0],  opposite_c });
			//vector<int> down_adjacent_side = almost_adjacent_side(source, down_adjacent); // a, b
			//int down_adjiacent_down_vertex_index = min_z_component(down_adjacent); // c
			//vector<int> down_touching_side = almost_adjacent_side(down_adjacent, down_touching);
			//int vertex_d = -1; //d
			//if (find(down_touching_side.begin(), down_touching_side.end(), down_touching.m_a) == down_touching_side.end())
			//	vertex_d = down_touching.m_a;
			//if (find(down_touching_side.begin(), down_touching_side.end(), down_touching.m_b) == down_touching_side.end())
			//	vertex_d = down_touching.m_b;
			//if (vertex_d == -1)
			//	vertex_d = down_touching.m_c;
			//if (vertex_d == -1)
			//	throw runtime_error("Unable to find the vertex_d in collision breaker!");

			//hkVector4 tdist0; tdist0.setSub4(geometry.m_vertices[down_adjiacent_down_vertex_index], geometry.m_vertices[down_adjacent_side[0]]);
			//hkVector4 tdist1; tdist1.setSub4(geometry.m_vertices[down_adjiacent_down_vertex_index], geometry.m_vertices[down_adjacent_side[1]]);

			//int vertex_a = tdist0.length3() < tdist1.length3() ? down_adjacent_side[0] : down_adjacent_side[1];
			//int vertex_b = vertex_a == down_adjacent_side[0] ? down_adjacent_side[1] : down_adjacent_side[0];
			//int vertex_c = down_adjiacent_down_vertex_index;

			////now calculate the break directions
			//hkVector4 vector_a = geometry.m_vertices[vertex_a];
			//hkVector4 vector_b = geometry.m_vertices[vertex_b];
			//hkVector4 vector_c = geometry.m_vertices[vertex_c];
			//hkVector4 vector_d = geometry.m_vertices[vertex_d];

			//hkVector4 vector_ac; vector_ac.setSub4(vector_c, vector_a);
			//hkVector4 vector_bd; vector_bd.setSub4(vector_d, vector_b);

			//// norm / z_component = new_norm / step_size -> new_nomr = (norm/z_component) *step_size
			//float ac_new_norm = STEP_SIZE / vector_ac.length3().getReal(); // (vector_ac.length3().getReal() / abs(vector_ac(2))) * STEP_SIZE;
			//float bd_new_norm = STEP_SIZE / vector_bd.length3().getReal(); // (vector_bd.length3().getReal() / abs(vector_bd(2))) * STEP_SIZE;

			//hkVector4 vector_ac_new = hkVector4(vector_ac(0)*ac_new_norm, vector_ac(1)*ac_new_norm, vector_ac(2)*ac_new_norm);
			//hkVector4 vector_bd_new = hkVector4(vector_bd(0)*bd_new_norm, vector_bd(1)*bd_new_norm, vector_bd(2)*bd_new_norm);

			//hkVector4 vector_a_new; vector_a_new.setAdd4(vector_a, vector_ac_new);
			//hkVector4 vector_b_new; vector_b_new.setAdd4(vector_b, vector_bd_new);

			//int vertex_a_new = geometry.m_vertices.getSize();
			//geometry.m_vertices.pushBack(vector_a_new);
			//int vertex_b_new = geometry.m_vertices.getSize();
			//geometry.m_vertices.pushBack(vector_b_new);

			////new triangles
			//hkGeometry::Triangle stairs_down;
			//stairs_down.m_a = vertex_a;
			//stairs_down.m_b = vertex_b;
			//stairs_down.m_c = vertex_a_new;
			//stairs_down.m_material = 0;
			//hkGeometry::Triangle stairs_touching;
			//stairs_touching.m_a = vertex_a_new;
			//stairs_touching.m_b = vertex_b_new;
			//stairs_touching.m_c = vertex_b;
			//stairs_touching.m_material = 0;
			//hkGeometry::Triangle non_stairs_down;
			//non_stairs_down.m_a = vertex_a_new;
			//non_stairs_down.m_b = vertex_b_new;
			//non_stairs_down.m_c = vertex_c;
			//non_stairs_down.m_material = 0;
			//hkGeometry::Triangle non_stairs_touching;
			//non_stairs_touching.m_a = vertex_c;
			//non_stairs_touching.m_b = vertex_d;
			//non_stairs_touching.m_c = vertex_b_new;
			//non_stairs_touching.m_material = 0;

			//stairs.push_back(non_stairs_down);
			//stairs.push_back(non_stairs_touching);

			//down_adjacent = stairs_down;
			//down_touching = stairs_touching;
		}

		std::array<int, 2> find_max_side(std::array<int, 3> tris) {
			hkVector4 a = geometry.m_vertices[tris[0]];
			hkVector4 b = geometry.m_vertices[tris[1]];
			hkVector4 c = geometry.m_vertices[tris[2]];
			hkVector4 ab = b; ab.sub4(a); float norm_ab = ab.lengthSquared3();
			hkVector4 bc = c; ab.sub4(b); float norm_bc = bc.lengthSquared3();
			hkVector4 ca = a; ab.sub4(c); float norm_ca = ca.lengthSquared3();
			return  norm_ab > norm_bc ?
				norm_ab > norm_ca ?
				std::array<int, 2>({ tris[0], tris[1] }) :
				std::array<int, 2>({ tris[2], tris[0] }) :
				norm_bc > norm_ca ?
				std::array<int, 2>({ tris[1], tris[2] }) :
				std::array<int, 2>({ tris[2], tris[0] });
		}

		void step(const std::array<int, 2>& side, int c) {
			if (visited.insert(side).second) {
				adjacent_map_t::iterator opposite = adjacent_map.find({ side[1], side[0] });
				if (opposite != adjacent_map.end() && opposite->second != c) {
					hkVector4 opposite_c = geometry.m_vertices[opposite->second];
					hkVector4 a = geometry.m_vertices[side[0]];
					hkVector4 b = geometry.m_vertices[side[1]];
					float max_ab_z = a(2) > b(2) ? a(2) : b(2);
					float min_ab_z = a(2) < b(2) ? a(2) : b(2);
					float step_size_left = abs(opposite_c(2) - max_ab_z);
					float step_size_right = abs(opposite_c(2) - min_ab_z);
					hkVector4 ab; ab.setSub4(b, a);
					hkVector4 a_opposite_c; a_opposite_c.setSub4(opposite_c, a);
					hkVector4 norm; norm.setCross(ab, a_opposite_c); norm.normalize3();
					float angle = norm.dot3(hkVector4(0.0, 0.0, 1.0));
					if (angle < sqrt(2) / 2) {
						if (step_size_left >(STEP_SIZE - STEP_SIZE_TOLERANCE) && step_size_left < (STEP_SIZE + STEP_SIZE_TOLERANCE) ||
							step_size_right >(STEP_SIZE - STEP_SIZE_TOLERANCE) && step_size_right < (STEP_SIZE + STEP_SIZE_TOLERANCE)) {
							//this is a walkable step up
							if (current_stair.empty()) {
								//first step, get the other adjiacent too
								std::array<int, 2> starting_side = find_max_side({ side[0], side[1],  c });
								adjacent_map_t::iterator opposite = adjacent_map.find({ starting_side[1], starting_side[0] });
								if (opposite != adjacent_map.end() && opposite->second != c) {
									current_stair.insert({ starting_side[0], starting_side[1],  opposite->second });
								}
							}
							current_stair.insert({ side[0], side[1],  c });
							current_stair.insert({ side[1], side[0],  opposite->second });

							step({ side[0], opposite->second }, side[1]);
							step({ opposite->second, side[1] }, side[0]);
						}
					}
					else {
						//Check corner cases: is there a really near normal plane

						if (!current_stair.empty()) {
							stairs.push_back(current_stair);
							current_stair.clear();
						}
					}

				}
			}
		}

		hkVector4 centeroid(const std::array<int, 3>& t) {
			hkVector4 res(0.0, 0.0, 0.0);
			res.add4(geometry.m_vertices[t[0]]);
			res.add4(geometry.m_vertices[t[1]]);
			res.add4(geometry.m_vertices[t[2]]);
			return hkVector4(res(0) / 3, res(1) / 3, res(2) / 3);
		}

		void restich() {
			vector<hkVector4> true_vertices;
			map<int, int> aliases;
			for (int d = 0; d < geometry.m_vertices.getSize(); d++) {
				bool is_alias = false;
				int true_index = -1;
				int alias_index = -1;
				for (int i = 0; i < true_vertices.size(); i++) {
					hkVector4 dist; dist.setSub4(geometry.m_vertices[d], true_vertices[i]);
					if (dist.lengthSquared3() < 0.0001) {
						is_alias = true;
						aliases[d] = i;
						break;
					}
				}
				if (!is_alias) {
					aliases[d] = true_vertices.size();
					true_vertices.push_back(geometry.m_vertices[d]);
				}
			}
			//clean triangle
			for (hkTriangle& t : geometry.m_triangles) {
				map<int, int>::iterator alias_it = aliases.find(t.m_a);
				if (alias_it != aliases.end())
					t.m_a = alias_it->second;
				alias_it = aliases.find(t.m_b);
				if (alias_it != aliases.end())
					t.m_b = alias_it->second;
				alias_it = aliases.find(t.m_c);
				if (alias_it != aliases.end())
					t.m_c = alias_it->second;
			}
			//clean vertices
			geometry.m_vertices.clear();
			for (hkVector4 v : true_vertices)
				geometry.m_vertices.pushBack(v);

			Log::Info("restiched!");
		}

	public:
		vector< set < std::array<int, 3 > >> stairs;

		walker(hkGeometry& geometry) :
			geometry(geometry) {
			// vanilla collisions suck
			restich();
			// build adjacency map			
			for (const hkTriangle& t : geometry.m_triangles) {
				std::array<int, 2> side = { t.m_a, t.m_b };
				adjacent_map[side] = t.m_c;
				side = { t.m_b, t.m_c };
				adjacent_map[side] = t.m_a;
				side = { t.m_c, t.m_a };
				adjacent_map[side] = t.m_b;
			}
			get_planar_z_triangles();
			get_normal_z_triangles();
			//walk all planes
			for (auto& tri : planes) {
				step({ tri[0],tri[1] }, tri[2]);
				step({ tri[1],tri[2] }, tri[0]);
				step({ tri[2],tri[0] }, tri[1]);
			}
		}
	};


	//madness.
	void check_stairs() {
		vector< set < std::array<int, 3 > >>  stairs = walker(geometry).stairs;
		//geometry.m_triangles.clear();
		for (const set< std::array<int, 3 >>& stair : stairs) {
			for (const std::array<int, 3 > &tris : stair) {
				hkTriangle t;
				t.m_a = tris[0];
				t.m_b = tris[1];
				t.m_c = tris[2];
				//geometry.m_triangles.pushBack(t);
				for (hkGeometry::Triangle& triangle : geometry.m_triangles) {
					if (is_equal(t, triangle)) {
						if (!is_stairs_material(materials[triangle.m_material]))
							triangle.m_material = convert_to_stairs(triangle.m_material);
						break;
					}
				}
			}
		}
	}

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

		try {
			//  add geometry to shape
			subPartId = shapeBuilder.beginSubpart(pCompMesh);
			shapeBuilder.addGeometry(geometry, hkMatrix4::getIdentity(), pCompMesh);
			shapeBuilder.endSubpart(pCompMesh);
			shapeBuilder.addInstance(subPartId, hkMatrix4::getIdentity(), pCompMesh);

			//add materials to shape
			for (int i = 0; i < materials.size(); i++) {
				pCompMesh->m_materials.pushBack(i);
			}
			//create welding info
			mci.m_enableChunkSubdivision = false;  //  PC version

			pMoppCode = hkpMoppUtility::buildCode(pCompMesh, mci);
		}
		catch (...) {
			throw runtime_error("Unable to calculate MOPP code!");
		}
		pMoppBvTree = new hkpMoppBvTreeShape(pCompMesh, pMoppCode);
		hkpMeshWeldingUtility::computeWeldingInfo(pCompMesh, pMoppBvTree, hkpWeldingUtility::WELDING_TYPE_TWO_SIDED);
		//----  Havok  ----  END

		//----  Merge  ----  START

		//  --- modify MoppBvTree ---
		// set origin
		pMoppShape->SetOrigin(Vector3(pMoppBvTree->getMoppCode()->m_info.m_offset(0), pMoppBvTree->getMoppCode()->m_info.m_offset(1), pMoppBvTree->getMoppCode()->m_info.m_offset(2)));

		// set scale
		pMoppShape->SetScale(pMoppBvTree->getMoppCode()->m_info.getScale());

		// set build Type
		pMoppShape->SetBuildType(MoppDataBuildType((Niflib::byte) pMoppCode->m_buildType));

		//  copy mopp data
		pMoppShape->SetMoppData(vector<Niflib::byte>(pMoppBvTree->m_moppData, pMoppBvTree->m_moppData + pMoppBvTree->m_moppDataSize));

		Accessor<CMSPacker> packer(pCompMesh, pData, materials);

		bhkCompressedMeshShapeRef shape = new bhkCompressedMeshShape();
		shape->SetRadius(pCompMesh->m_radius);
		shape->SetRadiusCopy(pCompMesh->m_radius);
		shape->SetData(pData);
		shape->SetTarget(target);

		pMoppShape->SetShape(DynamicCast<bhkShape>(shape));

		delete pMoppCode;
		delete pMoppBvTree;
		delete pCompMesh;
	}

	bool isGeometryValid() {
		return
			!geometry.m_triangles.isEmpty() &&
			!geometry.m_vertices.isEmpty() &&
			!materials.empty() &&
			!layers.empty() &&
			!isGeometryDegenerate();
	}

public:

	bhkMoppBvTreeShapeRef pMoppShape;

	CollisionShapeVisitor(bhkShapeRef shape, const NifInfo& info, NiAVObject* target) :
		target(target),
		RecursiveFieldVisitor(*this, info) {
		shape->accept(*this, info);
		if (pMoppShape == NULL)
			pMoppShape = new bhkMoppBvTreeShape();

		check_stairs();

		if (isGeometryValid()) {
			try {
				calculate_collision();
			}
			catch (...) {
				Log::Warn("Unable to upgrade shape, making a new convex hull");
				//Something bad happened, try to make a new collision
			}
		}
		else {
			//here we definetly want to generate a new collision. An example is paintbrush01

		}
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
	inline void visit_object(bhkPackedNiTriStripsShape& obj) {
		hkPackedNiTriStripsDataRef	pData(DynamicCast<hkPackedNiTriStripsData>(obj.GetData()));

		if (pData != NULL)
		{
			vector<Vector3> vertices(pData->GetVertices());
			vector<TriangleData> in_packed_triangles(pData->GetTriangles());
			vector<unsigned short> points(in_packed_triangles.size() * 3);
			for (TriangleData tpd : in_packed_triangles) {
				points.push_back(tpd.triangle.v1);
				points.push_back(tpd.triangle.v2);
				points.push_back(tpd.triangle.v3);
			}
			vector<Triangle> in_triangles(triangulate(points));

			geometry.m_vertices.setSize(vertices.size());
			geometry.m_triangles.setSize(in_triangles.size());

			for (int v = 0; v < vertices.size(); v++) {
				geometry.m_vertices[v] = TOVECTOR4(vertices[v] * COLLISION_RATIO);
			}


			vector<unsigned int> material_bounds;
			vector<OblivionSubShape> shapes(obj.GetSubShapes());
			if (shapes.empty()) {
				//fo
				shapes = pData->GetSubShapes();
			}

			int bound = 0;
			for (auto& shape : shapes) {
				bound += shape.numVertices;
				material_bounds.push_back(bound);
			}

			//vertices should be ordered by material, given the subshapes
			for (int t = 0; t < in_triangles.size(); t++) {
				int material_indexes[3];
				for (int i = 0; i < 3; i++) {
					int vertex_index = in_triangles[t][i];
					for (int m = 0; m < material_bounds.size(); m++) {
						if (vertex_index < material_bounds[m]) {
							material_indexes[i] = m;
							break;
						}
					}
				}
				int selected_index = material_indexes[0];
				if (material_indexes[0] != material_indexes[1] ||
					material_indexes[1] != material_indexes[2] ||
					material_indexes[0] != material_indexes[2])
				{
					Log::Info("Found Triangle with heterogeneus material!");
					std::map<int, int> frequencyMap;
					int maxFrequency = 0;
					int mostFrequentElement = 0;
					for (int x : material_indexes)
					{
						int f = ++frequencyMap[x];
						if (f > maxFrequency)
						{
							maxFrequency = f;
							mostFrequentElement = x;
						}
					}

					selected_index = mostFrequentElement;
				}
				SkyrimHavokMaterial s_material = convert_havok_material(shapes[selected_index].material.material_ob);
				int material_index = -1;
				auto material_it = find(materials.begin(), materials.end(), s_material);
				if (material_it != materials.end())
					material_index = distance(materials.begin(), material_it);
				else
				{
					material_index = materials.size();
					materials.push_back(s_material);
				}

				SkyrimLayer s_layer = convert_havok_layer(shapes[selected_index].havokFilter.layer_ob);
				int layer_index = -1;
				auto layer_it = find(layers.begin(), layers.end(), s_layer);
				if (layer_it != layers.end())
					layer_index = distance(layers.begin(), layer_it);
				else
				{
					layer_index = layers.size();
					layers.push_back(s_layer);
				}

				hkGeometry::Triangle htri;
				htri.m_a = in_triangles[t][0];
				htri.m_b = in_triangles[t][1];
				htri.m_c = in_triangles[t][2];
				htri.m_material = material_index;

				geometry.m_triangles[t] = htri;
			}
		}
	}

	SkyrimHavokMaterial material_from_flags(const VectorFlags& vf) {
		int value = (vf & MATERIAL_MASK) >> 6;
		return convert_havok_material((OblivionHavokMaterial)(value));
	}

	template<>
	inline void visit_object(bhkNiTriStripsShape& obj) {
		vector<NiTriStripsDataRef> shapes(obj.GetStripsData());
		vector<HavokFilter> data_layers(obj.GetDataLayers());

		//this is mysterious
		float							factor(COLLISION_RATIO * 0.1428f);

		int shape_index = 0;

		for (auto& shape : shapes) {
			size_t voffset = geometry.m_vertices.getSize();

			vector<Vector3> vertices(shape->GetVertices());
			for (auto& v : vertices) {
				geometry.m_vertices.pushBack(TOVECTOR4(v * factor));
			}

			SkyrimHavokMaterial s_material = material_from_flags(shape->GetVectorFlags());
			int material_index = -1;
			auto material_it = find(materials.begin(), materials.end(), s_material);
			if (material_it != materials.end())
				material_index = distance(materials.begin(), material_it);
			else
			{
				material_index = materials.size();
				materials.push_back(s_material);
			}

			SkyrimLayer s_layer = convert_havok_layer(data_layers[shape_index].layer_ob);
			int layer_index = -1;
			auto layer_it = find(layers.begin(), layers.end(), s_layer);
			if (layer_it != layers.end())
				layer_index = distance(layers.begin(), layer_it);
			else
			{
				layer_index = layers.size();
				layers.push_back(s_layer);
			}

			vector<Triangle> in_triangles(triangulate(shape->GetPoints()));

			for (int t = 0; t < in_triangles.size(); t++) {
				hkGeometry::Triangle htri;
				htri.m_a = in_triangles[t][0] + voffset;
				htri.m_b = in_triangles[t][1] + voffset;
				htri.m_c = in_triangles[t][2] + voffset;
				htri.m_material = material_index;

				geometry.m_triangles.pushBack(htri);
			}
			shape_index++;
		}
	}

	template<>
	inline void visit_object(bhkMeshShape& obj) {
		vector<NiTriStripsDataRef> shapes(obj.GetStripsData());

		//this is mysterious
		float							factor(COLLISION_RATIO * 0.1428f);

		int shape_index = 0;

		for (auto& shape : shapes) {
			size_t voffset = geometry.m_vertices.getSize();

			vector<Vector3> vertices(shape->GetVertices());
			for (auto& v : vertices) {
				geometry.m_vertices.pushBack(TOVECTOR4(v * factor));
			}

			SkyrimHavokMaterial s_material = material_from_flags(shape->GetVectorFlags());
			int material_index = -1;
			auto material_it = find(materials.begin(), materials.end(), s_material);
			if (material_it != materials.end())
				material_index = distance(materials.begin(), material_it);
			else
			{
				material_index = materials.size();
				materials.push_back(s_material);
			}

			SkyrimLayer s_layer = SKYL_STATIC;
			int layer_index = -1;
			auto layer_it = find(layers.begin(), layers.end(), s_layer);
			if (layer_it != layers.end())
				layer_index = distance(layers.begin(), layer_it);
			else
			{
				layer_index = layers.size();
				layers.push_back(s_layer);
			}

			vector<Triangle> in_triangles(triangulate(shape->GetPoints()));

			for (int t = 0; t < in_triangles.size(); t++) {
				hkGeometry::Triangle htri;
				htri.m_a = in_triangles[t][0] + voffset;
				htri.m_b = in_triangles[t][1] + voffset;
				htri.m_c = in_triangles[t][2] + voffset;
				htri.m_material = material_index;

				geometry.m_triangles.pushBack(htri);
			}
			shape_index++;
		}
	}
};

bhkShapeRef upgrade_shape(const bhkShapeRef& shape, const NifInfo& info, NiAVObject* target) {
	if (shape->IsSameType(bhkMoppBvTreeShape::TYPE) ||
		shape->IsSameType(bhkNiTriStripsShape::TYPE) ||
		shape->IsSameType(bhkPackedNiTriStripsShape::TYPE))
		return CollisionShapeVisitor(shape, info, target).pMoppShape;
	else
		return shape;
}

vector<bhkShapeRef> upgrade_shapes(const vector<bhkShapeRef>& shapes, const NifInfo& info, NiAVObject* target) {
	vector<bhkShapeRef> out;
	for (bhkShapeRef shape : shapes) {
		if (shape->IsSameType(bhkMoppBvTreeShape::TYPE) ||
			shape->IsSameType(bhkNiTriStripsShape::TYPE) ||
			shape->IsSameType(bhkPackedNiTriStripsShape::TYPE))
			out.push_back(upgrade_shape(shape, info, target));
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
	Accessor(bhkRigidBody& obj, const NifInfo& info, NiAVObject* target) : this_info(info) {
		//zero out
		obj.unknownInt = 0;

		obj.unusedByte1 = (byte)0;
		obj.unknownInt1 = (unsigned int)0;
		obj.unknownInt2 = (unsigned int)0;
		obj.unusedByte2 = (byte)0;
		obj.timeFactor = (float)1.0;
		obj.gravityFactor = (float)1.0;
		obj.rollingFrictionMultiplier = (float)0.0;

		obj.enableDeactivation = true; //Seems has to be fixed; obj.solverDeactivation != SOLVER_DEACTIVATION_OFF;
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

		//bhkMalleableConstraints are no more supported I guess; Nor limited hinges?
		for (int i = 0; i < obj.constraints.size(); i++) {
			if (obj.constraints[i]->IsDerivedType(bhkMalleableConstraint::TYPE)) {
				obj.constraints[i] = convert_malleable(DynamicCast<bhkMalleableConstraint>(obj.constraints[i]));
			}

		}

		//Seems like the old havok settings must be deactivated
		//obj.motionSystem = MO_SYS_BOX_STABILIZED;
		if (obj.motionSystem == MO_SYS_KEYFRAMED)
			obj.motionSystem = MO_SYS_BOX_INERTIA;
		if (obj.qualityType == MO_QUAL_KEYFRAMED || obj.qualityType == MO_QUAL_KEYFRAMED_REPORT)
			obj.qualityType = MO_QUAL_FIXED;

		//obsolete collisions
		if (obj.shape->IsSameType(bhkMoppBvTreeShape::TYPE) ||
			obj.shape->IsSameType(bhkNiTriStripsShape::TYPE) ||
			obj.shape->IsSameType(bhkPackedNiTriStripsShape::TYPE)) {
			obj.shape = upgrade_shape(obj.shape, this_info, target);
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

IndexString getStringFromPalette(std::string palette, size_t offset)
{
	size_t findex = palette.find_first_of('\0', offset);
	size_t len = findex - offset;
	return palette.substr(offset, len);
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
	const vector<NiObjectRef>& blocks;

	map<void*, void*> collision_target_map;

	vector<NiTriShapeRef> particle_geometry;

public:

	set<string> nisequences;

	//template<typename T>
	//struct replace<Ref<T>>
	//{		
	//	map<Ref<T>, Ref<T>> replace_map;
	//
	//	template<typename Ref<T>>
	//	Ref<T> convert(Ref<T> controller) {
	//		return controller;
	//	}

	//	template<>
	//	NiTimeControllerRef convert(NiMaterialColorControllerRef oldController) {
	//		BSLightingShaderPropertyColorControllerRef controller = new BSLightingShaderPropertyColorController();
	//		controller->SetFlags(oldController->GetFlags());
	//		controller->SetFrequency(oldController->GetFrequency());
	//		controller->SetPhase(oldController->GetPhase());
	//		controller->SetStartTime(oldController->GetStartTime());
	//		controller->SetStopTime(oldController->GetStopTime());
	//		controller->SetTarget(replace()(oldController->GetTarget());
	//		controller->SetInterpolator(oldController->GetInterpolator());
	//		if (oldController->GetTargetColor() == MaterialColor::TC_SELF_ILLUM)
	//			controller->SetTypeOfControlledColor(LightingShaderControlledColor::LSCC_EMISSIVE_COLOR);
	//		//constructor sets to specular.
	//		replace_map[oldController] = controller;
	//		return controller;			
	//	}

	//	Ref<T> operator()(Ref<T> niobj) {
	//		map<NiTimeControllerRef, NiTimeControllerRef>::iterator converted = replace_map.find(niobj);
	//		if (converted != replace_map.end())
	//			return converted->second;
	//		return convert(converted->second);	
	//	}

	//	T* operator()(T* niptr) {
	//		if (niptr != NULL) {
	//			for (map<NiTimeControllerRef, NiTimeControllerRef>::iterator converted = replace_map.begin();
	//				converted != replace_map.end(); converted++) {
	//			if (niptr == &*(converter->first))
	//				return &*converted->second;
	//		}
	//		return convert(converted->second);
	//	}

	//};



	map<NiMaterialColorControllerRef, BSLightingShaderPropertyColorControllerRef> material_controllers_map;
	map<NiAlphaControllerRef, BSLightingShaderPropertyFloatControllerRef> material_alpha_controllers_map;
	map<NiFlipControllerRef, BSLightingShaderPropertyFloatControllerRef> material_flip_controllers_map;
	map<NiTextureTransformControllerRef, BSLightingShaderPropertyFloatControllerRef> material_transform_controllers_map;

	ConverterVisitor(const NifInfo& info, NiObjectRef root, const vector<NiObjectRef>& blocks) :
		RecursiveFieldVisitor(*this, info), this_info(info), blocks(blocks)
	{
		root->accept(*this, info);
		if (root->IsDerivedType(NiNode::TYPE)) {
			NiNodeRef ninroot = DynamicCast<NiNode>(root);
			vector<Ref<NiAVObject>> children = ninroot->GetChildren();
			for (NiTriShapeRef pg : particle_geometry)
				children.push_back(StaticCast<NiAVObject>(pg));
			ninroot->SetChildren(children);
		}
		for (NiObjectRef obj : blocks) {
			if (obj->IsDerivedType(NiControllerSequence::TYPE)) {
				NiControllerSequenceRef nisblock = DynamicCast<NiControllerSequence>(obj);
				vector<ControlledBlock> blocks = nisblock->GetControlledBlocks();
				for (int i = 0; i != blocks.size(); i++) {
					if (blocks[i].controller->IsDerivedType(NiMaterialColorController::TYPE)) {
						map<NiMaterialColorControllerRef, BSLightingShaderPropertyColorControllerRef>::iterator cc = material_controllers_map.find(DynamicCast<NiMaterialColorController>(blocks[i].controller));
						if (cc != material_controllers_map.end())
							blocks[i].controller = cc->second;
						else
							Log::Info("Not Found!");
					}
					if (blocks[i].controller->IsDerivedType(NiAlphaController::TYPE)) {
						map<NiAlphaControllerRef, BSLightingShaderPropertyFloatControllerRef>::iterator cc = material_alpha_controllers_map.find(DynamicCast<NiAlphaController>(blocks[i].controller));
						if (cc != material_alpha_controllers_map.end())
							blocks[i].controller = cc->second;
						else
							Log::Info("Not Found!");
					}
					if (blocks[i].controller->IsDerivedType(NiFlipController::TYPE)) {
						map<NiFlipControllerRef, BSLightingShaderPropertyFloatControllerRef>::iterator cc = material_flip_controllers_map.find(DynamicCast<NiFlipController>(blocks[i].controller));
						if (cc != material_flip_controllers_map.end())
							blocks[i].controller = cc->second;
						else
							Log::Info("Not Found!");
					}
					if (blocks[i].controller->IsDerivedType(NiTextureTransformController::TYPE)) {
						map<NiTextureTransformControllerRef, BSLightingShaderPropertyFloatControllerRef>::iterator cc = material_transform_controllers_map.find(DynamicCast<NiTextureTransformController>(blocks[i].controller));
						if (cc != material_transform_controllers_map.end())
							blocks[i].controller = cc->second;
						else
							Log::Info("Not Found!");
					}
				}
				nisblock->SetControlledBlocks(blocks);
			}
		}
	}
	template<class T>
	inline void visit_object(T& obj) {}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	//Actual Handling
	template<class T>
	inline void visit_field(T& field) {
	}

	BSLightingShaderPropertyFloatControllerRef convert(NiTextureTransformControllerRef in) {
		if (!in == NULL && in->IsDerivedType(NiTextureTransformController::TYPE)) {
			NiTextureTransformControllerRef oldController = DynamicCast<NiTextureTransformController>(in);
			map<NiTextureTransformControllerRef, BSLightingShaderPropertyFloatControllerRef>::iterator converted = material_transform_controllers_map.find(oldController);
			if (converted != material_transform_controllers_map.end()) {
				return converted->second;
			}
			else {
				BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
				controller->SetNextController(oldController->GetNextController());
				controller->SetFlags(oldController->GetFlags());
				controller->SetFrequency(oldController->GetFrequency());
				controller->SetPhase(oldController->GetPhase());
				controller->SetStartTime(oldController->GetStartTime());
				controller->SetStopTime(oldController->GetStopTime());
				controller->SetTarget(oldController->GetTarget());
				controller->SetInterpolator(oldController->GetInterpolator());
				if (oldController->GetOperation() == TransformMember::TT_TRANSLATE_U)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_U_OFFSET);
				if (oldController->GetOperation() == TransformMember::TT_TRANSLATE_V)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_OFFSET);
				if (oldController->GetOperation() == TransformMember::TT_SCALE_U)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_U_SCALE);
				if (oldController->GetOperation() == TransformMember::TT_SCALE_V)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_SCALE);
				if (oldController->GetOperation() == TransformMember::TT_ROTATE)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_ALPHA);
				material_transform_controllers_map[oldController] = controller;
				return controller;
			}
		}
	}


	template<>
	void visit_field(NiTimeControllerRef& in) {
		if (!in == NULL && in->IsDerivedType(NiTextureTransformController::TYPE)) {
			NiTextureTransformControllerRef oldController = DynamicCast<NiTextureTransformController>(in);
			map<NiTextureTransformControllerRef, BSLightingShaderPropertyFloatControllerRef>::iterator converted = material_transform_controllers_map.find(oldController);
			if (converted != material_transform_controllers_map.end()) {
				in = converted->second;
			}
			else {
				BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
				controller->SetNextController(oldController->GetNextController());
				controller->SetFlags(oldController->GetFlags());
				controller->SetFrequency(oldController->GetFrequency());
				controller->SetPhase(oldController->GetPhase());
				controller->SetStartTime(oldController->GetStartTime());
				controller->SetStopTime(oldController->GetStopTime());
				controller->SetTarget(oldController->GetTarget());
				controller->SetInterpolator(oldController->GetInterpolator());
				if (oldController->GetOperation() == TransformMember::TT_TRANSLATE_U)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_U_OFFSET);
				if (oldController->GetOperation() == TransformMember::TT_TRANSLATE_V)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_OFFSET);
				if (oldController->GetOperation() == TransformMember::TT_SCALE_U)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_U_SCALE);
				if (oldController->GetOperation() == TransformMember::TT_SCALE_V)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_SCALE);
				if (oldController->GetOperation() == TransformMember::TT_ROTATE)
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_ALPHA);
				material_transform_controllers_map[oldController] = controller;
				in = controller;
			}
		}
	}


	template<>
	inline void visit_object(NiNode& obj) {
		vector<Ref<NiAVObject>> children = obj.GetChildren();
		vector<Ref<NiExtraData>> extras = obj.GetExtraDataList();
		int index = 0;

		if (obj.GetName() == "Bip01 Head")
			obj.SetName(IndexString("NPC Head [Head]"));
		else if (obj.GetName() == "Bip01 L Clavicle")
			obj.SetName(IndexString("NPC L Clavicle [LClv]"));
		else if (obj.GetName() == "Bip01 R Clavicle")
			obj.SetName(IndexString("NPC R Clavicle [RClv]"));
		else if (obj.GetName() == "Bip01 L UpperArmTwist")
			obj.SetName(IndexString("NPC L UpperarmTwist1 [LUt1]"));
		else if (obj.GetName() == "Bip01 R UpperArmTwist")
			obj.SetName(IndexString("NPC R UpperarmTwist1 [RUt1]"));
		else if (obj.GetName() == "Bip01 R ForearmTwist")
			obj.SetName(IndexString("NPC R ForearmTwist1 [RLt1]"));
		else if (obj.GetName() == "Bip01 L ForearmTwist")
			obj.SetName(IndexString("NPC L ForearmTwist1 [LLt1]"));
		else if (obj.GetName() == "Bip01 L UpperArm")
			obj.SetName(IndexString("NPC L UpperArm [LUar]"));
		else if (obj.GetName() == "Bip01 R UpperArm")
			obj.SetName(IndexString("NPC R UpperArm [RUar]"));
		else if (obj.GetName() == "Bip01 L Forearm")
			obj.SetName(IndexString("NPC L Forearm [LLar]"));
		else if (obj.GetName() == "Bip01 R Forearm")
			obj.SetName(IndexString("NPC R Forearm [RLar]"));
		else if (obj.GetName() == "Bip01 L Thigh") {
			obj.SetName(IndexString("NPC L Thigh [LThg]"));
			obj.SetTranslation(Vector3(-6.615073f, 0.000394f, 68.911301f));
			obj.SetRotation(Matrix33(
				-0.9943f, -0.0379f, 0.0999f,
				-0.0414f, 0.9986f, -0.0329f,
				-0.0985f, -0.0369f, -0.9944f));
		}
		else if (obj.GetName() == "Bip01 R Thigh") {
			obj.SetName(IndexString("NPC R Thigh [RThg]"));
			obj.SetTranslation(Vector3(6.615073f, 0.000394f, 68.911301f));
			obj.SetRotation(Matrix33(
				-0.9943f, 0.379f, -0.999f,
				0.0414f, 0.9986f, -0.0329f,
				0.0985f, -0.0368f, -0.9945f));
		}
		else if (obj.GetName() == "Bip01 L Hand")
			obj.SetName(IndexString("NPC L Hand [LHnd]"));
		else if (obj.GetName() == "Bip01 R Hand")
			obj.SetName(IndexString("NPC R Hand [RHnd]"));
		else if (obj.GetName() == "Bip01 Spine") {
			obj.SetName(IndexString("NPC Spine [Spn0]"));
			obj.SetTranslation(Vector3(0.000007f, -5.239862f, 72.702919f));
			obj.SetRotation(Matrix33(
				1.0f, -0.00f, 0.0f,
				0.0f, 0.9980f, -0.0436f,
				-0.00f, 0.0436f, 0.9990f));
		}
		else if (obj.GetName() == "Bip01 Spine1") {
			obj.SetName(IndexString("NPC Spine1 [Spn1]"));
			obj.SetTranslation(Vector3(0.000006f, -4.858528f, 81.443321f));
			obj.SetRotation(Matrix33(
				1.0f, -0.001f, 0.0f,
				0.0f, 0.9942f, 0.1071f,
				-0.001f, -0.1071f, 0.9942f));
		}
		else if (obj.GetName() == "Bip01 Spine2")
			obj.SetName(IndexString("NPC Spine2 [Spn2]"));
		else if (obj.GetName() == "Bip01 Pelvis") {
			obj.SetName(IndexString("NPC Pelvis [Pelv]"));
			obj.SetTranslation(Vector3(-0.000003f, -0.000010f, 68.911301f));
			obj.SetRotation(Matrix33());
		}
		else if (obj.GetName() == "Bip01 Neck1")
			obj.SetName(IndexString("NPC Neck [Neck]"));


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
			//if (block->IsSameType(NiParticleSystem::TYPE)) {
			//	NiParticleSystemRef stripsRef = DynamicCast<NiParticleSystem>(block);
			//	visit_particle(*stripsRef, obj);
			//}

			index++;
		}
		index = 0;
		//need to do furnitures here, we need to change BSFurnitureMarker to BSFurnitureMarkerNode
		for (NiExtraDataRef extra : extras)
		{
			if (extra->IsSameType(BSFurnitureMarker::TYPE)) {
				BSFurnitureMarkerRef oldNode = DynamicCast<BSFurnitureMarker>(extra);
				BSFurnitureMarkerNodeRef newNode = new BSFurnitureMarkerNode();
				newNode->SetName(IndexString("FRN")); //fix for furniture nodes with names which isn't FRN
				vector<FurniturePosition> newpositions;
				vector<FurniturePosition> positions = oldNode->GetPositions();

				for (FurniturePosition pos : positions)
				{
					FurniturePosition newpos = FurniturePosition();
					newpos.offset = pos.offset;
					newpos.offset.z += 35;

					if (pos.positionRef1 == 1) {
						newpos.animationType = AnimationType::SLEEP;
						newpos.entryProperties = FurnitureEntryPoints::LEFT;
						newpos.offset.x -= -90.826172f;
					}
					if (pos.positionRef1 == 2) {
						newpos.animationType = AnimationType::SLEEP;
						newpos.entryProperties = FurnitureEntryPoints::RIGHT;
						newpos.offset.x -= 90.826172f;
					}
					if (pos.positionRef1 == 3) {
						newpos.animationType = AnimationType::SLEEP;
						newpos.entryProperties = FurnitureEntryPoints::RIGHT;
					}
					if (pos.positionRef1 == 4) {
						newpos.animationType = AnimationType::SLEEP;
						newpos.entryProperties = FurnitureEntryPoints::BEHIND;
					}
					if (pos.positionRef1 == 11) {
						newpos.animationType = AnimationType::SIT;
						newpos.entryProperties = FurnitureEntryPoints::LEFT;
						newpos.offset.x -= -51.330994f;
					}
					if (pos.positionRef1 == 12) {
						newpos.animationType = AnimationType::SIT;
						newpos.entryProperties = FurnitureEntryPoints::RIGHT;
						newpos.offset.x -= 51.826050f;
					}
					if (pos.positionRef1 == 13) {
						newpos.animationType = AnimationType::SIT;
						newpos.entryProperties = FurnitureEntryPoints::BEHIND;
						newpos.offset.y -= -54.735596f;
					}
					if (pos.positionRef1 == 14) {
						newpos.animationType = AnimationType::SIT;
						newpos.entryProperties = FurnitureEntryPoints::FRONT;
						newpos.offset.y -= 55.295258f;
					}
					newpositions.push_back(newpos);
				}
				newNode->SetPositions(newpositions);
				if (extras.size() > 1) //fix markermatsideentry.nif vector crash.
					index++;
				extras[index] = DynamicCast<BSFurnitureMarkerNode>(newNode);
			}
		}
		//TODO
		//properties are deprecated
		obj.SetProperties(vector<NiPropertyRef>{});
		obj.SetChildren(children);
		obj.SetExtraDataList(extras);
	}

	template<>
	inline void visit_object(NiBillboardNode& obj) {
		vector<Ref<NiAVObject>> children = obj.GetChildren();
		vector<Ref<NiProperty>> properties = obj.GetProperties();
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
					if (obj.GetProperties().size() != 0) {
						vector<Ref<NiProperty>> sproperties = shape->GetProperties();
						sproperties.push_back(properties[0]);
						shape->SetProperties(sproperties);
					}
					children[index] = shape;
				}
			}

			index++;
		}
		obj.SetProperties(vector<NiPropertyRef>{});
		obj.SetExtraDataList(vector<NiExtraDataRef>{});
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
		bool hasSpecular = false;
		bool hasZbuffer = false;
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

				//TODO:: Create some kind of recursive method to modify GetNextControllers.
				if (material->GetController() != NULL) {
					if (material->GetController()->IsSameType(NiMaterialColorController::TYPE)) {
						NiMaterialColorControllerRef oldController = DynamicCast<NiMaterialColorController>(material->GetController());
						BSLightingShaderPropertyColorControllerRef controller = new BSLightingShaderPropertyColorController();
						controller->SetFlags(oldController->GetFlags());
						controller->SetFrequency(oldController->GetFrequency());
						controller->SetPhase(oldController->GetPhase());
						controller->SetStartTime(oldController->GetStartTime());
						controller->SetStopTime(oldController->GetStopTime());
						controller->SetTarget(lightingProperty);
						controller->SetInterpolator(oldController->GetInterpolator());
						if (oldController->GetTargetColor() == MaterialColor::TC_SELF_ILLUM)
							controller->SetTypeOfControlledColor(LightingShaderControlledColor::LSCC_EMISSIVE_COLOR);
						//constructor sets to specular.

						lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
						material_controllers_map[oldController] = controller;
					}
					if (material->GetController()->IsSameType(NiAlphaController::TYPE)) {
						NiAlphaControllerRef oldController = DynamicCast<NiAlphaController>(material->GetController());
						BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
						controller->SetFlags(oldController->GetFlags());
						controller->SetFrequency(oldController->GetFrequency());
						controller->SetPhase(oldController->GetPhase());
						controller->SetStartTime(oldController->GetStartTime());
						controller->SetStopTime(oldController->GetStopTime());
						controller->SetTarget(lightingProperty);
						controller->SetInterpolator(oldController->GetInterpolator());
						controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_ALPHA);

						lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
						material_alpha_controllers_map[oldController] = controller;
					}
				}
			}

			if (property->IsSameType(NiTexturingProperty::TYPE)) {
				texturing = DynamicCast<NiTexturingProperty>(property);
				string textureName;
				if (texturing->GetBaseTexture().source != NULL) {
					textureName += texturing->GetBaseTexture().source->GetFileName();
					//fix for orconebraid
					if (textureName == "Grey.dds" || textureName == "grey.dds")
						textureName = "textures\\characters\\hair\\Grey.dds";

					textureName.insert(9, "tes4\\");
					string textureNormal = textureName;
					textureNormal.erase(textureNormal.end() - 4, textureNormal.end());
					textureNormal += "_n.dds";

					//setup textureSet (TODO)
					std::vector<std::string> textures(9);
					textures[0] = textureName;

					Games& games = Games::Instance();
					const Games::GamesPathMapT& installations = games.getGames();

					if (fs::exists(games.data(Games::TES5) / textureNormal))
						textures[1] = textureNormal;
					else
						textures[1] = "textures\\default_n.dds";

					//finally set them.
					textureSet->SetTextures(textures);
				}

				if (texturing->GetController() == NULL)
					continue;

				if (texturing->GetController()->IsSameType(NiFlipController::TYPE)) {
					NiFlipControllerRef oldController = DynamicCast<NiFlipController>(texturing->GetController());
					BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
					controller->SetFlags(oldController->GetFlags());
					controller->SetFrequency(oldController->GetFrequency());
					controller->SetPhase(oldController->GetPhase());
					controller->SetStartTime(oldController->GetStartTime());
					controller->SetStopTime(oldController->GetStopTime());
					controller->SetTarget(lightingProperty);
					controller->SetInterpolator(oldController->GetInterpolator());
					controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_OFFSET);

					lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
					material_flip_controllers_map[oldController] = controller;
				}

				if (texturing->GetController()->IsSameType(NiTextureTransformController::TYPE)) {
					NiTextureTransformControllerRef oldController = DynamicCast<NiTextureTransformController>(texturing->GetController());
					BSLightingShaderPropertyFloatControllerRef controller = convert(oldController);
					controller->SetTarget(lightingProperty);
					lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
					while (controller->GetNextController() != NULL) {
						oldController = DynamicCast<NiTextureTransformController>(controller->GetNextController());
						if (oldController != NULL) {
							controller = convert(oldController);
						}
						else {
							controller = DynamicCast<BSLightingShaderPropertyFloatController>(controller->GetNextController());
						}
						if (controller != NULL) {
							controller->SetTarget(lightingProperty);
						}
						else
							break;
					}
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
			if (property->IsSameType(NiSpecularProperty::TYPE)) {
				hasSpecular = true;
			}
			if (property->IsSameType(NiZBufferProperty::TYPE)) {
				hasZbuffer = true;
				NiZBufferPropertyRef buffer = DynamicCast<NiZBufferProperty>(property);
			}
		}
		NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(obj.GetData());
		if (data != NULL) {

			if (!data->GetVertexColors().empty()) {
				data->SetHasVertexColors(true);
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() | SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
			}
			else {
				data->SetHasVertexColors(false);
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
			}
		}
		if (!hasSpecular)
			lightingProperty->SetShaderFlags1_sk(static_cast<SkyrimShaderPropertyFlags1>(lightingProperty->GetShaderFlags1_sk() & ~SkyrimShaderPropertyFlags1::SLSF1_SPECULAR));

		if (hasZbuffer) {
			lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_ZBUFFER_WRITE));
			//lightingProperty->SetShaderFlags1_sk(static_cast<SkyrimShaderPropertyFlags1>(lightingProperty->GetShaderFlags1_sk() & ~SkyrimShaderPropertyFlags1::SLSF1_ZBUFFER_TEST));
		}
		if(obj.GetSkinInstance() != NULL)
			lightingProperty->SetShaderFlags1_sk(static_cast<SkyrimShaderPropertyFlags1>(lightingProperty->GetShaderFlags1_sk() | SkyrimShaderPropertyFlags1::SLSF1_SKINNED));

		if (textureSet->GetTextures().size() == 0)
			obj.SetFlags(obj.GetFlags() + 1);

		lightingProperty->SetTextureSet(textureSet);
		obj.SetShaderProperty(DynamicCast<BSShaderProperty>(lightingProperty));
		obj.SetExtraDataList(vector<Ref<NiExtraData>> {});
		obj.SetProperties(vector<Ref<NiProperty>> {});

		if (obj.GetSkinInstance() != NULL) {
			NiSkinInstanceRef skinInstance = DynamicCast<NiSkinInstance>(obj.GetSkinInstance());
			NiSkinDataRef skinData = DynamicCast<NiSkinData>(skinInstance->GetData());
			NiSkinPartitionRef skinPartition = DynamicCast<NiSkinPartition>(skinInstance->GetSkinPartition());
			vector<SkinPartition> partitionBlocks = skinPartition->GetSkinPartitionBlocks();
			
			
			for (int i = 0; i != partitionBlocks.size(); i++) {
				SkinPartition* block = &partitionBlocks[i];
				block->triangles = triangulate(block->strips);
				block->numStrips = 0;			
				block->strips = vector<vector<unsigned short>>(0);
				block->stripLengths = vector<unsigned short>(0);
			}

			skinPartition->SetSkinPartitionBlocks(partitionBlocks);
			skinInstance->SetSkinPartition(skinPartition);
			obj.SetSkinInstance(skinInstance);			
		}
	}

	//inline void visit_particle(NiParticleSystem& obj, NiAVObject& parent) {
	template<>
	inline void visit_object(NiParticleSystem& obj) {
		bool hasSpecular = false;
		bool hasZBuffer = false;
		BSEffectShaderPropertyRef lightingProperty = new BSEffectShaderProperty();
		//BSShaderTextureSetRef textureSet = new BSShaderTextureSet();
		NiMaterialPropertyRef material = new NiMaterialProperty();
		NiTexturingPropertyRef texturing = new NiTexturingProperty();
		vector<Ref<NiProperty>> properties = obj.GetProperties();

		for (NiPropertyRef property : properties)
		{
			if (property->IsSameType(NiMaterialProperty::TYPE)) {
				material = DynamicCast<NiMaterialProperty>(property);
				lightingProperty->SetShaderType(BSShaderType::SHADER_DEFAULT);
				Color3 em = material->GetEmissiveColor();
				lightingProperty->SetEmissiveColor(Color4(em.r, em.g, em.b, 0.0));
				//lightingProperty->SetSpecularColor(material->GetSpecularColor());
				lightingProperty->SetEmissiveMultiple(1);
				//lightingProperty->SetGlossiness(material->GetGlossiness());
				//lightingProperty->SetAlpha(material->GetAlpha());

				//TODO:: Create some kind of recursive method to modify GetNextControllers.
				if (material->GetController() != NULL) {
					if (material->GetController()->IsSameType(NiMaterialColorController::TYPE)) {
						NiMaterialColorControllerRef oldController = DynamicCast<NiMaterialColorController>(material->GetController());
						BSLightingShaderPropertyColorControllerRef controller = new BSLightingShaderPropertyColorController();
						controller->SetFlags(oldController->GetFlags());
						controller->SetFrequency(oldController->GetFrequency());
						controller->SetPhase(oldController->GetPhase());
						controller->SetStartTime(oldController->GetStartTime());
						controller->SetStopTime(oldController->GetStopTime());
						controller->SetTarget(lightingProperty);
						controller->SetInterpolator(oldController->GetInterpolator());
						if (oldController->GetTargetColor() == MaterialColor::TC_SELF_ILLUM)
							controller->SetTypeOfControlledColor(LightingShaderControlledColor::LSCC_EMISSIVE_COLOR);
						//constructor sets to specular.

						lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
						material_controllers_map[oldController] = controller;
					}
					if (material->GetController()->IsSameType(NiAlphaController::TYPE)) {
						NiAlphaControllerRef oldController = DynamicCast<NiAlphaController>(material->GetController());
						BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
						controller->SetFlags(oldController->GetFlags());
						controller->SetFrequency(oldController->GetFrequency());
						controller->SetPhase(oldController->GetPhase());
						controller->SetStartTime(oldController->GetStartTime());
						controller->SetStopTime(oldController->GetStopTime());
						controller->SetTarget(lightingProperty);
						controller->SetInterpolator(oldController->GetInterpolator());
						controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_ALPHA);

						lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
						material_alpha_controllers_map[oldController] = controller;
					}
				}
			}

			if (property->IsSameType(NiTexturingProperty::TYPE)) {
				texturing = DynamicCast<NiTexturingProperty>(property);
				string textureName;
				if (texturing->GetBaseTexture().source != NULL) {
					textureName += texturing->GetBaseTexture().source->GetFileName();
					//fix for orconebraid
					if (textureName == "Grey.dds" || textureName == "grey.dds")
						textureName = "textures\\characters\\hair\\Grey.dds";

					textureName.insert(9, "tes4\\");
					string textureNormal = textureName;
					textureNormal.erase(textureNormal.end() - 4, textureNormal.end());
					textureNormal += "_n.dds";

					lightingProperty->SetSourceTexture(textureName);

					if (texturing->GetController() == NULL)
						continue;

					if (texturing->GetController()->IsSameType(NiFlipController::TYPE)) {
						NiFlipControllerRef oldController = DynamicCast<NiFlipController>(texturing->GetController());
						BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
						controller->SetFlags(oldController->GetFlags());
						controller->SetFrequency(oldController->GetFrequency());
						controller->SetPhase(oldController->GetPhase());
						controller->SetStartTime(oldController->GetStartTime());
						controller->SetStopTime(oldController->GetStopTime());
						controller->SetTarget(lightingProperty);
						controller->SetInterpolator(oldController->GetInterpolator());
						controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_SCALE);

						lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
						material_flip_controllers_map[oldController] = controller;
					}

					if (texturing->GetController()->IsSameType(NiTextureTransformController::TYPE)) {
						NiTextureTransformControllerRef oldController = DynamicCast<NiTextureTransformController>(texturing->GetController());
						BSLightingShaderPropertyFloatControllerRef controller = new BSLightingShaderPropertyFloatController();
						controller->SetFlags(oldController->GetFlags());
						controller->SetFrequency(oldController->GetFrequency());
						controller->SetPhase(oldController->GetPhase());
						controller->SetStartTime(oldController->GetStartTime());
						controller->SetStopTime(oldController->GetStopTime());
						controller->SetTarget(lightingProperty);
						controller->SetInterpolator(oldController->GetInterpolator());
						if (oldController->GetOperation() == TransformMember::TT_TRANSLATE_U)
							controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_U_OFFSET);
						if (oldController->GetOperation() == TransformMember::TT_TRANSLATE_V)
							controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_OFFSET);
						if (oldController->GetOperation() == TransformMember::TT_SCALE_U)
							controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_U_SCALE);
						if (oldController->GetOperation() == TransformMember::TT_SCALE_V)
							controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_V_SCALE);
						if (oldController->GetOperation() == TransformMember::TT_ROTATE)
							controller->SetTypeOfControlledVariable(LightingShaderControlledVariable::LSCV_ALPHA);

						lightingProperty->SetController(DynamicCast<NiTimeController>(controller));
						material_transform_controllers_map[oldController] = controller;
					}
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
			if (property->IsSameType(NiSpecularProperty::TYPE)) {
				hasSpecular = true;
			}
			if (property->IsSameType(NiZBufferProperty::TYPE)) {
				hasZBuffer = true;
			}
		}
		//extract geometry
		NiPSysDataRef data = DynamicCast<NiPSysData>(obj.NiGeometry::GetData());
		if (data != NULL) {
			//if (data->GetHasRadii()) {
			//	data->SetRadii(vector<float>{});
			//}
			//if (data->GetHasSizes()) {
			//	data->SetSizes(vector<float>{});
			//}
			//data->SetRotationAngles(vector<float>{});
			//data->SetHasTextureIndices(!data->GetSubtextureOffsets().empty());

			NiTriShapeDataRef geom_data = new NiTriShapeData();
			geom_data->SetHasVertices(data->GetHasVertices());
			geom_data->SetVertices(data->GetVertices());
			//data->SetHasVertices(true);
			data->SetBsMaxVertices(data->GetVertices().size());
			data->NiGeometryData::SetVertices(vector<Vector3>());
			data->SetVertices(vector<Vector3>());
			geom_data->SetVertexColors(data->GetVertexColors());
			data->SetVertexColors(vector<Color4>{});
			geom_data->SetHasVertexColors(data->GetHasVertexColors());
			data->SetHasVertexColors(false);

			NiTriShapeRef particle_geom = new NiTriShape();
			particle_geom->SetData(StaticCast<NiGeometryData>(geom_data));

			particle_geom->SetName(obj.GetName() + "Emitter");

			particle_geometry.push_back(particle_geom);

			NiPSysMeshEmitterRef emitter = new NiPSysMeshEmitter();
			vector<NiAVObject * > emittee = emitter->GetEmitterMeshes();
			emittee.push_back(StaticCast<NiAVObject>(particle_geom));
			emitter->SetEmitterMeshes(emittee);

			vector<Ref<NiPSysModifier > > mods = obj.GetModifiers();
			mods.push_back(StaticCast<NiPSysModifier>(emitter));
			obj.SetModifiers(mods);

			if (!data->GetVertexColors().empty()) {
				data->SetHasVertexColors(true);
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() | SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
			}
			else {
				data->SetHasVertexColors(false);
				lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
			}
		}
		if (!hasSpecular)
			lightingProperty->SetShaderFlags1_sk(static_cast<SkyrimShaderPropertyFlags1>(lightingProperty->GetShaderFlags1_sk() & ~SkyrimShaderPropertyFlags1::SLSF1_SPECULAR));

		if (hasZBuffer) {
			lightingProperty->SetShaderFlags2_sk(static_cast<SkyrimShaderPropertyFlags2>(lightingProperty->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_ZBUFFER_WRITE));
		}
		lightingProperty->SetShaderFlags1_sk(static_cast<SkyrimShaderPropertyFlags1>(SkyrimShaderPropertyFlags1::SLSF1_ZBUFFER_TEST | SkyrimShaderPropertyFlags1::SLSF1_OWN_EMIT));

		//lightingProperty->SetTextureSet(textureSet);
		obj.SetShaderProperty(DynamicCast<BSShaderProperty>(lightingProperty));
		obj.SetExtraDataList(vector<Ref<NiExtraData>> {});
		obj.SetProperties(vector<Ref<NiProperty>> {});
	}

	template<>
	inline void visit_object(NiControllerSequence& obj)
	{

		nisequences.insert(obj.GetName());
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
			if (blocks[i].stringPalette != NULL)
			{
				//Deprecated. Maybe we can handle with tri facegens
				if (blocks[i].controller != NULL && blocks[i].controller->IsDerivedType(NiGeomMorpherController::TYPE))
					continue;

				blocks[i].nodeName = getStringFromPalette(blocks[i].stringPalette->GetPalette().palette, blocks[i].nodeNameOffset);
				blocks[i].controllerType = getStringFromPalette(blocks[i].stringPalette->GetPalette().palette, blocks[i].controllerTypeOffset);

				if (blocks[i].propertyTypeOffset != 4294967295)
					blocks[i].propertyType = getStringFromPalette(blocks[i].stringPalette->GetPalette().palette, blocks[i].propertyTypeOffset);

				if (blocks[i].controllerIdOffset != 4294967295)
					blocks[i].controllerId = getStringFromPalette(blocks[i].stringPalette->GetPalette().palette, blocks[i].controllerIdOffset);

				if (blocks[i].interpolatorIdOffset != 4294967295)
					blocks[i].interpolatorId = getStringFromPalette(blocks[i].stringPalette->GetPalette().palette, blocks[i].interpolatorIdOffset);
			}

			if (blocks[i].controller != NULL && blocks[i].controller->IsDerivedType(NiMaterialColorController::TYPE))
			{
				blocks[i].propertyType = "BSLightingShaderProperty";
				blocks[i].controllerType = "BSLightingShaderPropertyColorController";
			}
			if (blocks[i].controller != NULL && blocks[i].controller->IsDerivedType(NiTextureTransformController::TYPE))
			{
				blocks[i].propertyType = "BSLightingShaderProperty";
				blocks[i].controllerType = "BSLightingShaderPropertyFloatController";
			}
			if (blocks[i].controller != NULL && (blocks[i].controller->IsDerivedType(NiAlphaController::TYPE) || blocks[i].controller->IsDerivedType(NiFlipController::TYPE))) //hoping this works
			{
				blocks[i].propertyType = "BSLightingShaderProperty";
				blocks[i].controllerType = "BSLightingShaderPropertyFloatController";
			}

			//set to default... if above doesn't work
			if (blocks[i].controllerType == "")
				throw runtime_error("controller type is null; will cause errors.");

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
			if (std::strstr(textKeys[i].data.c_str(), "Sound:") || std::strstr(textKeys[i].data.c_str(), "sound:")) {
				textKeys[i].data.insert(7, "TES4");
			}
		}

		obj.SetTextKeys(textKeys);

	}

	//from now on, we must switch from an old type to hkTransform, so it is useful to directly use havok data

	template<>
	inline void visit_object(bhkCollisionObject& obj) {
		if (already_upgraded.insert(&obj).second) {
			if (obj.GetBody() != NULL)
				collision_target_map[&*obj.GetBody()] = obj.GetTarget();
		}
	}

	template<>
	inline void visit_object(bhkBlendCollisionObject& obj) {
		if (already_upgraded.insert(&obj).second) {
			if (obj.GetBody() != NULL)
				collision_target_map[&*obj.GetBody()] = obj.GetTarget();
		}
	}

	template<>
	inline void visit_object(bhkSPCollisionObject& obj) {
		if (already_upgraded.insert(&obj).second) {
			if (obj.GetBody() != NULL)
				collision_target_map[&*obj.GetBody()] = obj.GetTarget();
		}
	}

	template<>
	inline void visit_object(bhkRigidBody& obj) {
		if (already_upgraded.insert(&obj).second) {
			NiAVObject* target = (NiAVObject*)collision_target_map[&obj];
			Accessor<bhkRigidBodyUpgrader> upgrader(obj, this_info, target);
		}
	}

	template<>
	inline void visit_object(bhkRigidBodyT& obj) {
		if (already_upgraded.insert(&obj).second) {
			NiAVObject* target = (NiAVObject*)collision_target_map[&obj];
			Accessor<bhkRigidBodyUpgrader> upgrader(obj, this_info, target);
		}
	}

	//Upgrade shapes

	template<typename T> void convertMaterialAndRadius(T& shape) {
		HavokMaterial material = shape.GetMaterial();
		material.material_sk = convert_havok_material(material.material_ob);
		shape.SetMaterial(material);
		shape.SetRadius(shape.GetRadius() * COLLISION_RATIO);
	}

	//phantoms
	template<>
	inline void visit_object(bhkSimpleShapePhantom& obj) {
		if (already_upgraded.insert(&obj).second) {
			HavokFilter layer = obj.GetHavokFilter();
			layer.layer_sk = convert_havok_layer(layer.layer_ob);
			obj.SetHavokFilter(layer);
		}
	}

	//Containers
	template<>
	inline void visit_object(bhkConvexTransformShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
			Matrix44 transform = obj.GetTransform();
			Vector3 trans = transform.GetTrans();
			trans.x *= COLLISION_RATIO;
			trans.y *= COLLISION_RATIO;
			trans.z *= COLLISION_RATIO;
			transform.SetTrans(trans);
			obj.SetTransform(transform);
			obj.SetShape(upgrade_shape(obj.GetShape(), this_info, NULL));
		}
	}

	template<>
	inline void visit_object(bhkTransformShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			convertMaterialAndRadius(obj);
			Matrix44 transform = obj.GetTransform();
			Vector3 trans = transform.GetTrans();
			trans.x *= COLLISION_RATIO;
			trans.y *= COLLISION_RATIO;
			trans.z *= COLLISION_RATIO;
			transform.SetTrans(trans);
			obj.SetTransform(transform);
			obj.SetShape(upgrade_shape(obj.GetShape(), this_info, NULL));
		}
	}

	template<>
	inline void visit_object(bhkListShape& obj) {
		if (already_upgraded.insert(&obj).second) {
			HavokMaterial material = obj.GetMaterial();
			material.material_sk = convert_havok_material(material.material_ob);
			obj.SetMaterial(material);
			obj.SetSubShapes(upgrade_shapes(obj.GetSubShapes(), this_info, NULL));
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
			convertMaterialAndRadius(obj);
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

class FixTargetsVisitor : public RecursiveFieldVisitor<FixTargetsVisitor> {
	vector<NiObjectRef>& blocks;
public:


	FixTargetsVisitor(NiObject* root, const NifInfo& info, vector<NiObjectRef>& blocks) :
		RecursiveFieldVisitor(*this, info), blocks(blocks) {
		root->accept(*this, info);
	}


	template<class T>
	inline void visit_object(T& obj) {}

	template<>
	inline void visit_object(NiDefaultAVObjectPalette& obj) {
		vector<AVObject > av_objects = obj.GetObjs();
		for (AVObject& av_object : av_objects) {
			for (NiObjectRef ref : blocks) {
				if (ref->IsDerivedType(NiAVObject::TYPE)) {
					NiAVObjectRef av_ref = DynamicCast<NiAVObject>(ref);
					if (av_ref->GetName() == av_object.name) {
						av_object.avObject = DynamicCast<NiAVObject>(av_ref);
					}
				}
			}
		}
		obj.SetObjs(av_objects);
	}

	template<>
	inline void visit_object(NiControllerSequence& obj) {
		vector<ControlledBlock> blocks = obj.GetControlledBlocks();

		for (int i = 0; i != blocks.size(); i++) {
			NiTimeControllerRef controller = blocks[i].controller;

			//specific fix for oblivion fountains. 
			if (controller->IsSameType(BSLightingShaderPropertyColorController::TYPE) && controller->GetTarget()->IsSameType(BSEffectShaderProperty::TYPE)) {

				BSLightingShaderPropertyColorControllerRef lightingController = DynamicCast<BSLightingShaderPropertyColorController>(controller);
				BSEffectShaderPropertyColorController* newController = new BSEffectShaderPropertyColorController();

				if (lightingController->GetNextController() != NULL)
					Log::Warn("Has nested controller");

				newController->SetFlags(lightingController->GetFlags());
				newController->SetFrequency(lightingController->GetFrequency());
				newController->SetPhase(lightingController->GetPhase());
				newController->SetStartTime(lightingController->GetStartTime());
				newController->SetStopTime(lightingController->GetStopTime());
				newController->SetPhase(lightingController->GetPhase());
				newController->SetTarget(lightingController->GetTarget());
				newController->SetTypeOfControlledColor(EffectShaderControlledColor::ECSC_EMISSIVE_COLOR);

				blocks[i].controller = newController;
				blocks[i].propertyType = "BSEffectShaderProperty";
				blocks[i].controllerType = "BSEffectShaderPropertyColorController";

				BSEffectShaderPropertyRef shader = DynamicCast<BSEffectShaderProperty>(newController->GetTarget());
				shader->SetController(blocks[i].controller);
			}
		}

		obj.SetControlledBlocks(blocks);
	}

	template<>
	inline void visit_object(NiTriShape& obj) {
		if (obj.GetSkinInstance() != NULL) {

			NiSkinInstanceRef skinInstance = DynamicCast<NiSkinInstance>(obj.GetSkinInstance());
			NiSkinDataRef skinData = DynamicCast<NiSkinData>(skinInstance->GetData());
			NiSkinPartitionRef skinPartition = DynamicCast<NiSkinPartition>(skinInstance->GetSkinPartition());
			NiTriShapeDataRef shapeData = DynamicCast<NiTriShapeData>(obj.GetData());

			vector<BoneData> boneList = skinData->GetBoneList();
			SkinPartition skinBlock = skinPartition->GetSkinPartitionBlocks()[0];

			for (int i = 0; i != skinInstance->GetBones().size(); i++) {
				NiNodeRef bone = skinInstance->GetBones()[i];

				if (bone->GetName() == "NPC Pelvis [Pelv]") {
					vector<BoneVertData> vertData = boneList[i].vertexWeights;
					vector<Vector3> vertices = vector<Vector3>(vertData.size());
					for (int i = 0; i != vertData.size(); i++) {
						vertices[i] = shapeData->GetVertices()[skinBlock.vertexMap[vertData[i].index]];
					}
					Vector3 newPos = centeroid(vertices);
					Vector3 bonePos = Vector3(-0.000003f, -0.000010f, 68.911301f);
					bonePos.x = -bonePos.x;
					bonePos.y = -bonePos.y;
					bonePos.z = -bonePos.z;
					boneList[i].skinTransform.translation = bonePos + newPos;
					boneList[i].skinTransform.rotation = Matrix33();
				}
				if (bone->GetName() == "NPC L Thigh [LThg]") {
					vector<BoneVertData> vertData = boneList[i].vertexWeights;
					vector<Vector3> vertices = vector<Vector3>(vertData.size());
					for (int i = 0; i != vertData.size(); i++) {
						vertices[i] = shapeData->GetVertices()[skinBlock.vertexMap[vertData[i].index]];
					}
					//correct? dunno :D
					Matrix44 matrix = Matrix44();
					matrix.rows[0][0] = bone->GetRotation().rows[0][0];
					matrix.rows[0][1] = bone->GetRotation().rows[0][1];
					matrix.rows[0][2] = bone->GetRotation().rows[0][2];
					matrix.rows[0][3] = bone->GetTranslation().x;
					matrix.rows[1][0] = bone->GetRotation().rows[1][0];
					matrix.rows[1][1] = bone->GetRotation().rows[1][1];
					matrix.rows[1][2] = bone->GetRotation().rows[1][2];
					matrix.rows[1][3] = bone->GetTranslation().y;
					matrix.rows[2][0] = bone->GetRotation().rows[2][0];
					matrix.rows[2][1] = bone->GetRotation().rows[2][1];
					matrix.rows[2][2] = bone->GetRotation().rows[2][2];
					matrix.rows[2][3] = bone->GetTranslation().z;






					//Vector3 newPos = centeroid(vertices);
					//Vector3 bonePos = Vector3(-6.615073f, 0.000394f, 68.911301f);
					//boneList[i].skinTransform.translation = bonePos + newPos;
					//boneList[i].skinTransform.translation = Vector3(-13.517195f, 1.999292f, 67.866142f);
					//boneList[i].skinTransform.rotation = Matrix33(
					//	-0.9942f, -0.0410f, -0.0933f,
					//	-0.0375f, 0.9986f, -0.0369f,
					//	0.1007f, -0.0330f, -0.9944);
				}
				//if (bone->GetName() == "NPC R Thigh [RThg]") {
				//	vector<BoneVertData> vertData = boneList[i].vertexWeights;
				//	vector<Vector3> vertices = vector<Vector3>(vertData.size());
				//	for (int i = 0; i != vertData.size(); i++) {
				//		vertices[i] = shapeData->GetVertices()[skinBlock.vertexMap[vertData[i].index]];
				//	}
				//	Vector3 newPos = centeroid(vertices);
				//	Vector3 bonePos = Vector3(6.615068f, 0.000394f, 68.911301f);
				//	boneList[i].skinTransform.translation = bonePos + newPos;
				//	//boneList[i].skinTransform.translation = Vector3(13.461305f, 1.992750f, 67.877457f);
				//	boneList[i].skinTransform.rotation = Matrix33(
				//		-0.9943f, 0.0413f, 0.0985f,
				//		0.0378f, 0.9986f, -0.0369f,
				//		-0.0998f, -0.0330f, -0.9945f);
				//}
				if (bone->GetName() == "NPC Spine [Spn0]") {
					boneList[i].skinTransform.translation = Vector3(-0.000022f, 8.403816f, -72.405434f);
					boneList[i].skinTransform.rotation = Matrix33(
						1.0f, 0.0000f, -0.0000f,
						-0.000f, 0.9990f, 0.0436f,
						0.000f, -0.0436f, 0.9990f);
				}
				//if (bone->GetName() == "NPC Spine1 [Spn1]") {
				//	boneList[i].skinTransform.translation = Vector3(-0.000018f, -3.890695f, -81.495285f);
				//	boneList[i].skinTransform.rotation = Matrix33(
				//		1.0f, 0.0000f, -0.0000f,
				//		-0.000f, 0.9942f, -0.1071f,
				//		0.000f, 0.1071f, 0.9942f);
				//}
				//if (bone->GetName() == "NPC Spine2 [Spn2]") {
				//	boneList[i].skinTransform.translation = Vector3(-0.000093f, 18.403816f, -89.638092f);
				//	boneList[i].skinTransform.rotation = Matrix33(
				//		1.0f, 0.0000f, -0.0000f,
				//		-0.000f, 0.9910f, 0.1336f,
				//		0.000f, -0.1336f, 0.9910f);
				//}
				//if (bone->GetName() == "NPC Head [Head]") {
				//	boneList[i].skinTransform.translation = Vector3(-0.000256f, -1.547517f, 0);
				//	boneList[i].skinTransform.rotation = Matrix33();
				//}
				//if (bone->GetName() == "NPC R Clavicle [RClv]") {
				//	boneList[i].skinTransform.translation = Vector3(93.688957f, 51.393600f, 25.169991f);
				//	boneList[i].skinTransform.rotation = Matrix33(
				//		-0.4490f, 0.3924f, 0.8028f,
				//		-0.3051f, 0.7771f, -0.5505f,
				//		-0.8398f, -0.4921f, -0.2292f);
				//}
				//if (bone->GetName() == "NPC L Clavicle [LClv]") {
				//	boneList[i].skinTransform.translation = Vector3(-93.689011f, 51.393459f, 25.170134f);
				//	boneList[i].skinTransform.rotation = Matrix33(
				//		-0.4490f, -0.3924f, -0.8028f,
				//		0.3051f, 0.7771f, -0.5505f,
				//		0.8398f, -0.4921f, -0.2292f);
				//}
			}


			skinData->SetBoneList(boneList);
			skinInstance->SetData(skinData);
			obj.SetSkinInstance(skinInstance);
		}
	}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<>
	void visit_compound(AVObject& avlink) {
		//relink av objects on converted nistrips;

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

	set<set<string>> sequences_groups;
	HKXWrapperCollection wrappers;
	if (nifs.empty()) {
		Log::Info("No NIFs found.. trying BSAs");



		Games& games = Games::Instance();
		const Games::GamesPathMapT& installations = games.getGames();

		for (const auto& bsa : games.bsas(Games::TES4)) {
			std::cout << "Checking: " << bsa.filename() << std::endl;
			BSAFile bsa_file(bsa);
			for (const auto& nif : bsa_file.assets(".*\.nif")) {
				Log::Info("Current File: %s", nif.c_str());

				fs::path out_path = nif_out / nif;

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
				if (nif.find("\\menus\\") != string::npos) {
					Log::Warn("Ignored obsolete menu nifs: %s", nif.c_str());
					continue;
				}
				if (nif.find("\\creatures\\") != string::npos) {
					Log::Warn("temporarily ignoring: %s", nif.c_str());
					continue;
				}
				size_t size = -1;
				const uint8_t* data = bsa_file.extract(nif, size);

				std::string sdata((char*)data, size);
				std::istringstream iss(sdata);

				//this is all hacky but ehhhh.
				bool isBillboardRoot = false;
				BillboardMode mode = BillboardMode::ALWAYS_FACE_CAMERA;

				vector<NiObjectRef> blocks = ReadNifList(iss, &info);
				NiObjectRef root = GetFirstRoot(blocks);
				NiNode* rootn = DynamicCast<NiNode>(root);

				ConverterVisitor fimpl(info, root, blocks);

				if (root->IsSameType(NiBillboardNode::TYPE)) {
					isBillboardRoot = true;
					mode = DynamicCast<NiBillboardNode>(root)->GetBillboardMode();
				}

				set<string> sequences = fimpl.nisequences;
				//root->accept(fimpl, info);
				if (!NifFile::hasExternalSkinnedMesh(blocks, rootn)) {
					root = convert_root(root);
					BSFadeNodeRef bsroot = DynamicCast<BSFadeNode>(root);
					//fixed?
					bsroot->SetFlags(524302);
					string out_havok_path = "";
					if (!sequences.empty()) {
						fs::path in_file = nif;
						string out_name = in_file.filename().replace_extension("").string();
						string newPath = in_file.parent_path().string();
						if (newPath.substr(0, 7) == "meshes\\")
							newPath.erase(newPath.begin(), newPath.begin()+7);
						fs::path out_path = fs::path("animations") / newPath / out_name;
						fs::path out_path_abs = nif_out / out_path;
						string out_path_a = out_path_abs.string();
						out_havok_path = wrappers.wrap(out_name, out_path.parent_path().string(), out_path_a, "TES4", sequences);
						vector<Ref<NiExtraData > > list = bsroot->GetExtraDataList();
						BSBehaviorGraphExtraDataRef havokp = new BSBehaviorGraphExtraData();
						havokp->SetName(string("BGED"));
						havokp->SetBehaviourGraphFile(out_havok_path);
						havokp->SetControlsBaseSkeleton(false);
						list.insert(list.begin(), StaticCast<NiExtraData>(havokp));
						bsroot->SetExtraDataList(list);
					}

					std::vector<NiAVObjectRef> children;

					if (isBillboardRoot) //fxmistoblivion01 has NiBillboardNode root
					{
						NiBillboardNode* proxyRoot = new NiBillboardNode();
						proxyRoot->SetBillboardMode(mode);
						proxyRoot->SetFlags(bsroot->GetFlags());
						proxyRoot->SetChildren(bsroot->GetChildren());
						proxyRoot->SetRotation(bsroot->GetRotation());
						proxyRoot->SetTranslation(bsroot->GetTranslation());
						proxyRoot->SetCollisionObject(bsroot->GetCollisionObject());
						proxyRoot->SetName(IndexString("ProxyNode"));

						if (proxyRoot->GetCollisionObject() != NULL)
							DynamicCast<NiCollisionObject>(proxyRoot->GetCollisionObject())->SetTarget(proxyRoot);

						children.push_back(proxyRoot);

					}
					else
					{
						if (bsroot->GetTranslation().Magnitude() != 0 ||
							!bsroot->GetRotation().isIdentity()) 
						{
							NiNode* proxyRoot = new NiNode();

							proxyRoot->SetFlags(bsroot->GetFlags());
							proxyRoot->SetChildren(bsroot->GetChildren());
							proxyRoot->SetRotation(bsroot->GetRotation());
							proxyRoot->SetTranslation(bsroot->GetTranslation());
							proxyRoot->SetCollisionObject(bsroot->GetCollisionObject());
							proxyRoot->SetName(IndexString("ProxyNode"));

							if (proxyRoot->GetCollisionObject() != NULL)
								DynamicCast<NiCollisionObject>(proxyRoot->GetCollisionObject())->SetTarget(proxyRoot);

							children.push_back(proxyRoot);

							bsroot->SetRotation(Matrix33());
							bsroot->SetTranslation(Vector3());
							bsroot->SetCollisionObject(NULL);
							bsroot->SetChildren(children);
						}
					}



					//to calculate the right flags, we need to rebuild the blocks
					vector<NiObjectRef> new_blocks = RebuildVisitor(root, info).blocks;

					//fix targets from nitrishapes substitution
					FixTargetsVisitor(root, info, new_blocks);

					if (DynamicCast<NiNode>(root) != NULL && DynamicCast<NiNode>(root)->GetCollisionObject() == NULL) {
						bhkCollisionObjectRef root_collision = NULL;
						int num_collisions = 0;
						//Optimize single collision models
						for (NiObjectRef block : new_blocks) {
							if (block->IsDerivedType(bhkCollisionObject::TYPE))
							{
								num_collisions++;
								root_collision = DynamicCast<bhkCollisionObject>(block);
							}
						}
						if (num_collisions == 1 && root_collision != NULL) {

							vector<NiAVObjectRef> children = bsroot->GetChildren();
							auto root_collision_position = find(children.begin(), children.end(), StaticCast<NiAVObject>(root_collision));
							if (root_collision_position != children.end()) {
								children.erase(root_collision_position);
								bsroot->SetCollisionObject(StaticCast<NiCollisionObject>(root_collision));
								bsroot->SetChildren(children);
							}
						}
					}


					bsx_flags_t calculated_flags = calculateSkyrimBSXFlags(new_blocks, info);

					for (NiObjectRef ref : blocks) {
						if (ref->IsDerivedType(BSXFlags::TYPE)) {
							BSXFlagsRef bref = DynamicCast<BSXFlags>(ref);
							bref->SetIntegerData(calculated_flags.to_ulong());
							break;
						}
					}
				}
				else {
//					FixTargetsVisitor(root, info, blocks);
				}

				info.userVersion = 12;
				info.userVersion2 = 83;
				info.version = Niflib::VER_20_2_0_7;

				out_path = nif_out / nif;
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

			//this is all hacky but ehhhh.
			bool isBillboardRoot = false;
			BillboardMode mode = BillboardMode::ALWAYS_FACE_CAMERA; 

			vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
			NiObjectRef root = GetFirstRoot(blocks);
			NiNode* rootn = DynamicCast<NiNode>(root);

			ConverterVisitor fimpl(info, root, blocks);
			
			if (root->IsSameType(NiBillboardNode::TYPE)) {
				isBillboardRoot = true; 
				mode = DynamicCast<NiBillboardNode>(root)->GetBillboardMode();
			}

			info.userVersion = 12;
			info.userVersion2 = 83;
			info.version = Niflib::VER_20_2_0_7;
			set<string> sequences = fimpl.nisequences;
			if (!NifFile::hasExternalSkinnedMesh(blocks, rootn)) {
				root = convert_root(root);
				BSFadeNodeRef bsroot = DynamicCast<BSFadeNode>(root);
				//fixed?
				bsroot->SetFlags(524302);
				string out_havok_path = "";
				if (!sequences.empty()) {
					fs::path in_file = nifs[i].filename();
					string out_name = in_file.filename().replace_extension("").string();
					fs::path out_path = fs::path("animations") / in_file.parent_path() / out_name;
					fs::path out_path_abs = nif_out / out_path;
					string out_path_a = out_path_abs.string();
					out_havok_path = wrappers.wrap(out_name, out_path.parent_path().string(), out_path_a, "TES4", sequences);
					vector<Ref<NiExtraData > > list = bsroot->GetExtraDataList();
					BSBehaviorGraphExtraDataRef havokp = new BSBehaviorGraphExtraData();
					havokp->SetName(string("BGED"));
					havokp->SetBehaviourGraphFile(out_havok_path);
					havokp->SetControlsBaseSkeleton(false);
					list.insert(list.begin(),StaticCast<NiExtraData>(havokp));
					bsroot->SetExtraDataList(list);
				}


				std::vector<NiAVObjectRef> children;

				if (isBillboardRoot) //fxmistoblivion01 has NiBillboardNode root
				{
					NiBillboardNode* proxyRoot = new NiBillboardNode();
					proxyRoot->SetBillboardMode(mode);
					proxyRoot->SetFlags(bsroot->GetFlags());
					proxyRoot->SetChildren(bsroot->GetChildren());
					proxyRoot->SetRotation(bsroot->GetRotation());
					proxyRoot->SetTranslation(bsroot->GetTranslation());
					proxyRoot->SetCollisionObject(bsroot->GetCollisionObject());

					if (proxyRoot->GetCollisionObject() != NULL)
						DynamicCast<NiCollisionObject>(proxyRoot->GetCollisionObject())->SetTarget(proxyRoot);

					children.push_back(proxyRoot);
					
				}
				else
				{
					NiNode* proxyRoot = new NiNode();

					proxyRoot->SetFlags(bsroot->GetFlags());
					proxyRoot->SetChildren(bsroot->GetChildren());
					proxyRoot->SetRotation(bsroot->GetRotation());
					proxyRoot->SetTranslation(bsroot->GetTranslation());
					proxyRoot->SetCollisionObject(bsroot->GetCollisionObject());

					if (proxyRoot->GetCollisionObject() != NULL)
						DynamicCast<NiCollisionObject>(proxyRoot->GetCollisionObject())->SetTarget(proxyRoot);

					children.push_back(proxyRoot);
				}

				bsroot->SetRotation(Matrix33());
				bsroot->SetTranslation(Vector3());
				bsroot->SetCollisionObject(NULL);
				bsroot->SetChildren(children);

				//to calculate the right flags, we need to rebuild the blocks
				vector<NiObjectRef> new_blocks = RebuildVisitor(root, info).blocks;

				//fix targets from nitrishapes substitution
				FixTargetsVisitor(root, info, new_blocks);

				if (DynamicCast<NiNode>(root) != NULL && DynamicCast<NiNode>(root)->GetCollisionObject() == NULL) {
					bhkCollisionObjectRef root_collision = NULL;
					int num_collisions = 0;
					//Optimize single collision models
					for (NiObjectRef block : new_blocks) {
						if (block->IsDerivedType(bhkCollisionObject::TYPE))
						{
							num_collisions++;
							root_collision = DynamicCast<bhkCollisionObject>(block);
						}
					}
					if (num_collisions == 1 && root_collision != NULL) {

						vector<NiAVObjectRef> children = bsroot->GetChildren();
						auto root_collision_position = find(children.begin(), children.end(), StaticCast<NiAVObject>(root_collision));
						if (root_collision_position != children.end()) {
							children.erase(root_collision_position);
							bsroot->SetCollisionObject(StaticCast<NiCollisionObject>(root_collision));
							bsroot->SetChildren(children);
						}
					}
				}


				bsx_flags_t calculated_flags = calculateSkyrimBSXFlags(new_blocks, info);

				for (NiObjectRef ref : blocks) {
					if (ref->IsDerivedType(BSXFlags::TYPE)) {
						BSXFlagsRef bref = DynamicCast<BSXFlags>(ref);
						bref->SetIntegerData(calculated_flags.to_ulong());
						break;
					}
				}
			}
			else {
				FixTargetsVisitor(root, info, blocks);
			}

			fs::path out_path = nif_out / nifs[i].filename();
			fs::create_directories(out_path.parent_path());
			WriteNifTree(out_path.string(), root, info);
			NifFile check(out_path.string());
			NiObject* lroot = check.GetRoot();
			if (lroot == NULL)
				throw runtime_error("Error converting");
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

//static bool ExecuteCmd(hkxcmdLine &cmdLine) {
//	InitializeHavok();
//	BeginConversion();
//	CloseHavok();
//	return true;
//}
//
//REGISTER_COMMAND(ConvertNif, HelpString, ExecuteCmd);