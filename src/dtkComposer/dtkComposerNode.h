/* dtkComposerNode.h --- 
 * 
 * Author: David Rey
 * Copyright (C) 2008-2011 - David Rey, Inria.
 * Created: Tue Feb 14 14:24:23 2012 (+0100)
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include "dtkComposerExport.h"

#include <dtkCore>
#include <QtCore>

class dtkComposerNodePrivate;
class dtkComposerTransmitter;
class dtkComposerNodeMetaData;

// /////////////////////////////////////////////////////////////////
// dtkComposerNode
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNode
{
public: 
             dtkComposerNode(void);
    virtual ~dtkComposerNode(void);

public:
    enum Kind {
        Atomic = 1,
        Composite = 2,
        Control = 3,
        Data = 4,
        Process = 5,
        View = 6,
        Actor = 7
    };

public:
    void setNodeMetaData(dtkComposerNodeMetaData *meta_data);
    dtkComposerNodeMetaData *nodeMetaData(void) const;

public:
    void appendEmitter(dtkComposerTransmitter *emitter);
    void removeEmitter(dtkComposerTransmitter *emitter);

    dtkComposerTransmitter *removeEmitter(int index);

    void appendReceiver(dtkComposerTransmitter *receiver);
    void removeReceiver(dtkComposerTransmitter *receiver);

    dtkComposerTransmitter *removeReceiver(int index);

    QList<dtkComposerTransmitter *> emitters(void);
    QList<dtkComposerTransmitter *> receivers(void);

public:
    virtual QString type(void);

public:
    virtual QString titleHint(void);

public:
    virtual QString  inputLabelHint(int port);
    virtual QString outputLabelHint(int port);

public:
    void setTitleHint(const QString& hint);

public:
    void  setInputLabelHint(const QString& hint, int port);
    void setOutputLabelHint(const QString& hint, int port);

private:
    dtkComposerNodePrivate *d;
};
