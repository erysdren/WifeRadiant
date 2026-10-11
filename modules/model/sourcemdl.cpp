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

#ifndef NO_SOURCEMDL

#include "sourcemdl.h"

#include "ifilesystem.h"
#include "imodel.h"

#include "imagelib.h"
#include "bytestreamutils.h"

#include "model.h"

#include <mdlpp/mdlpp.h>

static std::string findMaterialPath( std::vector<std::string>& paths, std::string& name ) {
	std::string material = "";
	for ( auto path : paths ) {
		std::string filename = std::format("materials/{}{}.vmt", path, name);
		for ( char& c : filename ) {
			if ( c == '\\' ) {
				c = '/';
			} else {
				c = std::tolower( c );
			}
		}
		ArchiveFile* file = GlobalFileSystem().openFile( filename.c_str() );
		if ( file ) {
			material = std::format("materials/{}{}", path, name);
			for ( char& c : material ) {
				if ( c == '\\' ) {
					c = '/';
				} else {
					c = std::tolower( c );
				}
			}
			file->release();
			break;
		}
	}
	return material;
}

static void processMesh(mdlpp::StudioModel& mdl, mdlpp::BakedModel& baked, mdlpp::BakedModel::Mesh& mesh, Model& model) {

	if ( !mesh.indices.size() || !baked.vertices.size() ) {
		return;
	}

	Surface& surface = model.newSurface();

	// read in geometry
	std::map<size_t, size_t> indicesMap;
	for ( size_t i = 0; i < mesh.indices.size(); i += 3 ) {
		bool faceOk = true;
		for ( size_t j = 2; j != (size_t)-1; j-- ) {
			if ( mesh.indices[i + j] >= baked.vertices.size() ) {
				faceOk = false;
				break;
			}
		}
		if ( faceOk ) {
			for ( size_t j = 2; j != (size_t)-1; j-- ) {
				if ( !indicesMap.contains( mesh.indices[i + j] ) ) {
					indicesMap[mesh.indices[i + j]] = surface.vertices().size();
					auto& vertex = baked.vertices[mesh.indices[i + j]];
					surface.vertices().push_back(
						ArbitraryMeshVertex(
							Vertex3f( vertex.position[0], vertex.position[1], vertex.position[2] ),
							Normal3f( vertex.normal[0], vertex.normal[1], vertex.normal[2] ),
							TexCoord2f( vertex.uv[0], vertex.uv[1] )
						)
					);
				}
				surface.indices().insert( indicesMap[mesh.indices[i + j]] );
			}
		}
	}

	// material
	bool foundShader = false;
	if ( mesh.materialIndex >= 0 ) {
		std::string material = findMaterialPath( mdl.mdl.materialDirectories, mdl.mdl.materials[mesh.materialIndex].name );
		if ( material.length() > 0 ) {
			surface.setShader( material.c_str() );
			foundShader = true;
		}
	}
	if ( !foundShader ) {
		surface.setShader( "nomodel" );
	}

	// update aabb
	surface.updateAABB();
	model.updateAABB();
}

#if !defined(MAX_PATH) && defined(PATH_MAX)
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH 260
#endif

scene::Node& loadSourceMDL( unsigned char* mdlBuffer, int mdlBufferLength, const char* mdlFilename ) {
	auto modelNode = new ModelNode();

	char vtxFilename[MAX_PATH];
	char vvdFilename[MAX_PATH];

	// glean base filename
	bool appendExtOnly = false;
	const char *mdlFilenameExtPtr = NULL;
	size_t mdlFilenameExtPos = 0;
	if ( !( mdlFilenameExtPtr = std::strrchr( mdlFilename, '.' ) ) ) {
		appendExtOnly = true;
	} else {
		mdlFilenameExtPos = mdlFilenameExtPtr - mdlFilename;
	}

	// find a vtx file
	// these suffixes are ordered in terms of priority
	const char *vtxSuffixes[4] = {
		".dx90.vtx",
		".dx80.vtx",
		".vtx",
		".sw.vtx"
	};
	ArchiveFile *vtxFile = NULL;
	for ( size_t i = 0; i < 4; i++ ) {
		if ( appendExtOnly ) {
			std::snprintf( vtxFilename, sizeof(vtxFilename), "%s%s", mdlFilename, vtxSuffixes[i] );
		} else {
			std::strncpy( vtxFilename, mdlFilename, sizeof(vtxFilename) );
			std::strncpy( vtxFilename + mdlFilenameExtPos, vtxSuffixes[i], sizeof(vtxFilename) - mdlFilenameExtPos );
		}
		vtxFile = GlobalFileSystem().openFile( vtxFilename );
		if ( vtxFile ) {
			break;
		}
	}
	if ( !vtxFile ) {
		Model_constructNull( modelNode->model() );
		return modelNode->node();
	}

	// find the vvd file
	if ( appendExtOnly ) {
		std::snprintf( vvdFilename, sizeof(vvdFilename), "%s%s", mdlFilename, ".vvd" );
	} else {
		std::strncpy( vvdFilename, mdlFilename, sizeof(vvdFilename) );
		std::strncpy( vvdFilename + mdlFilenameExtPos, ".vvd", sizeof(vvdFilename) - mdlFilenameExtPos );
	}
	ArchiveFile *vvdFile = GlobalFileSystem().openFile( vvdFilename );
	if ( !vvdFile ) {
		vtxFile->release();
		Model_constructNull( modelNode->model() );
		return modelNode->node();
	}

	// get data
	ScopedArchiveBuffer vtxBuffer( *vtxFile );
	ScopedArchiveBuffer vvdBuffer( *vvdFile );

	// parse data
	mdlpp::StudioModel mdl;
	if ( !mdl.open( mdlBuffer, mdlBufferLength, vtxBuffer.buffer, vtxBuffer.length, vvdBuffer.buffer, vvdBuffer.length ) ) {
		vtxFile->release();
		vvdFile->release();
		Model_constructNull( modelNode->model() );
		return modelNode->node();
	}

	// clean up
	vtxFile->release();
	vvdFile->release();

#if 0
	// set transform
	for (const auto& bone : mdl.mdl.bones) {
		if (bone.parent == -1) {
			modelNode->model().setLocalToWorld(matrix4_rotation_for_quaternion(Quaternion(bone.rotationQuat[0], bone.rotationQuat[1], bone.rotationQuat[2], bone.rotationQuat[3])));
			break;
		}
	}
#endif

	// bake and read in
	mdlpp::BakedModel baked = mdl.processModelData();
	if ( !baked.meshes.size() ) {
		Model_constructNull( modelNode->model() );
		return modelNode->node();
	}
	for ( size_t i = 0; i < baked.meshes.size(); i++ ) {
		processMesh( mdl, baked, baked.meshes[i], modelNode->model() );
	}

	return modelNode->node();
}

#endif // NO_SOURCEMDL
