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

#ifndef _CRASH_HANDLER_H_
#define _CRASH_HANDLER_H_

#if defined(USE_CRASHHANDLER)

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#include <QtCore/QMutex>

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>
#include <U2Core/Log.h>
#include <U2Core/LogCache.h>

#include "private.h"

namespace U2 {

#if defined( Q_OS_WIN )
#include <windows.h>
    //LONG NTAPI CrashHandlerFunc(PEXCEPTION_POINTERS pExceptionInfo );
#else 
#include <stdlib.h>
#include <signal.h>
#endif



struct ExceptionInfo {
    QString errorType;
};

class U2PRIVATE_EXPORT CrashHandler {
public:
    CrashHandler() {}
    static void setupHandler();
#if defined( Q_OS_WIN )
    static LONG NTAPI CrashHandlerFunc(PEXCEPTION_POINTERS pExceptionInfo );
    static LONG NTAPI CrashHandlerFuncSecond(PEXCEPTION_POINTERS pExceptionInfo );
    static LONG NTAPI CrashHandlerFuncThird(PEXCEPTION_POINTERS pExceptionInfo );

    static PVOID handler;
    static PVOID handler2;
#else
    static void signalHandler(int signo, siginfo_t *info, void *context);
    static struct sigaction sa;
#endif
    static void runMonitorProcess(const QString &exceptionType);
    static void getSubTasks(Task *t, QString& list, int lvl);
    static void allocateReserve();
    static void releaseReserve();

    static char *buffer;
};

}

#endif //crash handler flag

#endif
