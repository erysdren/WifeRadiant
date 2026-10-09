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

#include "sceneexport.h"
#include "isceneexport.h"

#include "gtkutil/filechooser.h"
#include "plugin.h"
#include "mainframe.h"

#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class SceneExporter : public QObject {
private:
	QWidget* m_window = nullptr;
	QComboBox* m_combo = nullptr;
	bool m_hasExported = false;

	void selectionChanged(int index) {
		// FIXME: this sucks
		const SceneExport* table = *static_cast<const SceneExport**>(m_combo->currentData().data());
		globalOutputStream() << table->getName() << '\n';
	}
public:
	void create() {
		if (m_window != nullptr) {
			return;
		}

		// create base window
		m_window = new QWidget( MainFrame_getWindow(), Qt::Dialog | Qt::WindowCloseButtonHint );
		m_window->setWindowTitle( "Export" );

		// collect descriptions for each available export module
		std::map<QString, const SceneExport&> exports;

		class CollectSceneExportsVisitor : public SceneExportModules::Visitor {
		private:
			std::map<QString, const SceneExport&>& m_exports;
		public:
			CollectSceneExportsVisitor(std::map<QString, const SceneExport&>& exports) : m_exports(exports) {

			}
			void visit(const char* minor, const SceneExport& table) const override {
				m_exports.emplace(minor, table);
			}
		};

		Radiant_getExportModules().foreachModule( CollectSceneExportsVisitor( exports ) );

		// setup gui
		{
			auto* grid = new QGridLayout( m_window );
			// first row
			{
				{
					auto* label = new QLabel( "Format:" );
					grid->addWidget( label, 0, 0 );
				}
				{
					m_combo = new QComboBox;
					for (const auto& [key, value] : exports) {
						m_combo->addItem(value.getName(), QVariant::fromValue(&value));
					}
					grid->addWidget( m_combo, 0, 1, 1, 2 );

					QObject::connect( m_combo, &QComboBox::activated, this, &SceneExporter::selectionChanged );
				}
			}
			// second row
			{
				{
					auto* label = new QLabel( "Options:" );
					grid->addWidget( label, 1, 0 );
				}
				{
					auto *container = new QWidget;
					grid->addWidget( container, 1, 1, 2, 2 );

					auto *vbox = new QVBoxLayout( container );
					vbox->setContentsMargins( 0, 0, 0, 0 );
				}
			}
			// fourth row
			{
				{
					auto* button = new QPushButton( "Export" );
					grid->addWidget( button, 3, 0 );
					// QObject::connect( button, &QAbstractButton::clicked, SceneExporter_doExport );
				}
				{
					auto* button = new QPushButton( "Export As" );
					grid->addWidget( button, 3, 1 );
					QObject::connect( button, &QAbstractButton::clicked, SceneExporter_doExportAs );
				}
				{
					auto* button = new QPushButton( "Cancel" );
					grid->addWidget( button, 3, 2 );
					QObject::connect( button, &QAbstractButton::clicked, m_window, &QWidget::hide );
				}
			}
		}
	}

	void show() {
		m_window->show();
	}

	bool hasExported() const {
		return m_hasExported;
	}

	void doExport(const char* filename) {
		m_hasExported = true;
		globalOutputStream() << "exported " << filename << '\n';
	}
};

static SceneExporter g_sceneExporter{};

void SceneExporter_show() {
	g_sceneExporter.create();
	g_sceneExporter.show();
}

bool SceneExporter_hasExported() {
	return g_sceneExporter.hasExported();
}

void SceneExporter_doExport(const char* path) {
	g_sceneExporter.doExport(path);
}

void SceneExporter_doExportAs() {
	const char* path = file_dialog( MainFrame_getWindow(), false, "Export As", nullptr, "obj", false, false, true );
	if (path != nullptr) {
		g_sceneExporter.doExport(path);
	}
}
