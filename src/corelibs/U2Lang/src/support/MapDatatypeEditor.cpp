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

#include <assert.h>

#include <QtGui/QListWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QTextEdit>
#include <QtGui/QStandardItemModel>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/IntegralBusType.h>

#include <U2Lang/WorkflowUtils.h>

#include "MapDatatypeEditor.h"

namespace U2 {

using namespace Workflow;

/*******************************
 * MapDatatypeEditor
 *******************************/
static const int KEY_COLUMN     = 0;
static const int VALUE_COLUMN   = 1;

MapDatatypeEditor::MapDatatypeEditor(Configuration* cfg, const QString& prop, 
    DataTypePtr from, DataTypePtr to) : cfg(cfg), propertyName(prop), from(from), to(to), table(NULL) 
{}

QWidget* MapDatatypeEditor::getWidget() {
    return createGUI(from, to);
}

QWidget* MapDatatypeEditor::createGUI(DataTypePtr from, DataTypePtr to) {
    if(!from || !to || !from->isMap() || !to->isMap()) {
        assert(false);
        return NULL;
    }
    
    bool infoMode = (to == from);
    if (infoMode) {
        table = new QTableWidget(0,1);
        //table->setHorizontalHeaderLabels((QStringList() << tr("Data on the bus")));
    } else {
        table = new QTableWidget(0,2);
        //table->setHorizontalHeaderLabels((QStringList() << tr("Accepted inputs") << tr("Data from bus") ));
        table->setItemDelegateForColumn(VALUE_COLUMN, new DescriptorListEditorDelegate(this));
    }
    
    table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(2);
    sizePolicy5.setHeightForWidth(table->sizePolicy().hasHeightForWidth());
    table->setSizePolicy(sizePolicy5);
    table->setBaseSize(QSize(0, 100));
    table->setEditTriggers(QAbstractItemView::CurrentChanged|QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
    table->setAlternatingRowColors(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setTextElideMode(Qt::ElideMiddle);
    table->setShowGrid(false);
    //table->setWordWrap(false);
    table->setCornerButtonEnabled(false);
    
    int height = QFontMetrics(QFont()).height() + 6;
    const QList<Descriptor>& keys = to->getAllDescriptors();
    QMap<QString,QString> bindingsMap = cfg->getParameter(propertyName)->getAttributeValue<QStrStrMap>();
    table->setRowCount(keys.size());
    
    int keysSz = keys.size();
    for(int i = 0; i < keysSz; i++) {
        Descriptor key = keys.at(i);
        
        // set key item
        QTableWidgetItem* keyItem = new QTableWidgetItem(key.getDisplayName());
        keyItem->setToolTip(to->getDatatypeByDescriptor(key)->getDisplayName());
        keyItem->setData(Qt::UserRole, qVariantFromValue<Descriptor>(key));
        keyItem->setFlags(Qt::ItemIsSelectable);
        table->setItem(i, KEY_COLUMN, keyItem);
        
        table->setRowHeight(i, height);
        if (infoMode) {
            continue;
        }
        
        // set value item
        DataTypePtr elementDatatype = to->getDatatypeByDescriptor(key);
        QList<Descriptor> candidates = WorkflowUtils::findMatchingCandidates(from, to, key);
        Descriptor current = WorkflowUtils::getCurrentMatchingDescriptor(candidates, to, key, bindingsMap);
        
        QTableWidgetItem* valueItem = new QTableWidgetItem(current.getDisplayName());
        valueItem->setData(Qt::UserRole, qVariantFromValue<Descriptor>(current));
        valueItem->setData(Qt::UserRole+1, qVariantFromValue<QList<Descriptor> >(candidates));
        if (elementDatatype->isList()) {
            valueItem->setData(Qt::UserRole+2, true);
        }
        table->setItem(i, VALUE_COLUMN, valueItem);
    }
    
    QWidget* widget = new QWidget();
    QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
    widget->setSizePolicy(sizePolicy1);
    
    QVBoxLayout* verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setSpacing(0);
    verticalLayout->setMargin(0);
    verticalLayout->addWidget(table);
    //verticalLayout->addWidget(doc = new QTextEdit(widget));
    //doc->setEnabled(false);
    connect(table, SIGNAL(itemSelectionChanged()), SLOT(sl_showDoc()));
    
    return widget;
}

int MapDatatypeEditor::getOptimalHeight() {
    if(table) {
        return table->rowHeight(0) * (table->model()->rowCount() + 1);
    } else {
        return 0;
    }
}

static QString formatDoc(const Descriptor& s, const Descriptor& d) {
    return U2::MapDatatypeEditor::tr("The input slot <b>%1</b><br>is bound to<br>the bus slot <b>%2</b>")
        .arg(s.getDisplayName())
        .arg(d.getDisplayName());
}

void MapDatatypeEditor::sl_showDoc() {
    QList<QTableWidgetItem *> list = table->selectedItems();
    QString text = "";
    if (list.size() == 1) {
        if (isInfoMode()) {
            //doc->setText(DesignerUtils::getRichDoc(list.at(0)->data(Qt::UserRole).value<Descriptor>()));
            text = WorkflowUtils::getRichDoc(list.at(0)->data(Qt::UserRole).value<Descriptor>());
        } else {
            int row = list.at(0)->row();
            Descriptor d = table->item(row, KEY_COLUMN)->data(Qt::UserRole).value<Descriptor>();
            Descriptor s = table->item(row, VALUE_COLUMN)->data(Qt::UserRole).value<Descriptor>();
            //doc->setText(formatDoc(d, s));
            text = formatDoc(d, s);
        }        
    } else {
        //doc->setText("");
    }

    emit si_showDoc(text);
}

void MapDatatypeEditor::commit() {
    QMap<QString,QString> map;
    if (table && !isInfoMode()) {
        for (int i = 0; i < table->rowCount(); i++) {
            QString key = table->item(i, KEY_COLUMN)->data(Qt::UserRole).value<Descriptor>().getId();
            QString val = table->item(i, VALUE_COLUMN)->data(Qt::UserRole).value<Descriptor>().getId();
            map[key] = val;
        }
    }
    cfg->setParameter(propertyName, qVariantFromValue<QStrStrMap>(map));
    sl_showDoc();
}

/*******************************
* BusPortEditor
*******************************/
BusPortEditor::BusPortEditor(IntegralBusPort* p) : MapDatatypeEditor(p, IntegralBusPort::BUS_MAP_ATTR_ID, DataTypePtr(), DataTypePtr()), port(p) {
    to = WorkflowUtils::getToDatatypeForBusport(p);
    from = WorkflowUtils::getFromDatatypeForBusport(p, to);
}

QWidget* BusPortEditor::createGUI(DataTypePtr from, DataTypePtr to) {
    QWidget* w = MapDatatypeEditor::createGUI(from, to);
    if (table && port->getWidth() == 0) {
        /*if (port->isInput()) {
            table->setHorizontalHeaderLabels((QStringList() << U2::MapDatatypeEditor::tr("Accepted inputs")));
        }
        else {
            table->setHorizontalHeaderLabels((QStringList() << U2::MapDatatypeEditor::tr("Provided outputs")));
        }*/
    } else if (table) {
        connect(table->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
                     this, SLOT(handleDataChanged(const QModelIndex&, const QModelIndex&)));
    }
    return w;
}

void BusPortEditor::handleDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight) {
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    commit();
}

/*******************************
* DescriptorListEditorDelegate
*******************************/
QWidget *DescriptorListEditorDelegate::createEditor(QWidget *parent,
                                              const QStyleOptionViewItem &/* option */,
                                              const QModelIndex &/* index */) const
{
    QComboBox* editor = new QComboBox(parent);
    editor->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    return editor;
}

void DescriptorListEditorDelegate::setEditorData(QWidget *editor,
                                           const QModelIndex &index) const
{
    QString current = index.model()->data(index, Qt::UserRole).value<Descriptor>().getId();
    QList<Descriptor> list = index.model()->data(index, Qt::UserRole+1).value<QList<Descriptor> >();
    QComboBox *combo = static_cast<QComboBox*>(editor);
    if (index.model()->data(index, Qt::UserRole+2).toBool()) {
        QStringList curList = current.split(";");
        QStandardItemModel* cm = new QStandardItemModel(list.size(), 1, combo);
        for (int i = 0; i < list.size(); ++i) {
            Descriptor d = list[i];
            QStandardItem* item = new QStandardItem(d.getDisplayName());
            item->setCheckable(true); item->setEditable(false); item->setSelectable(false);
            item->setCheckState(curList.contains(d.getId())? Qt::Checked: Qt::Unchecked);
            item->setData(qVariantFromValue<Descriptor>(d));
            cm->setItem(i, item);
        }
        combo->setModel(cm);
        QListView* vw = new QListView(combo);
        vw->setModel(cm);
        combo->setView(vw);
        //vw->setEditTriggers(QAbstractItemView::AllEditTriggers);
    } else {
        combo->clear();
        int currentIdx = 0;
        for (int i = 0; i < list.size(); ++i) {
            combo->addItem(list[i].getDisplayName(), qVariantFromValue<Descriptor>(list[i]));
            if (list[i] == current) {
                currentIdx = i;
            }
        }

        combo->setCurrentIndex(currentIdx);
    }
}

void DescriptorListEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                          const QModelIndex &index) const
{
    QComboBox *combo = static_cast<QComboBox*>(editor);
    QVariant value;
    if (index.model()->data(index, Qt::UserRole+2).toBool()) {
        QStandardItemModel* cm = qobject_cast<QStandardItemModel*>(combo->model());
        Descriptor res;
        QStringList ids;
        for(int i = 0; i< cm->rowCount();++i){
            if (cm->item(i)->checkState() == Qt::Checked) {
                res = cm->item(i)->data().value<Descriptor>();
                ids << res.getId();
            }
        }
        if (ids.isEmpty()) {
            value = qVariantFromValue<Descriptor>(Descriptor("", tr("<empty>"), tr("Default value")));
        } else if (ids.size() == 1) {
            value = qVariantFromValue<Descriptor>(res);
        } else {
            value = qVariantFromValue<Descriptor>(Descriptor(ids.join(";"), tr("<List of values>"), tr("List of values")));
        }
    } else {
        value = combo->itemData(combo->currentIndex());
    }
    model->setData(index, value, Qt::UserRole);
    model->setData(index, value.value<Descriptor>().getDisplayName(), Qt::DisplayRole);
}

}//namespace U2
