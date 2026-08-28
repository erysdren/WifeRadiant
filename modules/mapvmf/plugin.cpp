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

#include "plugin.h"

#include "iscriplib.h"
#include "ibrush.h"
#include "ipatch.h"
#include "ifiletypes.h"
#include "ieclass.h"
#include "qerplugin.h"
#include "eclasslib.h"
#include "ientity.h"
#include "layers.h"
#include "script/scripttokeniser.h"

#include "scenelib.h"
#include "string/string.h"
#include "stringio.h"
#include "generic/constant.h"

#include "modulesystem/singletonmodule.h"

#include "imap.h"

#include <kvpp/kvpp.h>

#include <format>
#include <map>

// copied from plugins/mapq3/parse.cpp
class LayersParser
{
	Layers& m_layers;
	std::vector<Layer::iterator> m_layersVec; // vector to access layers by index
	std::map<int32_t, Layer*> m_visgroupidMap;
	int m_currentLayer = LAYERIDX0;
public:
	LayersParser( scene::Node& root ) : m_layers( *Node_getLayers( root ) ){
	}
	void construct_tree(){
		{
			auto& list = m_layers.m_children;

#if 0
			if( list.size() > 1 ) // something was parsed, remove default inserted layer
				list.pop_front();
#endif

			m_layersVec.reserve( list.size() );
			for( auto it = list.begin(); it != list.end(); ++it )
				m_layersVec.push_back( it );
			// check safety // note possible circular reference is not checked
			if( std::ranges::find( list, LAYERIDXPARENT, &Layer::m_parentIndex ) == list.cend() ){
				globalErrorStream() << "No layers in the root\n";
				list.front().m_parentIndex = LAYERIDXPARENT;
			}
		}
		for( auto it : m_layersVec ){
			// check safety
			if( it->m_parentIndex < LAYERIDXPARENT || it->m_parentIndex >= static_cast<int>( m_layersVec.size() ) ){
				globalErrorStream() << it->m_parentIndex << " parent layer index out of bounds\n";
				it->m_parentIndex = LAYERIDXPARENT;
			}
			if( it->m_parentIndex > LAYERIDXPARENT && m_layersVec[ it->m_parentIndex ] == it ){
				globalErrorStream() << it->m_parentIndex << " parent layer index self reference\n";
				it->m_parentIndex = LAYERIDXPARENT;
			}
			// link & reference parent
			Layer& parent = it->m_parentIndex == LAYERIDXPARENT? m_layers : *m_layersVec[ it->m_parentIndex ];
			parent.m_children.splice( parent.m_children.cend(), m_layers.m_children, it );
			it->m_parent = &parent;
		}

		m_layers.m_currentLayer = &m_layers.m_children.front();
	}
	bool read_layers( kvpp::KV1ElementReadable<std::string_view>& visgroups ){
		for ( auto& visgroup : visgroups ) {
			auto& layer = m_layers.m_children.emplace_back( visgroup["name"].getValue().data(), nullptr );
			layer.m_parentIndex = LAYERIDXPARENT;
			sscanf(visgroup["color"].getValue().data(), "%d %d %d", &layer.m_color[ 0 ], &layer.m_color[ 1 ], &layer.m_color[ 2 ]);
			m_visgroupidMap[visgroup["visgroupid"].getValue<int32_t>()] = &layer;
		}
		return true;
	}
	Layer* from_visgroupid(int32_t visgroupid) {
		return m_visgroupidMap[visgroupid];
	}
	Layer* getCurrentLayer(){
		return m_layersVec[ m_currentLayer ].operator->();
	}
	Layer* getFirstlayer(){
		return &m_layers.m_children.front();
	}
};

inline MapImporter* Node_getMapImporter( scene::Node& node ){
	return NodeTypeCast<MapImporter>::cast( node );
}

inline MapExporter* Node_getMapExporter( scene::Node& node ){
	return NodeTypeCast<MapExporter>::cast( node );
}

NodeSmartReference g_nullNode( NewNullNode() );

