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

#include <QtGui/QMessageBox>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/Task.h>

#include <U2Algorithm/EnzymeModel.h>
#include "EnzymesIO.h"
#include "EditFragmentDialog.h"

namespace U2 {




EditFragmentDialog::EditFragmentDialog( DNAFragment& fragment, QWidget* p )
: QDialog(p), dnaFragment(fragment)
{
    
    setupUi(this);
    connect(lBluntButton, SIGNAL(toggled(bool)), SLOT(sl_onLeftBluntButtonToogled(bool)));
    connect(rBluntButton, SIGNAL(toggled(bool)), SLOT(sl_onRightBluntButtonToggled(bool)));

    static const int REGION_LEN = 10;
    
    seq = dnaFragment.getSequence();
    transl = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dnaFragment.getAlphabet());
    QByteArray data(seq.toAscii());
    transl->translate(data.data(), data.length());
    trseq = QString(data);

    if (seq.length() > REGION_LEN*2) {
        QString leftSeqPart = seq.mid(0, REGION_LEN);
        QString rightSeqPart = seq.mid(seq.length() - REGION_LEN - 1);
        seq = QString("%1 ... %2").arg(leftSeqPart).arg(rightSeqPart);

        leftSeqPart = trseq.mid(0, REGION_LEN);
        rightSeqPart = trseq.mid(trseq.length() - REGION_LEN - 1);
        trseq = QString("%1 ... %2").arg(leftSeqPart).arg(rightSeqPart);
    } 

    const DNAFragmentTerminus& leftTerm = dnaFragment.getLeftTerminus();
    const DNAFragmentTerminus& rightTerm = dnaFragment.getRightTerminus();

    lCustomOverhangEdit->setText(leftTerm.overhang);
    lCustomOverhangEdit->setMaxLength(8);
    rCustomOverhangEdit->setText(rightTerm.overhang);
    lCustomOverhangEdit->setMaxLength(8);


    leftTerm.termType == OVERHANG_TYPE_BLUNT ? lBluntButton->toggle() : lStickyButton->toggle();
    rightTerm.termType == OVERHANG_TYPE_BLUNT ? rBluntButton->toggle() : rStickyButton->toggle();

    updatePreview();

    connect(rStickyButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(rBluntButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lStickyButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lBluntButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lResetButton, SIGNAL(clicked()), SLOT(sl_onLeftResetClicked()));
    connect(rResetButton, SIGNAL(clicked()), SLOT(sl_onRightResetClicked()));
    connect(lCustomOverhangEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_updatePreview()));
    connect(rCustomOverhangEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_updatePreview()));



}

void EditFragmentDialog::accept()
{
    QByteArray lTermType = lBluntButton->isChecked() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    dnaFragment.setLeftTermType(lTermType);
    
    QByteArray rTermType = rBluntButton->isChecked() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY; 
    dnaFragment.setRightTermType(rTermType);
    
    if (lCustomOverhangBox->isChecked()) {
        if (lCustomOverhangEdit->text().isEmpty()) {
            QMessageBox::warning(this, windowTitle(), tr("Left overhang is empty. Please enter the overhang or set blunt 5'Terminus."));
            return;
        }
        if (!isValidOverhang(lCustomOverhangEdit->text())) {
            QMessageBox::warning(this, windowTitle(), tr("Invalid left overhang: unsupported alphabet!"));
            lCustomOverhangEdit->setFocus();
            lCustomOverhangEdit->setSelection(0, lCustomOverhangEdit->text().length());
            return;
        }
        
        dnaFragment.setLeftOverhang(lCustomOverhangEdit->text().toUpper().toAscii());
    }
    if (rCustomOverhangBox->isChecked()) {
        if (!isValidOverhang(rCustomOverhangEdit->text())) {
            QMessageBox::warning(this, windowTitle(), tr("Invalid right overhang: unsupported alphabet!"));
            rCustomOverhangEdit->setFocus();
            rCustomOverhangEdit->setSelection(0, rCustomOverhangEdit->text().length());
            return;
        }
        dnaFragment.setRightOverhang(rCustomOverhangEdit->text().toUpper().toAscii());
    }
    
    QDialog::accept();

}

