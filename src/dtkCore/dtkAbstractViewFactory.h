/* dtkAbstractViewFactory.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 15:48:10 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Jan 16 12:43:40 2014 (+0100)
 *           By: Selim Kraria
 *     Update #: 119
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef DTKABSTRACTVIEWERFACTORY_H
#define DTKABSTRACTVIEWERFACTORY_H

#include "dtkAbstractFactory.h"

                  class dtkAbstractView;
                  class dtkAbstractViewAnimator;
                  class dtkAbstractViewNavigator;
                  class dtkAbstractViewInteractor;
                  class dtkAbstractViewFactoryPrivate;
template<class T> class dtkSmartPointer;

class DTKCORE_EXPORT dtkAbstractViewFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef dtkAbstractView           *(*dtkAbstractViewCreator)          (void);
    typedef dtkAbstractViewAnimator   *(*dtkAbstractViewAnimatorCreator)  (void);
    typedef dtkAbstractViewNavigator  *(*dtkAbstractViewNavigatorCreator) (void);
    typedef dtkAbstractViewInteractor *(*dtkAbstractViewInteractorCreator)(void);

    typedef QPair<QString, QStringList> dtkAbstractViewTypeHandler;

public:
    static dtkAbstractViewFactory *instance(void);

    bool registerViewType          (const QString& type,                             dtkAbstractViewCreator           func);
    bool registerViewType          (const QString& type,                             dtkAbstractViewCreator           func, const QString& interface);
    bool registerViewAnimatorType  (const QString& type, const QStringList& handled, dtkAbstractViewAnimatorCreator   func);
    bool registerViewNavigatorType (const QString& type, const QStringList& handled, dtkAbstractViewNavigatorCreator  func);
    bool registerViewInteractorType(const QString& type, const QStringList& handled, dtkAbstractViewInteractorCreator func);

    unsigned int size(const QString& type) const;

    QStringList viewNames(void) const;

    QList<dtkAbstractView *> views(void);

    dtkAbstractView *view(const QString& name);

    QList<QString>                    creators(void) const;
    QList<dtkAbstractViewTypeHandler> animators(void) const;
    QList<dtkAbstractViewTypeHandler> interactors(void) const;
    QList<dtkAbstractViewTypeHandler> navigators(void) const;

    dtkSmartPointer<dtkAbstractView>               createSmartPointer(const QString& type);
    dtkSmartPointer<dtkAbstractViewAnimator>     animatorSmartPointer(const QString& type);
    dtkSmartPointer<dtkAbstractViewNavigator>   navigatorSmartPointer(const QString& type);
    dtkSmartPointer<dtkAbstractViewInteractor> interactorSmartPointer(const QString& type);

public:
    void remove(const QString& name);

public:
    QStringList implementations(void);
    QStringList implementations(const QString& interface);
    QStringList interfaces(void);

signals:
    void created(dtkAbstractView *view, const QString& type);
    void cleared(void);

public slots:
    dtkAbstractView           *create(const QString& type, QString name = QString());
    dtkAbstractViewAnimator   *animator(const QString& type);
    dtkAbstractViewNavigator  *navigator(const QString& type);
    dtkAbstractViewInteractor *interactor(const QString& type);
    
public slots:
    void clear(void);

protected:
     dtkAbstractViewFactory(void);
    ~dtkAbstractViewFactory(void);

private:    
    static dtkAbstractViewFactory *s_instance;

private:
    dtkAbstractViewFactoryPrivate *d;
};

#endif
