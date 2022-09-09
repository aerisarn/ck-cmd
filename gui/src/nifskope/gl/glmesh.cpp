/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2015, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#include "gl/glscene.h"
#include <src/models/nifmodel.h>

#include "glmesh.h"
#include "gl/controllers.h"
#include "gl/renderer.h"

#include <QBuffer>
#include <QDebug>
#include <QSettings>

#include <QOpenGLFunctions>

#include <src/log.h>

using namespace Niflib;
using namespace ckcmd;


//! @file glmesh.cpp Scene management for visible meshes such as NiTriShapes.

const char * NIMESH_ABORT = QT_TR_NOOP( "NiMesh rendering encountered unsupported types. Rendering may be broken." );


Shape::Shape( Scene * s, const QModelIndex & b ) : Node( s, b )
{
	shapeNumber = s->shapes.count();
}

Mesh::Mesh( Scene * s, const QModelIndex & b ) : Shape( s, b )
{
}


void Mesh::clear()
{
	Node::clear();

	iData = iSkin = iSkinData = iSkinPart = iTangentData = QModelIndex();
	bssp = nullptr;
	bslsp = nullptr;
	bsesp = nullptr;

	verts.clear();
	norms.clear();
	colors.clear();
	coords.clear();
	tangents.clear();
	bitangents.clear();
	triangles.clear();
	tristrips.clear();
	weights.clear();
	partitions.clear();
	sortedTriangles.clear();
	indices.clear();
	transVerts.clear();
	transNorms.clear();
	transColors.clear();
	transTangents.clear();
	transBitangents.clear();

	isLOD = false;
	isDoubleSided = false;
}

