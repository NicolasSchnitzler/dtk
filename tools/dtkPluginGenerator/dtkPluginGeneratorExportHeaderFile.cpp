/* dtkPluginGeneratorExportHeaderFile.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 11 14:30:26 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Jun 29 18:38:32 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkPluginGenerator.h"

bool dtkPluginGenerator::generateExportHeaderFile(void)
{
    QFile exportHeaderFile(d->target.absoluteFilePath(QString(d->plugin).append("PluginExport.h")));

    if(!exportHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
	qWarning() << "dtkPluginGenerator: unable to open" << QString(d->plugin).append("PluginExport.h") << "for writing";
	return false;
    }

    QTextStream stream(&exportHeaderFile);
    
    stream << QString(
	"/* %1PluginExport.h --- \n"
	" * \n"
	" * Author:\n"
	" * Copyright (C)\n"
	" * Created:\n"
	" * Version: $Id$\n"
	" * Last-Updated:\n"
	" *           By:\n"
	" *     Update #:\n"
	" */\n"
	"\n"
	"/* Commentary: \n"
	" * \n"
	" */\n"
	"\n"
	"/* Change log:\n"
	" * \n"
	" */\n"
	"\n"
	"#ifndef %2PLUGINEXPORT_H\n"
	"#define %2PLUGINEXPORT_H\n"
	"\n"
	"#ifdef WIN32\n"
	"    #ifdef %1Plugin_EXPORTS\n"
	"        #define %2PLUGIN_EXPORT __declspec(dllexport) \n"
	"    #else\n"
	"        #define %2PLUGIN_EXPORT __declspec(dllimport) \n"
	"    #endif\n"
	"#else\n"
	"    #define %2PLUGIN_EXPORT\n"
	"#endif\n"
	"\n"
	"#endif\n"
	"\n"
	)
	.arg(QString(d->plugin))
	.arg(QString(d->plugin).toUpper());

    exportHeaderFile.close();

    return true;
}
