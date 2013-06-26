/* dtkCreatorMainWindow_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Wed Feb  1 12:37:28 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jun 21 15:47:49 2013 (+0200)
 *           By: Selim Kraria
 *     Update #: 146
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCREATORMAINWINDOW_P_H
#define DTKCREATORMAINWINDOW_P_H

#include <QtCore>
#include <QtGui>

class dtkComposer;
class dtkComposerControls;
class dtkComposerFactoryView;
class dtkComposerGraphView;
class dtkComposerSceneModel;
class dtkComposerSceneNodeEditor;
class dtkComposerSceneView;
class dtkComposerStackView;

class dtkDistributor;

class dtkRecentFilesMenu;

class dtkSplitter;

class dtkLogView;

class dtkViewManager;

class dtkCreatorMainWindow;

class dtkCreatorMainWindowPrivate : public QObject
{
    Q_OBJECT

public:
    bool maySave(void);

public:
    void setCurrentFile(const QString& file);

#if defined(Q_WS_MAC) && (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_6)
    void enableFullScreenSupport(void);
    void showFullScreen(void);
#endif

public slots:
    void setModified(bool modified);

public:
    dtkComposer *composer;
    dtkComposerControls *controls;
    dtkComposerFactoryView *nodes;
    dtkComposerGraphView *graph;
    dtkComposerSceneModel *model;
    dtkComposerSceneNodeEditor *editor;
    dtkComposerSceneView *scene;
    dtkComposerStackView *stack;

public:
    dtkDistributor *distributor;

public:
    dtkViewManager *view;

public:
    dtkLogView *log_view;

public:
    dtkSplitter *inner;

public:
    QMenu *composition_menu;
    QAction *composition_open_action;
    QAction *composition_save_action;
    QAction *composition_saveas_action;
    QAction *composition_insert_action;
    QAction *composition_quit_action;

    QMenu *edit_menu;
    QAction *undo_action;
    QAction *redo_action;

    dtkRecentFilesMenu *recent_compositions_menu;

public:
    QPushButton *compo_button;
    QPushButton *distr_button;
    QPushButton *debug_button;
    QPushButton *view_button;

public:
    bool closing;

public:
    QString current_composition;

public:
    int wl;
    int wr;
    int wg;

public:
    dtkCreatorMainWindow *q;
};

#endif
