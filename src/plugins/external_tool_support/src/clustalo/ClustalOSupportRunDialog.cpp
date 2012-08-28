/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2012 UniPro <ugene@unipro.ru>
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

#include "ClustalOSupportRunDialog.h"
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Gui/DialogUtils.h>
#include <U2Core/DocumentUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Core/GUrlUtils.h>

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QToolButton>

namespace U2 {
////////////////////////////////////////
//ClustalOSupportRunDialog
ClustalOSupportRunDialog::ClustalOSupportRunDialog(const MAlignment& _ma, ClustalOSupportTaskSettings& _settings, QWidget* _parent) :
        QDialog(_parent), ma(_ma), settings(_settings)
{
    setupUi(this);
    inputGroupBox->setVisible(false);
    connect(this->cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(this->alignButton,SIGNAL(clicked()),this,SLOT(sl_align()));
    numberOfCPUSpinBox->setMaximum(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
    numberOfCPUSpinBox->setValue(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
}

void ClustalOSupportRunDialog::sl_align(){
    if(iterationNumberCheckBox->isChecked()){
        settings.numIterations = iterationNumberSpinBox->value();
    }
    if(maxGTIterationsCheckBox->isChecked()){
        settings.maxGuidetreeIterations = maxGTIterationsSpinBox->value();
    }
    if(maxHMMIterationsCheckBox->isChecked()){
        settings.maxHMMIterations = maxHMMIterationsSpinBox->value();
    }
    settings.setAutoOptions = setAutoCheckBox->isChecked();
    settings.numberOfProcessors = numberOfCPUSpinBox->value();
    accept();
}

////////////////////////////////////////
//ClustalOWithExtFileSpecifySupportRunDialog
ClustalOWithExtFileSpecifySupportRunDialog::ClustalOWithExtFileSpecifySupportRunDialog(ClustalOSupportTaskSettings& _settings, QWidget* _parent) :
        QDialog(_parent), settings(_settings)
{
    setupUi(this);
    connect(inputFilePathButton, SIGNAL(clicked()), SLOT(sl_inputPathButtonClicked()));
    connect(outputFilePathButton, SIGNAL(clicked()), SLOT(sl_outputPathButtonClicked()));

    connect(this->cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(this->alignButton,SIGNAL(clicked()),this,SLOT(sl_align()));

    numberOfCPUSpinBox->setMaximum(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
    numberOfCPUSpinBox->setValue(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
}
void ClustalOWithExtFileSpecifySupportRunDialog::sl_inputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = QFileDialog::getOpenFileName(this, tr("Open an alignment file"), lod.dir, 
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_ALIGNMENT, true));
    if (lod.url.isEmpty()) {
        return;
    }
    inputFileLineEdit->setText(lod.url);
}

void ClustalOWithExtFileSpecifySupportRunDialog::sl_outputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = QFileDialog::getSaveFileName(this, tr("Save an multiple alignment file"), lod.dir);
    if (lod.url.isEmpty()) {
        return;
    }
    outputFileLineEdit->setText(lod.url);
    buildMultipleAlignmentUrl(lod.url);
}

void ClustalOWithExtFileSpecifySupportRunDialog::buildMultipleAlignmentUrl(const GUrl &alnUrl) {
    GUrl url = GUrlUtils::rollFileName(alnUrl.dirPath() + "/" + alnUrl.baseFileName()+ ".aln", DocumentUtils::getNewDocFileNameExcludesHint());
    outputFileLineEdit->setText(url.getURLString());
}
void ClustalOWithExtFileSpecifySupportRunDialog::sl_align(){
    if(iterationNumberCheckBox->isChecked()){
        settings.numIterations = iterationNumberSpinBox->value();
    }
    if(maxGTIterationsCheckBox->isChecked()){
        settings.maxGuidetreeIterations = maxGTIterationsSpinBox->value();
    }
    if(maxHMMIterationsCheckBox->isChecked()){
        settings.maxHMMIterations = maxHMMIterationsSpinBox->value();
    }
    settings.setAutoOptions = setAutoCheckBox->isChecked();
    settings.numberOfProcessors = numberOfCPUSpinBox->value();
    if(inputFileLineEdit->text().isEmpty()){
        QMessageBox::information(this, tr("Kalign with Align"),
            tr("Input file is not set!") );
        }else if(outputFileLineEdit->text().isEmpty()){
            QMessageBox::information(this, tr("Kalign with Align"),
                tr("Output file is not set!") );
        }
        else{
            settings.outputFilePath=outputFileLineEdit->text();
            settings.inputFilePath=inputFileLineEdit->text();
            QDialog::accept();
            }

}

}//namespace
