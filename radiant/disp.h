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

#pragma once

/// \file
/// \brief The displacement primitive.
///
/// A quad subdivided into a grid of triangles
/// The displacement's power detemines density
/// Used for terrain & material blending


// include
#include "cullable.h"
#include "editable.h"
#include "ifilter.h"  // TODO: add disp filter
#include "iundo.h"
#include "nameable.h"
// libs
#include "generic/vector.h"
#include "scenelib.h"
#include "transformlib.h"


class DispVert {
  public:
    Vector3 m_vertex;  // offset from barycentric base
    float   m_blend;
};


typedef  Array<DispVert>  DispVertArray;


// referencing Patch (radiant/patch.h:316:1008)
class Disp final:
    public Bounded,  // libs/scenelib.h
    public Cullable,  // include/cullable.h
    public Filterable,  // include/ifilter.h
    public Nameable,  // include/nameable.h
    public Snappable,  // include/editable.h
    public TransformNode,  // libs/transformlib.h
    public Undoable  // include/iundo.h
  {
  private:
    /* nested classes */
    class SavedState final : public UndoMemento {
      public:
        CopiedString          m_shader;
        int                   m_power;
        const DispVertArray  &m_vertices;
        Vector3               m_start_pos;
        // how would start_pos be mutated? base brush changes?
        // this is a problem for evaluateTransform callback

        SavedState(
            CopiedString shader,
            int power,
            const DispVertArray &vertices,
            Vector3 start_pos
          ) :
            m_shader(shader),
            m_power(power),
            m_vertices(vertices),
            m_start_pos(start_pos)
        {}

        void release() override {
            delete this;
        }
    };

    /* private members */
    DispVertArray  m_vertices;
    // TODO: TriangleTagsArray (Array<TriangleTags>)
    // TODO: pointer(s) to parent brush/side
    // NOTE: we're using a simplified representation
    // -- not tracking & updating every vmf node
    // -- might have to extend in future
    // -- still have to learn about various nodes

    /* subclass members */
    scene::Node  *m_node;
    AABB          m_aabb_local;

    // for render methods
    CopiedString  m_shader;
    Shader       *m_state;
    // TODO: renderable displacement mesh
    // -- would we vertex edit this mesh?

  public:
    /* public members */
    int      m_power;
    Vector3  m_start_pos;  // first corner of base quad

    /* constructors */
    Disp(
        scene::Node &node,
        const Callback<void()> &evaluateTransform,
        const Callback<void()> &boundsChanged
      ) :
        m_node(&node),
        m_evaluateTransform(evaluateTransform),
        m_boundsChanged(boundsChanged)
      {
        construct();
    }

    // how does copying a displacement work?
    // - has the be copied as part of a brush
    // what about splitting?

    Disp(
        const Disp &other,
        scene::Node &node,
        const Callback<void()> &evaluateTransform,
        const Callback<void()> &boundsChanged
      ) :
        m_node(&node),
        m_evaluateTransform(evaluateTransform),
        m_boundsChanged(boundsChanged)
      {
        construct();
        clone(other);
    }

    Disp(
        const Disp &other
      ) :
        Bounded(other),
        Cullable(other),
        Filterable(other),
        Nameable(other),
        Snappable(),
        TransformNode(other),
        Undoable(other)
      {
        construct();
        clone(other);
    }

    ~Disp() {}

    /* subclass methods */
    void                     attach(const NameCallback &callback) override;  // Nameable
    void                     detach(const NameCallback &callback) override;  // Nameable
    UndoMemento             *exportState() const override;  // Undoable
    void                     importState(const UndoMemento *state) override;  // Undoable
    VolumeIntersectionValue  intersectVolume(const VolumeTest& test, const Matrix4 &localToWorld) override;  // Cullable
    const AABB              &localAABB() const override;  // Bounded
    const Matrix4           &localToParent() const override;  // TransformNode
    const char*              name() override;  // Nameable
    void                     snapto(float snap) override;  // Snappable
    void                     updateFiltered() override;  // Filterable

    /* public methods */
    void construct();  // prepare yourself
    bool isValid() const;

    const char* GetShader() const;
    void        SetShader(const char* name);

    // vertex indexing
    unsigned int    rowLength() const;  // static inline constexpr?
    DispVert       &vertAt(unsigned int row, unsigned int col);
    const DispVert &vertAt(unsigned int row, unsigned int col) const;

    // transforms
    void transform(const Matrix4 &matrix);
    void transformChanged();
    typedef  MemberCaller<Patch, void(), &Disp::transformChanged>  TransformChangedCaller;
    void evaluateTransform();
    void revertTransform();
    void freezeTransform();

  private:
    /* private methods */
    void clone(const Disp &other);  // copy private members

    // TODO: (quad, power, DispVertArray[index]) -> DispRenderVert(bary.pos + offset, bary.uv, blend)
};
