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

#ifndef _U2_REPEAT_WORKER_H_
#define _U2_REPEAT_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include "FindRepeatsTask.h"

namespace U2 {

namespace LocalWorkflow {

class RepeatPrompter;
typedef PrompterBase<RepeatPrompter> RepeatPrompterBase;

class RepeatPrompter : public RepeatPrompterBase {
    Q_OBJECT
public:
    RepeatPrompter(Actor* p = 0) : RepeatPrompterBase(p) {}
protected:
    QString composeRichDoc();
};

class RepeatWorker : public BaseWorker {
    Q_OBJECT
public:
    RepeatWorker(Actor* a);
    
    virtual void init();
    virtual bool isReady();
    virtual Task* tick();
    virtual bool isDone();
    virtual void cleanup();
    
private slots:
    void sl_taskFinished();

protected:
    CommunicationChannel *input, *output;
    QString resultName,transId;
    FindRepeatsTaskSettings cfg;
    
}; 

class RepeatWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    RepeatWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new RepeatWorker(a);}
};

} // Workflow namespace
} // U2 namespace

#endif
