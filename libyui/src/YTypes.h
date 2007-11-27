/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:		YTypes.h

  Author:	Stefan Hundhammer <sh@suse.de>

  Header file for frequently used simple types to reduce interdependencies
  between important headers (e.g., YWidget.h, YUI.h).

/-*/


#ifndef YTypes_h
#define YTypes_h

#include <list>

typedef double		YLayoutSize_t;
typedef long long	YFileSize_t;

class YWidget;

typedef std::list<YWidget *>			YWidgetList;
typedef std::list<YWidget *>::iterator		YWidgetListIterator;
typedef std::list<YWidget *>::const_iterator	YWidgetListConstIterator;


#define YUIAllDimensions	2

enum YUIDimension
{
    YD_HORIZ	= 0,
    YD_VERT	= 1
};


enum YAlignmentType
{
    YAlignUnchanged,
    YAlignBegin,
    YAlignEnd,
    YAlignCenter
};


#endif // YTypes_h