#pragma once

#include <commands/CommandBase.h>

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

//Collisions
#include <obj/NiTriBasedGeom.h>
#include <obj/NiTriBasedGeomData.h>
#include <obj/bhkPackedNiTriStripsShape.h>
#include <obj/hkPackedNiTriStripsData.h>
#include <obj/bhkRigidBody.h>
#include <obj/bhkMoppBvTreeShape.h>
#include <obj/bhkNiTriStripsShape.h>
#include <obj/NiTriStripsData.h>
#include <obj/bhkCompressedMeshShape.h>
#include <obj/bhkCompressedMeshShapeData.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#include <filesystem>

//Havok

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>

#include <cstdio>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/Container/LocalArray/hkLocalBuffer.h>
//
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/ConvexTranslate/hkpConvexTranslateShape.h>
#include <Physics/Collide/Shape/Convex/ConvexTransform/hkpConvexTransformShape.h>
#include <Physics/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/List/hkpListShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Internal/Collide/Mopp/Code/hkpMoppCode.h>

#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h"
#include "Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h"
#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h"
#include "Physics/Collide/Util/Welding/hkpMeshWeldingUtility.h"
#include "Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h"
#include <Physics\Collide\Shape\Compound\Collection\CompressedMesh\hkpCompressedMeshShape.h>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

static const fs::path nif_in = "D:\\git\\ck-cmd\\resources\\in";
static const fs::path resources = "resources\\";
static const fs::path nif_out = "D:\\skywind\\test\\skybliviont";

//void findFiles(fs::path startingDir, string extension, vector<fs::path>& results);
//bool BeginConversion();


class ConvertNif : public Command<ConvertNif>
{
    REGISTER_COMMAND_HEADER(ConvertNif)

private:

    ConvertNif();
    virtual ~ConvertNif();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};