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

  File:		YBusyIndicator.cc

  Author:	Thomas Goettlicher <tgoettlicher@suse.de>

/-*/


#define y2log_component "ui"
#include <ycp/y2log.h>

#include "YUISymbols.h"
#include "YBusyIndicator.h"

struct YBusyIndicatorPrivate
{
    YBusyIndicatorPrivate( const string &	label,
			 int 		timeout ,
			 bool		alive )
	: label( label )
	, timeout( timeout )
	, alive	(true)
	{
	}
    
    string	label;
    int 	timeout;
    bool	alive;
};




YBusyIndicator::YBusyIndicator( YWidget * 		parent,
			    const string &	label,
			    int			timeout,
			    bool		alive )
    : YWidget( parent )
    , priv( new YBusyIndicatorPrivate( label, timeout, alive ) )
{
    YUI_CHECK_NEW( priv );
    
    setDefaultStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT, false );
}


YBusyIndicator::~YBusyIndicator()
{
    // NOP
}


string YBusyIndicator::label()
{
    return priv->label;
}


void YBusyIndicator::setLabel( const string & label )
{
    priv->label = label;
}


int YBusyIndicator::timeout() const
{
    return priv->timeout;
}


void YBusyIndicator::setTimeout( int newTimeout )
{
    if ( newTimeout < 1 )
	newTimeout = 1;
    
    priv->timeout = newTimeout;
}


void YBusyIndicator::setAlive( bool alive )
{
    priv->alive = alive;
}

bool YBusyIndicator::alive() const
{
    return priv->alive; 
}

const YPropertySet &
YBusyIndicator::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property integer Timeout	timeout in ms until busy indicator changes to stalled state
	 * @property bool    Alive	busy indicator is in alive or stalled state
	 * @property string  Label	caption above the busy indicator
	 */
	propSet.add( YProperty( YUIProperty_Timeout,		YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_Alive,		YBoolProperty ) );
	propSet.add( YProperty( YUIProperty_Label,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YBusyIndicator::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Timeout	)	setTimeout( val.integerVal() );
    else if ( propertyName == YUIProperty_Alive		)	setAlive( val.boolVal() );
    else if ( propertyName == YUIProperty_Label		)	setLabel( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YBusyIndicator::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Timeout	)	return YPropertyValue( timeout() );
    else if ( propertyName == YUIProperty_Label		)	return YPropertyValue( label() );
    else if ( propertyName == YUIProperty_Alive		)	return YPropertyValue( alive() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}

