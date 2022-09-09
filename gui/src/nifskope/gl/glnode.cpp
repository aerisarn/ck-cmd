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

#include "glnode.h"

//#include "nifskope.h"
#include "gl/controllers.h"
#include "gl/glmarker.h"
#include "gl/marker/furniture.h"
#include "gl/marker/constraints.h"

//#include "ui/settingsdialog.h"

//#include "lib/nvtristripwrapper.h"

#include <QRegularExpression>
#include <QSettings>

#include <algorithm> // std::stable_sort



using namespace Niflib;

//! @file glnode.cpp Scene management for visible NiNodes and their children.

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif


int Node::SELECTING = 0;

static QColor highlightColor;
static QColor wireframeColor;

/*
 *  Node list
 */

NodeList::NodeList()
{
}

NodeList::NodeList( const NodeList & other )
{
	operator=( other );
}

NodeList::~NodeList()
{
	clear();
}

void NodeList::clear()
{
	foreach ( Node * n, nodes ) {
		del( n );
	}
}

NodeList & NodeList::operator=( const NodeList & other )
{
	clear();
	for ( Node * n : other.list() ) {
		add( n );
	}
	return *this;
}

void NodeList::add( Node * n )
{
	if ( n && !nodes.contains( n ) ) {
		++n->ref;
		nodes.append( n );
	}
}

void NodeList::del( Node * n )
{
	if ( nodes.contains( n ) ) {
		int cnt = nodes.removeAll( n );

		if ( n->ref <= cnt ) {
			delete n;
		} else {
			n->ref -= cnt;
		}
	}
}

Node * NodeList::get( const QModelIndex & index ) const
{
	for ( Node * n : nodes ) {
		if ( n->index().isValid() && n->index() == index )
			return n;
	}
	return nullptr;
}

void NodeList::validate()
{
	QList<Node *> rem;
	for ( Node * n : nodes ) {
		if ( !n->isValid() )
			rem.append( n );
	}
	foreach ( Node * n, rem ) {
		del( n );
	}
}

bool compareNodes( const Node * node1, const Node * node2 )
{
	bool p1 = node1->isPresorted();
	bool p2 = node2->isPresorted();

	// Presort meshes
	if ( p1 && p2 ) {
		return node1->id() < node2->id();
	}

	return p2;
}

bool compareNodesAlpha( const Node * node1, const Node * node2 )
{
	// Presorted meshes override other sorting
	// Alpha enabled meshes on top (sorted from rear to front)

	bool p1 = node1->isPresorted();
	bool p2 = node2->isPresorted();

	// Presort meshes
	if ( p1 && p2 ) {
		return node1->id() < node2->id();
	}

	bool a1 = node1->findProperty<AlphaProperty>();
	bool a2 = node2->findProperty<AlphaProperty>();

	float d1 = node1->viewDepth();
	float d2 = node2->viewDepth();

	// Alpha sort meshes
	if ( a1 == a2 ) {
		return (d1 < d2);
	}

	return a2;
}

void NodeList::sort()
{
	std::stable_sort( nodes.begin(), nodes.end(), compareNodes );
}

void NodeList::alphaSort()
{
	std::stable_sort( nodes.begin(), nodes.end(), compareNodesAlpha );
}

/*
 *	Node
 */


Node::Node( Scene * s, const QModelIndex & index ) : IControllable( s, index ), parent( 0 ), ref( 0 )
{
	nodeId = 0;
	flags.bits = 0;

	updateSettings();

	//connect( NifSkope::getOptions(), &SettingsDialog::saveSettings, this, &Node::updateSettings );
}


void Node::updateSettings()
{
	QSettings settings;
	settings.beginGroup( "Settings/Render/Colors/" );
	// TODO: Remove the registry read for every new Node
	cfg.highlight = settings.value( "Highlight", QColor( 255, 255, 0 ) ).value<QColor>();
	cfg.wireframe = settings.value( "Wireframe", QColor( 0, 255, 0 ) ).value<QColor>();

	highlightColor = cfg.highlight;
	wireframeColor = cfg.wireframe;

	settings.endGroup();
}

// Old Options API
//	TODO: Move away from the GL-like naming
void glHighlightColor()
{
	glColor( ::Color4( highlightColor ) );
}

void glNormalColor()
{
	glColor(::Color4( wireframeColor ) );
}

void Node::glHighlightColor() const
{
	glColor(::Color4( cfg.highlight ) );
}

void Node::glNormalColor() const
{
	glColor(::Color4( cfg.wireframe ) );
}


void Node::clear()
{
	IControllable::clear();

	nodeId = 0;
	flags.bits = 0;
	local = Transform();

	children.clear();
	properties.clear();
}

Controller * Node::findController( const QString & proptype, const QString & ctrltype, const QString & var1, const QString & var2 )
{
	if ( proptype != "<empty>" && !proptype.isEmpty() ) {
		for ( Property * prp : properties.list() ) {
			if ( prp->typeId() == proptype ) {
				return prp->findController( ctrltype, var1, var2 );
			}
		}
		return nullptr;
	}

	return IControllable::findController( ctrltype, var1, var2 );
}

Controller * Node::findController( const QString & proptype, const QModelIndex & index )
{
	Controller * c = nullptr;

	for ( Property * prp : properties.list() ) {
		if ( prp->typeId() == proptype ) {
			if ( c )
				break;

			c = prp->findController( index );
		}
	}

	return c;
}

void Node::update( const NifModel * nif, const QModelIndex & index )
{
	IControllable::update( nif, index );

	if ( !iBlock.isValid() ) {
		clear();
		return;
	}

	nodeId = nif->getBlockNumber( iBlock );
	auto node = DynamicCast<NiNode>(nif->getBlock(iBlock));
	if (node = nullptr)
		return;

	if ( iBlock == index ) {
		flags.bits = node->GetFlags();//nif->get<int>( iBlock, "Flags" );
		local = Transform( nif, iBlock );
	}

	if ( iBlock == index || !index.isValid() ) {
		PropertyList newProps;

		//UNUSED?
		//if (nullptr != )
		//for ( const auto l : nif->getLinkArray( iBlock, "Properties" ) ) {
		//	if ( Property * p = scene->getProperty( nif, nif->getBlock( l ) ) )
		//		newProps.add( p );
		//}

		if (node->IsDerivedType(NiGeometry::TYPE)) {
			auto geometry = DynamicCast<NiGeometry>(node);
			if (geometry != nullptr)
			{
				if (nullptr != geometry->GetShaderProperty())
				{
					Property* p = scene->getProperty(nif, nif->getBlockIndex(StaticCast<NiObject>(geometry->GetShaderProperty())));
					if (p)
						newProps.add(p);
				}
				if (nullptr != geometry->GetAlphaProperty())
				{
					Property* p = scene->getProperty(nif, nif->getBlockIndex(StaticCast<NiObject>(geometry->GetAlphaProperty())));
					if (p)
						newProps.add(p);
				}
			}
		}

		properties = newProps;

		children.clear();

		auto& children = node->GetChildren();
		for (const auto& child : children)
		{
			QModelIndex iChild = nif->getBlockIndex(StaticCast<NiObject>(child));
			Node* node = scene->getNode(nif, iChild);

			if (node) {
				node->makeParent(this);
			}
		}
	}
}

void Node::makeParent( Node * newParent )
{
	if ( parent )
		parent->children.del( this );

	parent = newParent;

	if ( parent )
		parent->children.add( this );
}

