/*
    Copyright (C) 2025-2026 erysdren (it/its)

    This file is part of WifeRadiant.

    WifeRadiant is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    WifeRadiant is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with WifeRadiant.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <format>

#include "plugin.h"

#include "../../radiant/brush.h" // FIXME: evil
#include "stream/filestream.h"
#include "isceneexport.h"

#include "obj.h"

inline Brush* Node_getBrush(scene::Node& node){
	return NodeTypeCast<Brush>::cast(node);
}

class ObjWalker : public scene::Traversable::Walker {
private:
public:
	FileOutputStream& m_outputStream;
	mutable unsigned int m_vertexCount = 0;
	mutable unsigned int m_texcoordCount = 0;

	void writeString(const char* buffer, size_t len) const {
		m_outputStream.write(reinterpret_cast<const FileOutputStream::byte_type*>(buffer), len);
	}

	void writeString(const char* s) const {
		writeString(s, strlen(s));
	}

	void writeString(std::string s) const {
		writeString(s.data(), s.size());
	}

	class ObjBrushVisitor : public BrushVisitor {
	private:
		const ObjWalker& m_objWalker;
		mutable std::vector<DoubleVector3> m_vertices{};
	public:
		ObjBrushVisitor(const ObjWalker& objWalker) : m_objWalker(objWalker) {

		}

		virtual void visit(Face& face) const override {
			const auto& winding = face.getWinding();

			bool weld = true;

			// write vertices
			size_t i = winding.numpoints;
			do{
				--i;
				++m_objWalker.m_texcoordCount;
				std::size_t vertexN = 0; // vertex index to use, 0 is special value = no vertex to weld to found
				const DoubleVector3& vertex = winding[i].vertex;
				if( weld ){
					auto found = std::ranges::find_if( m_vertices, [&vertex]( const DoubleVector3& othervertex ){
						return Edge_isDegenerate( vertex, othervertex );
					} );
					if( found == m_vertices.end() ){ // unique vertex, add to the list
						m_vertices.emplace_back( vertex );
					}
					else{
						vertexN = m_objWalker.m_vertexCount - std::distance( found, m_vertices.end() ) + 1; // reuse existing index
					}
				}
				// write vertices
				if( vertexN == 0 ){
					vertexN = ++m_objWalker.m_vertexCount;
					m_objWalker.writeString(std::format("v {} {} {}\n", vertex.x(), vertex.z(), -vertex.y()));
				}
			}
			while( i != 0 );
		}
	};

	void writeBrush(Brush* brush) const {
		brush->forEachFace(ObjBrushVisitor(*this));
	}

	ObjWalker(FileOutputStream& outputStream) : m_outputStream(outputStream) {
		writeString(std::format("# exported by WifeRadiant version {}\n\n", RADIANT_GIT_REVISION));
	}

	virtual bool pre(scene::Node& node) const override {
		if (Node_isBrush(node)) {
			writeBrush(Node_getBrush(node));
		}
		return true;
	}
};

void writeOBJ(scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream) {
	traverse(root, ObjWalker(outputStream));
}
