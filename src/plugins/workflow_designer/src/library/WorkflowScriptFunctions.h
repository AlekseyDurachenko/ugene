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

#ifndef _WORKFLOW_SCRIPT_FUNC_H_
#define _WORKFLOW_SCRIPT_FUNC_H_
#include <QtScript>

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNAAlphabet.h>

#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/MAlignment.h>

#include <U2Core/MSAUtils.h>


namespace U2 {
namespace LocalWorkflow {

// ================== Sequence =======================
static QScriptValue getSubsequence(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }
    else {
        int beg, end;
        QScriptValue calee = ctx->callee();
        DNASequence dna = ctx->argument(0).toVariant().value<DNASequence>();

        try {
            beg = ctx->argument(1).toInt32(); 
            end = ctx->argument(2).toInt32();            
        }
        catch(...) {
            return ctx->throwError(QObject::tr("Incorrect arguments"));
        }
        
        if(dna.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        DNASequence subsequence(dna.seq.mid(beg, end - beg),dna.alphabet);
        calee.setProperty("res", engine->newVariant(QVariant::fromValue<DNASequence>(subsequence)));
        return calee.property("res");
    }
}

static QScriptValue concatSequence(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }
    QByteArray result;
    DNAAlphabet *alph = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    for(int i = 0; i < ctx->argumentCount();i++) {
        DNASequence dna = ctx->argument(0).toVariant().value<DNASequence>();
        if(dna.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        if(dna.alphabet->isAmino()) {
            alph = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::AMINO_DEFAULT());
        }
        result.append(dna.seq);
    }
    DNASequence concatenation("joined sequence", result, alph);
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<DNASequence>(concatenation)));
    return calee.property("res");
}

static QScriptValue complement(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence dna = ctx->argument(0).toVariant().value<DNASequence>();
    if(dna.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    if( !dna.alphabet->isNucleic() ) {
        return ctx->throwError(QObject::tr("Alphabet must be nucleotide"));
    }

    DNATranslation *complTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dna.alphabet);
    complTT->translate(dna.seq.data(),dna.seq.size(), dna.seq.data(), dna.seq.size());

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<DNASequence>(dna)));
    return calee.property("res");
}

static QScriptValue sequenceSize(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    int size = seq.length();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(size));
    return calee.property("res");
}

static QScriptValue translate(QScriptContext *ctx, QScriptEngine *engine){
    if(ctx->argumentCount() < 1 && ctx->argumentCount() > 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    bool aminoSeq = seq.alphabet->isAmino();
    if( aminoSeq ) {
        return ctx->throwError(QObject::tr("Alphabet must be nucleotide"));        
    }
    int offset = 0;
    if(ctx->argumentCount() == 2) {
        offset = ctx->argument(1).toInt32();
        if(offset < 0 && offset > 2) {
            return ctx->throwError(QObject::tr("Offset must be from interval [0,2]"));
        }
    }

    DNATranslationType dnaTranslType = (seq.alphabet->getType() == DNAAlphabet_NUCL) ? DNATranslationType_NUCL_2_AMINO : DNATranslationType_RAW_2_AMINO;
    QList<DNATranslation*> aminoTTs = AppContext::getDNATranslationRegistry()->lookupTranslation( seq.alphabet, dnaTranslType );
    if( aminoTTs.isEmpty() ) {
        return ctx->throwError(QObject::tr("Translation table is empty"));
    }

    DNATranslation *aminoT = aminoTTs.first();
    aminoT->translate(seq.seq.data() + offset, seq.length() - offset, seq.seq.data(), seq.length());
    seq.seq.resize(seq.length()/3);

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<DNASequence>(seq)));
    return calee.property("res");
}

static QScriptValue charAt(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    if(!ctx->argument(1).isNumber()) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }
    int position = ctx->argument(1).toInt32();
    if(position > seq.length() || position < 0) {
        return ctx->throwError(QObject::tr("Position is out of range"));
    }

    QString c(seq.seq.at(position));
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(c));
    return calee.property("res");
}

static QScriptValue sequenceName(QScriptContext *ctx, QScriptEngine *) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    QString name = seq.getName();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", name);
    return calee.property("res");
}

static QScriptValue alphabetType(QScriptContext *ctx, QScriptEngine *engine)  {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    QString alph = seq.alphabet->getName();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(alph));
    return calee.property("res");
}

static QScriptValue sequenceFromText(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString text = ctx->argument(0).toString();
    //QString name = ctx->argument(1).toString();
    DNASequence seq(text.toAscii());
    DNAAlphabetRegistry* r = AppContext::getDNAAlphabetRegistry();
    seq.alphabet = r->findAlphabet(seq.seq);
    if(seq.alphabet->getId() == BaseDNAAlphabetIds::RAW()) {
        seq.seq = QByteArray();
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<DNASequence>(seq)));
    return calee.property("res");

}

