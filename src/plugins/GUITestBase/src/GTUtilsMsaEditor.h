/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2014 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GT_UTILS_MSA_EDITOR_H_
#define _U2_GT_UTILS_MSA_EDITOR_H_

#include <QtCore/QRect>

#include <QtGui/QColor>

namespace U2 {

class MSAEditorNameList;
class MSAGraphOverview;
class U2OpStatus;

// If you can't find an appropriate method check the GTUtilsMsaEditorSequenceArea class
class GTUtilsMsaEditor {
public:
    static QColor getGraphOverviewPixelColor(U2OpStatus &os, const QPoint &point);
    static MSAGraphOverview *getGraphOverview(U2OpStatus &os);

    static MSAEditorNameList *getNameListArea(U2OpStatus &os);
    static QRect getSequenceNameRect(U2OpStatus &os, const QString &sequenceName);

    static void clickSequenceName(U2OpStatus &os, const QString &sequenceName);

    static void toggleCollapsingMode(U2OpStatus &os);
    static void toggleCollapsingGroup(U2OpStatus &os, const QString &groupName);
};

}   // namespace U2

#endif // _U2_GT_UTILS_MSA_EDITOR_H_