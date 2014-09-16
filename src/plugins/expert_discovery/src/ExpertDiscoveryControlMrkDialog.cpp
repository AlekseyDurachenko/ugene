/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2014 UniPro <ugene@unipro.ru>
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

#include <QtCore/qglobal.h>
#if (QT_VERSION < 0x050000) //Qt 5
#include <QtGui/QMessageBox>
#else
#include <QtWidgets/QMessageBox>
#endif

#include <U2Core/GObjectTypes.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "ExpertDiscoveryControlMrkDialog.h"

namespace U2 {

ExpertDiscoveryControlMrkDialog::ExpertDiscoveryControlMrkDialog(QWidget *parent)
: QDialog(parent){

    setupUi(this);
    new HelpButton(this, buttonBox, "7668033");

    connect(openFirstButton, SIGNAL(clicked()), SLOT(sl_openFirstFile()));

    filter = DialogUtils::prepareFileFilter("Markup files", QStringList() << "xml" << "gb");

}

void ExpertDiscoveryControlMrkDialog::accept(){
    Q_ASSERT(firstFileEdit);

    firstFileName = firstFileEdit->text();


    if (!firstFileName.isEmpty()) {
        QDialog::accept();
    }else {
        QMessageBox mb(QMessageBox::Critical, tr("Select files"), tr("Select files for ExpertDiscovery"));
        mb.exec();
    }
}

void ExpertDiscoveryControlMrkDialog::sl_openFirstFile(){
    LastUsedDirHelper lod("ExpertDiscovery control sequences markup file");
    lod.url = U2FileDialog::getOpenFileName(NULL, tr("Open control sequences markup file"), lod.dir, filter);

    Q_ASSERT(firstFileEdit);
    if (!lod.url.isEmpty()) {
        firstFileEdit->setText(lod.url);
    }
}

}//namespace