class MapDependencies :
	public GlobalRadiantModuleRef,
	public GlobalBrushModuleRef,
	public GlobalPatchModuleRef,
	public GlobalFiletypesModuleRef,
	public GlobalScripLibModuleRef,
	public GlobalEntityClassManagerModuleRef,
	public GlobalSceneGraphModuleRef
{
public:
	MapDependencies() :
		GlobalBrushModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "brushtypes" ) ),
		GlobalPatchModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "patchtypes" ) ),
		GlobalEntityClassManagerModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "entityclass" ) ){
	}
};

class MapVMFAPI final : public TypeSystemRef, public MapFormat
{
public:
	typedef MapFormat Type;
	STRING_CONSTANT( Name, "mapvmf" );

	MapVMFAPI(){
		GlobalFiletypesModule::getTable().addType( Type::Name, Name, filetype_t( "vmf maps", "*.vmf" ) );
		GlobalFiletypesModule::getTable().addType( Type::Name, Name, filetype_t( "vmf region", "*.reg" ) );
	}
	MapFormat* getTable(){
		return this;
	}
	class MapVMFTextInputStream : public TextInputStream
	{
		std::string m_string;
		std::size_t m_pos;
	public:
		MapVMFTextInputStream( std::string& string ) : m_string( string ), m_pos( 0 ) {
		}
		virtual std::size_t read( char* buffer, std::size_t length ) {
			for ( std::size_t i = 0; i < length; i++ ) {
				if ( m_pos >= m_string.length() ) {
					return i;
				}
				buffer[i] = m_string[m_pos++];
			}
			return length;
		}
	};
	static bool dispDef_from( kvpp::KV1 dispinfo, std::string *dispDef) {
		std::string head = "dispDef\n{\n";

		float startpos[3];
		sscanf(
			dispinfo["startposition"].getValue().data(), "[%f %f %f]",
			&startpos[0], &startpos[1], &startpos[2]);

		int power;
		sscanf(
			dispinfo["power"].getValue().data(), "%i",
			&power);

		std::string params = std::format(
			"{} ( {} {} {} )\n",
			power, startpos[0], startpos[1], startpos[2]);

		int row_len = power * power;

		float normals[row_len][row_len][3];
		float distances[row_len][row_len];
		float alphas[row_len][row_len];
		// NOTE: ignoring offsets, offset_normals & triangle_tags
		for (int row = 0; row < row_len; row++) {
			std::string row_index = std::format("row{}", row);
			// is this a valid lookup? what if the key is absent?
			StringTokeniser normals_tokeniser(dispinfo["normals"][row_index].getValue().data());
			StringTokeniser distances_tokeniser(dispinfo["distances"][row_index].getValue().data());
			StringTokeniser alphas_tokeniser(dispinfo["alphas"][row_index].getValue().data());
			for (int col = 0; col < row_len; col++) {
				const char* normal_token_0 = normals_tokeniser.getToken();
				const char* normal_token_1 = normals_tokeniser.getToken();
				const char* normal_token_2 = normals_tokeniser.getToken();
				const char* distance_token = distances_tokeniser.getToken();
				const char* alpha_token = alphas_tokeniser.getToken();

				if (!string_parse_float(normal_token_0, normals[row][col][0])
				 || !string_parse_float(normal_token_1, normals[row][col][1])
				 || !string_parse_float(normal_token_2, normals[row][col][2])
				 || !string_parse_float(distance_token, distances[row][col])
				 || !string_parse_float(alpha_token, alphas[row][col]) ) {
					return false;  // empty token; panic!
				}
			}
		}

		std::string rows = "(\n";
		for (int row = 0; row < row_len; row++) {
			rows += "(";
			for (int col = 0; col < row_len; col++) {
				rows += " " + std::format(
					"( {} {} {} {} )",
					normals[row][col][0] * distances[row][col],
					normals[row][col][1] * distances[row][col],
					normals[row][col][2] * distances[row][col],
					alphas[row][col]);
			}
			rows += " )\n";
		}

		std::string tail = ")\n}\n";

		dispDef = head + params + rows + tail;
		return true;
	}
	// TODO: kvpp::KV1 dispinfo_from(... dispDef);  // for writer
	void readGraph( scene::Node& root, TextInputStream& inputStream, EntityCreator& entityTable ) const override {

		char buffer[2048];
		size_t len = 0;
		std::string kv1Data = "";

		// FIXME: should this be hardcoded?
		GlobalBrushCreator().toggleFormat(eBrushTypeValveSource);

		while ( ( len = inputStream.read(buffer, sizeof(buffer)) ) > 0 ) {
			kv1Data.append(buffer, len);
		}

		kvpp::KV1 kv1(kv1Data);

		LayersParser layersParser( root );

		for ( auto elem : kv1 ) {
			auto key = elem.getKey();
			if (string_equal_nocase(key.data(), "versioninfo")) {
				// FIXME: do we care about any of this?
			} else if (string_equal_nocase(key.data(), "cameras")) {
				// FIXME: do we care about any of this?
			} else if (string_equal_nocase(key.data(), "cordon")) {
				// FIXME: do we care about any of this?
			} else if (string_equal_nocase(key.data(), "visgroups")) {
				layersParser.read_layers( elem );
			} else if (string_equal_nocase(key.data(), "world") || string_equal_nocase(key.data(), "entity")) {
				bool hasSolids = false;
				EntityClass* entityClass = NULL;
				for ( auto e : elem ) {
					if (string_equal_nocase(e.getKey().data(), "solid")) {
						if ( e.hasChild("side") ) {
							hasSolids = true;
						}
					}
				}
				for ( auto e : elem ) {
					if (string_equal_nocase(e.getKey().data(), "classname")) {
						entityClass = GlobalEntityClassManager().findOrInsert( e.getValue().data(), hasSolids );
					}
				}
				if ( !entityClass ) {
					globalWarningStream() << "no classname in entity" << '\n';
					continue;
				}
				scene::Node& entity( entityTable.createEntity( entityClass ) );
				entity.m_layer = nullptr;
				for ( auto e : elem ) {
					if (string_equal_nocase(e.getKey().data(), "id")) {
						// FIXME: do we need to keep track of this?
					} else if (string_equal_nocase(e.getKey().data(), "solid")) {
						if ( !e.hasChild("side") ) {
							Node_getEntity( entity )->setKeyValue( e.getKey().data(), e.getValue().data() );
							continue;
						}
						scene::Node& solid( GlobalBrushCreator().createBrush() );
						solid.m_layer = nullptr;
						for ( auto solidelem : e ) {
							if (string_equal_nocase(solidelem.getKey().data(), "side")) {

								float uvaxis[2][4];
								float scale[2];
								auto uaxis = solidelem["uaxis"];
								auto vaxis = solidelem["vaxis"];
								sscanf(uaxis.getValue().data(), "[%f %f %f %f] %f", &uvaxis[0][0], &uvaxis[0][1], &uvaxis[0][2], &uvaxis[0][3], &scale[0]);
								sscanf(vaxis.getValue().data(), "[%f %f %f %f] %f", &uvaxis[1][0], &uvaxis[1][1], &uvaxis[1][2], &uvaxis[1][3], &scale[1]);

								std::string faceData = std::format("{} {} [{} {} {} {}] [{} {} {} {}] {} {} {} {}\n}}\n",
									solidelem["plane"].getValue(), solidelem["material"].getValue(),
									uvaxis[0][0], uvaxis[0][1], uvaxis[0][2], uvaxis[0][3],
									uvaxis[1][0], uvaxis[1][1], uvaxis[1][2], uvaxis[1][3],
									solidelem["rotation"].getValue(),
									scale[0], scale[1],
									solidelem["lightmapscale"].getValue()
								);

								MapImporter* importer = Node_getMapImporter( solid );
								MapVMFTextInputStream istream( faceData );
								importer->importTokens( NewScriptTokeniser( istream ) );

								for ( auto sideelem : solidelem ) {
									if (string_equal_nocase(sideelem.getKey().data(), "dispinfo")) {
										std::string dispDef;
										if ( dispDef_from( sideelem, &dispdef ) ) {
											MapVMFTextInputStream istream_dd( dispDef );
											importer->importTokens( NewScriptTokeniser( istream_dd ) );
										} else {
											globalWarningStream() << "FIXME: bad dispinfo in " << Node_getEntity( entity )->getClassName() << '\n';
										}
									}
								}
							}  else if (string_equal_nocase(solidelem.getKey().data(), "editor")) {
								if ( solidelem.hasChild("visgroupid") ) {
									int32_t visgroupid = solidelem["visgroupid"].getValue<int32_t>();
									solid.m_layer = layersParser.from_visgroupid(visgroupid);
								}
							}
						}
						if ( !solid.m_layer ) {
							solid.m_layer = layersParser.getFirstlayer();
						}
						NodeSmartReference solidnode( solid );
						if ( !Node_getTraversable( entity ) ) {
							globalWarningStream() << "FIXME: bad solid data in " << Node_getEntity( entity )->getClassName() << '\n';
						} else {
							Node_getTraversable( entity )->insert( solidnode );
						}
					} else if (string_equal_nocase(e.getKey().data(), "editor")) {
						if ( e.hasChild("visgroupid") ) {
							int32_t visgroupid = e["visgroupid"].getValue<int32_t>();
							entity.m_layer = layersParser.from_visgroupid(visgroupid);
						}
					} else if (string_equal_nocase(e.getKey().data(), "connections")) {
						for ( auto connection : e ) {
							Node_getEntity( entity )->addOutput( connection.getKey().data(), connection.getValue().data() );
						}
					} else {
						if (hasSolids && string_equal_nocase(e.getKey().data(), "origin")) {
							// do nothing
						} else {
							Node_getEntity( entity )->setKeyValue( e.getKey().data(), e.getValue().data() );
						}
					}
				}
				if ( !entity.m_layer ) {
					entity.m_layer = layersParser.getFirstlayer();
				}
				NodeSmartReference node( entity );
				Node_getTraversable( root )->insert( node );
			}
		}

		layersParser.construct_tree();
	}
	class MapVMFWriteKeyValue : public Entity::Visitor
	{
		int64_t& m_childID;
		kvpp::KV1ElementWritable<std::string>& m_element;
	public:
		MapVMFWriteKeyValue( kvpp::KV1ElementWritable<std::string>& element, int64_t& childID ) : m_childID( childID ), m_element( element ) {
			m_element["id"] = m_childID++;
		}
		void visit( const char* key, const char* value ) override {
			m_element[key] = value;
		}
	};
	class MapVMFWriteOutput : public Entity::OutputVisitor
	{
		kvpp::KV1ElementWritable<std::string>& m_element;
	public:
		MapVMFWriteOutput( kvpp::KV1ElementWritable<std::string>& element ) : m_element( element ) {
		}
		void visit( EntityOutput* output ) override {
			m_element["connections"].addChild( output->key(), output->value() );
		}
	};
	class MapVMFWalker : public scene::Traversable::Walker
	{
		mutable int64_t m_childID; // global element index, seems to be shared between entities and solids
		mutable int64_t m_childIndex;
		int64_t m_worldIndex;
		kvpp::KV1Writer<std::string>& m_writer;
	public:
		MapVMFWalker( kvpp::KV1Writer<std::string>& writer, int64_t worldIndex ) : m_childID( 1 ), m_childIndex( worldIndex ), m_worldIndex( worldIndex ), m_writer( writer ) {
		}
		class MapVMFTokenWriter : public TokenWriter
		{
			std::vector<std::string> m_lines;
		public:
			MapVMFTokenWriter() {
				m_lines.push_back("");
			}
			virtual void release() {
			}
			virtual void nextLine() {
				m_lines.push_back("");
			}
			virtual void writeToken( const char* token ) {
				m_lines.back().append( std::format( "{} ", token ) );
			}
			virtual void writeString( const char* string ) {
				m_lines.back().append( std::format( "\"{}\" ", string ) );
			}
			virtual void writeInteger( int i ) {
				m_lines.back().append( std::format( "{} ", i ) );
			}
			virtual void writeUnsigned( std::size_t i ) {
				m_lines.back().append( std::format( "{} ", i ) );
			}
			virtual void writeFloat( double f ) {
				m_lines.back().append( std::format( "{} ", f ) );
			}
			void forEachLine( int64_t& childID, kvpp::KV1ElementWritable<std::string>& solid, std::function<void(int64_t& childID, std::string& line, kvpp::KV1ElementWritable<std::string>& side)> func ) {
				for ( auto line : m_lines ) {
					if ( line[0] == '(' ) {
						func( childID, line, solid.addChild("side") );
					}
				}
			}
		};
		static static writeDisp(std::vector<std::string>& lines, int& i, kvpp::KV1ElementWritable<std::string> &disp) {
			// parse dispDef lines
			std::string line = lines.at(i);
			// TODO: assert line == "dispDef\n"; line++
			// TODO: assert line == "{\n"; line++
			int power;
			double startpos[3];
			sscanf(line.c_str(), "%d ( %lf %lf %lf )", &power, &startpos[0], &startpos[1], &startpos[2]);
			int num_rows = power * power;
			// (
			for (int j; j < num_rows; i++) {
				// ( ( x y z a ) ... )
				// ...
				line = lines.at(i + j);
				// TODO: parse row tokens
			}
			// TODO: assert line == ")\n"; line++
			// TODO: assert line == "}\n"; line++

			// write dispinfo node
			disp["power"] = power;
			disp["startposition"] = std::format( "[{} {} {}]", startpos[0], startpos[1], startpos[2] );
			// unimplemented:
			disp["flags"] = 0;
			disp["elevation"] = 0;
			disp["subdiv"] = 0;
			// child nodes
			kvpp::KV1ElementWritable<std::string> normals = disp.addChild("normals");
			// TODO: rows
			kvpp::KV1ElementWritable<std::string> distances = disp.addChild("distances");
			// TODO: rows
			kvpp::KV1ElementWritable<std::string> offsets = disp.addChild("offsets");
			// TODO: rows
			kvpp::KV1ElementWritable<std::string> offset_normals = disp.addChild("offset_normals");
			// TODO: rows
			kvpp::KV1ElementWritable<std::string> alphas = disp.addChild("alphas");
			// TODO: rows
			kvpp::KV1ElementWritable<std::string> triangle_tags = disp.addChild("triangle_tags");  // unimplemented
			// TODO: rows
			kvpp::KV1ElementWritable<std::string> allowed_verts = disp.addChild("allowed_verts");
			allowed_verts["10"] = "-1 -1 -1 -1 -1 -1 -1 -1 -1 -1";  // default; unused?
		}
		static static writeSide(int64_t &childID, std::string &line, kvpp::KV1ElementWritable<std::string> &side) {
			// parse line
			double points[3][3];
			char material[128];
			double uaxis[4];
			double vaxis[4];
			double rotation;
			double scale[2];
			double lightmapscale;
			sscanf(
				line.c_str(),
				"( %lf %lf %lf ) ( %lf %lf %lf ) ( %lf %lf %lf ) %128s [ %lf %lf %lf %lf ] [ %lf %lf %lf %lf ] %lf %lf %lf %lf",
				&points[0][0], &points[0][1], &points[0][2],
				&points[1][0], &points[1][1], &points[1][2],
				&points[2][0], &points[2][1], &points[2][2],
				material,
				&uaxis[0], &uaxis[1], &uaxis[2], &uaxis[3],
				&vaxis[0], &vaxis[1], &vaxis[2], &vaxis[3],
				&rotation, &scale[0], &scale[1], &lightmapscale);
			// write node
			side["id"] = childID++;
			side["plane"] = std::format(
					"({} {} {}) ({} {} {}) ({} {} {})",
					points[0][0], points[0][1], points[0][2],
					points[1][0], points[1][1], points[1][2],
					points[2][0], points[2][1], points[2][2]);
			side["material"] = material;
			side["uaxis"] = std::format(
					"[{} {} {} {}] {}",
					uaxis[0], uaxis[1], uaxis[2], uaxis[3], scale[0]);
			side["vaxis"] = std::format(
					"[{} {} {} {}] {}",
					vaxis[0], vaxis[1], vaxis[2], vaxis[3], scale[1] );
			side["rotation"] = std::format( "{}", rotation );
			side["lightmapscale"] = std::format( "{}", lightmapscale );
			side["smoothing_groups"] = 0; // FIXME: make configurable
		}
		void writeSolid( scene::Node& node, kvpp::KV1ElementWritable<std::string>& solid ) const {
			solid["id"] = m_childID++;

			MapExporter* exporter = Node_getMapExporter( node );

			// since VMFs are a different beast from MAP files, we need to use Radiant's normal exporter and then do our own thing with it
			// FIXME: this sucks and could surely be done better.
			MapVMFTokenWriter writer;
			exporter->exportTokens( writer );
			writer.forEachLine( m_childID, solid, writeSide );
		}
		virtual bool pre( scene::Node& node ) const {
			Entity* entity = Node_getEntity( node );
			if ( entity ) {
				if ( string_equal( entity->getClassName(), "worldspawn" ) ) {
					MapVMFWriteKeyValue visitor( m_writer["world"], m_childID );
					entity->forEachKeyValue( visitor );
					MapVMFWriteOutput outputVisitor( m_writer["world"] );
					entity->forEachOutput( outputVisitor );
					m_childIndex = m_worldIndex;
				} else {
					auto& child = m_writer.addChild("entity");
					MapVMFWriteKeyValue visitor( child, m_childID );
					entity->forEachKeyValue( visitor );
					MapVMFWriteOutput outputVisitor( child );
					entity->forEachOutput( outputVisitor );
					m_childIndex = m_writer.getChildCount() - 1;
				}
			} else if ( Node_isBrush( node ) ) {
				writeSolid( node, m_writer[m_childIndex].addChild("solid") );
			}

			return true;
		}
	};
	void writeGraph( scene::Node& root, GraphTraversalFunc traverse, TextOutputStream& outputStream ) const override {

		// FIXME: should this be hardcoded?
		GlobalBrushCreator().toggleFormat(eBrushTypeValveSource);

		kvpp::KV1Writer writer;

		// make up some shit
		writer["versioninfo"]["editorextension"] = "wiferadiant"; // strata checks this
		writer["versioninfo"]["editorversion"] = RADIANT_VERSION; // probably should be a single number
		writer["versioninfo"]["editorbuild"] = 0; // not tracked in radiant
		writer["versioninfo"]["mapversion"] = 0; // not tracked in radiant
		writer["versioninfo"]["formatversion"] = 100; // not sure what this corresponds to
		writer["versioninfo"]["prefab"] = 0; // FIXME: make configurable

#if 0
		// write visgroups
		auto* layers = Node_getLayers( root );
		auto& visgroups = writer.addChild("visgroups");
		layers->update_ownIndices();
		layers->forEach([&]( Layer& layer ){
			auto& visgroup = visgroups.addChild("visgroup");
			visgroup["name"] = layer.m_name.c_str();
			visgroup["visgroupid"] = layer.m_ownIndex;
			visgroup["color"] = std::format("{} {} {}", layer.m_color[0], layer.m_color[1], layer.m_color[2]);
		});
#endif

		// traverse tree
		traverse(root, MapVMFWalker( writer, writer.getChildCount() ));

		// extra garbage
		writer["cameras"]["activecamera"] = -1; // shrug
		writer["cordons"]["active"] = 0;

		// bake and write
		auto baked = writer.bake();
		outputStream.write(baked.c_str(), baked.length());
	}
};

typedef SingletonModule<MapVMFAPI, MapDependencies> MapVMFModule;

MapVMFModule g_MapVMFModule;


extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules( ModuleServer& server ){
	initialiseModule( server );

	g_MapVMFModule.selfRegister();
}
