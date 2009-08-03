/* dtkPluginGeneratorMainWindow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 10 00:11:13 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Mar 16 09:45:03 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKPLUGINGENERATORMAINWINDOW_H
#define DTKPLUGINGENERATORMAINWINDOW_H

#include <QtGui>

class dtkPluginGeneratorMainWindowPrivate;

class dtkPluginGeneratorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
     dtkPluginGeneratorMainWindow(QWidget *parent = 0);
    ~dtkPluginGeneratorMainWindow(void);

public slots:
    void generate(void);

public slots:
    void about(void);
    void onOutputPathClicked(void);
    void onOutputPathChanged(void);
    void onPluginTypeChanged(void);
    void onPrefixChanged(void);
    void onSuffixChanged(void);
    void update(void);

private:
    dtkPluginGeneratorMainWindowPrivate *d;
};

#endif
