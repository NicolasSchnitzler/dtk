/* dtkPlugin.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct 31 13:49:50 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Sep  5 13:13:16 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 64
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTPLUGIN_H
#define DTKABSTRACTPLUGIN_H

#include <dtkCore/dtkAbstractObject.h>
#include <dtkCore/dtkCoreExport.h>
#include <dtkCore/dtkGlobal.h>

class dtkPluginPrivate;

class DTKCORE_EXPORT dtkPlugin : public dtkAbstractObject
{
    Q_OBJECT

public:
     dtkPlugin(QObject *parent = 0);
    ~dtkPlugin(void);
    
    virtual bool initialize(void) = 0;
    virtual bool uninitialize(void) = 0;
    
    virtual QString contact(void) const;
    virtual QString description(void) const = 0;
    virtual QString name(void) const = 0;
    virtual QString version(void) const;

    virtual QStringList authors(void) const;
    virtual QStringList contributors(void) const;
    virtual QStringList dependencies(void) const;
    virtual QStringList tags(void) const = 0;
    virtual QStringList types(void) const = 0;

private:
    dtkPluginPrivate *d;
};

Q_DECLARE_INTERFACE(dtkPlugin, "fr.inria.dtk/0.1.0")

#endif
