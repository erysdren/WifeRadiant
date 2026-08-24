/*
   Copyright (C) 2026, Bikkie.
   All Rights Reserved.

   This file is part of WifeRadiant.

   WifeRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   WifeRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with WifeRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "disp.h"


// TODO: void Disp::attach(const NameCallback &callback) override {  // Nameable
// TODO: void Disp::detach(const NameCallback &callback) override {  // Nameable
// TODO: UndoMemento *Disp::exportState() const override {  // Undoable
// TODO: void Disp::importState(const UndoMemento *state) override {  // Undoable


VolumeIntersectionValue Disp::intersectVolume(  // Cullable
    const VolumeTest &test,
    const Matrix4    &localToWorld
  ) const override {
    return test.TestAABB(m_aabb_local, localToWorld);
}


const AABB &Disp::localAABB() const override {  // Bounded
    return m_aabb_local;
}


const Matrix4 &Disp::localToParent() const override {  // TransformNode
    return g_matrix4_identity;
}


// TODO: const char* Disp::name() override {  // Nameable
// TODO: void Disp::snapto(float snap) override {  // Snappable
// TODO: void Disp::updateFiltered() override {  // Filterable


/* public methods */

unsigned int Disp::rowLength() const {
    return m_power * m_power;
}


DispVert &Disp::vertAt(unsigned int row, unsigned int col) {
    return m_vertices[row * rowLength() + col];
}


const DispVert &Disp::vertAt(unsigned int row, unsigned int col) const {
    return m_vertices[row * rowLength() + col];
}
