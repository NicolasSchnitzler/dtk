/* dtkComposerNodeLeafData.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Thu Jun 28 09:56:37 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Jun 28 11:59:53 2012 (+0200)
 *           By: tkloczko
 *     Update #: 39
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERNODELEAFDATA_H
#define DTKCOMPOSERNODELEAFDATA_H

#include "dtkComposerExport.h"
#include "dtkComposerNodeLeaf.h"

#include <QStringList>

class dtkAbstractData;
class dtkComposerNodeLeafDataPrivate;

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeLeafData interface
// /////////////////////////////////////////////////////////////////

class DTKCOMPOSER_EXPORT dtkComposerNodeLeafData : public dtkComposerNodeLeaf
{
public:
             dtkComposerNodeLeafData(void);
    virtual ~dtkComposerNodeLeafData(void);

public:
    virtual bool isAbstractData(void) const = 0;

    virtual QString abstractDataType(void) const = 0;

public:
    QString currentImplementation(void);

    QStringList implementations(void);

public:
    dtkAbstractData *createData(const QString& implementation);

    dtkAbstractData *data(void);

private:
    dtkComposerNodeLeafDataPrivate *d;
};

#endif
