/* dtkGuiExport.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Mar 13 21:49:35 2009 (+0100)
 * Version: $Id$
 * Last-Updated:  Thu Apr  9 18:48:36 2009
 *           By: Jean-Christophe Lombardo
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifdef WIN32
    #ifdef dtkGui_EXPORTS
        #define DTKGUI_EXPORT __declspec(dllexport) 
    #else
        #define DTKGUI_EXPORT __declspec(dllimport) 
    #endif
#else
    #define DTKGUI_EXPORT	
#endif
