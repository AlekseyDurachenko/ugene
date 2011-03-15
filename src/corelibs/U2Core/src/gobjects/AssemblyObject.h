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

#ifndef _U2_ASSEMBLY_OBJECT_H_
#define _U2_ASSEMBLY_OBJECT_H_

#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/U2Type.h>

namespace U2 {

class U2CORE_EXPORT AssemblyObject : public GObject {
    Q_OBJECT
public:
    AssemblyObject(const U2DataRef& ref, const QString& objectName, const QVariantMap& hints) 
        : GObject(GObjectTypes::ASSEMBLY, objectName, hints), objectView(0), dbiRef(ref){};

    virtual GObject* clone() const;
    const U2DataRef& getDbiRef() const {return dbiRef;}
    inline void setView(QObject * view) {objectView = view;}
    inline QObject * getView() const {return objectView;}
protected:
    QObject * objectView;
    U2DataRef dbiRef;
};

}//namespace


#endif