void Node::setController( const NifModel * nif, const QModelIndex & iController )
{
	QString cname = nif->itemName( iController );

	if ( cname == "NiTransformController" ) {
		Controller * ctrl = new TransformController( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	} else if ( cname == "NiMultiTargetTransformController" ) {
		Controller * ctrl = new MultiTargetTransformController( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	} else if ( cname == "NiControllerManager" ) {
		Controller * ctrl = new ControllerManager( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	} else if ( cname == "NiKeyframeController" ) {
		Controller * ctrl = new KeyframeController( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	} else if ( cname == "NiVisController" ) {
		Controller * ctrl = new VisibilityController( this, iController );
		ctrl->update( nif, iController );
		controllers.append( ctrl );
	}
}

void Node::activeProperties( PropertyList & list ) const
{
	list.merge( properties );

	if ( parent )
		parent->activeProperties( list );
}

const Transform & Node::viewTrans() const
{
	if ( scene->viewTrans.contains( nodeId ) )
		return scene->viewTrans[ nodeId ];

	Transform t;

	if ( parent )
		t = parent->viewTrans() * local;
	else
		t = scene->view * worldTrans();

	scene->viewTrans.insert( nodeId, t );
	return scene->viewTrans[ nodeId ];
}

const Transform & Node::worldTrans() const
{
	if ( scene->worldTrans.contains( nodeId ) )
		return scene->worldTrans[ nodeId ];

	Transform t = local;

	if ( parent )
		t = parent->worldTrans() * t;

	scene->worldTrans.insert( nodeId, t );
	return scene->worldTrans[ nodeId ];
}

Transform Node::localTrans( int root ) const
{
	Transform trans;
	const Node * node = this;

	while ( node && node->nodeId != root ) {
		trans = node->local * trans;
		node  = node->parent;
	}

	return trans;
}

const ::Vector3 Node::center() const
{
	return worldTrans().translation;
}

float Node::viewDepth() const
{
	return viewTrans().translation[2];
}

Node * Node::findParent( int id ) const
{
	Node * node = parent;

	while ( node && node->nodeId != id )
		node = node->parent;

	return node;
}

Node * Node::findChild( int id ) const
{
	for ( Node * child : children.list() ) {
		if ( child ) {
			if ( child->nodeId == id )
				return child;

			child = child->findChild( id );
			if ( child )
				return child;
		}
	}
	return nullptr;
}

Node * Node::findChild( const QString & str ) const
{
	if ( this->name == str )
		return const_cast<Node *>( this );

	for ( Node * child : children.list() ) {
		Node * n = child->findChild( str );

		if ( n )
			return n;
	}
	return nullptr;
}

bool Node::isHidden() const
{
	if ( scene->options & Scene::ShowHidden )
		return false;

	if ( flags.node.hidden || ( parent && parent->isHidden() ) )
		return true;

	return false; /*!Options::cullExpression().pattern().isEmpty() && name.contains( Options::cullExpression() );*/
}

void Node::transform()
{
	IControllable::transform();

	// if there's a rigid body attached, then calculate and cache the body's transform
	// (need this later in the drawing stage for the constraints)
	const NifModel * nif = static_cast<const NifModel *>( iBlock.model() );

	if ( iBlock.isValid() && nif ) {
		// Scale up for Skyrim
		float havokScale = (nif->checkVersion( 0x14020007, 0x14020007 ) && nif->getUserVersion() >= 12) ? 10.0f : 1.0f;

		auto node = DynamicCast<NiNode>(nif->getBlock(iBlock));
		if (node != nullptr)
		{
			auto collision = DynamicCast<bhkCollisionObject>(node->GetCollisionObject());
			if (nullptr != collision)
			{
				auto body = DynamicCast<bhkRigidBody>(collision->GetBody());
				if (body)
				{
					Transform t;
					t.scale = 7.0f;

					if (body->IsSameType(bhkRigidBodyT::TYPE))
					{
						t.rotation.fromQuat(body->GetRotation());
						t.translation = ::Vector3(body->GetTranslation() * 7.0f * havokScale);
					}
				}
			}
		}

		/*QModelIndex iObject = nif->getBlock( nif->getLink( iBlock, "Collision Object" ) );
		if ( nif->getUserVersion2() > 0 && iObject.isValid() ) {
			QModelIndex iBody = nif->getBlock( nif->getLink( iObject, "Body" ) );

			if ( iBody.isValid() ) {
				Transform t;
				t.scale = 7.0f;

				if ( nif->isNiBlock( iBody, "bhkRigidBodyT" ) ) {
					t.rotation.fromQuat( nif->get<Quat>( iBody, "Rotation" ) );
					t.translation = ::Vector3( nif->get<Vector4>( iBody, "Translation" ) * 7.0f * havokScale );
				}

				scene->bhkBodyTrans.insert( nif->getBlockNumber( iBody ), worldTrans() * t );
			}
		}*/
	}

	for ( Node * node : children.list() ) {
		node->transform();
	}
}

void Node::transformShapes()
{
	for ( Node * node : children.list() ) {
		node->transformShapes();
	}
}

void Node::draw()
{
	if ( isHidden() || iBlock == scene->currentBlock )
		return;

	if ( !(scene->selMode & Scene::SelObject) )
		return;

	if ( Node::SELECTING ) {
		int s_nodeId = ID2COLORKEY( nodeId );
		glColor4ubv( (GLubyte *)&s_nodeId );
		glLineWidth( 5 ); // make hitting a line a litlle bit more easy
	} else {
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LEQUAL );
		glDepthMask( GL_TRUE );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_NORMALIZE );
		glDisable( GL_LIGHTING );
		glDisable( GL_COLOR_MATERIAL );
		glEnable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glNormalColor();
		glLineWidth( 2.5 );
	}

	glPointSize( 8.5 );

	::Vector3 a = viewTrans().translation;
	::Vector3 b = a;

	if ( parent )
		b = parent->viewTrans().translation;

	glBegin( GL_POINTS );
	glVertex( a );
	glEnd();

	if ( Node::SELECTING ) {
		glBegin( GL_LINES );
		glVertex( a );
		glVertex( b );
		glEnd();
	} else {
		auto c = cfg.wireframe;
		glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() / 3.0 );
		drawDashLine( a, b, 144 );
	}

	for ( Node * node : children.list() ) {
		node->draw();
	}
}

void Node::drawSelection() const
{
	auto nif = static_cast<const NifModel *>(scene->currentIndex.model());
	if ( !nif )
		return;

	if ( !(scene->selMode & Scene::SelObject) )
		return;

	bool extraData = false;
	auto currentBlock = nif->getBlockName( scene->currentBlock );
	if ( currentBlock == "BSConnectPoint::Parents" )
		extraData = nif->getBlockNumber( iBlock ) == 0; // Root Node only

	if ( scene->currentBlock != iBlock && !extraData )
		return;

	//auto n = scene->currentIndex.data( NifSkopeDisplayRole ).toString();

	if ( Node::SELECTING ) {
		int s_nodeId = ID2COLORKEY( nodeId );
		glColor4ubv( (GLubyte *)&s_nodeId );
		glLineWidth( 5 );
	} else {
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_ALWAYS );
		glDepthMask( GL_TRUE );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_NORMALIZE );
		glDisable( GL_LIGHTING );
		glDisable( GL_COLOR_MATERIAL );
		glEnable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glHighlightColor();
		glLineWidth( 2.5 );
	}

	glPointSize( 8.5 );

	glPushMatrix();
	glMultMatrix( viewTrans() );

	float sceneRadius = scene->bounds().radius;
	float normalScale = (sceneRadius > 150.0) ? 1.0 : sceneRadius / 150.0;

	//if ( currentBlock == "BSConnectPoint::Parents" ) {
	//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	//	auto cp = nif->getIndex( scene->currentBlock, "Connect Points" );
	//	bool isChild = scene->currentIndex.parent().data( NifSkopeDisplayRole ).toString() == "Connect Points";

	//	int sel = -1;
	//	if ( n == "Connect Points" && !nif->isArray( scene->currentIndex ) ) {
	//		sel = scene->currentIndex.row();
	//	} else if ( isChild ) {
	//		sel = scene->currentIndex.parent().row();
	//	}

	//	int ct = nif->rowCount( cp );
	//	for ( int i = 0; i < ct; i++ ) {
	//		auto p = cp.child( i, 0 );

	//		auto trans = nif->get<Vector3>( p, "Translation" );
	//		auto rot = nif->get<Quat>( p, "Rotation" );
	//		//auto scale = nif->get<float>( p, "Scale" );

	//		Transform t;
	//		Matrix m;
	//		m.fromQuat( rot );
	//		t.rotation = m;
	//		t.translation = trans;
	//		t.scale = normalScale * 16;

	//		if ( i == sel ) {
	//			glHighlightColor();
	//		} else {
	//			glNormalColor();
	//		}

	//		glPushMatrix();
	//		glMultMatrix( t );

	//		auto pos = Vector3( 0, 0, 0 );

	//		drawDashLine( pos, Vector3( 0, 1, 0 ), 15 );
	//		drawDashLine( pos, Vector3( 1, 0, 0 ), 15 );
	//		drawDashLine( pos, Vector3( 0, 0, 1 ), 15 );
	//		drawCircle( pos, Vector3( 0, 1, 0 ), 1, 64 );

	//		glPopMatrix();
	//	}

	//}

	const auto& node = DynamicCast<NiNode>(nif->getBlock(scene->currentBlock));

	if (node != nullptr && scene->options & Scene::ShowNodes && scene->options & Scene::ShowAxes ) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		Transform t;
		Matrix m(node->GetRotation());
		t.rotation = m;

		glPushMatrix();
		glMultMatrix( t );

		auto pos = ::Vector3( 0, 0, 0 );

		glColor( { 0, 1, 0 } );
		drawDashLine( pos, ::Vector3( 0, 1, 0 ), 15 );
		glColor( { 1, 0, 0 } );
		drawDashLine( pos, ::Vector3( 1, 0, 0 ), 15 );
		glColor( { 0, 0, 1 } );
		drawDashLine( pos, ::Vector3( 0, 0, 1 ), 15 );

		glPopMatrix();
	}

	glPopMatrix();

	if ( extraData )
		return;

	::Vector3 a = viewTrans().translation;
	::Vector3 b = a;

	if ( parent )
		b = parent->viewTrans().translation;

	glBegin( GL_POINTS );
	glVertex( a );
	glEnd();

	auto c = cfg.highlight;
	glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() * 0.8 );
	glBegin( GL_LINES );
	glVertex( a );
	glVertex( b );
	glEnd();

	for ( Node * node : children.list() ) {
		node->draw();
	}
}

