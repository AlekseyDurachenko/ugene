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

#ifndef _U2_WORKFLOW_PALETTE_H_
#define _U2_WORKFLOW_PALETTE_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>

#include <QtGui/QAction>
#include <QtGui/QTreeWidget>

namespace U2 {
using namespace Workflow;
class WorkflowView;
class WorkflowScene;


class WorkflowPalette : public QTreeWidget
{
    Q_OBJECT

public:
    static const QString MIME_TYPE;

    WorkflowPalette(ActorPrototypeRegistry* reg, QWidget *parent = 0);
    QMenu* createMenu(const QString& name);

    QVariant saveState() const;
    void restoreState(const QVariant&);

public slots:
    void resetSelection();

signals:
    void processSelected(Workflow::ActorPrototype*);
    void si_protoDeleted(const QString &);
    
protected:
    void contextMenuEvent(QContextMenuEvent *e);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void leaveEvent ( QEvent * event );

private slots:
    void handleItemAction();
    void sl_selectProcess(bool checked = false);
    void rebuild();
    void editElement();
    void removeElement();

private:
    QTreeWidgetItem* createItemWidget(QAction* a);
    QAction* createItemAction(Workflow::ActorPrototype* item);
    void setContent(ActorPrototypeRegistry*);
    void sortTree();
private:
    QMap<QString,QList<QAction*> > categoryMap;
    QMap<QAction*, QTreeWidgetItem*> actionMap;
    QTreeWidgetItem *overItem;
    QAction* currentAction;
    QPoint dragStartPosition;
    friend class PaletteDelegate;
};


}//namespace

Q_DECLARE_METATYPE(QAction *)
Q_DECLARE_METATYPE(U2::Workflow::ActorPrototype *)

#endif
