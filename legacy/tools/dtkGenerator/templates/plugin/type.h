// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef %2_H
#define %2_H

#include %6

#include "%1PluginExport.h"

class %1Private;

// /////////////////////////////////////////////////////////////////
// %1 interface
// /////////////////////////////////////////////////////////////////
// 1- %1
// 2- %2
// 3- %3
// 4- %4
// 5- %5
// 6- %6

class %2PLUGIN_EXPORT %1 : public %5
{
    Q_OBJECT

public:
             %1(void);
             %1(const %1& other);
    virtual ~%1(void);

public:
    %1& operator = (const %1& other);

protected:
    void copy(const dtkAbstract%3& other);

public:
    QString identifier(void) const;

    static bool registered(void);

private:
    DTK_DECLARE_PRIVATE(%1);
};

dtkAbstract%3 *create%4(void);

#endif
