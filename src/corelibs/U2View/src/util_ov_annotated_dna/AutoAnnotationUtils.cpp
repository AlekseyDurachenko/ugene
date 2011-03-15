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

#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>

#include "ADVSequenceObjectContext.h"
#include "AutoAnnotationUtils.h"


namespace U2 {

AutoAnnotationsADVAction::AutoAnnotationsADVAction(ADVSequenceWidget* v, AutoAnnotationObject* obj) 
: ADVSequenceWidgetAction("AutoAnnotationUpdateAction", tr("Automatic annotation highlighting")), aaObj(obj)
{
    seqWidget = v;
    addToBar = true;

    menu = new QMenu();
    setIcon(QIcon(":core/images/predefined_annotation_groups.png"));
    setMenu(menu);
    
    updateMenu();

}


void AutoAnnotationsADVAction::updateMenu()
{
    AutoAnnotationConstraints constraints;
    if (seqWidget->getSequenceContexts().count() > 0) {
        constraints.alphabet = seqWidget->getSequenceContexts().first()->getAlphabet();
    }

    QList<AutoAnnotationsUpdater*> updaters = AppContext::getAutoAnnotationsSupport()->getAutoAnnotationUpdaters();
    foreach (AutoAnnotationsUpdater* updater, updaters) {
        QAction* toggleAction = new QAction(updater->getName(), this);
        bool enabled = updater->checkConstraints(constraints);
        toggleAction->setEnabled(enabled);
        toggleAction->setCheckable(true);
        bool checked = updater->isCheckedByDefault();
        toggleAction->setChecked(checked);
        aaObj->setGroupEnabled(updater->getGroupName(), checked);
        aaObj->update(updater);
        connect( toggleAction, SIGNAL(toggled(bool)), SLOT(sl_toggle(bool)) );
        menu->addAction(toggleAction);
    }
    menu->update();
}

void AutoAnnotationsADVAction::sl_toggle( bool toggled )
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action == NULL) {
        return;
    }
    AutoAnnotationsUpdater* updater = AppContext::getAutoAnnotationsSupport()->findUpdaterByName(action->text());
    if (updater != NULL) {
        QString groupName = updater->getGroupName();
        aaObj->setGroupEnabled(groupName, toggled);
        aaObj->update(updater);
    }
}


} //namespace
