/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/
/*
 *      $Id: osdMessageQueue.cpp,v 1.1 2003-05-28 21:17:52 bcda Exp $
 *
 *      Author  W. Eric Norum
 *              norume@aps.anl.gov
 *              630 252 4793
 */

#define epicsExportSharedSymbols
#include <limits.h>
#include "epicsMessageQueue.h"

extern "C" int sysClkRateGet(void);

epicsShareFunc epicsMessageQueueId epicsShareAPI epicsMessageQueueCreate(
    unsigned int capacity,
    unsigned int maximumMessageSize)
{
    epicsMessageQueueId id = (epicsMessageQueueId)callocMustSucceed(1, sizeof(*id), "epicsMessageQueueCreate");
    if ((id->msgq = msgQCreate(capacity, maximumMessageSize, MSG_Q_FIFO)) == NULL) {
        free(id);
        return NULL;
    }
    id->nBytes = maximumMessageSize;
    return id;
}

epicsShareFunc void epicsShareAPI epicsMessageQueueDestroy(epicsMessageQueueId id)
{
    msgQDelete(id->msgq);
    free(id);
}

epicsShareFunc int epicsShareAPI epicsMessageQueueSendWithTimeout(
    epicsMessageQueueId id,
    void *message,
    unsigned int messageSize,
    double timeout)
{
    int ticks;

    if (timeout<=0.0) {
        ticks = 0;
    } else {
        ticks = (int)(timeout*sysClkRateGet());
        if(ticks<=0) ticks = 1;
    }
    return msgQSend(id->msgq, (char *)message, messageSize, ticks, MSG_PRI_NORMAL);
}

epicsShareFunc int epicsShareAPI epicsMessageQueueReceiveWithTimeout(
    epicsMessageQueueId id,
    void *message,
    double timeout)
{
    int ticks;

    if (timeout<=0.0) {
        ticks = 0;
    } else {
        ticks = (int)(timeout*sysClkRateGet());
        if(ticks<=0) ticks = 1;
    }
    return msgQReceive(id->msgq, (char *)message, id->nBytes, ticks);
}
