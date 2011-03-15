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

#ifndef _U2_FIND_REPEATS_DIALOG_H_
#define _U2_FIND_REPEATS_DIALOG_H_

#include <ui/ui_FindRepeatsDialog.h>
#include "FindRepeatsTask.h"

#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Core/U2Region.h>

#include <QtGui/QDialog>

namespace U2 {
class CreateAnnotationWidgetController;
class ADVSequenceObjectContext;

class FindRepeatsDialog : public QDialog, public Ui_FindRepeatsDialog {
    Q_OBJECT
public:
    FindRepeatsDialog(ADVSequenceObjectContext* seq);

    static FindRepeatsTaskSettings defaultSettings();
    
    virtual void accept();

protected slots:
    void sl_setPredefinedAnnotationName();
    void sl_minDistChanged(int i);
    void sl_maxDistChanged(int i);
    void sl_startRangeChanged(int i);
    void sl_endRangeChanged(int i);
    void sl_minLenHeuristics();
    void sl_hundredPercent();
    void sl_repeatParamsChanged(int);
    void sl_minMaxToggle(bool);

private:
    U2Region getActiveRange() const;
    void saveState();
    QStringList getAvailableAnnotationNames() const;
    bool getRegions(QCheckBox* cb, QLineEdit* le, QVector<U2Region>& res);
    void prepareAMenu(QToolButton* tb, QLineEdit* le, const QStringList& names);
    void updateStatus();

    //methods for results count estimation
    quint64 areaSize() const ;
    int estimateResultsCount() const;

    ADVSequenceObjectContext*           sc;
    CreateAnnotationWidgetController*   ac;
};

class SetAnnotationNameAction : public QAction {
    Q_OBJECT
public:
    SetAnnotationNameAction(const QString& txt, QObject* p, QLineEdit*  _le) : QAction(txt, p), le(_le) {}
    QLineEdit* le;
};

} //namespace

#endif