void Shape::setController( const NifModel * nif, const QModelIndex & iController )
{
	if ( nif->itemName( iController ) == "NiGeomMorpherController" ) {
		Controller * ctrl = new MorphController( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	} else if ( nif->itemName( iController ) == "NiUVController" ) {
		Controller * ctrl = new UVController( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	} else {
		Node::setController( nif, iController );
	}
}


void Shape::update( const NifModel * nif, const QModelIndex & index )
{
	Node::update( nif, index );

	// If shaders are reenabled, reset
	if ( !(scene->options & Scene::DisableShaders) && shader.isNull() 
		 && nif->checkVersion( 0x14020007, 0x14020007 ) )
	{
		updateShaderProperties( nif );
	}
}

void Shape::updateShaderProperties( const NifModel * nif )
{
	auto shape = DynamicCast<NiGeometry>(nif->getBlock( iBlock));
	if (shape == nullptr )
		return;

	auto prop = shape->GetShaderProperty();
	if (prop == nullptr)
		return;
	auto propIndex = nif->getBlockIndex(prop);
	bool iLSP = prop->IsSameType(Niflib::BSLightingShaderProperty::TYPE);
	bool iESP = prop->IsSameType(Niflib::BSEffectShaderProperty::TYPE);

	// Reset stored shader so it can reassess conditions
	shader = "";

	if ( iLSP ) {
		if ( !bslsp )
			bslsp = properties.get<::BSLightingShaderProperty>();

		if ( !bslsp )
			return;

		bssp = bslsp;

		bslsp->updateParams( nif, propIndex);

		// Mesh alpha override
		translucent = (bslsp->getAlpha() < 1.0);
		translucent |= bslsp->hasRefraction;

	} else if ( iESP ) {
		if ( !bsesp )
			bsesp = properties.get<::BSEffectShaderProperty>();

		if ( !bsesp )
			return;

		bssp = bsesp;

		bsesp->updateParams( nif, propIndex);

		// Mesh alpha override
		translucent = (bsesp->getAlpha() < 1.0) && !findProperty<AlphaProperty>();
	}

	// Draw mesh second
	drawSecond |= translucent;

	if ( !bssp )
		return;

	depthTest = bssp->getDepthTest();
	depthWrite = bssp->getDepthWrite();
	isDoubleSided = bssp->getIsDoubleSided();
	isVertexAlphaAnimation = bssp->getFlags2() & ShaderFlags::SLSF2_Tree_Anim;
}

void Shape::boneSphere( const NifModel * nif, const QModelIndex & index ) const
{
	//TODO
	//Node * root = findParent( 0 );
	//Node * bone = root ? root->findChild( bones.value( index.row() ) ) : 0;
	//if ( !bone )
	//	return;

	//Transform boneT = Transform( nif, index );
	//Transform t = (scene->options & Scene::DoSkinning) ? viewTrans() : Transform();
	//t = t * skeletonTrans * bone->localTrans( 0 ) * boneT;

	//auto bSphere = BoundSphere( nif, nif->getIndex( index, "Bounding Sphere" ) );
	//if ( bSphere.radius > 0.0 ) {
	//	glColor4f( 1, 1, 1, 0.33 );
	//	auto pos = boneT.rotation.inverted() * (bSphere.center - boneT.translation);
	//	drawSphereSimple( t * pos, bSphere.radius, 36 );
	//}
}

void Mesh::update( const NifModel * nif, const QModelIndex & index )
{
	Shape::update( nif, index );

	// Was Skinning toggled?
	// If so, switch between partition triangles and data triangles
	bool doSkinningCurr = (scene->options & Scene::DoSkinning);
	updateSkin |= (doSkinning != doSkinningCurr) && nif->checkVersion( 0, 0x14040000 );
	updateData |= updateSkin;
	doSkinning = doSkinningCurr;

	if ( (!iBlock.isValid() || !index.isValid()) && !updateSkin )
		return;

	isLOD = nif->isNiBlock( iBlock, "BSLODTriShape" );
	if ( isLOD )
		emit nif->lodSliderChanged( true );

	updateData |= ( iData == index ) || ( iTangentData == index );
	updateSkin |= ( iSkin == index );
	updateSkin |= ( iSkinData == index );
	updateSkin |= ( iSkinPart == index );
	updateSkin |= ( updateData && isSkinned && doSkinning );

	//if ( nif->checkVersion( 0x14050000, 0 ) && nif->inherits( iBlock, "NiMesh" ) )
	//	updateSkin = false;

	isVertexAlphaAnimation = false;
	isDoubleSided = false;

	// Update shader from this mesh's shader property
	if ( nif->checkVersion( 0x14020007, 0 ) && nif->inherits<NiTriBasedGeom>( iBlock ) )
		updateShaderProperties( nif );

	if ( iBlock == index ) {

		auto node = DynamicCast<NiNode>(nif->getBlock(iBlock));
		if (nullptr != node)
		{
			for (const auto child : node->GetChildren()) {
				QModelIndex iChild = nif->getBlockIndex(child);
					if (!iChild.isValid())
						continue;

					if (nif->inherits<NiTriShapeData>(iChild) || nif->inherits<NiTriStripsData>(iChild)) {
						if (!iData.isValid()) {
							iData = iChild;
								updateData = true;
						}
						else if (iData != iChild) {
							//LOGINFO << tr("Warnings were generated while updating meshes.") <<
							//	tr("Block %1 has multiple data blocks").arg(id()) << log_endl;
						}
					}
					else if (nif->inherits<NiSkinInstance>(iChild)) {
						if (!iSkin.isValid()) {
							iSkin = iChild;
							updateSkin = true;
						}
						else if (iSkin != iChild) {
							//Message::append(tr("Warnings were generated while updating meshes."),
							//	tr("Block %1 has multiple skin instances").arg(id())
							//);
						}
					}
			}
		}
	}

	updateBounds |= updateData;
}

QModelIndex Mesh::vertexAt( int idx ) const
{
	//auto nif = static_cast<const NifModel *>(iBlock.model());
	//if ( !nif )
	//	return QModelIndex();

	//auto iVertexData = nif->getIndex( iData, "Vertices" );
	//auto iVertex = iVertexData.child( idx, 0 );

	return QModelIndex();
}


bool Mesh::isHidden() const
{
	return ( Node::isHidden()
	         || ( !(scene->options & Scene::ShowHidden) /*&& Options::onlyTextured()*/
	              && !properties.get<TexturingProperty>()
	              && !properties.get<::BSShaderLightingProperty>()
	         )
	);
}

bool compareTriangles( const QPair<int, float> & tri1, const QPair<int, float> & tri2 )
{
	return ( tri1.second < tri2.second );
}

void Mesh::transform()
{
	const NifModel * nif = static_cast<const NifModel *>( iBlock.model() );

	if ( !nif || !iBlock.isValid() ) {
		clear();
		return;
	}

	if ( updateData ) {
		nifVersion = nif->getUserVersion2();
		updateData = false;


		const auto& geometrydata = DynamicCast<NiGeometryData>(nif->getBlock(iData));

		const auto& geometry_verts = geometrydata->GetVertices();
		for (const auto& vert : geometry_verts)
			verts.push_back(::Vector3(vert));
		const auto& geometry_norms = geometrydata->GetNormals();
		for (const auto& norm : geometry_norms)
			norms.push_back(::Vector3(norm));
		const auto& geometry_colors = geometrydata->GetVertexColors();
		for (const auto& color : geometry_colors)
			colors.push_back(::Color4(color));

		// Detect if "Has Vertex Colors" is set to Yes in NiTriShape
		//	Used to compare against SLSF2_Vertex_Colors
		hasVertexColors = true;
		if ( colors.length() == 0 ) {
			hasVertexColors = false;
		}

		if ( isVertexAlphaAnimation ) {
			for ( int i = 0; i < colors.count(); i++ )
				colors[i].setRGBA( colors[i].red(), colors[i].green(), colors[i].blue(), 1 );
		}

		const auto& geometry_tangents = geometrydata->GetTangents();
		for (const auto& tang : geometry_tangents)
			tangents.push_back(::Vector3(tang));
		const auto& geometry_bitangents = geometrydata->GetBitangents();
		for (const auto& bitang : geometry_bitangents)
			bitangents.push_back(::Vector3(bitang));


		if ( norms.count() < verts.count() )
			norms.clear();

		if ( colors.count() < verts.count() )
			colors.clear();

		coords.clear();

		const auto& uvs = geometrydata->GetUvSets();
		if (!uvs.empty())
		{
			TexCoords tc;
			for (const auto& uv : uvs.at(0))
			{
				tc.append({ uv.u, uv.v });
			}
			if (tc.count() < verts.count())
				tc.clear();
			coords.append(tc);
		}

		if (geometrydata->IsSameType(NiTriShapeData::TYPE)) {

			const auto& tridata = DynamicCast<NiTriShapeData>(geometrydata);

			// check indexes
			// TODO: check other indexes as well
			const auto& ftriangles = tridata->GetTriangles();
			triangles.clear();
			int inv_idx = 0;
			int inv_cnt = 0;

			for (int i = 0; i < ftriangles.size(); i++) {
				::Triangle t = { ftriangles[i].v1, ftriangles[i].v2, ftriangles[i].v3 };
				inv_idx = 0;

				for (int j = 0; j < 3; j++) {
					if (t[j] >= verts.count()) {
						inv_idx = 1;
						break;
					}
				}

				if (!inv_idx)
					triangles.append(t);
			}

			inv_cnt = ftriangles.size() - triangles.count();

			if (inv_cnt > 0) {
				//TODO
				/*int block_idx = nif->getBlockNumber(nif->getIndex(iData, "Triangles"));
				Message::append(tr("Warnings were generated while rendering mesh."),
					tr("Block %1: %2 invalid indices in NiTriShapeData.Triangles").arg(block_idx).arg(inv_cnt)
				);*/
			}

			tristrips.clear();
		}
	}

	if ( updateSkin ) {
		updateSkin = false;
		isSkinned = false;
		weights.clear();
		partitions.clear();

		const auto& skin_instance = DynamicCast< NiSkinInstance>(nif->getBlock(iSkin));
		iSkinData = nif->getBlockIndex(skin_instance->GetData()); // nif->getBlock(nif->getLink(iSkin, "Data"), "NiSkinData");
		iSkinPart = nif->getBlockIndex(skin_instance->GetSkinPartition());  // nif->getBlock( nif->getLink( iSkin, "Skin Partition" ), "NiSkinPartition" );

		skeletonRoot  = nif->getBlockNumber(nif->getBlockIndex(skin_instance->GetSkeletonRoot())); //  nif->getLink( iSkin, "Skeleton Root" );
		skeletonTrans = Transform( nif, iSkinData );

		const auto& nibones = skin_instance->GetBones();
		for (const auto& nibone : nibones)
		{
			bones.append(nif->getBlockNumber(nif->getBlockIndex(nibone)));
		}

		//bones = nif->getLinkArray( iSkin, "Bones" );

		const auto& skin_data = skin_instance->GetData();
		const auto& skin_partition = skin_instance->GetSkinPartition();

		const auto& bonedatalist = skin_data->GetBoneList();

		if ( !bonedatalist.empty() ) {
			bool hvw = skin_data->GetHasVertexWeights(); //nif->get<unsigned char>( iSkinData, "Has Vertex Weights" );
			// Ignore weights listed in NiSkinData if NiSkinPartition exists
			hvw = hvw && !iSkinPart.isValid();
			int vcnt = hvw ? verts.count() : 0;
			for ( int b = 0; b < bonedatalist.size() && b < bones.count(); b++ ) {
				weights.append( BoneWeights( nif, bonedatalist[b], bones[ b ], vcnt ) );
			}
		}

		if ( iSkinPart.isValid() && doSkinning ) {
			const auto& blocks = skin_partition->GetSkinPartitionBlocks();

			//QModelIndex idx = nif->getIndex( iSkinPart, "Skin Partition Blocks" );

			uint numTris = 0;
			uint numStrips = 0;
			for ( int i = 0; i < blocks.size(); i++ ) {
				partitions.append( ::SkinPartition( nif, blocks[i] ) );
				numTris += partitions[i].triangles.size();
				numStrips += partitions[i].tristrips.size();
			}

			triangles.clear();
			tristrips.clear();

			triangles.reserve( numTris );
			tristrips.reserve( numStrips );

			for ( const ::SkinPartition& part : partitions ) {
				triangles << part.getRemappedTriangles();
				tristrips << part.getRemappedTristrips();
			}
		}

		isSkinned = weights.count() || partitions.count();

	}

	Node::transform();
}

void Mesh::transformShapes()
{
	if ( isHidden() )
		return;

	Node::transformShapes();

	transformRigid = true;

	if ( isSkinned && doSkinning ) {
		transformRigid = false;

		int vcnt = verts.count();
		int ncnt = norms.count();
		int tcnt = tangents.count();
		int bcnt = bitangents.count();

		transVerts.resize( vcnt );
		transVerts.fill( ::Vector3() );
		transNorms.resize( vcnt );
		transNorms.fill( ::Vector3() );
		transTangents.resize( vcnt );
		transTangents.fill( ::Vector3() );
		transBitangents.resize( vcnt );
		transBitangents.fill( ::Vector3() );

		Node * root = findParent( skeletonRoot );

		if ( partitions.count() ) {
			for ( const ::SkinPartition& part : partitions ) {
				QVector<Transform> boneTrans( part.boneMap.count() );

				for ( int t = 0; t < boneTrans.count(); t++ ) {
					Node * bone = root ? root->findChild( bones.value( part.boneMap[t] ) ) : 0;
					boneTrans[ t ] = scene->view;

					if ( bone )
						boneTrans[ t ] = boneTrans[ t ] * bone->localTrans( skeletonRoot ) * weights.value( part.boneMap[t] ).trans;

					//if ( bone ) boneTrans[ t ] = bone->viewTrans() * weights.value( part.boneMap[t] ).trans;
				}

				for ( int v = 0; v < part.vertexMap.count(); v++ ) {
					int vindex = part.vertexMap[ v ];
					if ( vindex < 0 || vindex >= vcnt )
						break;

					if ( transVerts[vindex] == ::Vector3() ) {
						for ( int w = 0; w < part.numWeightsPerVertex; w++ ) {
							QPair<int, float> weight = part.weights[ v * part.numWeightsPerVertex + w ];


							Transform trans = boneTrans.value( weight.first );

							if ( vcnt > vindex )
								transVerts[vindex] += trans * verts[vindex] * weight.second;
							if ( ncnt > vindex )
								transNorms[vindex] += trans.rotation * norms[vindex] * weight.second;
							if ( tcnt > vindex )
								transTangents[vindex] += trans.rotation * tangents[vindex] * weight.second;
							if ( bcnt > vindex )
								transBitangents[vindex] += trans.rotation * bitangents[vindex] * weight.second;
						}
					}
				}
			}
		} else {
			int x = 0;
			for ( const BoneWeights& bw : weights ) {
				Transform trans = viewTrans() * skeletonTrans;
				Node * bone = root ? root->findChild( bw.bone ) : 0;

				if ( bone )
					trans = trans * bone->localTrans( skeletonRoot ) * bw.trans;

				if ( bone )
					weights[x++].tcenter = bone->viewTrans() * bw.center;
				else
					x++;

				for ( const VertexWeight& vw : bw.weights ) {
					int vindex = vw.vertex;
					if ( vindex < 0 || vindex >= vcnt )
						break;

					if ( vcnt > vindex )
						transVerts[vindex] += trans * verts[vindex] * vw.weight;
					if ( ncnt > vindex )
						transNorms[vindex] += trans.rotation * norms[vindex] * vw.weight;
					if ( tcnt > vindex )
						transTangents[vindex] += trans.rotation * tangents[vindex] * vw.weight;
					if ( bcnt > vindex )
						transBitangents[vindex] += trans.rotation * bitangents[vindex] * vw.weight;
				}
			}
		}

		for ( int n = 0; n < transNorms.count(); n++ )
			transNorms[n].normalize();

		for ( int t = 0; t < transTangents.count(); t++ )
			transTangents[t].normalize();

		for ( int t = 0; t < transBitangents.count(); t++ )
			transBitangents[t].normalize();

		boundSphere = BoundSphere( transVerts );
		boundSphere.applyInv( viewTrans() );
		updateBounds = false;
	} else {
		transVerts = verts;
		transNorms = norms;
		transTangents = tangents;
		transBitangents = bitangents;
		transColors = colors;
	}

	sortedTriangles = triangles;

	MaterialProperty * matprop = findProperty<MaterialProperty>();
	if ( matprop && matprop->alphaValue() != 1.0 ) {
		float a = matprop->alphaValue();
		transColors.resize( colors.count() );

		for ( int c = 0; c < colors.count(); c++ )
			transColors[c] = colors[c].blend( a );
	} else {
		transColors = colors;
		if ( bslsp ) {
			if ( !(bslsp->getFlags1() & ShaderFlags::SLSF1_Vertex_Alpha) ) {
				for ( int c = 0; c < colors.count(); c++ )
					transColors[c] = ::Color4( colors[c].red(), colors[c].green(), colors[c].blue(), 1.0f );
			}
		}
	}
}

BoundSphere Mesh::bounds() const
{
	if ( updateBounds ) {
		updateBounds = false;
		boundSphere = BoundSphere( verts );
	}

	return worldTrans() * boundSphere;
}

void Mesh::drawShapes( NodeList * secondPass, bool presort )
{
	if ( isHidden() )
		return;

	// TODO: Only run this if BSXFlags has "EditorMarkers present" flag
	if ( !(scene->options & Scene::ShowMarkers) && name.startsWith( "EditorMarker" ) )
		return;

	auto nif = static_cast<const NifModel *>(iBlock.model());
	
	if ( Node::SELECTING ) {
		if ( scene->selMode & Scene::SelObject ) {
			int s_nodeId = ID2COLORKEY( nodeId );
			glColor4ubv( (GLubyte *)&s_nodeId );
		} else {
			glColor4f( 0, 0, 0, 1 );
		}
	}

	// BSOrderedNode
	presorted |= presort;

	// Draw translucent meshes in second pass

	AlphaProperty * aprop = findProperty<AlphaProperty>();

	drawSecond |= (aprop && aprop->blend());

	if ( secondPass && drawSecond ) {
		secondPass->add( this );
		return;
	}

	// TODO: Option to hide Refraction and other post effects

	// rigid mesh? then pass the transformation on to the gl layer

	if ( transformRigid ) {
		glPushMatrix();
		glMultMatrix( viewTrans() );
	}

	//if ( !Node::SELECTING ) {
	//	qDebug() << viewTrans().translation;
		//qDebug() << Vector3( nif->get<Vector4>( iBlock, "Translation" ) );
	//}

	// Debug axes
	//drawAxes(Vector3(), 35.0);

	// setup array pointers

	// Render polygon fill slightly behind alpha transparency and wireframe
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 2.0f );

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, transVerts.constData() );

	if ( !Node::SELECTING ) {
		if ( transNorms.count() ) {
			glEnableClientState( GL_NORMAL_ARRAY );
			glNormalPointer( GL_FLOAT, 0, transNorms.constData() );
		}

		// Do VCs if legacy or if either bslsp or bsesp is set
		bool doVCs = (!bssp) || (bssp && (bssp->getFlags2() & ShaderFlags::SLSF2_Vertex_Colors));

		if ( transColors.count()
			&& ( scene->options & Scene::DoVertexColors )
			&& doVCs )
		{
			glEnableClientState( GL_COLOR_ARRAY );
			glColorPointer( 4, GL_FLOAT, 0, transColors.constData() );
		} else {
			if ( !hasVertexColors && (bslsp && bslsp->hasVertexColors) ) {
				// Correctly blacken the mesh if SLSF2_Vertex_Colors is still on
				//	yet "Has Vertex Colors" is not.
				glColor( ::Color3( 0.0f, 0.0f, 0.0f ) );
			} else {
				glColor( ::Color3( 1.0f, 1.0f, 1.0f ) );
			}
		}
	}

	// TODO: Hotspot.  See about optimizing this.
	if ( !Node::SELECTING )
		shader = scene->renderer->setupProgram( this, shader );

	if ( isDoubleSided ) {
		glDisable( GL_CULL_FACE );
	}

	if ( !isLOD ) {
		// render the triangles
		if ( sortedTriangles.count() )
			glDrawElements( GL_TRIANGLES, sortedTriangles.count() * 3, GL_UNSIGNED_SHORT, sortedTriangles.constData() );

	} else if ( sortedTriangles.count() ) {
		auto lod_shape = DynamicCast<BSLODTriShape>(nif->getBlock(iBlock));
		

		auto lod0 = lod_shape->GetLod0Size();// nif->get<uint>(iBlock, "LOD0 Size");
		auto lod1 = lod_shape->GetLod1Size(); //nif->get<uint>( iBlock, "LOD1 Size" );
		auto lod2 = lod_shape->GetLod2Size(); //nif->get<uint>( iBlock, "LOD2 Size" );

		auto lod0tris = sortedTriangles.mid( 0, lod0 );
		auto lod1tris = sortedTriangles.mid( lod0, lod1 );
		auto lod2tris = sortedTriangles.mid( lod0 + lod1, lod2 );

		// If Level2, render all
		// If Level1, also render Level0
		switch ( scene->lodLevel ) {
		case Scene::Level2:
			if ( lod2tris.count() )
				glDrawElements( GL_TRIANGLES, lod2tris.count() * 3, GL_UNSIGNED_SHORT, lod2tris.constData() );
		case Scene::Level1:
			if ( lod1tris.count() )
				glDrawElements( GL_TRIANGLES, lod1tris.count() * 3, GL_UNSIGNED_SHORT, lod1tris.constData() );
		case Scene::Level0:
		default:
			if ( lod0tris.count() )
				glDrawElements( GL_TRIANGLES, lod0tris.count() * 3, GL_UNSIGNED_SHORT, lod0tris.constData() );
			break;
		}
	}

	// render the tristrips
	for ( auto & s : tristrips )
		glDrawElements( GL_TRIANGLE_STRIP, s.count(), GL_UNSIGNED_SHORT, s.constData() );

	if ( isDoubleSided ) {
		glEnable( GL_CULL_FACE );
	}

	if ( !Node::SELECTING )
		scene->renderer->stopProgram();

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );

	glDisable( GL_POLYGON_OFFSET_FILL );

	glPointSize( 8.5 );
	if ( scene->selMode & Scene::SelVertex ) {
		drawVerts();
	}

	if ( transformRigid )
		glPopMatrix();
}

