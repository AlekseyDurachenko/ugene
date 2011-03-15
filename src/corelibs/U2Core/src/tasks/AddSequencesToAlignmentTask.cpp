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

#include "AddSequencesToAlignmentTask.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>

namespace U2 {


AddSequencesToAlignmentTask::AddSequencesToAlignmentTask( MAlignmentObject* obj, const QString& fileWithSequencesUrl )
: Task("Add sequences to alignment task", TaskFlag_NoRun), maObj(obj)
{
    assert(!fileWithSequencesUrl.isEmpty());
    QList<DocumentFormat*> detectedFormats = DocumentUtils::detectFormat(fileWithSequencesUrl);    
    if (!detectedFormats.isEmpty()) {
        IOAdapterFactory* factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        DocumentFormat* format = detectedFormats.first();
        loadTask = new LoadDocumentTask(format->getFormatId(), fileWithSequencesUrl, factory);
        addSubTask(loadTask);
    } else {
        setError("Unknown format");
    }
}

QList<Task*> AddSequencesToAlignmentTask::onSubTaskFinished( Task* subTask )
{
    QList<Task*> subTasks;

    propagateSubtaskError();
    if ( (subTask != loadTask )|| (isCanceled()) || (hasErrors()) ) {
        return subTasks;
    }

    Document* doc = loadTask->getDocument();
    QList<GObject*> seqObjects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    
    foreach(GObject* obj, seqObjects) {
        DNASequenceObject* dnaObj = qobject_cast<DNASequenceObject*>(obj);
        assert(dnaObj != NULL);
        DNAAlphabet* dnaAl = dnaObj->getAlphabet();
        if (maObj->getAlphabet()->getType() == dnaAl->getType()) {
            maObj->addRow(dnaObj->getDNASequence());
        } else {
            stateInfo.setError(tr("Sequence %1 from %2 has different alphabet").arg(dnaObj->getGObjectName()).arg(loadTask->getDocument()->getURLString()));
        }
    }
    
    return subTasks;

}

Task::ReportResult AddSequencesToAlignmentTask::report()
{
    return ReportResult_Finished;
}


}