static QScriptValue isAmino(QScriptContext *ctx, QScriptEngine *) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    bool isAmino = seq.alphabet->isAmino();
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", isAmino);
    return calee.property("res");
}

static QScriptValue getMinimumQuality(QScriptContext *ctx, QScriptEngine *) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    QByteArray codes = seq.quality.qualCodes;

    int minQual = 100;
    for(int i = 0; i < codes.size();i++) {
        if(minQual > codes[i]) {
            minQual = codes[i];
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", minQual);
    return calee.property("res");
}

static QScriptValue hasQuality(QScriptContext *ctx, QScriptEngine *) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    QByteArray codes = seq.quality.qualCodes;
    bool isHasQuality = !codes.isEmpty();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", isHasQuality);
    return calee.property("res");
}


// ================== Alignment =======================

static QScriptValue getSequenceFromAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2 && ctx->argumentCount() != 4) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MAlignment align = ctx->argument(0).toVariant().value<MAlignment>();
    if(align.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    if(!ctx->argument(1).isNumber()) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }
    int row = ctx->argument(1).toVariant().toInt();
    if(row < 0 || row > align.getNumRows()) {
        return ctx->throwError(QObject::tr("Row is out of range"));
    }

    MAlignmentRow aRow = align.getRow(row);
    aRow.simplify();
    QByteArray arr = aRow.toByteArray(aRow.getCoreLength());
    if(ctx->argumentCount() == 4) {
        int beg = ctx->argument(1).toInt32();
        int len = ctx->argument(2).toInt32();
        if(beg <= 0 || beg > arr.length()) {
            return ctx->throwError(QObject::tr("Offset is out of range"));
        }
        if(len <= 0 || (beg + len) > arr.length()) {
            return ctx->throwError(QObject::tr("Length is out of range"));
        }
        arr.mid(beg,len);
    }
    DNASequence seq(aRow.getName(),arr,align.getAlphabet());

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<DNASequence>(seq)));
    return calee.property("res");
}

static QScriptValue findInAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString name;
    DNASequence seq;
    MAlignment aln = ctx->argument(0).toVariant().value<MAlignment>();
    if(aln.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    int row = 0;
    QScriptValue val = ctx->argument(1);
    if(val.isString()) {
        name = val.toString();
        row = aln.getRowNames().indexOf(name);
    }
    else {
        seq = val.toVariant().value<DNASequence>();
        if(seq.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        foreach(const DNASequence& alnSeq, MSAUtils::ma2seq(aln,true)) {
            if(alnSeq.seq == seq.seq) {
                break;
            }
            row++;
        }
        if(row == aln.getNumRows()) {
            row = -1;
        }
    }
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(row));
    return calee.property("res");
}

static QScriptValue createAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }
    
    MAlignment align;
    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    align.setAlphabet(seq.alphabet);
    align.addRow( MAlignmentRow(seq.getName(),seq.seq) );

    for(int i = 1; i < ctx->argumentCount(); i++) {
        DNASequence seq = ctx->argument(i).toVariant().value<DNASequence>();
        if(seq.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        if(seq.alphabet != align.getAlphabet()) {
            return ctx->throwError(QObject::tr("Alphabets of each sequence must be the same"));
        }
        align.addRow(MAlignmentRow(seq.getName(),seq.seq));
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<MAlignment>(align)));
    return calee.property("res");
}

static QScriptValue addToAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 2 && ctx->argumentCount() >3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MAlignment align = ctx->argument(0).toVariant().value<MAlignment>();
    
    DNASequence seq = ctx->argument(1).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    if(align.isEmpty()) {
        //return ctx->throwError(QObject::tr("Invalid alignment"));
        //align = new MAlignment("alignment");
        align.setAlphabet(seq.alphabet);
    }

    if(seq.alphabet != align.getAlphabet()) {
        return ctx->throwError(QObject::tr("Alphabets don't match"));
    }

    int row = -1;
    if(ctx->argumentCount() == 3) {
        if(!ctx->argument(2).isNumber()) {
            return ctx->throwError(QObject::tr("Third argument must be a number"));
        }
        row = ctx->argument(2).toInt32();
        if(row > align.getLength()) {
            row = -1;
        }
    }
    align.addRow(MAlignmentRow(seq.getName(),seq.seq),row);

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<MAlignment>(align)));
    return calee.property("res");
}

