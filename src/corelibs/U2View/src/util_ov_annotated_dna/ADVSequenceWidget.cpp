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

#include "ADVSequenceWidget.h"
#include "ADVSequenceObjectContext.h"
#include "AnnotatedDNAView.h"

namespace U2 {

ADVSequenceWidget::ADVSequenceWidget(AnnotatedDNAView* _ctx) :QWidget(_ctx->getScrolledWidget()), ctx(_ctx) {
}

void ADVSequenceWidget::addADVSequenceWidgetAction(ADVSequenceWidgetAction* action) {
    assert(!action->objectName().isEmpty());
    assert(getADVSequenceWidgetAction(action->objectName()) == NULL);
    action->setParent(this);
    action->seqWidget = this;
    wActions.append(action);
}

ADVSequenceWidgetAction* ADVSequenceWidget::getADVSequenceWidgetAction(const QString& objName) const {
    Q_UNUSED(objName);
    foreach(ADVSequenceWidgetAction* action, wActions) {
        if (action->objectName() == objectName()) {
            return action;
        }
    }
    return NULL;
}

QList<DNASequenceObject*> ADVSequenceWidget::getSequenceObjects() const {
    QList<DNASequenceObject*> res;
    foreach(ADVSequenceObjectContext* ctx, seqContexts) {
        res.append(ctx->getSequenceObject());
    }
    return res;
}

void ADVSequenceWidget::buildPopupMenu(QMenu& m) {
    Q_UNUSED(m);
}

}//namespace