void DrawVertexSelection( QVector<::Vector3> & verts, int i )
{
	glPointSize( 3.5 );
	glDepthFunc( GL_LEQUAL );
	glNormalColor();
	glBegin( GL_POINTS );

	for ( int j = 0; j < verts.count(); j++ )
		glVertex( verts.value( j ) );

	glEnd();

	if ( i >= 0 ) {
		glDepthFunc( GL_ALWAYS );
		glHighlightColor();
		glBegin( GL_POINTS );
		glVertex( verts.value( i ) );
		glEnd();
	}
}

void DrawTriangleSelection( QVector<::Vector3> const & verts, ::Triangle const & tri )
{
	glLineWidth( 1.5f );
	glDepthFunc( GL_ALWAYS );
	glHighlightColor();
	glBegin( GL_LINE_STRIP );
	glVertex( verts.value( tri.v1() ) );
	glVertex( verts.value( tri.v2() ) );
	glVertex( verts.value( tri.v3() ) );
	glVertex( verts.value( tri.v1() ) );
	glEnd();
}

void DrawTriangleIndex( QVector<::Vector3> const & verts, ::Triangle const & tri, int index )
{
	::Vector3 c = ( verts.value( tri.v1() ) + verts.value( tri.v2() ) + verts.value( tri.v3() ) ) /  3.0;
	renderText( c, QString( "%1" ).arg( index ) );
}