static QScriptValue removeFromAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MAlignment aln = ctx->argument(0).toVariant().value<MAlignment>();
    if(aln.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    if(!ctx->argument(1).isNumber()) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }
    int row = ctx->argument(1).toInt32();
    if(row < 0 || row >= aln.getLength()) {
        return ctx->throwError(QObject::tr("Row is out of range"));
    }

    aln.removeRow(row);
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<MAlignment>(aln)));
    return calee.property("res");
}

static QScriptValue rowNum(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MAlignment aln = ctx->argument(0).toVariant().value<MAlignment>();
    if(aln.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    int num = aln.getNumRows();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(num));
    return calee.property("res");
}

static QScriptValue columnNum(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MAlignment aln = ctx->argument(0).toVariant().value<MAlignment>();
    if(aln.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    int num = aln.getLength();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(num));
    return calee.property("res");
}

static QScriptValue alignmentAlphabetType(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MAlignment aln = ctx->argument(0).toVariant().value<MAlignment>();
    if(aln.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    QString alph = aln.getAlphabet()->getName();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(alph));
    return calee.property("res");
}

// ================== Annotations =======================

static QScriptValue getAnnotationRegion(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = ctx->argument(0).toVariant().value<DNASequence>();
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    QList<SharedAnnotationData> anns = ctx->argument(1).toVariant().value< QList<SharedAnnotationData> >();
    if(anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString name = ctx->argument(2).toString();
    if(name.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty name"));
    }
    QList<DNASequence> result;

    foreach(const SharedAnnotationData &ann, anns) {
        if(ann->name == name) {
            DNASequence resultedSeq;
            const QByteArray & sequence = seq.seq;
            QVector<U2Region> location = ann->getRegions();
            QByteArray & res = resultedSeq.seq;
            QVector<U2Region> extendedRegions; 

            //extend regions
            U2Region sequenceRange(0, sequence.size());
            foreach(const U2Region& reg, location) {
                U2Region ir = reg.intersect(sequenceRange);
                extendedRegions << ir;
            }

            for( int i = 0, end = extendedRegions.size(); i < end; ++i ) {
                U2Region reg = extendedRegions.at(i);
                QByteArray partSeq(sequence.constData() + reg.startPos, reg.length);
                res.append(partSeq);
            }
            resultedSeq.alphabet = seq.alphabet;
            result << resultedSeq;
        }
    }

    QScriptValue calee = ctx->callee();
    QScriptValue flag = engine->globalObject();
    flag.setProperty("list", engine->newVariant(true));
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<QList<DNASequence> >(result)));
    return calee.property("res");
}

static QScriptValue filterByQualifier(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QList<SharedAnnotationData> anns = ctx->argument(0).toVariant().value< QList<SharedAnnotationData> >();
    if(anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString qual = ctx->argument(1).toString();
    if(qual.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier name"));
    }
    QString val = ctx->argument(2).toString();
    if(val.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier value"));
    }

    QList<SharedAnnotationData> res;
    foreach(const SharedAnnotationData &ann, anns) {
        if(ann->qualifiers.contains(U2Qualifier(qual, val))) {
            res << ann;
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<QList<SharedAnnotationData> >(res)));
    return calee.property("res");
}

static QScriptValue addQualifier(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 3 && ctx->argumentCount() > 4) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QList<SharedAnnotationData> anns = ctx->argument(0).toVariant().value< QList<SharedAnnotationData> >();
    if(anns.isEmpty()) {
       return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString qual = ctx->argument(1).toString();
    if(qual.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier name"));
    }
    QString val = ctx->argument(2).toString();
    if(val.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier value"));
    }

    if(ctx->argumentCount() == 4) {
        QString name = ctx->argument(3).toString();
        if(name.isEmpty()) {
            return ctx->throwError(QObject::tr("forth argument must be a string"));
        }
        for(int i = 0;i<anns.size();i++) {
            if(anns[i]->name == name) {
                anns[i]->qualifiers.append(U2Qualifier(qual,val));
            }
        }
    }

    else {
        for(int i = 0;i<anns.size();i++) {
            anns[i]->qualifiers.append(U2Qualifier(qual,val));
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<QList<SharedAnnotationData> >(anns)));
    return calee.property("res");
}

static QScriptValue getLocation(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QList<SharedAnnotationData> anns = ctx->argument(0).toVariant().value< QList<SharedAnnotationData> >();
    if(anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    if(!ctx->argument(1).isNumber()) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }
    int num = ctx->argument(1).toInt32();
    if(num < 0 || num > anns.size()) {
        return ctx->throwError(QObject::tr("Index is out of range"));
    }
    QVector<U2Region> loc = anns[num]->getRegions();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<QVector<U2Region> >(loc)));
    return calee.property("res");
}

}
}

#endif