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

#ifndef _U2_ANNOTATION_UTILS_H_
#define _U2_ANNOTATION_UTILS_H_

#include <U2Core/U2Annotation.h>

namespace U2 {

/**                                           
    U2Annotation and related structures utility functions
*/
class U2CORE_EXPORT U2AnnotationUtils : public QObject {
    Q_OBJECT
private:
    U2AnnotationUtils(){}
public:

    enum AnnotationStrategyForResize{
        AnnotationStrategyForResize_Resize, 
        AnnotationStrategyForResize_Remove,
        AnnotationStrategyForResize_Split
    };

    /** Corrects annotation locations for a sequence. The passed list is updated with new locations, 
        the returned list contains removed locations if any.
        If specified strategy is 'remove', removes all locations which intersect the removed region or fall inside it.
    */
    static QVector<U2Region> fixLocationsForRemovedRegion(const U2Region& regionToDelete, QVector<U2Region>& loc, AnnotationStrategyForResize s);

    /** Corrects annotation locations for a sequence. The passed list is updated with new locations, 
    the returned list contains removed locations if any.
    If specified strategy is 'remove', removes all locations which contain the insert position inside them.
    Note, if a region starts at the insert position, it is just moved to the right.
    */
    static QVector<U2Region> fixLocationsForInsertedRegion(qint64 insertPos, qint64 len, QVector<U2Region>& loc, AnnotationStrategyForResize s);

    /** Corrects annotation locations for a sequence. The passed list is updated with new locations, 
    the returned list contains removed locations if any.
    If specified strategy is 'remove', removes all locations which intersect the modified region or fall inside it.
    */
    static QVector<U2Region> fixLocationsForReplacedRegion(const U2Region& regionToDelete, qint64 newLen, QVector<U2Region>& loc, AnnotationStrategyForResize s);

    /** returns translation frame[0,1,2] the region is placed on */
    static int getRegionFrame(int sequenceLen, U2Strand strand, bool order, int region, const QVector<U2Region>& location);

};

} //namespace

#endif