void drawHvkShape( const NifModel * nif, const QModelIndex & iShape, QStack<QModelIndex> & stack, const Scene * scene, const float origin_color3fv[3] )
{
	QString name = (nif) ? nif->itemName( iShape ) : "";

	bool extraData = (name == "hkPackedNiTriStripsData");

	if ( (!nif || !iShape.isValid() || stack.contains( iShape )) && !extraData )
		return;

	if ( !(scene->selMode & Scene::SelObject) )
		return;

	stack.push( iShape );

	// Scale up for Skyrim
	float havokScale = (nif->checkVersion( 0x14020007, 0x14020007 ) && nif->getUserVersion() >= 12) ? 10.0f : 1.0f;

	//qDebug() << "draw shape" << nif->getBlockNumber( iShape ) << nif->itemName( iShape );

	if ( name.endsWith( "ListShape" ) ) {

		auto& shapeList = DynamicCast<bhkListShape>(nif->getBlock(iShape));
		if (shapeList)
		{
			auto& subShapes = shapeList->GetSubShapes();
			for (int r = 0; r < subShapes.size(); r++) 
			{
				auto sub_index = nif->getBlockIndex(StaticCast<NiObject>(subShapes[r]));
				if (!Node::SELECTING) {
					if ( scene->currentBlock == sub_index) {
						// fix: add selected visual to havok meshes
						glHighlightColor();
						glLineWidth( 2.5 );
					} else {
						if ( scene->currentBlock != iShape ) {
							// allow group highlighting
							glLineWidth( 1.0 );
							glColor3fv( origin_color3fv );
						}
					}
				}

				drawHvkShape( nif, sub_index, stack, scene, origin_color3fv );
			}
		}
	} else if ( name == "bhkTransformShape" || name == "bhkConvexTransformShape" ) {
		auto& transformShape = DynamicCast<bhkTransformShape>(nif->getBlock(iShape));
		glPushMatrix();
		Transform t(transformShape->GetTransform());
		t.translation *= havokScale;
		glMultMatrix( t );
		auto shapeIndex = nif->getBlockIndex(StaticCast<NiObject>(transformShape->GetShape()));
		drawHvkShape( nif, shapeIndex, stack, scene, origin_color3fv );
		glPopMatrix();
	} else if ( name == "bhkSphereShape" ) {
		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}
		auto shape = DynamicCast<bhkSphereShape>(nif->getBlock(iShape));
		if (shape != nullptr)
		{
			drawSphere(::Vector3(), shape->GetRadius() * havokScale);
		}
	} else if ( name == "bhkMultiSphereShape" ) {
		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}
		auto shape = DynamicCast<bhkMultiSphereShape>(nif->getBlock(iShape));
		if (shape != nullptr)
		{
			auto& spheres = shape->GetSpheres();
			for (int r = 0; r < spheres.size(); r++) {
				drawSphere(::Vector3(spheres[r].center), spheres[r].radius);
			}
		}
	} else if ( name == "bhkBoxShape" ) {
		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}
		auto shape = DynamicCast<bhkBoxShape>(nif->getBlock(iShape));
		if (shape != nullptr)
		{
			::Vector3 v(shape->GetDimensions());
			v *= havokScale;
			drawBox(v, -v);
		}
	} else if ( name == "bhkCapsuleShape" ) {
		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}
		auto shape = DynamicCast<bhkCapsuleShape>(nif->getBlock(iShape));
		if (shape != nullptr)
		{
			drawCapsule(
				::Vector3(shape->GetFirstPoint()) * havokScale,
				::Vector3(shape->GetSecondPoint()) * havokScale,
				shape->GetRadius() * havokScale
			);
		}
	} else if ( name == "bhkNiTriStripsShape" ) {
		glPushMatrix();
		float s = 1.0f / 7.0f;
		glScalef( s, s, s );

		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}

		drawNiTSS( nif, iShape );
		glPopMatrix();
	} else if ( name == "bhkConvexVerticesShape" ) {
		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}

		drawConvexHull( nif, iShape, havokScale );

	} else if ( name == "bhkMoppBvTreeShape" ) {
		auto shape = DynamicCast<bhkMoppBvTreeShape>(nif->getBlock(iShape));
		if (shape != nullptr)
		{
			if ( !Node::SELECTING ) {
				if ( scene->currentBlock == nif->getBlockIndex(shape->GetShape()) ) {
					// fix: add selected visual to havok meshes
					glHighlightColor();
					glLineWidth( 1.5f ); // taken from "DrawTriangleSelection"
				} else {
					glLineWidth( 1.0 );
					glColor3fv( origin_color3fv );
				}
			}

			auto shapeIndex = nif->getBlockIndex(DynamicCast<NiObject>(shape->GetShape()));
			if (shapeIndex.isValid())
			{
				drawHvkShape(nif, shapeIndex, stack, scene, origin_color3fv);
			}
		}
	} else if ( name == "bhkPackedNiTriStripsShape" ) {
		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}
		auto shape = DynamicCast<bhkPackedNiTriStripsShape>(nif->getBlock(iShape));
		if (nullptr != shape)
		{
			auto data = shape->GetData();
			if (nullptr != data)
			{
				auto& triangles = data->GetTriangles();
				auto& vertices = data->GetVertices();
				for (int t = 0; t < triangles.size(); t++) {
					::Triangle tri(triangles[t].triangle.v1, triangles[t].triangle.v2, triangles[t].triangle.v3);
					if (tri[0] != tri[1] || tri[1] != tri[2] || tri[2] != tri[0]) {
						glBegin(GL_LINE_STRIP);
						glVertex(::Vector3(vertices.at(tri[0])));
						glVertex(::Vector3(vertices.at(tri[1])));
						glVertex(::Vector3(vertices.at(tri[2])));
						glVertex(::Vector3(vertices.at(tri[0])));
						glEnd();
					}
				}
			}
		}
	} else if (name == "hkPackedNiTriStripsData") {
		if (Node::SELECTING) {
			int s_nodeId = ID2COLORKEY(nif->getBlockNumber(iShape));
			glColor4ubv((GLubyte*)&s_nodeId);
		}

		auto data = DynamicCast<hkPackedNiTriStripsData>(nif->getBlock(iShape));
		if (nullptr != data)
		{
			auto& triangles = data->GetTriangles();
			auto& vertices = data->GetVertices();
			for (int t = 0; t < triangles.size(); t++) {
				::Triangle tri(triangles[t].triangle.v1, triangles[t].triangle.v2, triangles[t].triangle.v3);
				if (tri[0] != tri[1] || tri[1] != tri[2] || tri[2] != tri[0]) {
					glBegin(GL_LINE_STRIP);
					glVertex(::Vector3(vertices.at(tri[0])));
					glVertex(::Vector3(vertices.at(tri[1])));
					glVertex(::Vector3(vertices.at(tri[2])));
					glVertex(::Vector3(vertices.at(tri[0])));
					glEnd();
				}
			}
		}

		//QModelIndex iData = nif->getBlock( nif->getLink( iShape, "Data" ) );

		//if ( iData.isValid() ) {
		//	QVector<Vector3> verts = nif->getArray<Vector3>( iData, "Vertices" );
		//	QModelIndex iTris = nif->getIndex( iData, "Triangles" );

		//	for ( int t = 0; t < nif->rowCount( iTris ); t++ ) {
		//		Triangle tri = nif->get<Triangle>( iTris.child( t, 0 ), "Triangle" );

		//		if ( tri[0] != tri[1] || tri[1] != tri[2] || tri[2] != tri[0] ) {
		//			glBegin( GL_LINE_STRIP );
		//			glVertex( verts.value( tri[0] ) );
		//			glVertex( verts.value( tri[1] ) );
		//			glVertex( verts.value( tri[2] ) );
		//			glVertex( verts.value( tri[0] ) );
		//			glEnd();
		//		}
		//	}

			//// Handle Selection of hkPackedNiTriStripsData
			//if ( scene->currentBlock == iData ) {
			//	int i = -1;
			//	QString n = scene->currentIndex.data( NifSkopeDisplayRole ).toString();
			//	QModelIndex iParent = scene->currentIndex.parent();

			//	if ( iParent.isValid() && iParent != iData ) {
			//		n = iParent.data( NifSkopeDisplayRole ).toString();
			//		i = scene->currentIndex.row();
			//	}

			//	if ( n == "Vertices" || n == "Normals" || n == "Vertex Colors" || n == "UV Sets" ) {
			//		DrawVertexSelection( verts, i );
			//	} else if ( ( n == "Faces" || n == "Triangles" ) ) {
			//		if ( i == -1 ) {
			//			glDepthFunc( GL_ALWAYS );
			//			glHighlightColor();

			//			//for ( int t = 0; t < nif->rowCount( iTris ); t++ )
			//			//	DrawTriangleIndex( verts, nif->get<Triangle>( iTris.child( t, 0 ), "Triangle" ), t );
			//		} else if ( nif->isCompound( nif->getBlockType( scene->currentIndex ) ) ) {
			//			Triangle tri = nif->get<Triangle>( iTris.child( i, 0 ), "Triangle" );
			//			DrawTriangleSelection( verts, tri );
			//			//DrawTriangleIndex( verts, tri, i );
			//		} else if ( nif->getBlockName( scene->currentIndex ) == "Normal" ) {
			//			Triangle tri = nif->get<Triangle>( scene->currentIndex.parent(), "Triangle" );
			//			Vector3 triCentre = ( verts.value( tri.v1() ) + verts.value( tri.v2() ) + verts.value( tri.v3() ) ) /  3.0;
			//			glLineWidth( 1.5f );
			//			glDepthFunc( GL_ALWAYS );
			//			glHighlightColor();
			//			glBegin( GL_LINES );
			//			glVertex( triCentre );
			//			glVertex( triCentre + nif->get<Vector3>( scene->currentIndex ) );
			//			glEnd();
			//		}
			//	} else if ( n == "Sub Shapes" ) {
			//		int start_vertex = 0;
			//		int end_vertex = 0;
			//		int num_vertices = nif->get<int>( scene->currentIndex, "Num Vertices" );

			//		int ct = nif->rowCount( iTris );
			//		int totalVerts = 0;
			//		if ( num_vertices > 0 ) {
			//			QModelIndex iParent = scene->currentIndex.parent();
			//			int rowCount = nif->rowCount( iParent );
			//			for ( int j = 0; j < i; j++ ) {
			//				totalVerts += nif->get<int>( iParent.child( j, 0 ), "Num Vertices" );
			//			}

			//			end_vertex += totalVerts + num_vertices;
			//			start_vertex += totalVerts;

			//			ct = (end_vertex - start_vertex) / 3;
			//		}

			//		for ( int t = 0; t < nif->rowCount( iTris ); t++ ) {
			//			Triangle tri = nif->get<Triangle>( iTris.child( t, 0 ), "Triangle" );

			//			if ( (start_vertex <= tri[0]) && (tri[0] < end_vertex) ) {
			//				if ( (start_vertex <= tri[1]) && (tri[1] < end_vertex) && (start_vertex <= tri[2]) && (tri[2] < end_vertex) ) {
			//					DrawTriangleSelection( verts, tri );
			//					//DrawTriangleIndex( verts, tri, t );
			//				} else {
			//					qDebug() << "triangle with multiple materials?" << t;
			//				}
			//			}
			//		}
			//	}
			//}
			//// Handle Selection of bhkPackedNiTriStripsShape
			//else if ( scene->currentBlock == iShape ) {
			//	int i = -1;
			//	QString n = scene->currentIndex.data( NifSkopeDisplayRole ).toString();
			//	QModelIndex iParent = scene->currentIndex.parent();

			//	if ( iParent.isValid() && iParent != iShape ) {
			//		n = iParent.data( NifSkopeDisplayRole ).toString();
			//		i = scene->currentIndex.row();
			//	}

			//	//qDebug() << n;
			//	// n == "Sub Shapes" if the array is selected and if an element of the array is selected
			//	// iParent != iShape only for the elements of the array
			//	if ( ( n == "Sub Shapes" ) && ( iParent != iShape ) ) {
			//		// get subshape vertex indices
			//		QModelIndex iSubShapes = iParent;
			//		QModelIndex iSubShape  = scene->currentIndex;
			//		int start_vertex = 0;
			//		int end_vertex = 0;

			//		for ( int subshape = 0; subshape < nif->rowCount( iSubShapes ); subshape++ ) {
			//			QModelIndex iCurrentSubShape = iSubShapes.child( subshape, 0 );
			//			int num_vertices = nif->get<int>( iCurrentSubShape, "Num Vertices" );
			//			//qDebug() << num_vertices;
			//			end_vertex += num_vertices;

			//			if ( iCurrentSubShape == iSubShape ) {
			//				break;
			//			} else {
			//				start_vertex += num_vertices;
			//			}
			//		}

			//		// highlight the triangles of the subshape
			//		for ( int t = 0; t < nif->rowCount( iTris ); t++ ) {
			//			Triangle tri = nif->get<Triangle>( iTris.child( t, 0 ), "Triangle" );

			//			if ( (start_vertex <= tri[0]) && (tri[0] < end_vertex) ) {
			//				if ( (start_vertex <= tri[1]) && (tri[1] < end_vertex) && (start_vertex <= tri[2]) && (tri[2] < end_vertex) ) {
			//					DrawTriangleSelection( verts, tri );
			//					//DrawTriangleIndex( verts, tri, t );
			//				} else {
			//					qDebug() << "triangle with multiple materials?" << t;
			//				}
			//			}
			//		}
			//	}
			//}
		//}
	} else if ( name == "bhkCompressedMeshShape" ) {
		glPushMatrix();
		float s = 1.0f;
		glScalef( s, s, s );

		if ( Node::SELECTING ) {
			int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iShape ) );
			glColor4ubv( (GLubyte *)&s_nodeId );
		}

		drawCMS( nif, iShape );

		//if ( Options::getHavokState() == HAVOK_SOLID ) {
		//	QColor c = Options::hlColor();
		//	c.setAlphaF( 0.3 );
		//	glColor( Color4( c ) );
		//
		//	drawCMS( nif, iShape, true );
		//}

		glPopMatrix();
	}

	stack.pop();
}

