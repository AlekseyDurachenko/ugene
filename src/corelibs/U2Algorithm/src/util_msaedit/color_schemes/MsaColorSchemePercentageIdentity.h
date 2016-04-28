/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2016 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_COLOR_SCHEME_PERCENTAGE_IDENTITY_H_
#define _U2_MSA_COLOR_SCHEME_PERCENTAGE_IDENTITY_H_

#include <QColor>
#include <QVector>

#include "MsaColorScheme.h"

namespace U2 {

class U2ALGORITHM_EXPORT MsaColorSchemePercentageIdentity : public MsaColorScheme {
    Q_OBJECT
public:
    MsaColorSchemePercentageIdentity(QObject *parent, const MsaColorSchemeFactory *factory, MAlignmentObject *maObj);

    QColor getColor(int seq, int pos, char c) const;

private slots:
    void sl_alignmentChanged();

protected:
    void updateCache() const;

    mutable QVector<quint32>    indentCache;
    mutable int                 cacheVersion;
    int                         objVersion;
    QColor                      colorsByRange[4];
    int                         mask4[4];
    mutable char                tmpChars[4];
    mutable int                 tmpRanges[4];
};

class MsaColorSchemePercentageIdentityFactory : public MsaColorSchemeFactory {
    Q_OBJECT
public:
    MsaColorSchemePercentageIdentityFactory(QObject *parent, const QString &id, const QString &name, DNAAlphabetType alphabetType);

    MsaColorScheme * create(QObject *parent, MAlignmentObject *maObj) const;
};


}   // namespace U2

#endif // _U2_MSA_COLOR_SCHEME_PERCENTAGE_IDENTITY_H_
