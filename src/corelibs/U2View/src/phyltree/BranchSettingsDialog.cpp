/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2015 UniPro <ugene@unipro.ru>
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

#include "BranchSettingsDialog.h"
#include <QColorDialog>
#include <U2Gui/HelpButton.h>

namespace U2 {

BranchSettingsDialog::BranchSettingsDialog(QWidget *parent, const OptionsMap& settings) :
    BaseSettingsDialog(parent) {
    changedSettings[BRANCH_COLOR] = settings[BRANCH_COLOR];
    changedSettings[BRANCH_THICKNESS] = settings[BRANCH_THICKNESS];
    setupUi(this);
    new HelpButton(this, buttonBox, "14059089");

    thicknessSpinBox->setValue(changedSettings[BRANCH_THICKNESS].toInt());
    updateColorButton();

    connect(colorButton, SIGNAL(clicked()), SLOT(sl_colorButton()));

}

void BranchSettingsDialog::updateColorButton() {
    static const QString COLOR_STYLE("QPushButton { background-color : %1;}");
    QColor branchColor = qvariant_cast<QColor>(changedSettings[BRANCH_COLOR]);
    colorButton->setStyleSheet(COLOR_STYLE.arg(branchColor.name()));
}

void BranchSettingsDialog::sl_colorButton() {
    QColorDialog::ColorDialogOptions options;
#ifdef Q_OS_MAC
    if (qgetenv("UGENE_GUI_TEST").toInt() == 1 && qgetenv("UGENE_USE_NATIVE_DIALOGS").toInt() == 0) {
        options |= QColorDialog::DontUseNativeDialog;
    }
#endif

    QColor branchColor = qvariant_cast<QColor>(changedSettings[BRANCH_COLOR]);
    QColor newColor = QColorDialog::getColor(branchColor, this, tr("Select Color"), options);
    if (newColor.isValid()) {
        changedSettings[BRANCH_COLOR] = newColor;
        updateColorButton();
    }
}

void BranchSettingsDialog::accept() {
    changedSettings[BRANCH_THICKNESS] = thicknessSpinBox->value();
    QDialog::accept();
}
} //namespace