void drawHvkConstraint( const NifModel * nif, const QModelIndex & iConstraint, const Scene * scene )
{
	if ( !( nif && iConstraint.isValid() && scene && (scene->options & Scene::ShowConstraints) ) )
		return;

	if ( !(scene->selMode & Scene::SelObject) )
		return;

	QList<Transform> tBodies;
	auto constraint = DynamicCast<bhkConstraint>(nif->getBlock(iConstraint));
	if (constraint == nullptr)
		return;

	auto iBodies = constraint->GetEntities();

	for ( int r = 0; r < iBodies.size(); r++ ) {

		auto body = DynamicCast<bhkRigidBody>(iBodies[r]);

		qint32 l = nif->getBlockNumber(nif->getBlockIndex(StaticCast<NiObject>(body)));

		if ( !scene->bhkBodyTrans.contains( l ) )
			return; // TODO: Make sure this is not supposed to be continue;

		tBodies.append(Transform(body->GetTranslation(), body->GetRotation()));
	}

	if ( tBodies.count() != 2 )
		return;

	::Color3 color_a( 0.8f, 0.6f, 0.0f );
	::Color3 color_b( 0.6f, 0.8f, 0.0f );

	if ( Node::SELECTING ) {
		int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iConstraint ) );
		glColor4ubv( (GLubyte *)&s_nodeId );
		glLineWidth( 5 ); // make hitting a line a litlle bit more easy
	} else {
		if ( scene->currentBlock == iConstraint ) {
			// fix: add selected visual to havok meshes
			glHighlightColor();
			color_a.fromQColor( highlightColor );
			color_b.setRGB( highlightColor.blueF(), highlightColor.redF(), highlightColor.greenF() );
		}
	}

	glPushMatrix();
	glLoadMatrix( scene->view );

	glPushAttrib( GL_ENABLE_BIT );
	glEnable( GL_DEPTH_TEST );

	QString name = nif->itemName( iConstraint );

	//Unsupported because refactor
	//Compound* descriptor = nullptr;

	//if ( name == "bhkMalleableConstraint") {
	//	auto& malleable = DynamicCast<bhkMalleableConstraint>(constraint)->GetMalleable();
	//	switch (malleable.type)
	//	{
	//	case BALLANDSOCKET:
	//		name = "bhkBallAndSocketConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.ballAndSocket);
	//		break;
	//	case HINGE:
	//		name = "bhkHingeConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.hinge);
	//		break;
	//	case LIMITED_HINGE:
	//		name = "bhkLimitedHingeConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.limitedHinge);
	//		break;
	//	case PRISMATIC:
	//		name = "bhkPrismaticConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.prismatic);
	//		break;
	//	case RAGDOLL:
	//		name = "bhkRagdollConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.ragdoll);
	//		break;
	//	case STIFFSPRING:
	//		name = "bhkStiffSpringConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.stiffSpring);
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//else if (name == "bhkBreakableConstraint") {
	//	auto& malleable = DynamicCast<bhkBreakableConstraint>(constraint)->GetConstraintData().malleable;
	//	switch (malleable.type)
	//	{
	//	case BALLANDSOCKET:
	//		name = "bhkBallAndSocketConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.ballAndSocket);
	//		break;
	//	case HINGE:
	//		name = "bhkHingeConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.hinge);
	//		break;
	//	case LIMITED_HINGE:
	//		name = "bhkLimitedHingeConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.limitedHinge);
	//		break;
	//	case PRISMATIC:
	//		name = "bhkPrismaticConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.prismatic);
	//		break;
	//	case RAGDOLL:
	//		name = "bhkRagdollConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.ragdoll);
	//		break;
	//	case STIFFSPRING:
	//		name = "bhkStiffSpringConstraint";
	//		descriptor = static_cast<Compound*>(&malleable.stiffSpring);
	//		break;
	//	default:
	//		break;
	//	}
	//}

	if ( name == "bhkLimitedHingeConstraint" ) {
		auto iHinge = DynamicCast<bhkLimitedHingeConstraint>(constraint);
		if (iHinge != nullptr)
		{
			auto& descriptor = iHinge->GetLimitedHinge();
			const ::Vector3 pivotA(descriptor.pivotA);
			const ::Vector3 pivotB(descriptor.pivotB);

			const ::Vector3 axleA(descriptor.axleA);
			const ::Vector3 axleA1(descriptor.perp2AxleInA1);
			const ::Vector3 axleA2(descriptor.perp2AxleInA2);

			const ::Vector3 axleB(descriptor.axleB);
			const ::Vector3 axleB2(descriptor.perp2AxleInB2);

			const float minAngle = descriptor.minAngle;
			const float maxAngle = descriptor.maxAngle;

			glPushMatrix();
			glMultMatrix(tBodies.value(0));

			if (!Node::SELECTING)
				glColor(color_a);

			glBegin(GL_POINTS); glVertex(pivotA); glEnd();
			glBegin(GL_LINES); glVertex(pivotA); glVertex(pivotA + axleA); glEnd();
			drawDashLine(pivotA, pivotA + axleA1, 14);
			drawDashLine(pivotA, pivotA + axleA2, 14);
			drawCircle(pivotA, axleA, 1.0f);
			drawSolidArc(pivotA, axleA / 5, axleA2, axleA1, minAngle, maxAngle, 1.0f);
			glPopMatrix();

			glPushMatrix();
			glMultMatrix(tBodies.value(1));

			if (!Node::SELECTING)
				glColor(color_b);

			glBegin(GL_POINTS); glVertex(pivotB); glEnd();
			glBegin(GL_LINES); glVertex(pivotB); glVertex(pivotB + axleB); glEnd();
			drawDashLine(pivotB + axleB2, pivotB, 14);
			drawDashLine(pivotB + ::Vector3::crossproduct(axleB2, axleB), pivotB, 14);
			drawCircle(pivotB, axleB, 1.01f);
			drawSolidArc(pivotB, axleB / 7, axleB2, ::Vector3::crossproduct(axleB2, axleB), minAngle, maxAngle, 1.01f);
			glPopMatrix();

			glMultMatrix(tBodies.value(0));
			float angle = ::Vector3::angle(tBodies.value(0).rotation * axleA2, tBodies.value(1).rotation * axleB2);

			if (!Node::SELECTING)
				glColor(color_a);

			glBegin(GL_LINES);
			glVertex(pivotA);
			glVertex(pivotA + axleA1 * cosf(angle) + axleA2 * sinf(angle));
			glEnd();
		}
	} else if ( name == "bhkHingeConstraint" ) {
		auto iHinge = DynamicCast<bhkHingeConstraint>(constraint);
		if (iHinge != nullptr)
		{
			auto& descriptor = iHinge->GetHinge();
			const ::Vector3 pivotA(descriptor.pivotA);
			const ::Vector3 pivotB(descriptor.pivotB);

			const ::Vector3 axleA1(descriptor.perp2AxleInA1);
			const ::Vector3 axleA2(descriptor.perp2AxleInA2);
			const ::Vector3 axleA(::Vector3::crossproduct(axleA1, axleA2));

			const ::Vector3 axleB(descriptor.axleB);

			const ::Vector3 axleB1(axleB[1], axleB[2], axleB[0]);
			const ::Vector3 axleB2(::Vector3::crossproduct(axleB, axleB1));

			/*
			 * This should be correct but is visually strange...
			 *
			Vector3 axleB1temp;
			Vector3 axleB2temp;

			if ( nif->checkVersion( 0, 0x14000002 ) )
			{
				Vector3 axleB1temp( axleB[1], axleB[2], axleB[0] );
				Vector3 axleB2temp( Vector3::crossproduct( axleB, axleB1temp ) );
			}
			else if ( nif->checkVersion( 0x14020007, 0 ) )
			{
				Vector3 axleB1temp( nif->get<Vector4>( iHinge, "Perp2 Axle In B1" ) );
				Vector3 axleB2temp( nif->get<Vector4>( iHinge, "Perp2 Axle In B2" ) );
			}

			const Vector3 axleB1( axleB1temp );
			const Vector3 axleB2( axleB2temp );
			*/

			const float minAngle = (float)-PI;
			const float maxAngle = (float)+PI;

			glPushMatrix();
			glMultMatrix(tBodies.value(0));

			if (!Node::SELECTING)
				glColor(color_a);

			glBegin(GL_POINTS); glVertex(pivotA); glEnd();
			drawDashLine(pivotA, pivotA + axleA1);
			drawDashLine(pivotA, pivotA + axleA2);
			drawSolidArc(pivotA, axleA / 5, axleA2, axleA1, minAngle, maxAngle, 1.0f, 16);
			glPopMatrix();

			glMultMatrix(tBodies.value(1));

			if (!Node::SELECTING)
				glColor(color_b);

			glBegin(GL_POINTS); glVertex(pivotB); glEnd();
			glBegin(GL_LINES); glVertex(pivotB); glVertex(pivotB + axleB); glEnd();
			drawSolidArc(pivotB, axleB / 7, axleB2, axleB1, minAngle, maxAngle, 1.01f, 16);
		}
	} else if ( name == "bhkStiffSpringConstraint" ) {
		auto stiff = DynamicCast<bhkStiffSpringConstraint>(constraint);
		if (stiff != nullptr)
		{
			auto& descriptor = stiff->GetStiffSpring();
			const ::Vector3 pivotA = tBodies.value(0) * ::Vector3(descriptor.pivotA);
			const ::Vector3 pivotB = tBodies.value(1) * ::Vector3(descriptor.pivotB);
			const float length = descriptor.length;

			if (!Node::SELECTING)
				glColor(color_b);

			drawSpring(pivotA, pivotB, length);
		}
	} else if ( name == "bhkRagdollConstraint" ) {
		auto ragdoll = DynamicCast<bhkRagdollConstraint>(constraint);
		if (ragdoll != nullptr)
		{
			auto& descriptor = ragdoll->GetRagdoll();
			const ::Vector3 pivotA(descriptor.pivotA);
			const ::Vector3 pivotB(descriptor.pivotB);

			const ::Vector3 planeA(descriptor.planeA);
			const ::Vector3 planeB(descriptor.planeB);

			const ::Vector3 twistA(descriptor.twistA);
			const ::Vector3 twistB(descriptor.twistB);

			const float coneAngle(descriptor.coneMaxAngle);

			const float minPlaneAngle(descriptor.planeMinAngle);
			const float maxPlaneAngle(descriptor.planeMaxAngle);

			// Unused? GCC complains
			/*
			const float minTwistAngle( nif->get<float>( iRagdoll, "Twist Min Angle" ) );
			const float maxTwistAngle( nif->get<float>( iRagdoll, "Twist Max Angle" ) );
			*/

			glPushMatrix();
			glMultMatrix(tBodies.value(0));

			if (!Node::SELECTING)
				glColor(color_a);

			glPopMatrix();

			glPushMatrix();
			glMultMatrix(tBodies.value(0));

			if (!Node::SELECTING)
				glColor(color_a);

			glBegin(GL_POINTS); glVertex(pivotA); glEnd();
			glBegin(GL_LINES); glVertex(pivotA); glVertex(pivotA + twistA); glEnd();
			drawDashLine(pivotA, pivotA + planeA, 14);
			drawRagdollCone(pivotA, twistA, planeA, coneAngle, minPlaneAngle, maxPlaneAngle);
			glPopMatrix();

			glPushMatrix();
			glMultMatrix(tBodies.value(1));

			if (!Node::SELECTING)
				glColor(color_b);

			glBegin(GL_POINTS); glVertex(pivotB); glEnd();
			glBegin(GL_LINES); glVertex(pivotB); glVertex(pivotB + twistB); glEnd();
			drawDashLine(pivotB + planeB, pivotB, 14);
			drawRagdollCone(pivotB, twistB, planeB, coneAngle, minPlaneAngle, maxPlaneAngle);
			glPopMatrix();
		}
	} else if ( name == "bhkPrismaticConstraint" ) {
		auto prismatic = DynamicCast<bhkPrismaticConstraint>(constraint);
		if (prismatic != nullptr)
		{
			auto& descriptor = prismatic->GetPrismatic();
			const ::Vector3 pivotA(descriptor.pivotA);
			const ::Vector3 pivotB(descriptor.pivotB);

			const ::Vector3 planeNormal(descriptor.planeA);
			const ::Vector3 slidingAxis(descriptor.slidingA);

			const float minDistance = descriptor.minDistance;
			const float maxDistance = descriptor.maxDistance;

			const ::Vector3 d1 = pivotA + slidingAxis * minDistance;
			const ::Vector3 d2 = pivotA + slidingAxis * maxDistance;

			/* draw Pivot A and Plane */
			glPushMatrix();
			glMultMatrix(tBodies.value(0));

			if (!Node::SELECTING)
				glColor(color_a);

			glBegin(GL_POINTS); glVertex(pivotA); glEnd();
			glBegin(GL_LINES); glVertex(pivotA); glVertex(pivotA + planeNormal); glEnd();
			drawDashLine(pivotA, d1, 14);

			/* draw rail */
			if (minDistance < maxDistance) {
				drawRail(d1, d2);
			}

			/*draw first marker*/
			Transform t;
			float angle = atan2f(slidingAxis[1], slidingAxis[0]);

			if (slidingAxis[0] < 0.0001f && slidingAxis[1] < 0.0001f) {
				angle = (float)PI / 2.0f;
			}

			t.translation = d1;
			t.rotation.fromEuler(0.0f, 0.0f, angle);
			glMultMatrix(t);

			angle = -asinf(slidingAxis[2] / slidingAxis.length());
			t.translation = ::Vector3(0.0f, 0.0f, 0.0f);
			t.rotation.fromEuler(0.0f, angle, 0.0f);
			glMultMatrix(t);

			drawMarker(&BumperMarker01);

			/*draw second marker*/
			t.translation = ::Vector3(minDistance < maxDistance ? (d2 - d1).length() : 0.0f, 0.0f, 0.0f);
			t.rotation.fromEuler(0.0f, 0.0f, (float)PI);
			glMultMatrix(t);

			drawMarker(&BumperMarker01);
			glPopMatrix();

			/* draw Pivot B */
			glPushMatrix();
			glMultMatrix(tBodies.value(1));

			if (!Node::SELECTING)
				glColor(color_b);

			glBegin(GL_POINTS); glVertex(pivotB); glEnd();
			glPopMatrix();
		}
	}

	glPopAttrib();
	glPopMatrix();
}

