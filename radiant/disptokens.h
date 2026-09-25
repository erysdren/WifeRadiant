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


// libs
#include "stringio.h"

#include "disp.h"


class DispTokenImporter: public MapImporter {
  private:
    Disp &m_disp;

    inline static bool importHeader(Disp &disp, Tokeniser &tokeniser);
    inline static bool importParams(Disp &disp, Tokeniser &tokeniser);
    inline static bool importVertex(Disp &disp, Tokeniser &tokeniser);
    inline static bool importFooter(Disp &disp, Tokeniser &tokeniser);

  public:
    DispTokenImporter(Disp &disp) : m_disp(disp) {};

    bool importTokens(Tokeniser &tokeniser) override;
};


class DispTokenExporter : public MapExporter {
  private:
    const Disp &m_disp;

    inline static void exportHeader(Disp &disp, TokenWriter &writer);
    inline static void exportParams(Disp &disp, TokenWriter &writer);
    inline static void exportVertex(Disp &disp, TokenWriter &writer);
    inline static void exportFooter(Disp &disp, TokenWriter &writer);

  public:
    DispTokenExporter(Disp &disp) : m_disp(disp) {}

    void exportTokens(TokenWriter &writer) const override;
};
