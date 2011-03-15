/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2011 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef _U2_BIOSTRUCT3D_TUBE_RENDERER_H_
#define _U2_BIOSTRUCT3D_TUBE_RENDERER_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QVector>
#include <U2Core/BioStruct3D.h>

#include "BioStruct3DGLRender.h"


namespace U2 {


class TubeGLRenderer : public BioStruct3DGLRenderer {
        
    typedef QVector< SharedAtom > AtomsVector;
    
    struct Tube {
        QMap<int, AtomsVector> modelsMap;
    }; 
    

    QMap<int, Tube> tubeMap;

    void drawTubes(const BioStruct3DColorScheme* colorScheme); 
public:
        
    TubeGLRenderer(const BioStruct3D& struc, const BioStruct3DColorScheme* s);
        
    void drawBioStruct3D();

    RENDERER_FACTORY(TubeGLRenderer)

};

} //namespace

#endif // _U2_BIOSTRUCT3D_TUBE_RENDERER_H_
