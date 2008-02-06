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

  File:		YCPMacroPlayer.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#include <stdio.h>
#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/Parser.h>
#include <ycp/YBlock.h>
#include <ycp/YCPCode.h>

#define YUILogComponent "ui-macro"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YWidget.h"
#include "YCPMacroPlayer.h"



YCPMacroPlayer::YCPMacroPlayer( const string & macroFileName )
    : YMacroPlayer( macroFileName )
    , _macro( 0 )
{
    _nextBlockNo = -1;
    readMacroFile( macroFileName );
}


YCPMacroPlayer::~YCPMacroPlayer()
{
    yuiDebug() << "Deleting macro player." << endl;
}



void YCPMacroPlayer::readMacroFile( const string & macroFileName )
{
    clearError();
    FILE * macroFile = fopen( macroFileName.c_str(), "r" );

    if ( ! macroFile )
    {
	setError();
	yuiError() << "Can't open macro file " << macroFileName << endl;
	return ;
    }

    yuiMilestone() << "Loading macro file " << macroFileName << endl;

    Parser parser( macroFile, macroFileName.c_str() );
    YCodePtr parsed = parser.parse();

    if ( !parsed || parsed->isError() )
    {
	setError();
	yuiError() << "Error parsing macro file " << macroFileName
		   << " -- macro execution aborted"
		   << endl;
	return;
    }

    if ( !parsed->isBlock() )
    {
	setError();
	yuiError() << "Macro syntax error in file " << macroFileName
		   << " -- expected YCP block"
		   << endl;
	return;
    }

    _macro = static_cast <YBlockPtr> (parsed) ;

    yuiDebug() << "Playing macro from file " << macroFileName
	       << " - " << _macro->statementCount() << " macro blocks"
	       << endl;
    _nextBlockNo = 0;

    fclose( macroFile );
}


bool YCPMacroPlayer::finished()
{
    if ( error() || !_macro || _nextBlockNo < 0 )
    {
	yuiWarning() << "Test for error() first before testing finished() !" << endl;
	return true;
    }
    yuiDebug() << "_nextBlockNo: " 	<< _nextBlockNo 
	       << ", size: "		<< _macro->statementCount()
	       << ", finished(): " 	<< boolalpha << ( _nextBlockNo >= _macro->statementCount() )
	       << endl;

    return _nextBlockNo >= _macro->statementCount();
}


YCPValue YCPMacroPlayer::evaluateNextBlock()
{
    if ( error() || finished() )
    {
	return YCPNull();
    }

    yuiMilestone() << "Evaluating macro block #" << _nextBlockNo << endl;

    return _macro->evaluate( _nextBlockNo++ );
}


void YCPMacroPlayer::rewind()
{
    _nextBlockNo = 0;
}
