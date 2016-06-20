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

#include <QPushButton>
#include <QMessageBox>
#include <ui_ExportHighlightedDialog.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportHighlightedDialogController.h"
#include "ov_msa/MSAEditorSequenceArea.h"

namespace U2{

ExportHighligtingDialogController::ExportHighligtingDialogController(MSAEditorUI *msaui_, QWidget* p )
    : QDialog(p),
      msaui(msaui_),
      saveController(NULL),
      ui(new Ui_ExportHighlightedDialog())
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "17470558");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    CHECK(AppContext::getAppSettings(), );
    CHECK(AppContext::getAppSettings()->getUserAppsSettings(), );
    CHECK(msaui->getEditor(), );
    CHECK(msaui->getEditor()->getMSAObject(), );

    initSaveController();

    connect(ui->endPosBox, SIGNAL(valueChanged(int)), SLOT(endPosValueChanged()));

    int alignLength = msaui->getEditor()->getMSAObject()->getLength();
    QRect selection = msaui->getSequenceArea()->getSelection().getRect();

    int startPos = -1;
    int endPos = -1;
    if (selection.isNull()) {
        startPos = 1;
        endPos = alignLength;
    } else {
        startPos = selection.x() + 1;
        endPos = selection.x() + selection.width();
    }

    ui->startPosBox->setMaximum(endPos);
    ui->endPosBox->setMaximum(alignLength);

    ui->startPosBox->setMinimum(1);
    ui->endPosBox->setMinimum(2);

    ui->startPosBox->setValue(startPos);
    ui->endPosBox->setValue(endPos);
}
ExportHighligtingDialogController::~ExportHighligtingDialogController(){
    delete ui;
}

void ExportHighligtingDialogController::accept(){
    startPos = ui->startPosBox->value();
    endPos = ui->endPosBox->value();
    if(ui->oneIndexRB->isChecked()){
        startingIndex = 1;
    }else{
        startingIndex = 0;
    }
    if (saveController->getSaveFileName().isEmpty()){
        QMessageBox::warning(this, tr("Warning"), tr("Export to file URL is empty!"));
        return;
    }
    keepGaps = ui->keepGapsBox->isChecked();
    dots = ui->dotsBox->isChecked();
    transpose = ui->transposeBox->isChecked();
    url = GUrl(saveController->getSaveFileName());

    QDialog::accept();
}

void ExportHighligtingDialogController::lockKeepGaps(){
    ui->keepGapsBox->setChecked(true);
    ui->keepGapsBox->setDisabled(true);
}

void ExportHighligtingDialogController::endPosValueChanged(){
    ui->startPosBox->setMaximum(ui->endPosBox->value() - 1);
}

void ExportHighligtingDialogController::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFileName = GUrlUtils::getDefaultDataPath() + "/" + msaui->getEditor()->getMSAObject()->getGObjectName() + "_highlighting.txt";
    config.defaultFormatId = BaseDocumentFormats::PLAIN_TEXT;
    config.fileDialogButton = ui->fileButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save...");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::PLAIN_TEXT;

    saveController = new SaveDocumentController(config, formats, this);
}

}
