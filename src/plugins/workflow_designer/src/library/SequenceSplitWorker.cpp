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

#include "SequenceSplitWorker.h"
#include "GenericReadActor.h"

#include <U2Core/AppContext.h>

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequence.h>

#include <U2Core/TaskSignalMapper.h>
#include <U2Core/FailTask.h>
#include <U2Core/MultiTask.h>

#include <U2Core/TextUtils.h>

#include <U2Lang/ConfigurationEditor.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>

#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>

namespace U2 {
namespace LocalWorkflow {

const QString SequenceSplitWorkerFactory::ACTOR("extract-annotated-sequence");

const static QString REGIONED_SEQ_TYPE("regioned.sequence");


const static QString TRANSLATE_ATTR( "translate" );
const static QString COMPLEMENT_ATTR( "complement" );
const static QString EXTEND_LEFT_ATTR( "extend-left" );
const static QString EXTEND_RIGHT_ATTR( "extend-right" );
const static QString GAP_LENGTH_ATTR( "merge-gap-length" );

QString SequenceSplitPromter::composeRichDoc() {
    IntegralBusPort * input = qobject_cast<IntegralBusPort *> ( target->getPort(BasePorts::IN_SEQ_PORT_ID()) );
    Actor * seqProducer = input->getProducer( BaseSlots::DNA_SEQUENCE_SLOT().getId() );
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString seqProducerText = tr("from <u>%1</u>").arg(seqProducer ? seqProducer->getLabel() : unsetStr);
    
    

    //translate or not?
    bool translate = getParameter( TRANSLATE_ATTR ).toBool();
    QString translateText = translate ? tr( "<u>translate</u> it if annotation marks translated subsequence, " ) : "";

    //complement or not?
    bool complement = getParameter( COMPLEMENT_ATTR ).toBool();
    QString complementText = complement ? tr( "make it <u>reverse-complement</u> if annotation is located on complement strand, " ) : "";

    //expand
    QString expandText;
    int expandLeft = getParameter( EXTEND_LEFT_ATTR ).toInt();
    int expandRight = getParameter( EXTEND_RIGHT_ATTR ).toInt();
    if( expandLeft ) {
        expandText += tr("expand it to left with <u>%1</u>, ").arg(expandLeft);
    }
    if( expandRight ) {
        expandText += tr("expand it to right with <u>%1</u>").arg(expandRight);
    }
    if( !expandRight && expandLeft ){
        expandText.remove( expandText.size()-1, 2);
    }

    //merge result
    QString doc = tr("Extract each annotated sequence region %5 %2 %3%4")
        //.arg(filterText)
        .arg(complementText) 
        .arg(translateText)
        .arg(expandText)
        .arg(seqProducerText);
    doc.remove( QRegExp("[\\,\\s]*$") ); //remove all commas and spaces from the end;
    doc.append(".");
    return doc;

}

void SequenceSplitWorker::init() {
    seqPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
    outPort = ports.value(BasePorts::OUT_SEQ_PORT_ID());
}

bool SequenceSplitWorker::isReady() {
    return seqPort->hasMessage();
}

Task * SequenceSplitWorker::tick() {
    Message inputMessage = getMessageAndSetupScriptValues(seqPort);
    
    cfg.translate = actor->getParameter( TRANSLATE_ATTR )->getAttributeValue<bool>();
    cfg.complement = actor->getParameter( COMPLEMENT_ATTR )->getAttributeValue<bool>();
    cfg.extLeft = actor->getParameter( EXTEND_LEFT_ATTR )->getAttributeValue<int>();
    cfg.extRight = actor->getParameter( EXTEND_RIGHT_ATTR )->getAttributeValue<int>();
    cfg.gapLength = actor->getParameter( GAP_LENGTH_ATTR )->getAttributeValue<int>();
    cfg.gapSym = '-'; //FIXME
    QVariantMap qm = inputMessage.getData().toMap();
    
    DNASequence inputSeq = qm.value( BaseSlots::DNA_SEQUENCE_SLOT().getId() ).value<DNASequence>();
    inputAnns = qVariantValue<QList<SharedAnnotationData> >( qm.value(BaseSlots::ANNOTATION_TABLE_SLOT().getId()) );
    
    bool noSeq = inputSeq.isNull();
    bool noAnns = inputAnns.isEmpty();
    if( noSeq || noAnns ) {
        //if( failFast ) {
            if( noSeq ) {
                return new FailTask( tr("No sequence provided to split worker") );
            } else if( noAnns ) {
                return new FailTask( tr("Nothing to extract. No annotations provided to split worker") );
            } else {
                assert(false);
            }
       // } 
        
        outPort->put( Message(BaseTypes::ANNOTATION_TABLE_TYPE(), QVariant()) );
        if( seqPort->isEnded() ) {
            outPort->setEnded();
        }
        return 0;
    } 

    ssTasks.clear();

    foreach( SharedAnnotationData ann, inputAnns ) {   
        Task * t = new ExtractAnnotatedRegionTask( inputSeq, ann, cfg );
        ssTasks.push_back(t);
    }
    if( ssTasks.isEmpty() ) {
        return new FailTask(tr("Nothing to extract: no sequence region match the constraints"));
    }
    
    Task * t = new MultiTask( "Sequence split tasks", ssTasks );
    connect( new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onTaskFinished(Task*)) );
    return t;
}

void SequenceSplitWorker::cleanup() {
}

bool SequenceSplitWorker::isDone() {
    return seqPort->isEnded();
}

void SequenceSplitWorker::sl_onTaskFinished( Task * ) {
    int count = 0;

    foreach( Task * t, ssTasks ) {
        ExtractAnnotatedRegionTask * ssT = qobject_cast<ExtractAnnotatedRegionTask *>(t);
        assert( ssT );

        DNASequence resSeq = ssT->getResultedSequence();
        SharedAnnotationData resAnn = ssT->getResultedAnnotation();
        QString name = resSeq.getName() + "|" + resAnn->name + "|" + QString::number(++count);
        resSeq.info[DNAInfo::ID] = name;

        QVariant vSeq = qVariantFromValue<DNASequence>(resSeq);

        QList<SharedAnnotationData> annToPut;
        annToPut.push_back( resAnn );
        QVariant vAnns = qVariantFromValue<QList<SharedAnnotationData> >(annToPut);

        QVariantMap messageData;
        messageData[ BaseSlots::DNA_SEQUENCE_SLOT().getId() ] = vSeq;
        messageData[ BaseSlots::ANNOTATION_TABLE_SLOT().getId() ] = vAnns;

        DataTypePtr messageType = WorkflowEnv::getDataTypeRegistry()->getById( REGIONED_SEQ_TYPE );
        if( outPort ) {
            outPort->put( Message(messageType, messageData) );
        }
    }
    if( seqPort->isEnded() ) {
        outPort->setEnded();
    }
}

void SequenceSplitWorkerFactory::init() {
    QList<PortDescriptor*> portDescs; 
    QList<Attribute*> attribs;

    //accept sequence and annotated regions as input
    QMap<Descriptor, DataTypePtr> inputMap;
    inputMap[ BaseSlots::DNA_SEQUENCE_SLOT() ] = BaseTypes::DNA_SEQUENCE_TYPE();
    inputMap[ BaseSlots::ANNOTATION_TABLE_SLOT() ] = BaseTypes::ANNOTATION_TABLE_TYPE();

    DataTypePtr inSet( new MapDataType(Descriptor(REGIONED_SEQ_TYPE), inputMap) );
    DataTypeRegistry * dr = WorkflowEnv::getDataTypeRegistry();
    assert(dr);
    dr->registerEntry( inSet );

    { //Create input port descriptors 
        Descriptor seqDesc( BasePorts::IN_SEQ_PORT_ID(), SequenceSplitWorker::tr("Input sequence"), 
            SequenceSplitWorker::tr("A sequence which will be split into annotated regions.") );
        Descriptor outDesc( BasePorts::OUT_SEQ_PORT_ID(), SequenceSplitWorker::tr("Annotated regions"), 
            SequenceSplitWorker::tr("Resulted subsequences, translated and complemented according to corresponding annotations.") );
        
        portDescs << new PortDescriptor( seqDesc, inSet, /*input*/ true );
        portDescs << new PortDescriptor( outDesc, inSet, /*input*/false, /*multi*/true );
    }

    { //Create attributes descriptors    
        Descriptor translateDesc( TRANSLATE_ATTR,
                                  SequenceSplitWorker::tr("Translate"),
                                  SequenceSplitWorker::tr("Translate the annotated regions.") );
        Descriptor complementDesc( COMPLEMENT_ATTR,
                                   SequenceSplitWorker::tr("Complement"),
                                   SequenceSplitWorker::tr("Complement the annotated regions if the corresponding annotation is located on complement strand.") );
        Descriptor extendLeftDesc( EXTEND_LEFT_ATTR,
                                   SequenceSplitWorker::tr("Extend left"),
                                   SequenceSplitWorker::tr("Extend the resulted regions to left") );
        Descriptor extendRightDesc( EXTEND_RIGHT_ATTR,
                                    SequenceSplitWorker::tr("Extend right"),
                                    SequenceSplitWorker::tr("Extend the resulted regions to right") );
        Descriptor gapLengthDesc( GAP_LENGTH_ATTR, 
                                  SequenceSplitWorker::tr("Gap length"),
                                  SequenceSplitWorker::tr("Insert gap of specified length between merged locations of annotation.") );

        attribs << new Attribute( translateDesc, BaseTypes::BOOL_TYPE(), /*required*/ false, QVariant(false) );
        attribs << new Attribute( complementDesc, BaseTypes::BOOL_TYPE(), /*required*/ false, QVariant(true) );
        attribs << new Attribute( extendLeftDesc, BaseTypes::NUM_TYPE(), /*required*/ false, QVariant(0) );
        attribs << new Attribute( extendRightDesc, BaseTypes::NUM_TYPE(), /*required*/ false, QVariant(0) );
        attribs << new Attribute( gapLengthDesc, BaseTypes::NUM_TYPE(), false, QVariant(1) );
    }

    Descriptor desc( SequenceSplitWorkerFactory::ACTOR, 
                     SequenceSplitWorker::tr("Get sequences by annotations"), 
                     SequenceSplitWorker::tr("Creates sequences from annotated regions of input sequence") );
    ActorPrototype * proto = new IntegralBusActorPrototype( desc, portDescs, attribs );

    //create delegates for attribute editing
    QMap<QString, PropertyDelegate *> delegates;   
    {
        QVariantMap eMap; eMap["minimum"] = (0); eMap["maximum"] = (INT_MAX);
        delegates[EXTEND_LEFT_ATTR] = new SpinBoxDelegate( eMap );
        delegates[EXTEND_RIGHT_ATTR] = new SpinBoxDelegate( eMap );
        delegates[GAP_LENGTH_ATTR] = new SpinBoxDelegate( eMap );
    }

    proto->setEditor( new DelegateEditor(delegates) );

//    proto->setIconPath( "" );
    proto->setPrompter( new SequenceSplitPromter() );
    WorkflowEnv::getProtoRegistry()->registerProto( BaseActorCategories::CATEGORY_BASIC(), proto );

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById( LocalDomainFactory::ID );
    localDomain->registerEntry( new SequenceSplitWorkerFactory() );
}

Worker * SequenceSplitWorkerFactory::createWorker( Actor * a ) {
    return new SequenceSplitWorker(a);
}

} //ns LocalWorkflow
} //ns U2
