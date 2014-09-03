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

#include <QtCore/QFile>

#if (QT_VERSION < 0x050000) //Qt 5
#include <QtGui/QDialogButtonBox>
#include <QtGui/QStyleFactory>
#else
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QStyleFactory>
#endif

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CleanupFileStorageTask.h>
#include <U2Core/Log.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/U2FileDialog.h>

#include "UserApplicationsSettingsGUIController.h"

namespace U2
{
#define TRANSMAP_FILE_NAME "translations.txt"

UserApplicationsSettingsPageController::UserApplicationsSettingsPageController(QObject* p) 
: AppSettingsGUIPageController(tr("General"), APP_SETTINGS_USER_APPS, p)
{
    translations.insert(tr("Autodetection"), "");
    QFile file( QString(PATH_PREFIX_DATA)+ ":" + TRANSMAP_FILE_NAME );

    if(!file.exists() || !file.open(QIODevice::ReadOnly)){
        coreLog.error(tr("File with translations is not found: %1").arg(TRANSMAP_FILE_NAME));
        translations.insert("English", "transl_en");
        return;
    } 
    QTextStream in(&file);
    in.setCodec("UTF-8");
    in.setAutoDetectUnicode(true);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(line.length()==0 || line.startsWith("#")) {
            continue;
        }
        QStringList fields = line.split("|");
        if (fields.size()!=2) {
            coreLog.error(tr("Illegal entry in translations file: %1").arg(line));
            continue;
        }
        translations.insert(fields[0], fields[1].trimmed());
    }
    file.close();
}


AppSettingsGUIPageState* UserApplicationsSettingsPageController::getSavedState() {
    UserApplicationsSettingsPageState* state = new UserApplicationsSettingsPageState();
    UserAppsSettings* s = AppContext::getAppSettings()->getUserAppsSettings();
    state->webBrowserUrl = s->getWebBrowserURL();
    state->translFile = s->getTranslationFile();
    state->useDefaultWebBrowser = s->useDefaultWebBrowser();
    state->openLastProjectFlag = s->openLastProjectAtStartup();
    state->askToSaveProject = s->getAskToSaveProject();
    state->style = s->getVisualStyle();
    state->downloadsDirPath = s->getDownloadDirPath();
    state->temporaryDirPath = s->getUserTemporaryDirPath();
    state->enableStatistics = s->isStatisticsCollectionEnabled();
    state->tabbedWindowLayout = s->tabbedWindowLayout();
    state->resetSettings = s->resetSettings();
    state->fileStorageDirPath = s->getFileStorageDir();
    return state;
}

void UserApplicationsSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    UserApplicationsSettingsPageState* state = qobject_cast<UserApplicationsSettingsPageState*>(s);
    UserAppsSettings* st = AppContext::getAppSettings()->getUserAppsSettings();
    st->setWebBrowserURL(state->webBrowserUrl);
    st->setTranslationFile(state->translFile);
    st->setUseDefaultWebBrowser(state->useDefaultWebBrowser);
    st->setOpenLastProjectAtStartup(state->openLastProjectFlag);
    st->setAskToSaveProject(state->askToSaveProject);
    st->setDownloadDirPath(state->downloadsDirPath);
    st->setEnableCollectingStatistics(state->enableStatistics);
    st->setTabbedWindowLayout(state->tabbedWindowLayout);
    st->setResetSettings(state->resetSettings);
    
    QStyle* style = QStyleFactory::create(state->style);
    if (style!=NULL) {
        QApplication::setStyle(style);
        st->setVisualStyle(state->style);
    }


    TmpDirChecker tmpDirChecker;
    if (!tmpDirChecker.checkPath(state->temporaryDirPath)) {
        uiLog.error("You do not have permission to write to \"" + state->temporaryDirPath + "\" directory\"");
    }
    else {
        st->setUserTemporaryDirPath(state->temporaryDirPath);
    }

    if (!tmpDirChecker.checkPath(state->fileStorageDirPath)) {
        uiLog.error("You do not have permission to write to \"" + state->fileStorageDirPath + "\" directory\"");
    }
    else {
        st->setFileStorageDir(state->fileStorageDirPath);
    }
}

AppSettingsGUIPageWidget* UserApplicationsSettingsPageController::createWidget(AppSettingsGUIPageState* state) {
    UserApplicationsSettingsPageWidget* r = new UserApplicationsSettingsPageWidget(this);
    r->setState(state);
    return r;
}

const QString UserApplicationsSettingsPageController::helpPageId = QString("4227262");

UserApplicationsSettingsPageWidget::UserApplicationsSettingsPageWidget(UserApplicationsSettingsPageController* ctrl) {
    setupUi(this);
    connect(webBrowserButton, SIGNAL(clicked()), SLOT(sl_wbURLClicked()));
    connect(langButton, SIGNAL(clicked()), SLOT(sl_transFileClicked()));
    connect(browseDownloadDirButton, SIGNAL(clicked()), SLOT(sl_browseButtonClicked()));
    connect(browseTmpDirButton,SIGNAL(clicked()),SLOT(sl_browseTmpDirButtonClicked()));
    connect(browseFileStorageButton,SIGNAL(clicked()),SLOT(sl_browseFileStorageButtonClicked()));
    connect(cleanupStorageButton,SIGNAL(clicked()),SLOT(sl_cleanupStorage()));
    
    QMapIterator<QString, QString> it(ctrl->translations);
    while (it.hasNext()) {
        it.next();
        langCombo->addItem(it.key(), it.value());
    }
    styleCombo->addItems(QStyleFactory::keys());
}

void UserApplicationsSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    UserApplicationsSettingsPageState* state = qobject_cast<UserApplicationsSettingsPageState*>(s);
    defaultWebBrowser->setChecked(state->useDefaultWebBrowser);
    customWebBrowser->setChecked(!state->useDefaultWebBrowser);
    webBrowserButton->setEnabled(!state->useDefaultWebBrowser);
    webBrowserEdit->setEnabled(!state->useDefaultWebBrowser);
    webBrowserEdit->setText(state->webBrowserUrl);
    downloadsDirPathEdit->setText(state->downloadsDirPath);
    tmpDirPathEdit->setText(state->temporaryDirPath);
    enableStatisticsEdit->setChecked(state->enableStatistics);
    tabbedButton->setChecked(state->tabbedWindowLayout);
    mdiButton->setChecked(!state->tabbedWindowLayout);
    fileStorageDirPathEdit->setText(state->fileStorageDirPath);
    
    int idx = langCombo->findData(state->translFile);
    if (idx < 0) {
        idx = langCombo->count();
        langCombo->addItem(tr("Custom translation [%1]").arg(state->translFile), state->translFile);
    }
    langCombo->setCurrentIndex(idx);
    
    int styleIdx = styleCombo->findText(state->style, Qt::MatchFixedString); //case insensitive
    if (styleIdx!=-1) {
        styleCombo->setCurrentIndex(styleIdx);
    }

    autoOpenProjectBox->setChecked(state->openLastProjectFlag);
    askToSaveProject->addItem(tr("Ask to save new project on exit"), QDialogButtonBox::NoButton);
    askToSaveProject->addItem(tr("Don't save new project on exit"), QDialogButtonBox::No);
    askToSaveProject->addItem(tr("Always save new project on exit"), QDialogButtonBox::Yes);
    askToSaveProject->setCurrentIndex(askToSaveProject->findData(state->askToSaveProject));

    resetSettingsBox->setChecked(state->resetSettings);
}

AppSettingsGUIPageState* UserApplicationsSettingsPageWidget::getState(QString& err) const {
    UserApplicationsSettingsPageState* state = new UserApplicationsSettingsPageState();
    if (defaultWebBrowser->isChecked()){
        state->useDefaultWebBrowser=true;
    } else {
        QString wbUrl = webBrowserEdit->text();
        QFile wbFile(wbUrl);
        if (!wbFile.exists()) {
            webBrowserEdit->setFocus();
            err = tr("file_not_exists");
            return NULL;
        }		
        state->webBrowserUrl = wbUrl;
        state->useDefaultWebBrowser=false;
    }
    state->translFile = langCombo->itemData(langCombo->currentIndex()).toString();
    state->openLastProjectFlag = autoOpenProjectBox->isChecked();
    state->askToSaveProject = askToSaveProject->itemData(askToSaveProject->currentIndex()).toInt();
    state->style = styleCombo->currentText();
    state->downloadsDirPath = downloadsDirPathEdit->text();
    state->temporaryDirPath = tmpDirPathEdit->text();
    state->enableStatistics = enableStatisticsEdit->isChecked();
    state->tabbedWindowLayout = tabbedButton->isChecked();
    state->resetSettings = resetSettingsBox->isChecked();
    state->fileStorageDirPath = fileStorageDirPathEdit->text();

    return state;
}

void UserApplicationsSettingsPageWidget::sl_wbURLClicked() {
    QString file = U2FileDialog::getOpenFileName(this, tr("select_wb_file_title"), QString(), 0);
    if (!file.isEmpty()) {
        webBrowserEdit->setText(file);
    }
}

void UserApplicationsSettingsPageWidget::sl_transFileClicked() {
    QString file = U2FileDialog::getOpenFileName(this, tr("select_trans_file_title"), QString(), 0);
    if (!file.isEmpty()) {
        QFileInfo fi(file);
        int idx = langCombo->findData(fi.baseName());
        if (idx < 0) {
            idx = langCombo->findData(fi.fileName());
        }
        if (idx < 0) {
            idx = langCombo->findData(fi.absoluteFilePath());
        }
        if (idx < 0) {
            idx = langCombo->count();
            langCombo->addItem(tr("Custom translation [%1]").arg(file), fi.absoluteFilePath());
        }
        langCombo->setCurrentIndex(idx);
    }
}

void UserApplicationsSettingsPageWidget::sl_browseButtonClicked()
{
    QString path = downloadsDirPathEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Directory"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        downloadsDirPathEdit->setText(dir);
    }
    
}

void UserApplicationsSettingsPageWidget::sl_browseTmpDirButtonClicked()
{

    QString path = tmpDirPathEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Directory"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        tmpDirPathEdit->setText(dir);
    }

}

void UserApplicationsSettingsPageWidget::sl_browseFileStorageButtonClicked()
{

    QString path = fileStorageDirPathEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Directory"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        fileStorageDirPathEdit->setText(dir);
    }

}

void UserApplicationsSettingsPageWidget::sl_cleanupStorage()
{
    CleanupFileStorageTask *t = new CleanupFileStorageTask();
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

} //namespace
