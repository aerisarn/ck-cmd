//// ***** BEGIN LICENSE BLOCK *****
////
//// Copyright (c) 2006-2011, Havok Tools.
//// All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification, are permitted provided that the following conditions
//// are met:
////
////    * Redistributions of source code must retain the above copyright
////      notice, this list of conditions and the following disclaimer.
////
////    * Redistributions in binary form must reproduce the above
////      copyright notice, this list of conditions and the following
////      disclaimer in the documentation and/or other materials provided
////      with the distribution.
////
////    * Neither the name of the Havok Tools
////      project nor the names of its contributors may be used to endorse
////      or promote products derived from this software without specific
////      prior written permission.
////
//// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//// POSSIBILITY OF SUCH DAMAGE.
////
//// ***** END LICENSE BLOCK *****
//
//#include "stdafx.h"
//
//#include <core/hkxcmd.h>
//#include <core/hkxutils.h>
//#include <core/hkfutils.h>
//#include <core/log.h>
//
//#include <niflib.h>
//#include <nif_math.h>
//#include <obj/NiObject.h>
//#include <obj/NiNode.h>
//#include <obj/NiTriBasedGeom.h>
//#include <obj/NiTriBasedGeomData.h>
//#include <obj/bhkPackedNiTriStripsShape.h>
//#include <obj/hkPackedNiTriStripsData.h>
//#include <obj/bhkRigidBody.h>
//#include <obj/bhkMoppBvTreeShape.h>
//#include <obj/bhkNiTriStripsShape.h>
//#include <obj/NiTriStripsData.h>
//#include <obj/bhkCompressedMeshShape.h>
//#include <obj/bhkCompressedMeshShapeData.h>
//
//
////////////////////////////////////////////////////////////////////////////
//// Havok Includes
////////////////////////////////////////////////////////////////////////////
//
//#include <Common/Base/hkBase.h>
//#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
//#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
//#include <Common/Base/System/Io/IStream/hkIStream.h>
//
//#include <cstdio>
//#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
//#include <Common/Base/Container/LocalArray/hkLocalBuffer.h>
////
//#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
//#include <Physics/Collide/Shape/Convex/ConvexTranslate/hkpConvexTranslateShape.h>
//#include <Physics/Collide/Shape/Convex/ConvexTransform/hkpConvexTransformShape.h>
//#include <Physics/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
//#include <Physics/Collide/Shape/Compound/Collection/List/hkpListShape.h>
//#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
//#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
//#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
//#include <Physics/Internal/Collide/Mopp/Code/hkpMoppCode.h>
//
//#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h"
//#include "Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h"
//#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h"
//#include "Physics/Collide/Util/Welding/hkpMeshWeldingUtility.h"
//#include "Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h"
//#include <Physics\Collide\Shape\Compound\Collection\CompressedMesh\hkpCompressedMeshShape.h>
//
//#pragma comment(lib, "hkBase.lib")
//#pragma comment(lib, "hkSerialize.lib")
//#pragma comment(lib, "hkpInternal.lib")
//#pragma comment(lib, "hkpUtilities.lib")
//#pragma comment(lib, "hkpCollide.lib")
//#pragma comment(lib, "hkpConstraintSolver.lib")
//
//using namespace std;
//
//static hkpSimpleMeshShape * ConstructHKMesh( vector<Niflib::Vector3>& verts, vector<Niflib::Triangle>& tris)
//{
//	hkpSimpleMeshShape * storageMeshShape = new hkpSimpleMeshShape( 0.01f );
//	hkArray<hkVector4> &vertices = storageMeshShape->m_vertices;
//	hkArray<hkpSimpleMeshShape::Triangle> &triangles = storageMeshShape->m_triangles;
//
//	triangles.setSize( 0 );
//	for (unsigned int i=0;i<tris.size();++i) {
//		Niflib::Triangle &tri = tris[i];
//		hkpSimpleMeshShape::Triangle hktri;
//		hktri.m_a = tri[0];
//		hktri.m_b = tri[1];
//		hktri.m_c = tri[2];
//		triangles.pushBack( hktri );
//	}
//
//	vertices.setSize( 0 );
//	for (unsigned int i=0;i<verts.size();++i) {
//		Niflib::Vector3 &vert = verts[i];
//		vertices.pushBack( hkVector4(vert.x, vert.y, vert.z) );
//	}
//	//storageMeshShape->setRadius(1.0f);
//	return storageMeshShape;
//}
//
//static hkpSimpleMeshShape * ConstructHKMesh(Niflib::NiTriBasedGeomRef shape )
//{
//	Niflib::NiTriBasedGeomDataRef data = shape->GetData();
//	return ConstructHKMesh(data->GetVertices(), data->GetTriangles());
//}
//
//static hkpSimpleMeshShape * ConstructHKMesh(Niflib::bhkPackedNiTriStripsShapeRef shape )
//{
//	Niflib::hkPackedNiTriStripsDataRef data = shape->GetData();
//	return ConstructHKMesh(data->GetVertices(), data->GetTriangles());
//}
//
//static void BuildCollision(Niflib::NiNodeRef top, vector<Niflib::NiTriBasedGeomRef>& bodies)
//{
//	vector<const hkpShape *> shapeList;
//	for (vector<Niflib::NiTriBasedGeomRef>::iterator itr = bodies.begin(); itr != bodies.end(); ++itr)
//		shapeList.push_back( ConstructHKMesh(*itr));
//
//
//	hkpListShape *list = new hkpListShape( &shapeList[0], shapeList.size() );
//	hkpMoppCompilerInput mfr;
//	mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 100.0f, 100.0f, 100.0f ) );
//	mfr.setAbsoluteFitToleranceOfTriangles(100.0f);
//	mfr.setAbsoluteFitToleranceOfInternalNodes(100.0f);
//
//	hkpMoppCode* code = hkpMoppUtility::buildCode(list, mfr);
//
//	hkpMoppBvTreeShape* moppBvTreeShape = new hkpMoppBvTreeShape(list, code);
//	code->removeReference();
//
//	for(unsigned int i=0;i<shapeList.size(); ++i)
//		shapeList[i]->removeReference();
//	list->removeReference();
//	moppBvTreeShape->removeReference();
//}
//
//static void BuildCollision(Niflib::NiNodeRef top, vector<Niflib::bhkRigidBodyRef>& bodies)
//{
//	for (vector<Niflib::bhkRigidBodyRef>::iterator itr = bodies.begin(); itr != bodies.end(); ++itr)
//	{
//		Niflib::bhkRigidBodyRef body = (*itr);
//		Niflib::bhkShapeRef shape = body->GetShape();
//		hkpShapeCollection * list = NULL;
//		
//		// Mopp already in place
//		Niflib::bhkMoppBvTreeShapeRef mopp;
//		if (shape->IsDerivedType(Niflib::bhkMoppBvTreeShape::TYPE))
//		{
//			if ( mopp = Niflib::DynamicCast<Niflib::bhkMoppBvTreeShape>(shape) )
//			{
//				if (Niflib::bhkPackedNiTriStripsShapeRef mesh = Niflib::DynamicCast<Niflib::bhkPackedNiTriStripsShape>( mopp->GetShape() ) )
//				{
//					list = ConstructHKMesh(mesh);
//				}
//			}
//		}
//		else if ( shape->IsDerivedType(Niflib::bhkPackedNiTriStripsShape::TYPE) )
//		{
//			Niflib::bhkPackedNiTriStripsShapeRef mesh = Niflib::DynamicCast<Niflib::bhkPackedNiTriStripsShape>( shape );
//			mopp = new Niflib::bhkMoppBvTreeShape();
//			//mopp->SetMaterial(Niflib::OblivionHavokMaterial::OB_HAV_MAT_CHAIN);
//			body->SetShape( mopp );
//			mopp->SetShape( mesh );
//			list = ConstructHKMesh(mesh);
//		}
//		else if ( shape->IsDerivedType(Niflib::bhkNiTriStripsShape::TYPE) )
//		{
//			Niflib::bhkNiTriStripsShapeRef mesh = Niflib::DynamicCast<Niflib::bhkNiTriStripsShape>( shape );
//			
//			Niflib::bhkPackedNiTriStripsShapeRef packedMesh = new Niflib::bhkPackedNiTriStripsShape();
//			Niflib::hkPackedNiTriStripsDataRef packedData = new Niflib::hkPackedNiTriStripsData();
//
//			vector<Niflib::OblivionSubShape> shapes;
//			vector<Niflib::Vector3> verts, norms;
//			vector<Niflib::Triangle> tris;
//			for (int i=0;i<mesh->GetNumStripsData(); ++i){
//				int toff = tris.size();
//				Niflib::NiTriStripsDataRef data = mesh->GetStripsData(i);
//
//				vector<Niflib::Vector3> v = data->GetVertices();
//				verts.reserve( verts.size() + v.size() );
//				for (vector<Niflib::Vector3>::iterator vi=v.begin(); vi != v.end(); ++vi )
//					verts.push_back( *vi / 7.0 );
//
//				vector<Niflib::Vector3> n = data->GetNormals();
//				norms.reserve( norms.size() + n.size() );
//				for (vector<Niflib::Vector3>::iterator ni=n.begin(); ni != n.end(); ++ni )
//					norms.push_back(*ni);
//
//				vector<Niflib::Triangle> t = data->GetTriangles();
//				tris.reserve( tris.size() + t.size() );
//				for (vector<Niflib::Triangle>::iterator ti=t.begin(); ti != t.end(); ++ti )
//					tris.push_back(Niflib::Triangle(ti->v1 + toff, ti->v2 + toff, ti->v3 + toff) );
//
//				Niflib::OblivionSubShape subshape;
//				//subshape.material = mesh->GetMaterial();
//				//if ( i < (int)mesh->GetNumDataLayers() )
//				//	subshape.colFilter.layer_ob = mesh->GetOblivionLayer( i );
//				//else
//				//	subshape.havokColFilter.layer_ob = Niflib::OL_STATIC;
//
//				shapes.push_back( subshape );
//			}
//			packedData->SetNumFaces( tris.size() );
//			packedData->SetVertices( verts );
//			packedData->SetTriangles( tris );
//			//packedData->SetNormals( norms );
//			packedMesh->SetData( packedData );
//			packedMesh->SetSubShapes( shapes );
//
//			list = ConstructHKMesh(verts, tris);
//
//			mopp = new Niflib::bhkMoppBvTreeShape();
//			//mopp->SetMaterial( mesh->GetMaterial() );
//			body->SetShape( mopp );
//			mopp->SetShape( packedMesh );
//		}
//
//		if (!mopp || NULL == list)
//			continue;
//
//		//////////////////////////////////////////////////////////////////////////
//
//		hkpMoppCompilerInput mfr;
//		mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 0.1f, 0.1f, 0.1f ) );
//		//mfr.setAbsoluteFitToleranceOfTriangles(0.1f);
//		//mfr.setAbsoluteFitToleranceOfInternalNodes(0.0001f);
//
//		hkpMoppCode* code = hkpMoppUtility::buildCode(list, mfr);
//
//		vector<Niflib::byte> moppcode;
//		moppcode.resize( code->m_data.getSize() );
//		for (int i=0; i<code->m_data.getSize(); ++i )
//			moppcode[i] = code->m_data[i];
//		mopp->SetMoppCode( moppcode );
//		mopp->SetMoppOrigin(Niflib::Vector3(code->m_info.m_offset(0), code->m_info.m_offset(1), code->m_info.m_offset(2) ));
//		mopp->SetMoppScale( code->m_info.getScale() );
//	
//		code->removeReference();
//		list->removeReference();
//	}
//}
//
//using namespace Niflib;
//bool injectCollisionData(vector<hkGeometry>& geometryMap, bhkMoppBvTreeShapeRef pMoppShape, bhkCompressedMeshShapeDataRef pData, bhkRigidBodyRef pRigidBody)
//{
//	if (pMoppShape == NULL)   return false;
//	if (pData == NULL)   return false;
//	if (geometryMap.empty())  return false;
//
//	//----  Havok  ----  START
//	hkpCompressedMeshShape*					pCompMesh(NULL);
//	hkpMoppCode*							pMoppCode(NULL);
//	hkpMoppBvTreeShape*						pMoppBvTree(NULL);
//	hkpCompressedMeshShapeBuilder			shapeBuilder;
//	hkpMoppCompilerInput					mci;
//	vector<int>								geometryIdxVec;
//	vector<bhkCMSDMaterial>					tMtrlVec;
//	SkyrimHavokMaterial						material(SKY_HAV_MAT_STONE);
//	int										subPartId(0);
//	int										tChunkSize(0);
//
//	//  initialize shape Builder
//	shapeBuilder.m_stripperPasses = 5000;
//
//	//  create compressedMeshShape
//	pCompMesh = shapeBuilder.createMeshShape(0.001f, hkpCompressedMeshShape::MATERIAL_SINGLE_VALUE_PER_CHUNK);
//
//	//  add geometries to compressedMeshShape
//	for (vector<hkGeometry>::iterator geoIter = geometryMap.begin(); geoIter != geometryMap.end(); geoIter++)
//	{
//		size_t		matIdx(0);
//
//		//  determine material index
//		material = (SkyrimHavokMaterial)geoIter->m_triangles[0].m_material;
//
//		//  material already known?
//		for (matIdx = 0; matIdx < tMtrlVec.size(); ++matIdx)
//		{
//			//TODO
//			//if (tMtrlVec[matIdx].skyrimMaterial == material)		break;
//		}
//
//		//  add new material?
//		if (matIdx >= tMtrlVec.size())
//		{
//			bhkCMSDMaterial		tChunkMat;
//
//			//  TODO create single material
//			//tChunkMat.skyrimMaterial = material;
//			//tChunkMat.skyrimLayer = pRigidBody->GetSkyrimLayer();
//
//			//  add material to list
//			tMtrlVec.push_back(tChunkMat);
//		}
//
//		//  set material index to each triangle of geometry
//		for (int idx(0); idx < geoIter->m_triangles.getSize(); ++idx)
//		{
//			geoIter->m_triangles[idx].m_material = matIdx;
//		}
//
//		//  add geometry to shape
//		subPartId = shapeBuilder.beginSubpart(pCompMesh);
//		shapeBuilder.addGeometry(*geoIter, hkMatrix4::getIdentity(), pCompMesh);
//		shapeBuilder.endSubpart(pCompMesh);
//		shapeBuilder.addInstance(subPartId, hkMatrix4::getIdentity(), pCompMesh);
//
//	}  //  for (vector<hkGeometry>::iterator geoIter = geometryMap.begin(); geoIter != geometryMap.end(); geoIter++)
//
//	   //  create welding info
//	mci.m_enableChunkSubdivision = false;  //  PC version
//	pMoppCode = hkpMoppUtility::buildCode(pCompMesh, mci);
//	pMoppBvTree = new hkpMoppBvTreeShape(pCompMesh, pMoppCode);
//	hkpMeshWeldingUtility::computeWeldingInfo(pCompMesh, pMoppBvTree, hkpWeldingUtility::WELDING_TYPE_TWO_SIDED);
//	//----  Havok  ----  END
//
//	//----  Merge  ----  START
//	hkArray<hkpCompressedMeshShape::Chunk>  chunkListHvk;
//	vector<bhkCMSDChunk>                    chunkListNif = pData->GetChunks();
//	vector<Vector4>                         tVec4Vec;
//	vector<bhkCMSDBigTris>                  tBTriVec;
//	vector<bhkCMSDTransform>                tTranVec;
//	map<unsigned int, bhkCMSDMaterial>		tMtrlMap;
//	short                                   chunkIdxNif(0);
//
//	//  --- modify MoppBvTree ---
//	//  set origin
//	pMoppShape->SetMoppOrigin(Vector3(pMoppBvTree->getMoppCode()->m_info.m_offset(0), pMoppBvTree->getMoppCode()->m_info.m_offset(1), pMoppBvTree->getMoppCode()->m_info.m_offset(2)));
//
//	//  set scale
//	pMoppShape->SetMoppScale(pMoppBvTree->getMoppCode()->m_info.getScale());
//
//	// TODO set build Type
//	//pMoppShape->SetBuildType(MoppDataBuildType((Niflib::byte) pMoppCode->m_buildType));
//
//	//  copy mopp data
//	pMoppShape->SetMoppCode(vector<Niflib::byte>(pMoppBvTree->m_moppData, pMoppBvTree->m_moppData + pMoppBvTree->m_moppDataSize));
//
//	//  set boundings
//	pData->SetBoundsMin(Vector4(pCompMesh->m_bounds.m_min(0), pCompMesh->m_bounds.m_min(1), pCompMesh->m_bounds.m_min(2), pCompMesh->m_bounds.m_min(3)));
//	pData->SetBoundsMax(Vector4(pCompMesh->m_bounds.m_max(0), pCompMesh->m_bounds.m_max(1), pCompMesh->m_bounds.m_max(2), pCompMesh->m_bounds.m_max(3)));
//
//	//  resize and copy bigVerts
//	pData->SetNumBigVerts(pCompMesh->m_bigVertices.getSize());
//	tVec4Vec = pData->GetBigVerts();
//	tVec4Vec.resize(pData->GetNumBigVerts());
//	for (unsigned int idx(0); idx < pData->GetNumBigVerts(); ++idx)
//	{
//		tVec4Vec[idx].x = pCompMesh->m_bigVertices[idx](0);
//		tVec4Vec[idx].y = pCompMesh->m_bigVertices[idx](1);
//		tVec4Vec[idx].z = pCompMesh->m_bigVertices[idx](2);
//		tVec4Vec[idx].w = pCompMesh->m_bigVertices[idx](3);
//	}
//	pData->SetBigVerts(tVec4Vec);
//
//	//  resize and copy bigTris
//	pData->SetNumBigTris(pCompMesh->m_bigTriangles.getSize());
//	tBTriVec = pData->GetBigTris();
//	tBTriVec.resize(pData->GetNumBigTris());
//	for (unsigned int idx(0); idx < pData->GetNumBigTris(); ++idx)
//	{
//		tBTriVec[idx].triangle1 = pCompMesh->m_bigTriangles[idx].m_a;
//		tBTriVec[idx].triangle2 = pCompMesh->m_bigTriangles[idx].m_b;
//		tBTriVec[idx].triangle3 = pCompMesh->m_bigTriangles[idx].m_c;
//		tBTriVec[idx].material = pCompMesh->m_bigTriangles[idx].m_material;
//		tBTriVec[idx].weldingInfo = pCompMesh->m_bigTriangles[idx].m_weldingInfo;
//	}
//	pData->SetBigTris(tBTriVec);
//
//	//  resize and copy transform data
//	pData->SetNumTransforms(pCompMesh->m_transforms.getSize());
//	tTranVec = pData->GetChunkTransforms();
//	tTranVec.resize(pData->GetNumTransforms());
//	for (unsigned int idx(0); idx < pData->GetNumTransforms(); ++idx)
//	{
//		tTranVec[idx].translation.x = pCompMesh->m_transforms[idx].m_translation(0);
//		tTranVec[idx].translation.y = pCompMesh->m_transforms[idx].m_translation(1);
//		tTranVec[idx].translation.z = pCompMesh->m_transforms[idx].m_translation(2);
//		tTranVec[idx].translation.w = pCompMesh->m_transforms[idx].m_translation(3);
//		tTranVec[idx].rotation.x = pCompMesh->m_transforms[idx].m_rotation(0);
//		tTranVec[idx].rotation.y = pCompMesh->m_transforms[idx].m_rotation(1);
//		tTranVec[idx].rotation.z = pCompMesh->m_transforms[idx].m_rotation(2);
//		tTranVec[idx].rotation.w = pCompMesh->m_transforms[idx].m_rotation(3);
//	}
//	pData->SetChunkTransforms(tTranVec);
//
//	//  set material list
//	pData->SetChunkMaterials(tMtrlVec);
//
//	//  get chunk list from mesh
//	chunkListHvk = pCompMesh->m_chunks;
//
//	// resize nif chunk list
//	chunkListNif.resize(chunkListHvk.getSize());
//
//	//  for each chunk
//	for (hkArray<hkpCompressedMeshShape::Chunk>::iterator pCIterHvk = pCompMesh->m_chunks.begin(); pCIterHvk != pCompMesh->m_chunks.end(); pCIterHvk++)
//	{
//		//  get nif chunk
//		bhkCMSDChunk&	chunkNif = chunkListNif[chunkIdxNif];
//
//		//  set offset => translation
//		chunkNif.translation.x = pCIterHvk->m_offset(0);
//		chunkNif.translation.y = pCIterHvk->m_offset(1);
//		chunkNif.translation.z = pCIterHvk->m_offset(2);
//		chunkNif.translation.w = pCIterHvk->m_offset(3);
//
//		//  force flags to fixed values
//		chunkNif.materialIndex = pCIterHvk->m_materialInfo;
//		chunkNif.reference = 65535;
//		chunkNif.transformIndex = pCIterHvk->m_transformIndex;
//
//		//  vertices
//		chunkNif.numVertices = pCIterHvk->m_vertices.getSize();
//		chunkNif.vertices.resize(chunkNif.numVertices);
//		for (unsigned int i(0); i < chunkNif.numVertices; ++i)
//		{
//			chunkNif.vertices[i] = pCIterHvk->m_vertices[i];
//		}
//
//		//  indices
//		chunkNif.numIndices = pCIterHvk->m_indices.getSize();
//		chunkNif.indices.resize(chunkNif.numIndices);
//		for (unsigned int i(0); i < chunkNif.numIndices; ++i)
//		{
//			chunkNif.indices[i] = pCIterHvk->m_indices[i];
//		}
//
//		//  strips
//		chunkNif.numStrips = pCIterHvk->m_stripLengths.getSize();
//		chunkNif.strips.resize(chunkNif.numStrips);
//		for (unsigned int i(0); i < chunkNif.numStrips; ++i)
//		{
//			chunkNif.strips[i] = pCIterHvk->m_stripLengths[i];
//		}
//
//		// TODO welding
//		//chunkNif.numWeldings = pCIterHvk->m_weldingInfo.getSize();
//		//chunkNif.weldings.resize(chunkNif.numWeldings);
//		//for (unsigned int i(0); i < chunkNif.numWeldings; ++i)
//		//{
//		//	chunkNif.weldings[i] = pCIterHvk->m_weldingInfo[i];
//		//}
//
//		//  next chunk
//		++chunkIdxNif;
//
//	}  //  for (hkArray<hkpCompressedMeshShape::Chunk>::iterator pCIterHvk = 
//
//	   //  set modified chunk list to compressed mesh shape data
//	pData->SetChunks(chunkListNif);
//	//----  Merge  ----  END
//
//	return true;
//}
//
//static void HK_CALL errorReport(const char* msg, void*)
//{
//	//printf("%s", msg);
//}
//
//static void HK_CALL debugReport(const char* msg, void* userContext)
//{
//	Log::Debug("%s", msg);
//}
//
//
//static hkThreadMemory* threadMemory = NULL;
//static char* stackBuffer = NULL;
//static void InitializeHavok()
//{
//	// Initialize the base system including our memory system
//	hkMallocAllocator baseMalloc;
//	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(&baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024));
//	hkBaseSystem::init(memoryRouter, debugReport);
//	LoadDefaultRegistry();
//}
//
//static void CloseHavok()
//{
//	hkBaseSystem::quit();
//	hkMemoryInitUtil::quit();
//}
//
//
//static void HelpString(hkxcmd::HelpType type){
//   switch (type)
//   {
//   case hkxcmd::htShort: Log::Info("GenMopp - Generate Mopp for a specified NIF file."); break;
//   case hkxcmd::htLong:  
//      {
//         char fullName[MAX_PATH], exeName[MAX_PATH];
//         GetModuleFileName(NULL, fullName, MAX_PATH);
//         _splitpath(fullName, NULL, NULL, exeName, NULL);
//		 Log::Info("Usage: %s GenMopp [-opts[modifiers]] <NIF file>", exeName);
//		 Log::Info("  Generate Mopp for a specified NIF file.");
//		 Log::Info("");
//		 Log::Info("<Switches>");
//		 Log::Info("  i <path>          Input File");
//		 Log::Info("  o <path>          Output File - Defaults to input file with '-out' appended");
//		 Log::Info("");
//      }
//      break;
//   }
//}
//
//
//static bool ExecuteCmd(hkxcmdLine &cmdLine)
//{
//
//   string inpath;
//   string outpath;
//   int argc = cmdLine.argc;
//   char **argv = cmdLine.argv;
//   vector<string> paths;
//
//   list<hkxcmd *> plugins;
//
//   for (int i = 0; i < argc; i++)
//   {
//      char *arg = argv[i];
//      if (arg == NULL)
//         continue;
//      if (arg[0] == '-' || arg[0] == '/')
//      {
//		  switch (tolower(arg[1]))
//		  {
//		  case 'o':
//		  {
//			  const char *param = arg + 2;
//			  if (*param == ':' || *param == '=') ++param;
//			  argv[i] = NULL;
//			  if (param[0] == 0 && (i + 1<argc && (argv[i + 1][0] != '-' || argv[i + 1][0] != '/'))) {
//				  param = argv[++i];
//				  argv[i] = NULL;
//			  }
//			  if (param[0] == 0)
//				  break;
//			  if (outpath.empty())
//			  {
//				  outpath = param;
//			  }
//			  else
//			  {
//				  Log::Error("Output file already specified as '%s'", outpath.c_str());
//			  }
//		  } break;
//		  }
//      }
//	  else if (inpath.empty())
//	  {
//		  inpath = arg;
//	  }
//	  else if (outpath.empty())
//	  {
//		  outpath = arg;
//	  }
//   }
//   char rootDir[MAX_PATH];
//   strcpy(rootDir, inpath.c_str());
//   GetFullPathName(rootDir, MAX_PATH, rootDir, NULL);
//   if (!PathIsDirectory(rootDir))
//	   PathRemoveFileSpec(rootDir);
//   char infile[MAX_PATH], relpath[MAX_PATH];
//   try
//   {
//	   strcpy(infile, inpath.c_str());
//	   GetFullPathName(infile, MAX_PATH, infile, NULL);
//
//	   LPCSTR extn = PathFindExtension(infile);
//	   if (stricmp(extn, ".nif") != 0)
//	   {
//		   Log::Verbose("Unexpected extension. Skipping '%s'", infile);
//		   return false;
//	   }
//	   PathRelativePathTo(relpath, rootDir, FILE_ATTRIBUTE_DIRECTORY, infile, 0);
//	   char outfile[MAX_PATH];
//	   if (outpath.empty())
//	   {
//		   char drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
//		   _splitpath(infile, drive, dir, fname, ext);
//		   strcat(fname, "-out");
//		   _makepath(outfile, drive, dir, fname, ext);
//		   outpath = rootDir;
//	   }
//	   unsigned int ver = Niflib::GetNifVersion(infile);
//	   if (ver == Niflib::VER_UNSUPPORTED) {
//		   Log::Verbose("Unexpected Version. Skipping '%s'", infile);
//		   return false;
//	   }
//	   else if (ver == Niflib::VER_INVALID) {
//		   Log::Verbose("Invalid Version. Skipping '%s'", infile);
//		   return false;
//	   }
//	   Niflib::NifInfo info;
//	   vector<Niflib::NiObjectRef> blocks = Niflib::ReadNifList(infile, &info);
//	   Niflib::NiObjectRef root = blocks[0];
//
//	   Niflib::NiNodeRef top = Niflib::DynamicCast<Niflib::NiNode>(root);
//
//	   InitializeHavok();
//	   BuildCollision(top, Niflib::DynamicCast<Niflib::bhkRigidBody>(blocks));
//	   //BuildCollision(top, DynamicCast<NiTriBasedGeom>(blocks));
//	   CloseHavok();
//
//	   WriteNifTree(outfile, root, info);
//	   return true;
//
//   }
//   catch (...)
//   {
//	   Log::Error("Unexpected exception occurred while processing '%s'", relpath);
//   }
//   return true;
//}
//
//REGISTER_COMMAND(GenMopp, HelpString, ExecuteCmd);
