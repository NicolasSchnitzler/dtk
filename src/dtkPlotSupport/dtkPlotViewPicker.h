/* dtkPlotViewPicker.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Sun Jun 10 01:13:02 2012 (+0200)
 * Version: $Id$
 * Last-Updated: mer. avril  9 13:21:24 2014 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKPLOTVIEWPICKER_H
#define DTKPLOTVIEWPICKER_H

#include "dtkPlotSupportExport.h"

#include <QtCore/QObject>
#include <QColor>

class dtkPlotView;
class dtkPlotViewPickerPrivate;

class DTKPLOTSUPPORT_EXPORT dtkPlotViewPicker : public QObject
{
    Q_OBJECT

public:
     dtkPlotViewPicker(dtkPlotView *parent);
    ~dtkPlotViewPicker(void);

public:
    void   activate(void);
    void deactivate(void);

public:
    QColor color(void) const;

    void setColor(const QColor& color);

private:
    dtkPlotViewPickerPrivate *d;
};

#endif