void EditFragmentDialog::updatePreview()
{
    QString preview;
    QString invertedStr = dnaFragment.isInverted() ? tr(" (INVERTED)") : QString();

    preview += tr("Fragment of %1%2<br>").arg(dnaFragment.getSequenceDocName()).arg(invertedStr);

    const QString uLeftOverhang = lBluntButton->isChecked() ? QByteArray() : lCustomOverhangEdit->text().toUpper();
    const QString bRightOverhang = rBluntButton->isChecked() ? QByteArray() : rCustomOverhangEdit->text().toUpper();
    
    preview+=("<table cellspacing=\"10\" >");
    preview += tr("<tr> <td align=\"center\"> 5'End </td><td></td> <td align=\"center\"> 3'End </td> </tr>");

    preview += QString("<tr> <td align=\"center\" >%1</td><td align=\"center\" >%2</td><td align=\"center\" >%3</td> </tr>").
        arg(uLeftOverhang).arg(seq).arg(QString());
    preview += QString("<tr> <td align=\"center\" >%1</td><td align=\"center\" >%2</td><td align=\"center\" >%3</td> </tr>").
        arg(QString()).arg(trseq).arg(bRightOverhang);


    preview+=("</table>");
    previewEdit->setText(preview);
}

void EditFragmentDialog::sl_updatePreview()
{
    updatePreview();
}

void EditFragmentDialog::sl_customOverhangSet( const QString& text )
{
    updatePreview();
}

void EditFragmentDialog::sl_onLeftResetClicked()
{
    resetLeftOverhang();
    updatePreview();
}

void EditFragmentDialog::sl_onRightResetClicked()
{
    resetRightOverhang();
    updatePreview();

}

bool EditFragmentDialog::isValidOverhang( const QString& text )
{
    QByteArray seq(text.toAscii());
    DNAAlphabetRegistry *r = AppContext::getDNAAlphabetRegistry();
    DNAAlphabet *alph = r->findAlphabet(seq);
    if (alph == NULL ) {
        return false;
    }

    return alph->isNucleic() ? true : false;

}

void EditFragmentDialog::resetLeftOverhang()
{
    QByteArray enzymeId = dnaFragment.getLeftTerminus().enzymeId;
    if (enzymeId.isEmpty()) {
        return;
    }
    const QList<SEnzymeData>& enzymes = EnzymesIO::getDefaultEnzymesList();
    SEnzymeData enz = EnzymesIO::findEnzymeById(enzymeId, enzymes);
    
    int startPos = dnaFragment.getFragmentRegions().first().startPos - enz->cutDirect;
    int fragLen = enz->seq.length() - enz->cutDirect - enz->cutComplement;

    QByteArray overhang = dnaFragment.getSourceSequence().mid(startPos - fragLen + 1, fragLen);

    lCustomOverhangEdit->setText(overhang);
}

void EditFragmentDialog::resetRightOverhang()
{
    QByteArray enzymeId = dnaFragment.getRightTerminus().enzymeId;
    if (enzymeId.isEmpty()) {
        return;
    }
    
    const QList<SEnzymeData>& enzymes = EnzymesIO::getDefaultEnzymesList();
    SEnzymeData enz = EnzymesIO::findEnzymeById(enzymeId, enzymes);

    int startPos = dnaFragment.getFragmentRegions().last().endPos();
    int fragLen = enz->seq.length() - enz->cutComplement - enz->cutDirect;

    QByteArray overhang = dnaFragment.getSourceSequence().mid(startPos, fragLen);

    rCustomOverhangEdit->setText(overhang);
}

void EditFragmentDialog::sl_onLeftBluntButtonToogled( bool toogle )
{
    bool enabled = lCustomOverhangBox->isChecked() && !toogle;
    lCustomOverhangEdit->setEnabled(enabled);
    lResetButton->setEnabled(enabled);
}

void EditFragmentDialog::sl_onRightBluntButtonToggled( bool toggle )
{
    bool enabled = rCustomOverhangBox->isChecked() && !toggle;
    rCustomOverhangEdit->setEnabled(enabled);
    rResetButton->setEnabled(enabled);
}


} // namespace
