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
#include "entitylib.h"

#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>

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
	mutable size_t m_brushCount = 0;

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
		mutable size_t m_faceCount = 0;
	public:
		ObjBrushVisitor(const ObjWalker& objWalker) : m_objWalker(objWalker) {
			m_objWalker.writeString(std::format("# brush {}\n", ++m_objWalker.m_brushCount));
		}

		~ObjBrushVisitor() {
			m_objWalker.writeString("\n");
		}

		virtual void visit(Face& face) const override {
			const auto& winding = face.getWinding();

			bool weld = true;

			// faces
			std::string faceLine = "f";

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
				// write texcoords
				m_objWalker.writeString(std::format("vt {} {}\n", winding[i].texcoord.x(), winding[i].texcoord.y()));
				faceLine += std::format(" {}/{}", vertexN, m_objWalker.m_texcoordCount); // store faces
			}
			while( i != 0 );

			faceLine += "\n";
			m_objWalker.writeString(faceLine);
		}
	};

	void writeBrush(Brush* brush) const {
		brush->forEachFace(ObjBrushVisitor(*this));
	}

	void writeEntity(Entity* entity) const {

	}

	ObjWalker(FileOutputStream& outputStream) : m_outputStream(outputStream) {
		writeString(std::format("# exported by WifeRadiant version {}\n\n", RADIANT_GIT_REVISION));
	}

	virtual bool pre(scene::Node& node) const override {
		if (Node_isBrush(node)) {
			writeBrush(Node_getBrush(node));
		} else if (Node_isEntity(node)) {
			writeEntity(Node_getEntity(node));
		}
		return true;
	}
};

class ObjWindow : public QWidget {
	scene::Node& m_root;
	GraphTraversalFunc m_traverse;
	FileOutputStream& m_outputStream;
public:
	void doExport() const {
		m_traverse(m_root, ObjWalker(m_outputStream));
	}

	ObjWindow(scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream) :
		QWidget(GlobalRadiant().getMainWindow(), Qt::Dialog | Qt::WindowCloseButtonHint),
		m_root(root),
		m_traverse(traverse),
		m_outputStream(outputStream)
	{
		this->setWindowModality(Qt::ApplicationModal);
		this->setWindowTitle( "Wavefront OBJ Export Options" );

		// setup gui
		{
			auto* grid = new QGridLayout( this );
			// first row
			{
				{
					auto* label = new QLabel( "Options:" );
					grid->addWidget( label, 0, 0 );
				}
				{
					auto *container = new QWidget;
					grid->addWidget( container, 0, 1, 2, 2 );

					auto *vbox = new QVBoxLayout( container );
					vbox->setContentsMargins( 0, 0, 0, 0 );
				}
			}
			// second row
			{
				{
					auto* button = new QPushButton( "Continue" );
					grid->addWidget( button, 1, 0 );
					QObject::connect( button, &QAbstractButton::clicked, this, &ObjWindow::doExport );
				}
				{
					auto* button = new QPushButton( "Cancel" );
					grid->addWidget( button, 1, 1 );
					QObject::connect( button, &QAbstractButton::clicked, this, &QWidget::hide );
				}
			}
		}
	}

	~ObjWindow() = default;
};

static ObjWindow* g_window = nullptr;

void writeOBJ(scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream) {
	if (g_window == nullptr) {
		g_window = new ObjWindow(root, traverse, outputStream);
	}
	g_window->show();
}
