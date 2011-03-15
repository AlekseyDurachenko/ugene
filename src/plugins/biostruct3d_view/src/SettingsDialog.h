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

#ifndef _U2_SETTINGSDIALOG_H_
#define _U2_SETTINGSDIALOG_H_

#include "BioStruct3DGLWidget.h"
#include <QtGui/QColorDialog>
#include "ui_SettingsDialog.h"

namespace U2 {


class GlassesColorScheme
{
public:
    QColor leftEyeColor;
    QColor rightEyeColor;

    QString name;

    GlassesColorScheme()
    {
        name="";
        leftEyeColor=QColor(0, 0, 0);
        rightEyeColor = leftEyeColor;
    }

    GlassesColorScheme(QString name, QColor leftEyeColor, QColor rightEyeColor)
    {
        this->name = name;
        this->leftEyeColor = leftEyeColor;
        this->rightEyeColor = rightEyeColor;
    }
};


class BioStruct3DSettingsDialog : public QDialog, private Ui::SettingsDialog
{
    Q_OBJECT

public:
    BioStruct3DSettingsDialog();

private slots:
    void sl_setBackgroundColor();
    void sl_setSelectionColor();
    void sl_setShadingLevel();
    void sl_setRenderDetailLevel();
    void sl_setAnaglyph();

    void sl_setEyesShift();

    void sl_setGlassesColorScheme();
    void sl_setLeftEyeColor();
    void sl_setRightEyeColor();
    void sl_swapColors();


public:
    QColor getBackgroundColor()const;
    QColor getSelectionColor()const;

    QColor getLeftEyeColor()const;
    QColor getRightEyeColor()const;

    int getRenderDetailLevel()const;
    int getShadingLevel()const;
    bool getAnaglyph()const;
    int getEyesShift()const;

    void setBackgroundColor(QColor color);
    void setSelectionColor(QColor color);

    void setLeftEyeColor(QColor leftEyecolor);
    void setRightEyeColor(QColor rightEyecolor);

    void setGlassesColorScheme(QColor &leftEyeColor, QColor &rightEyeColor);
    void setGlassesColorScheme(int num);

    void setRenderDetailLevel(int renderDetailLevel);
    void setShadingLevel(int shading);

    void setAnaglyph(bool anaglyph);
    void setAnaglyphAvailability(bool anaglyph);
    void setEyesShift(int eyesShift);

    void setWidget(BioStruct3DGLWidget *glWidget);

private:
    QVariantMap state;

    BioStruct3DGLWidget *glWidget;
    QList<GlassesColorScheme> glassesColorSchemes;

    void initColorSchemes();

    QColor backgroundColor;
    QColor selectionColor;

    QColor leftEyeColor;
    QColor rightEyeColor;

    int renderDetailLevel;
    int shadingLevel;
    bool anaglyph;
    int eyesShift;
};

} // namespace

#endif // _U2_SETTINGSDIALOG_H_