void Mesh::drawVerts() const
{
	glDisable( GL_LIGHTING );
	glNormalColor();

	glBegin( GL_POINTS );

	for ( int i = 0; i < transVerts.count(); i++ ) {
		if ( Node::SELECTING ) {
			int id = ID2COLORKEY( (shapeNumber << 16) + i );
			glColor4ubv( (GLubyte *)&id );
		}
		glVertex( transVerts.value( i ) );
	}

	// Highlight selected vertex
	if ( !Node::SELECTING && iData == scene->currentBlock ) {
		auto idx = scene->currentIndex;
		if ( idx.data( Qt::DisplayRole ).toString() == "Vertices" ) {
			glHighlightColor();
			glVertex( transVerts.value( idx.row() ) );
		}
	}

	glEnd();
}

void Mesh::drawSelection() const
{
	if ( scene->options & Scene::ShowNodes )
		Node::drawSelection();

	if ( isHidden() || !(scene->selMode & Scene::SelObject) )
		return;

	auto idx = scene->currentIndex;
	auto blk = scene->currentBlock;

	auto nif = static_cast<const NifModel *>(idx.model());
	if ( !nif )
		return;

	if ( blk != iBlock && blk != iData && blk != iSkinPart && blk != iSkinData
	     && ( !iTangentData.isValid() || blk != iTangentData ) )
	{
		return;
	}

	if ( transformRigid ) {
		glPushMatrix();
		glMultMatrix( viewTrans() );
	}

	glDisable( GL_LIGHTING );
	glDisable( GL_COLOR_MATERIAL );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_NORMALIZE );
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_ALPHA_TEST );

	glDisable( GL_CULL_FACE );

	glLineWidth( 1.0 );
	glPointSize( 3.5 );

	QString n = "Faces";
	int i = -1;

	/*if ( blk == iBlock || idx == iData ) {
		n = "Faces";
	} else if ( blk == iData || blk == iSkinPart ) {
		n = idx.data( NifSkopeDisplayRole ).toString();

		QModelIndex iParent = idx.parent();
		if ( iParent.isValid() && iParent != iData ) {
			n = iParent.data( NifSkopeDisplayRole ).toString();
			i = idx.row();
		}
	} else if ( blk == iTangentData ) {
		n = "TSpace";
	} else {
		n = idx.data( NifSkopeDisplayRole ).toString();
	}*/

	glDepthFunc( GL_LEQUAL );
	glNormalColor();

	glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );

	if ( n == "Vertices" || n == "Normals" || n == "Vertex Colors"
	     || n == "UV Sets" || n == "Tangents" || n == "Bitangents" )
	{
		glBegin( GL_POINTS );

		for ( int j = 0; j < transVerts.count(); j++ )
			glVertex( transVerts.value( j ) );

		glEnd();

		if ( i >= 0 ) {
			glDepthFunc( GL_ALWAYS );
			glHighlightColor();
			glBegin( GL_POINTS );
			glVertex( transVerts.value( i ) );
			glEnd();
		}
	}

	//if ( n == "Points" ) {
	//	glBegin( GL_POINTS );
	//	const NifModel * nif = static_cast<const NifModel *>( iData.model() );
	//	QModelIndex points = nif->getIndex( iData, "Points" );

	//	if ( points.isValid() ) {
	//		for ( int j = 0; j < nif->rowCount( points ); j++ ) {
	//			QModelIndex iPoints = points.child( j, 0 );

	//			for ( int k = 0; k < nif->rowCount( iPoints ); k++ ) {
	//				glVertex( transVerts.value( nif->get<quint16>( iPoints.child( k, 0 ) ) ) );
	//			}
	//		}
	//	}

	//	glEnd();

	//	if ( i >= 0 ) {
	//		glDepthFunc( GL_ALWAYS );
	//		glHighlightColor();
	//		glBegin( GL_POINTS );
	//		QModelIndex iPoints = points.child( i, 0 );

	//		if ( nif->isArray( idx ) ) {
	//			for ( int j = 0; j < nif->rowCount( iPoints ); j++ ) {
	//				glVertex( transVerts.value( nif->get<quint16>( iPoints.child( j, 0 ) ) ) );
	//			}
	//		} else {
	//			iPoints = idx.parent();
	//			glVertex( transVerts.value( nif->get<quint16>( iPoints.child( i, 0 ) ) ) );
	//		}

	//		glEnd();
	//	}
	//}

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// TODO: Reenable as an alternative to MSAA when MSAA is not supported
	//glEnable( GL_LINE_SMOOTH );
	//glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

	if ( n == "Normals" || n == "TSpace" ) {
		float normalScale = bounds().radius / 20;

		if ( normalScale < 0.1f )
			normalScale = 0.1f;

		glBegin( GL_LINES );

		for ( int j = 0; j < transVerts.count() && j < transNorms.count(); j++ ) {
			glVertex( transVerts.value( j ) );
			glVertex( transVerts.value( j ) + transNorms.value( j ) * normalScale );
		}

		if ( n == "TSpace" ) {
			for ( int j = 0; j < transVerts.count() && j < transTangents.count() && j < transBitangents.count(); j++ ) {
				glVertex( transVerts.value( j ) );
				glVertex( transVerts.value( j ) + transTangents.value( j ) * normalScale );
				glVertex( transVerts.value( j ) );
				glVertex( transVerts.value( j ) + transBitangents.value( j ) * normalScale );
			}
		}

		glEnd();

		if ( i >= 0 ) {
			glDepthFunc( GL_ALWAYS );
			glHighlightColor();
			glBegin( GL_LINES );
			glVertex( transVerts.value( i ) );
			glVertex( transVerts.value( i ) + transNorms.value( i ) * normalScale );
			glEnd();
		}
	}

	if ( n == "Tangents" ) {
		float normalScale = bounds().radius / 20;
		normalScale /= 2.0f;

		if ( normalScale < 0.1f )
			normalScale = 0.1f;

		glBegin( GL_LINES );

		for ( int j = 0; j < transVerts.count() && j < transTangents.count(); j++ ) {
			glVertex( transVerts.value( j ) );
			glVertex( transVerts.value( j ) + transTangents.value( j ) * normalScale * 2 );
			glVertex( transVerts.value( j ) );
			glVertex( transVerts.value( j ) - transTangents.value( j ) * normalScale / 2 );
		}

		glEnd();

		if ( i >= 0 ) {
			glDepthFunc( GL_ALWAYS );
			glHighlightColor();
			glBegin( GL_LINES );
			glVertex( transVerts.value( i ) );
			glVertex( transVerts.value( i ) + transTangents.value( i ) * normalScale * 2 );
			glVertex( transVerts.value( i ) );
			glVertex( transVerts.value( i ) - transTangents.value( i ) * normalScale / 2 );
			glEnd();
		}
	}

	if ( n == "Bitangents" ) {
		float normalScale = bounds().radius / 20;
		normalScale /= 2.0f;

		if ( normalScale < 0.1f )
			normalScale = 0.1f;

		glBegin( GL_LINES );

		for ( int j = 0; j < transVerts.count() && j < transBitangents.count(); j++ ) {
			glVertex( transVerts.value( j ) );
			glVertex( transVerts.value( j ) + transBitangents.value( j ) * normalScale * 2 );
			glVertex( transVerts.value( j ) );
			glVertex( transVerts.value( j ) - transBitangents.value( j ) * normalScale / 2 );
		}

		glEnd();

		if ( i >= 0 ) {
			glDepthFunc( GL_ALWAYS );
			glHighlightColor();
			glBegin( GL_LINES );
			glVertex( transVerts.value( i ) );
			glVertex( transVerts.value( i ) + transBitangents.value( i ) * normalScale * 2 );
			glVertex( transVerts.value( i ) );
			glVertex( transVerts.value( i ) - transBitangents.value( i ) * normalScale / 2 );
			glEnd();
		}
	}

	if ( n == "Faces" || n == "Triangles" ) {
		glLineWidth( 1.5f );

		for ( const ::Triangle& tri : triangles ) {
			glBegin( GL_TRIANGLES );
			glVertex( transVerts.value( tri.v1() ) );
			glVertex( transVerts.value( tri.v2() ) );
			glVertex( transVerts.value( tri.v3() ) );
			//glVertex( transVerts.value( tri.v1() ) );
			glEnd();
		}

		if ( i >= 0 ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glDepthFunc( GL_ALWAYS );
			glHighlightColor();
			::Triangle tri = triangles.value( i );
			glBegin( GL_TRIANGLES );
			glVertex( transVerts.value( tri.v1() ) );
			glVertex( transVerts.value( tri.v2() ) );
			glVertex( transVerts.value( tri.v3() ) );
			//glVertex( transVerts.value( tri.v1() ) );
			glEnd();
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
	}

	if ( n == "Faces" || n == "Strips" || n == "Strip Lengths" ) {
		glLineWidth( 1.5f );

		for ( const TriStrip& strip : tristrips ) {
			quint16 a = strip.value( 0 );
			quint16 b = strip.value( 1 );

			for ( int v = 2; v < strip.count(); v++ ) {
				quint16 c = strip[v];

				if ( a != b && b != c && c != a ) {
					glBegin( GL_LINE_STRIP );
					glVertex( transVerts.value( a ) );
					glVertex( transVerts.value( b ) );
					glVertex( transVerts.value( c ) );
					glVertex( transVerts.value( a ) );
					glEnd();
				}

				a = b;
				b = c;
			}
		}

		if ( i >= 0 && !tristrips.isEmpty() ) {
			TriStrip strip = tristrips[i];

			quint16 a = strip.value( 0 );
			quint16 b = strip.value( 1 );

			for ( int v = 2; v < strip.count(); v++ ) {
				quint16 c = strip[v];

				if ( a != b && b != c && c != a ) {
					glDepthFunc( GL_ALWAYS );
					glHighlightColor();
					glBegin( GL_LINE_STRIP );
					glVertex( transVerts.value( a ) );
					glVertex( transVerts.value( b ) );
					glVertex( transVerts.value( c ) );
					glVertex( transVerts.value( a ) );
					glEnd();
				}

				a = b;
				b = c;
			}
		}
	}

	if ( n == "Skin Partition Blocks" ) {

		for ( int c = 0; c < partitions.count(); c++ ) {
			if ( c == i )
				glHighlightColor();
			else
				glNormalColor();

			QVector<int> vmap = partitions[c].vertexMap;

			for ( const ::Triangle& tri : partitions[c].triangles ) {
				glBegin( GL_LINE_STRIP );
				glVertex( transVerts.value( vmap.value( tri.v1() ) ) );
				glVertex( transVerts.value( vmap.value( tri.v2() ) ) );
				glVertex( transVerts.value( vmap.value( tri.v3() ) ) );
				glVertex( transVerts.value( vmap.value( tri.v1() ) ) );
				glEnd();
			}
			for ( const TriStrip& strip : partitions[c].tristrips ) {
				quint16 a = vmap.value( strip.value( 0 ) );
				quint16 b = vmap.value( strip.value( 1 ) );

				for ( int v = 2; v < strip.count(); v++ ) {
					quint16 c = vmap.value( strip[v] );

					if ( a != b && b != c && c != a ) {
						glBegin( GL_LINE_STRIP );
						glVertex( transVerts.value( a ) );
						glVertex( transVerts.value( b ) );
						glVertex( transVerts.value( c ) );
						glVertex( transVerts.value( a ) );
						glEnd();
					}

					a = b;
					b = c;
				}
			}
		}
	}

	//if ( n == "Bone List" ) {
	//	if ( nif->isArray( idx ) ) {
	//		for ( int i = 0; i < nif->rowCount( idx ); i++ )
	//			boneSphere( nif, idx.child( i, 0 ) );
	//	} else {
	//		boneSphere( nif, idx );
	//	}
	//}

	if ( transformRigid )
		glPopMatrix();
}

QString Mesh::textStats() const
{
	return Node::textStats() + QString( "\nshader: %1\n" ).arg( shader );
}
