/* dtk3DViewTest.h ---
 * 
 * Author: Julien Wintz
 * Created: Fri Mar 22 11:35:20 2013 (+0100)
 * Version: 
 * Last-Updated: Mon Mar 25 13:00:53 2013 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Change Log:
 * 
 */

#pragma once

#include <dtkTest>

class dtk3DViewTestCase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase(void);
    void init(void);

private slots:
    void testInstanciation(void);
    void testShapes(void);

private slots:
    void cleanupTestCase(void);
    void cleanup(void);
};