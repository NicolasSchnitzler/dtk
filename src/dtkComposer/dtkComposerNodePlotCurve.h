/* dtkComposerNodePlotCurve.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Tue May 29 14:32:55 2012 (+0200)
 * Version: $Id$
 * Last-Updated: jeu. sept. 20 23:47:28 2012 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 30
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERNODEPLOTCURVE_H
#define DTKCOMPOSERNODEPLOTCURVE_H


#include "dtkComposerNodeLeaf.h"

#include <dtkPlot/dtkPlotCurve.h>

#include <QtCore>

class dtkComposerNodePlotCurvePrivate;

class  dtkComposerNodePlotCurve : public QObject, public dtkComposerNodeLeaf
{
    Q_OBJECT

public:
     dtkComposerNodePlotCurve(void);
    ~dtkComposerNodePlotCurve(void);

public:
    inline QString type(void) {
        return "dtkPlotCurve";
    }

    inline QString titleHint(void) {
        return "Plot curve";
    }

public:
    inline QString inputLabelHint(int port) {
        switch(port) {
        case 0:
            return "x";
        case 1:
            return "y";
        default:
            return "port";
        }
    }

    inline QString outputLabelHint(int) {
        return "curve";
    }

public:
    dtkPlotCurve *curve(void);

public:
    void run(void);

private:
    dtkComposerNodePlotCurvePrivate *d;
};

#endif