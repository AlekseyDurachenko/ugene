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

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Core/Log.h>
#include <U2Core/GUrl.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Misc/DialogUtils.h>

#include "ImportQualityScoresWorker.h"


/* TRANSLATOR U2::LocalWorkflow::ImportPhredQualityWorker */

namespace U2 {
namespace LocalWorkflow {

/*************************************
 * ImportPhredQualityWorkerFactory
 *************************************/
const QString ImportPhredQualityWorkerFactory::ACTOR_ID("import-phred-qualities");
const QString QUALITY_TYPE_ATTR("quality-format");

void ImportPhredQualityWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_SEQ_PORT_ID(), ImportPhredQualityWorker::tr("DNA sequences"), 
        ImportPhredQualityWorker::tr("The PHRED scores will be imported to these sequences"));
    Descriptor oud(BasePorts::OUT_SEQ_PORT_ID(), ImportPhredQualityWorker::tr("DNA sequences with imported qualities"),         
        ImportPhredQualityWorker::tr("These sequences have quality scores."));
    
    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("import.qual.in", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("import.qual.out", outM)), false /*input*/, true /*multi*/);
    
    Descriptor qualUrl(BaseAttributes::URL_IN_ATTRIBUTE().getId(), ImportPhredQualityWorker::tr("PHRED input"), 
         ImportPhredQualityWorker::tr("Path to file with PHRED quality scores."));
    Descriptor qualType(QUALITY_TYPE_ATTR, ImportPhredQualityWorker::tr("Quality format"), 
        ImportPhredQualityWorker::tr("Choose format to encode quality scores."));

    a << new Attribute(qualUrl, BaseTypes::STRING_TYPE(), true /*required*/, QString());
    a << new Attribute(qualType, BaseTypes::STRING_TYPE(), false/*required*/, DNAQuality::getDNAQualityNameByType(DNAQualityType_Sanger) );    

    Descriptor desc(ACTOR_ID, ImportPhredQualityWorker::tr("Import PHRED qualities"), 
        ImportPhredQualityWorker::tr("Add corresponding PHRED quality scores to the sequences.\nYou can use this worker to convert .fasta and .qual pair to fastq format"));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;    

    delegates[BaseAttributes::URL_IN_ATTRIBUTE().getId()] = new URLDelegate(DialogUtils::prepareDocumentsFileFilter(true), QString(), true);
    
    QVariantMap m;
    QStringList qualFormats = DNAQuality::getDNAQualityTypeNames();
    foreach( const QString& name, qualFormats  ) {
        m[name] = name;
    }
    delegates[QUALITY_TYPE_ATTR] = new ComboBoxDelegate(m);
    
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ImportPhredQualityPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ImportPhredQualityWorkerFactory());
}

/*************************************
 * ImportPhredQualityPrompter
 *************************************/
QString ImportPhredQualityPrompter::composeRichDoc() {
    
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_SEQ_PORT_ID());
    QString producerName = producer ? tr(" from <u>%1</u>").arg(producer->getLabel()) : "";
    QString qualUrl = getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId()).toString();
    QString qualSeq = (qualUrl.isEmpty() ? "<font color='red'>"+tr("unset")+"</font>" : QString("<u>%1</u>").arg(GUrl(qualUrl).fileName()) );

    QString doc = tr("Import PHRED quality scores in file %1  to the sequences %2 and send the sequences to the output.")
        .arg(qualSeq).arg(producerName);

    return doc;
}

/*************************************
 * ImportPhredQualityWorker
 *************************************/
ImportPhredQualityWorker::ImportPhredQualityWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void ImportPhredQualityWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_SEQ_PORT_ID());
    cfg.fileName = actor->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId())->getAttributeValue<QString>();
    cfg.type = DNAQuality::getDNAQualityTypeByName( actor->getParameter(QUALITY_TYPE_ATTR)->getAttributeValue<QString>() );
}

bool ImportPhredQualityWorker::isReady() {
    return (input && input->hasMessage());
}

Task* ImportPhredQualityWorker::tick() {
    while (!input->isEnded()) {
        DNASequence dna = input->get().getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<DNASequence>();
        seqObjs.append(new DNASequenceObject(dna.getName(), dna));
    }
    
    if( seqObjs.isEmpty() ) {
         algoLog.error( tr("Sequence list is empty.") );
         return NULL;
    }

    Task* t = new ImportPhredQualityScoresTask(seqObjs, cfg);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    
    return t;
}

void ImportPhredQualityWorker::sl_taskFinished() {
    ImportPhredQualityScoresTask* t = qobject_cast<ImportPhredQualityScoresTask*>(sender());
    if (t->getState() != Task::State_Finished) {
       return;
    }
    
    foreach (DNASequenceObject* obj, seqObjs) {
        DNASequence dna = obj->getDNASequence();
        QVariant v = qVariantFromValue<DNASequence>(dna);
        output->put(Message(BaseTypes::DNA_SEQUENCE_TYPE(), v));
    }
    
    if (input->isEnded()) {
        output->setEnded();
    }
    
    algoLog.trace(tr("Import of qualities is finished.") );
}

bool ImportPhredQualityWorker::isDone() {
    return !input || input->isEnded();
}

void ImportPhredQualityWorker::cleanup() {
    qDeleteAll(seqObjs);
}

} //namespace LocalWorkflow
} //namespace U2