void Node::drawHavok()
{
	if ( !(scene->selMode & Scene::SelObject) )
		return;

	// TODO: Why are all these here - "drawNodes", "drawFurn", "drawHavok"?
	// Idea: Make them go to their own classes in different cpp files
	for ( Node * node : children.list() ) {
		node->drawHavok();
	}

	const NifModel * nif = static_cast<const NifModel *>( iBlock.model() );

	if ( !( iBlock.isValid() && nif ) )
		return;

	auto node = DynamicCast<NiNode>(nif->getBlock(iBlock));
	if (node == nullptr)
		return;

	if (node->IsSameType(BSMultiBoundNode::TYPE))
	{
		auto mbbnode = DynamicCast<BSMultiBoundNode>(node);
		auto mbbcontainer = mbbnode->GetMultiBound();

		if (nullptr != mbbcontainer && nullptr != mbbcontainer->GetData())
		{

			auto data = mbbcontainer->GetData();

			::Vector3 a, b;

			glPushMatrix();
			glLoadMatrix(scene->view);
			glMultMatrix(worldTrans());

			if (data->IsSameType(BSMultiBoundAABB::TYPE))
			{
				auto aabb = DynamicCast<BSMultiBoundAABB>(data);
				auto pos = ::Vector3(aabb->GetPosition());
				auto extent = ::Vector3(aabb->GetExtent());

				a = pos + extent;
				b = pos - extent;
			}

			if (data->IsSameType(BSMultiBoundOBB::TYPE))
			{
				auto obb = DynamicCast<BSMultiBoundOBB>(data);
				auto center = ::Vector3(obb->GetCenter());
				auto size = ::Vector3(obb->GetSize());
				auto matrix = ::Matrix(obb->GetRotation());

				a = size;
				b = -size;

				Transform t;
				t.rotation = matrix;
				t.translation = center;
				glMultMatrix(t);
			}

			if (Node::SELECTING) {
				int s_nodeId = ID2COLORKEY(nif->getBlockNumber(iBlock));
				glColor4ubv((GLubyte*)&s_nodeId);
				glLineWidth(5);
			}
			else {
				glColor(::Color4(1.0f, 1.0f, 1.0f, 0.6f));
				glDisable(GL_LIGHTING);
				glLineWidth(1.0f);
			}

			drawBox(a, b);
			glPopMatrix();
		}
	}

	/*OBSOLETE ?
	// Draw BSBound dimensions
	QModelIndex iExtraDataList = nif->getIndex( iBlock, "Extra Data List" );

	if ( iExtraDataList.isValid() ) {
		for ( int d = 0; d < nif->rowCount( iExtraDataList ); d++ ) {
			QModelIndex iBound = nif->getBlock( nif->getLink( iExtraDataList.child( d, 0 ) ), "BSBound" );

			if ( !iBound.isValid() )
				continue;

			::Vector3 center = nif->get<Vector3>( iBound, "Center" );
			::Vector3 dim = nif->get<Vector3>( iBound, "Dimensions" );

			glPushMatrix();
			glLoadMatrix( scene->view );
			// Not sure if world transform is taken into account
			glMultMatrix( worldTrans() );

			if ( Node::SELECTING ) {
				int s_nodeId = ID2COLORKEY( nif->getBlockNumber( iBound ) );
				glColor4ubv( (GLubyte *)&s_nodeId );
			} else {
				glColor( Color3( 1.0f, 0.0f, 0.0f ) );
				glDisable( GL_LIGHTING );
			}

			glLineWidth( 1.0f );
			drawBox( dim + center, -dim + center );

			glPopMatrix();
		}
	}
	*/

	if (nullptr == node->GetCollisionObject())
		return;

	auto body = DynamicCast<bhkRigidBody>(node->GetCollisionObject());
	if (nullptr == body)
		return;

	glPushMatrix();
	glLoadMatrix( scene->view );
	glMultMatrix( scene->bhkBodyTrans.value( nif->getBlockNumber(nif->getBlockIndex(StaticCast<NiObject>(body)))));


	//qDebug() << "draw obj" << nif->getBlockNumber( iObject ) << nif->itemName( iObject );

	if ( !Node::SELECTING ) {
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
		glDepthFunc( GL_LEQUAL );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_NORMALIZE );
		glDisable( GL_LIGHTING );
		glDisable( GL_COLOR_MATERIAL );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDisable( GL_ALPHA_TEST );
	}

	glPointSize( 4.5 );
	glLineWidth( 1.0 );

	static const float colors[8][3] = {
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 1.0f },
		{ 1.0f, 0.8f, 0.0f },
		{ 1.0f, 0.8f, 0.4f },
		{ 0.0f, 1.0f, 1.0f }
	};

	int color_index = body->GetHavokFilter().layer_sk & 7; // nif->get<int>(iBody, "Layer") & 7;
	glColor3fv( colors[ color_index ] );

	auto bodyIndex = nif->getBlockIndex(StaticCast<NiObject>(body));
	auto shapeIndex = nif->getBlockIndex(StaticCast<NiObject>(body->GetShape()));

	if ( !Node::SELECTING ) {
		if ( scene->currentBlock == shapeIndex) {
			// fix: add selected visual to havok meshes
			glHighlightColor(); // TODO: idea: I do not recommend mimicking the Open GL API
			                    // It confuses the one who reads the code. And the Open GL API is
			                    // in constant development.
			glLineWidth( 2.5 );
			//glPointSize( 8.5 );
		}
	}

	QStack<QModelIndex> shapeStack;

	if ( Node::SELECTING )
		glLineWidth( 5 ); // make selection click a little more easy

	drawHvkShape( nif, shapeIndex, shapeStack, scene, colors[ color_index ] );

	// Scale up for Skyrim
	float havokScale = (nif->checkVersion( 0x14020007, 0x14020007 ) && nif->getUserVersion() >= 12) ? 10.0f : 1.0f;

	if ( Node::SELECTING && scene->options & Scene::ShowAxes ) {
		int s_nodeId = ID2COLORKEY( nif->getBlockNumber(bodyIndex) );
		glColor4ubv( (GLubyte *)&s_nodeId );
		glDepthFunc( GL_ALWAYS );
		drawAxes( ::Vector3( body->GetCenter() ) * havokScale, 1.0f, false );
		glDepthFunc( GL_LEQUAL );
	} else if ( scene->options & Scene::ShowAxes ) {
		drawAxes( ::Vector3( body->GetCenter() ) * havokScale, 1.0f );
	}

	glPopMatrix();

	for ( const auto constraint : body->GetConstraints()) {
		QModelIndex iConstraint = nif->getBlockIndex(StaticCast<NiObject>(constraint));

		if (constraint->IsDerivedType(bhkConstraint::TYPE ) )
			drawHvkConstraint( nif, iConstraint, scene );
	}
}

