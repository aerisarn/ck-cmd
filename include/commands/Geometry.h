#pragma once

#include <memory>

#include <obj\NiGeometryData.h>
#include <obj\NiTriShape.h>

#include <fbxsdk.h>

namespace ckcmd {
namespace Geometry {

	using namespace Niflib;

	template<class From, class To> To convert(const From& mesh_in) ;
	template<class From, class To, class Container> To convert(const From& mesh_in, Container container);

	template<> FbxMesh* convert(const NiTriShape& mesh_in, FbxScene* container) {
		FbxMesh* mesh_out = FbxMesh::Create(container, mesh_in.GetName().c_str());

		//Get materials from the shape

		//move vertices
		mesh_out->InitControlPoints(mesh_in.GetData()->GetVertices().size());

		return mesh_out;
	}

}
}