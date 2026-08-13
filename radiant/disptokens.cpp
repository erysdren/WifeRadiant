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

#include "disptokens.h"


/*********************/
/* DispTokenImporter */
/*********************/

inline bool DispTokenImporter::importHeader(Disp &disp, Tokeniser &tokeniser) {
    tokeniser.nextLine();
    RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, "{"));
    return true;
}


inline bool DispTokenImporter::importShader(Disp &disp, Tokeniser &tokeniser) {
    tokeniser.nextLine();
    const char* texture = tokeniser.getToken();
    if (texture == 0) {
        Tokeniser_unexpectedError(tokeniser, texture, "#texture-name");
        return false;
    }
    if (string_equal(texture, "NULL"))
        disp.SetShader(texdef_name_default());
    else
        disp.SetShader(StringStream<64>(GlobalTexturePrefix_get(), texture));
    return true;
}


inline bool DispTokenImporter::importParams(Disp &disp, Tokeniser &tokeniser) {
    tokeniser.nextLine();
    RETURN_FALSE_IF_FAIL(Tokeniser_getInteger(tokeniser, disp.m_power));
    RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, "("));
    RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, disp.m_start_pos[0]));
    RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, disp.m_start_pos[1]));
    RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, disp.m_start_pos[2]));
    RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, ")"));
    return true;
}


inline bool DispTokenImporter::importVertex(Disp &disp, Tokeniser &tokeniser) {
    unsigned int limit;
    DispVert dv;

    tokeniser.nextLine();
    RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, "("));
    limit = disp.row_length;
    for (unsigned int col = 0; col < limit; ++col) {
        tokeniser.nextLine();
        RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, "("));
        for (unsigned int row = 0; row < limit; ++row) {
            dv = disp.vertAt(row, col);
            RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, "("));
            RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, dv.m_vertex[0]));
            RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, dv.m_vertex[1]));
            RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, dv.m_vertex[2]));
            RETURN_FALSE_IF_FAIL(Tokeniser_getFloat(tokeniser, dv.m_blend));
            RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, ")"));
        }
        RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, ")"));
    }
    tokeniser.nextLine();
    RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, ")"));
    return true;
}


inline bool DispTokenImporter::importFooter(Disp &disp, Tokeniser &tokeniser) {
    tokeniser.nextLine();
    RETURN_FALSE_IF_FAIL(Tokeniser_parseToken(tokeniser, "}"));
    return true;
}


/*********************/
/* DispTokenExporter */
/*********************/

inline void DispTokenExporter::exportHeader(const Disp &disp, TokenWriter &writer) {
    writer.writeToken("{");
    writer.nextLine();
}


inline void DispTokenExporter::exportShader(const Disp &disp, TokenWriter &writer) {
    if (*(shader_get_textureName(disp.GetShader())) == '\0')
        writer.writeToken("NULL");
    else
        writer.writeToken(shader_get_textureName(patch.GetShader()));
    writer.nextLine();
}


inline void DispTokenExporter::exportParams(const Disp &disp, TokenWriter &writer) {
    writer.writeInteger(disp.m_power);
    writer.writeToken("(");
    writer.writeFloat(disp.m_start_pos[0]);
    writer.writeFloat(disp.m_start_pos[1]);
    writer.writeFloat(disp.m_start_pos[2]);
    writer.writeToken(")");
    writer.nextLine();
}


inline void DispTokenExporter::exportVertex(const Disp &disp, TokenWriter &writer) {
    unsigned int limit;
    DispVert dv;

    writer.writeToken("(");
    writer.nextLine();
    limit = disp.row_length;
    for (unsigned int col = 0; col < limit; ++col) {
        writer.writeToken("(");
        for (unsigned int row = 0; row < limit; ++row) {
            dv = disp.vertAt(row, col);
            writer.writeToken("(");
            writer.writeFloat(dv.m_vertex[0]);
            writer.writeFloat(dv.m_vertex[1]);
            writer.writeFloat(dv.m_vertex[2]);
            writer.writeFloat(dv.m_blend);
            writer.writeToken(")");
        }
        writer.writeToken(")");
        writer.nextLine();
    }
    writer.writeToken(")");
    writer.nextLine();
}


inline void DispTokenExporter::exportFooter(m_disp, writer) {
    writer.writeToken("}");
    writer.nextLine();
}