//This is dangerous, havok animatios is the real master
//void drawFurnitureMarker( const NifModel * nif, const QModelIndex & iPosition )
//{
//	::Vector3 offs = nif->get<::Vector3>( iPosition, "Offset" );
//	quint16 orient = nif->get<quint16>( iPosition, "Orientation" );
//	quint8 ref1 = nif->get<quint8>( iPosition, "Position Ref 1" );
//	quint8 ref2 = nif->get<quint8>( iPosition, "Position Ref 2" );
//
//	const GLMarker * mark[5];
//	::Vector3 flip[5];
//	::Vector3 pos( 1, 1, 1 );
//	::Vector3 neg( -1, 1, 1 );
//
//	float xOffset = 0.0f;
//	float zOffset = 0.0f;
//	float yOffset = 0.0f;
//	float roll;
//
//	int i = 0;
//
//	if ( ref1 == 0 ) {
//		float heading = nif->get<float>( iPosition, "Heading" );
//		quint16 type = nif->get<quint16>( iPosition, "Animation Type" );
//		int entry = nif->get<int>( iPosition, "Entry Properties" );
//
//		if ( type == 0 ) return;
//
//		// Sit=1, Sleep=2, Lean=3
//		// Front=1, Behind=2, Right=4, Left=8, Up=16(0x10)
//
//		switch ( type ) {
//		case 1:
//			// Sit Type
//
//			zOffset = -34.00f;
//
//			if ( entry & 0x1 ) {
//				// Chair Front
//				flip[i] = pos;
//				mark[i] = &ChairFront;
//				i++;
//			}
//			if ( entry & 0x2 ) {
//				// Chair Behind
//				flip[i] = pos;
//				mark[i] = &ChairBehind;
//				i++;
//			}
//			if ( entry & 0x4 ) {
//				// Chair Right
//				flip[i] = neg;
//				mark[i] = &ChairLeft;
//				i++;
//			}
//			if ( entry & 0x8 ) {
//				// Chair Left
//				flip[i] = pos;
//				mark[i] = &ChairLeft;
//				i++;
//			}
//			break;
//		case 2:
//			// Sleep Type
//
//			zOffset = -34.00f;
//
//			if ( entry & 0x1 ) {
//				// Bed Front
//				//flip[i] = pos;
//				//mark[i] = &FurnitureMarker03;
//				//i++;
//			}
//			if ( entry & 0x2 ) {
//				// Bed Behind
//				//flip[i] = pos;
//				//mark[i] = &FurnitureMarker04;
//				//i++;
//			}
//			if ( entry & 0x4 ) {
//				// Bed Right
//				flip[i] = neg;
//				mark[i] = &BedLeft;
//				i++;
//			}
//			if ( entry & 0x8 ) {
//				// Bed Left
//				flip[i] = pos;
//				mark[i] = &BedLeft;
//				i++;
//			}
//			if ( entry & 0x10 ) {
//				// Bed Up????
//				// This is sometimes used as a real bed position
//				// Other times it is a dummy
//				flip[i] = neg;
//				mark[i] = &BedLeft;
//				i++;
//			}
//			break;
//		case 3:
//			break;
//		default:
//			break;
//		}
//
//		roll = heading;
//	} else {
//		if ( ref1 != ref2 ) {
//			qDebug() << "Position Ref 1 and 2 are not equal";
//			return;
//		}
//
//		switch ( ref1 ) {
//		case 1:
//			mark[0] = &FurnitureMarker01; // Single Bed
//			break;
//
//		case 2:
//			flip[0] = neg;
//			mark[0] = &FurnitureMarker01;
//			break;
//
//		case 3:
//			mark[0] = &FurnitureMarker03; // Ground Bed?
//			break;
//
//		case 4:
//			mark[0] = &FurnitureMarker04; // Ground Bed? Behind
//			break;
//
//		case 11:
//			mark[0] = &FurnitureMarker11; // Chair Left
//			break;
//
//		case 12:
//			flip[0] = neg;
//			mark[0] = &FurnitureMarker11;
//			break;
//
//		case 13:
//			mark[0] = &FurnitureMarker13; // Chair Behind
//			break;
//
//		case 14:
//			mark[0] = &FurnitureMarker14; // Chair Front
//			break;
//
//		default:
//			qDebug() << "Unknown furniture marker " << ref1;
//			return;
//		}
//
//		i = 1;
//
//		// TODO: FIX: This makes no sense
//		roll = float( orient ) / 6284.0 * 2.0 * (-M_PI);
//	}
//
//	if ( Node::SELECTING ) {
//		GLint id = ( nif->getBlockNumber( iPosition ) & 0xffff ) | ( ( iPosition.row() & 0xffff ) << 16 );
//		int s_nodeId = ID2COLORKEY( id );
//		glColor4ubv( (GLubyte *)&s_nodeId );
//	}
//
//	for ( int n = 0; n < i; n++ ) {
//		glPushMatrix();
//
//		Transform t;
//		t.rotation.fromEuler( 0, 0, roll );
//		t.translation = offs;
//		t.translation[0] += xOffset;
//		t.translation[1] += yOffset;
//		t.translation[2] += zOffset;
//
//		glMultMatrix( t );
//
//		glScale( flip[n] );
//
//		drawMarker( mark[n] );
//
//		glPopMatrix();
//	}
//}
//
//void Node::drawFurn()
//{
//	for ( Node * node : children.list() ) {
//		node->drawFurn();
//	}
//
//	const NifModel * nif = static_cast<const NifModel *>( iBlock.model() );
//
//	if ( !( iBlock.isValid() && nif ) )
//		return;
//
//	if ( !(scene->selMode & Scene::SelObject) )
//		return;
//
//	QModelIndex iExtraDataList = nif->getIndex( iBlock, "Extra Data List" );
//
//	if ( !iExtraDataList.isValid() )
//		return;
//
//	if ( !Node::SELECTING ) {
//		glEnable( GL_DEPTH_TEST );
//		glDepthMask( GL_FALSE );
//		glDepthFunc( GL_LEQUAL );
//		glDisable( GL_TEXTURE_2D );
//		glDisable( GL_NORMALIZE );
//		glDisable( GL_LIGHTING );
//		glDisable( GL_COLOR_MATERIAL );
//		glDisable( GL_CULL_FACE );
//		glDisable( GL_BLEND );
//		glDisable( GL_ALPHA_TEST );
//		glColor4f( 1, 1, 1, 1 );
//		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
//	}
//
//	glLineWidth( 1.0 );
//
//	glPushMatrix();
//
//	glMultMatrix( viewTrans() );
//
//	for ( int p = 0; p < nif->rowCount( iExtraDataList ); p++ ) {
//		// DONE: never seen Furn in nifs, so there may be a need of a fix here later - saw one, fixed a bug
//		QModelIndex iFurnMark = nif->getBlock( nif->getLink( iExtraDataList.child( p, 0 ) ), "BSFurnitureMarker" );
//
//		if ( !iFurnMark.isValid() )
//			continue;
//
//		QModelIndex iPositions = nif->getIndex( iFurnMark, "Positions" );
//
//		if ( !iPositions.isValid() )
//			break;
//
//		for ( int j = 0; j < nif->rowCount( iPositions ); j++ ) {
//			QModelIndex iPosition = iPositions.child( j, 0 );
//
//			if ( scene->currentIndex == iPosition )
//				glHighlightColor();
//			else
//				glNormalColor();
//
//			drawFurnitureMarker( nif, iPosition );
//		}
//	}
//
//	glPopMatrix();
//}

