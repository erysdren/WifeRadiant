/*
   Copyright (C) 2001-2006, William Joseph.
   All Rights Reserved.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "iqm.h"
#include "iqmformat.h"

#include "ifilesystem.h"
#include "imodel.h"

#include "imagelib.h"
#include "bytestreamutils.h"

#include "model.h"
#include "ident.h"

template<typename ReturnType, typename DataType>
static ReturnType IQM_GetVec3(iqm_header_t* iqm, iqm_vertex_array_t* vertexArray, uint32_t elementIndex) {
	constexpr uint32_t typeSize = sizeof(DataType);
	const DataType* data = reinterpret_cast<DataType*>(reinterpret_cast<uint8_t*>(iqm) + vertexArray->offset + ((typeSize * vertexArray->size) * elementIndex));
	return ReturnType( data[0], data[1], data[2] );
}

template<typename ReturnType, typename DataType>
static ReturnType IQM_GetVec2(iqm_header_t* iqm, iqm_vertex_array_t* vertexArray, uint32_t elementIndex) {
	constexpr uint32_t typeSize = sizeof(DataType);
	const DataType* data = reinterpret_cast<DataType*>(reinterpret_cast<uint8_t*>(iqm) + vertexArray->offset + ((typeSize * vertexArray->size) * elementIndex));
	return ReturnType( data[0], data[1] );
}

scene::Node& IQMModel_fromBuffer( unsigned char* buffer, int bufferLength ){
	auto modelNode = new ModelNode();

	iqm_header_t* iqm = (iqm_header_t*)buffer;

	// check magic
	if (memcmp(iqm->magic, IQM_MAGIC, IQM_MAGIC_SIZE) != 0) {
		globalErrorStream() << "IQM magic identifier not found" << '\n';
		Model_constructNull( modelNode->model() );
		return modelNode->node();
	}

	// check version
	if (iqm->version != IQM_VERSION) {
		globalErrorStream() << "Invalid IQM version" << iqm->version << " found" << '\n';
		Model_constructNull( modelNode->model() );
		return modelNode->node();
	}

	// getter functions for vertex arrays
	auto getVec2 = [&iqm]<typename T>(uint32_t vertexArrayType, uint32_t elementIndex) {
		iqm_vertex_array_t* vertexArrays = IQM_GET_VERTEX_ARRAYS(iqm);
		if (iqm->num_vertex_arrays > vertexArrayType) {
			iqm_vertex_array_t* vertexArray = vertexArrays + vertexArrayType;
			switch (vertexArray->format) {
				case IQM_VERTEX_ARRAY_FORMAT_S8: return IQM_GetVec2<T, int8_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_U8: return IQM_GetVec2<T, uint8_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_S16: return IQM_GetVec2<T, int16_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_U16: return IQM_GetVec2<T, uint16_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_S32: return IQM_GetVec2<T, int32_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_U32: return IQM_GetVec2<T, uint32_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_F16: return IQM_GetVec2<T, uint16_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_F32: return IQM_GetVec2<T, float>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_F64: return IQM_GetVec2<T, double>(iqm, vertexArray, elementIndex);
				default: break;
			}
		}
		return T(0.0f, 0.0f);
	};

	auto getVec3 = [&iqm]<typename T>(uint32_t vertexArrayType, uint32_t elementIndex) {
		iqm_vertex_array_t* vertexArrays = IQM_GET_VERTEX_ARRAYS(iqm);
		if (iqm->num_vertex_arrays > vertexArrayType) {
			iqm_vertex_array_t* vertexArray = vertexArrays + vertexArrayType;
			switch (vertexArray->format) {
				case IQM_VERTEX_ARRAY_FORMAT_S8: return IQM_GetVec3<T, int8_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_U8: return IQM_GetVec3<T, uint8_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_S16: return IQM_GetVec3<T, int16_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_U16: return IQM_GetVec3<T, uint16_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_S32: return IQM_GetVec3<T, int32_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_U32: return IQM_GetVec3<T, uint32_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_F16: return IQM_GetVec3<T, uint16_t>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_F32: return IQM_GetVec3<T, float>(iqm, vertexArray, elementIndex);
				case IQM_VERTEX_ARRAY_FORMAT_F64: return IQM_GetVec3<T, double>(iqm, vertexArray, elementIndex);
				default: break;
			}
		}
		return T(0.0f, 0.0f, 0.0f);
	};

	// parse meshes
	Model& model = modelNode->model();
	iqm_mesh_t* meshes = IQM_GET_MESHES(iqm);
	iqm_triangle_t* triangles = IQM_GET_TRIANGLES(iqm);
	for (uint32_t i = 0; i < iqm->num_meshes; i++) {
		Surface& surface = model.newSurface();

		// read vertices
		for (uint32_t j = 0; j < meshes[i].num_vertices; j++) {
			surface.vertices().push_back(
				ArbitraryMeshVertex(
					getVec3.template operator()<Vertex3f>(IQM_VERTEX_ARRAY_TYPE_POSITION, meshes[i].first_vertex + j),
					getVec3.template operator()<Normal3f>(IQM_VERTEX_ARRAY_TYPE_NORMAL, meshes[i].first_vertex + j),
					getVec2.template operator()<TexCoord2f>(IQM_VERTEX_ARRAY_TYPE_TEXCOORD, meshes[i].first_vertex + j)
				)
			);
		}

		// read triangles
		for (uint32_t j = 0; j < meshes[i].num_triangles; j++) {
			iqm_triangle_t& triangle = triangles[meshes[i].first_triangle + j];
			surface.indices().insert(triangle.vertices[0] - meshes[i].first_vertex);
			surface.indices().insert(triangle.vertices[1] - meshes[i].first_vertex);
			surface.indices().insert(triangle.vertices[2] - meshes[i].first_vertex);
		}

		// set shader
		surface.setShader( IQM_GET_TEXT(iqm) + meshes[i].material );

		// update aabb
		surface.updateAABB();
		model.updateAABB();
	}

	return modelNode->node();
}

scene::Node& loadIQM( ArchiveFile& file ){
	ScopedArchiveBuffer buffer( file );
	return IQMModel_fromBuffer( buffer.buffer, buffer.length );
}