void Node::drawShapes( NodeList * secondPass, bool presort )
{
	if ( isHidden() )
		return;

	const NifModel * nif = static_cast<const NifModel *>(iBlock.model());
	
	// BSOrderedNode support
	//	Only set if true (|=) so that it propagates to all children
	presort |= nif->getBlock( iBlock, "BSOrderedNode" ) != nullptr;

	presorted = presort;
	if ( presorted )
		children.sort();

	for ( Node * node : children.list() ) {
		node->drawShapes( secondPass, presort );
	}
}

#define Farg( X ) arg( X, 0, 'f', 5 )

QString trans2string( Transform t )
{
	float xr, yr, zr;
	t.rotation.toEuler( xr, yr, zr );
	return QString( "translation  X %1, Y %2, Z %3\n" ).Farg( t.translation[0] ).Farg( t.translation[1] ).Farg( t.translation[2] )
	       +   QString( "rotation     Y %1, P %2, R %3  " ).Farg( xr * 180 / PI ).Farg( yr * 180 / PI ).Farg( zr * 180 / PI )
	       +   QString( "( (%1, %2, %3), " ).Farg( t.rotation( 0, 0 ) ).Farg( t.rotation( 0, 1 ) ).Farg( t.rotation( 0, 2 ) )
	       +   QString( "(%1, %2, %3), " ).Farg( t.rotation( 1, 0 ) ).Farg( t.rotation( 1, 1 ) ).Farg( t.rotation( 1, 2 ) )
	       +   QString( "(%1, %2, %3) )\n" ).Farg( t.rotation( 2, 0 ) ).Farg( t.rotation( 2, 1 ) ).Farg( t.rotation( 2, 2 ) )
	       +   QString( "scale        %1\n" ).Farg( t.scale );
}

QString Node::textStats() const
{
	return QString( "%1\n\nglobal\n%2\nlocal\n%3\n" ).arg( name, trans2string( worldTrans() ), trans2string( localTrans() ) );
}

BoundSphere Node::bounds() const
{
	BoundSphere boundsphere;

	auto opts = scene->options;

	// the node itself
	if ( (opts & Scene::ShowNodes) || (opts & Scene::ShowCollision) ) {
		boundsphere |= BoundSphere( worldTrans().translation, 0 );
	}

	const NifModel * nif = static_cast<const NifModel *>( iBlock.model() );

	if ( !( iBlock.isValid() && nif ) )
		return boundsphere;

	// old style collision bounding box
	//auto node = DynamicCast<NiNode>(nif->getBlock(iBlock));
	//if (node && node->GetHasBoundingVolume() == true ) {
	//	auto bb = node->GetBoundingVolume();
	//	QModelIndex iBox = nif->getIndex( iBlock, "Bounding Box" );
	//	::Vector3 trans = bb.;
	//	::Vector3 rad = nif->get<Vector3>( iBox, "Radius" );
	//	boundsphere |= BoundSphere( trans, rad.length() );
	//}

	//if ( nif->getBlockType( iBlock ) == "NiMesh" ) {
	//	auto iBound = nif->getIndex( iBlock, "Bound" );
	//	if ( iBound.isValid() ) {
	//		auto center = nif->get<Vector3>( iBound, "Center" );
	//		auto radius = nif->get<float>( iBound, "Radius" );
	//		boundsphere |= BoundSphere( center, radius );
	//	}
	//}

	// BSBound collision bounding box
	auto node = DynamicCast<NiNode>(nif->getBlock(iBlock));
	auto& extraDataList = node->GetExtraDataList();
	//QModelIndex iExtraDataList = nif->getIndex( iBlock, "Extra Data List" );

	//if ( iExtraDataList.isValid() ) {
		for ( int d = 0; d < extraDataList.size(); d++ ) {
			auto ed = extraDataList[d];
			if (ed->IsSameType(BSBound::TYPE))
			{
				auto bb = DynamicCast<BSBound>(ed);
				if (bb == nullptr)
					continue;
				::Vector3 center(bb->GetCenter());
				::Vector3 dim(bb->GetDimensions());
				boundsphere |= BoundSphere(center, dim.length());
			}
		}
	//}

	return boundsphere;
}


LODNode::LODNode( Scene * scene, const QModelIndex & iBlock )
	: Node( scene, iBlock )
{
}

void LODNode::clear()
{
	Node::clear();
	ranges.clear();
}

void LODNode::update( const NifModel * nif, const QModelIndex & index )
{
	//Node::update( nif, index );

	//if ( ( iBlock.isValid() && index == iBlock ) || ( iData.isValid() && index == iData ) ) {
	//	ranges.clear();
	//	iData = nif->getBlock( nif->getLink( iBlock, "LOD Level Data" ), "NiRangeLODData" );
	//	QModelIndex iLevels;

	//	//if ( iData.isValid() ) {
	//	//	center  = nif->get<Vector3>( iData, "LOD Center" );
	//	//	iLevels = nif->getIndex( iData, "LOD Levels" );
	//	//} else {
	//	//	center  = nif->get<Vector3>( iBlock, "LOD Center" );
	//	//	iLevels = nif->getIndex( iBlock, "LOD Levels" );
	//	//}

	//	if ( iLevels.isValid() ) {
	//		for ( int r = 0; r < nif->rowCount( iLevels ); r++ ) {
	//			ranges.append( { nif->get<float>( iLevels.child( r, 0 ), "Near Extent" ),
	//			                 nif->get<float>( iLevels.child( r, 0 ), "Far Extent" ) }
	//			);
	//		}
	//	}


	//}
}

void LODNode::transform()
{
	Node::transform();

	if ( children.list().isEmpty() )
		return;

	if ( ranges.isEmpty() ) {
		for ( Node * child : children.list() ) {
			child->flags.node.hidden = true;
		}
		children.list().first()->flags.node.hidden = false;
		return;
	}

	float distance = ( viewTrans() * center ).length();

	int c = 0;
	for ( Node * child : children.list() ) {
		if ( c < ranges.count() )
			child->flags.node.hidden = !( ranges[c].first <= distance && distance < ranges[c].second );
		else
			child->flags.node.hidden = true;

		c++;
	}
}


BillboardNode::BillboardNode( Scene * scene, const QModelIndex & iBlock )
	: Node( scene, iBlock )
{
}

const Transform & BillboardNode::viewTrans() const
{
	if ( scene->viewTrans.contains( nodeId ) )
		return scene->viewTrans[ nodeId ];

	Transform t;

	if ( parent )
		t = parent->viewTrans() * local;
	else
		t = scene->view * worldTrans();

	t.rotation = Matrix();

	scene->viewTrans.insert( nodeId, t );
	return scene->viewTrans[ nodeId ];
}
