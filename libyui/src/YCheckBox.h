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

  File:		YCheckBox.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YCheckBox_h
#define YCheckBox_h

#include <string>

#include "YWidget.h"
#include "ImplPtr.h"

using std::string;
class YMacroRecorder;
class YCheckBoxPrivate;

enum YCheckBoxState
{
    YCheckBox_dont_care	= -1,	// tristate
    YCheckBox_off 	= 0,
    YCheckBox_on  	= 1
};


class YCheckBox : public YWidget
{
protected:
    /**
     * Constructor.
     **/
    YCheckBox( YWidget * parent, const string & label );
    
public:
    /**
     * Destructor.
     **/
    virtual ~YCheckBox();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YCheckBox"; }


    /**
     * Get the current value:
     *
     * YCheckBox_on 		CheckBox is checked
     * YCheckBox_off 		CheckBox is unchecked
     *
     * YCheckBox_dont_care	tri-state: CheckBox is greyed out,
     *				neither checked nor unchecked
     * 
     * The user cannot set YCheckBox_dont_care directly. This status is always
     * only set from the outside, usually because a setting cannot be clearly
     * determined. For example, a checkbox
     *
     *     [ ] Read only
     *
     * would be set to "don't care" (by the application, not directly by the
     * user) when it is to display the read-only state of a group of files
     * where some are read-only and some are writeable.
     *
     * Derived classes are required to implement this function.
     * (Intentionally not const)
     **/
    virtual YCheckBoxState value() = 0;

    /**
     * Set the CheckBox value (on/off/don't care).
     *
     * Derived classes are required to implement this.
     **/
    virtual void setValue( YCheckBoxState state ) = 0;

    /**
     * Simplified access to value(): Return 'true' if the CheckBox is checked.
     **/
    bool isChecked() { return value() == YCheckBox_on; }

    /**
     * Simplified access to setValue(): Check of uncheck the CheckBox.
     **/
    void setChecked( bool checked )
	{ setValue( checked ? YCheckBox_on : YCheckBox_off ); }

    /**
     * Simplified access to tri-state ("don't care").
     **/
    bool dontCare() { return value() == YCheckBox_dont_care; }
    
    /**
     * Simplified access to setting tri-state ("don't care").
     **/
    void setDontCare() { setValue( YCheckBox_dont_care ); }
    
    /**
     * Get the label (the text on the CheckBox).
     **/
    string label() const;

    /**
     * Set the label (the text on the CheckBox).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const string & label );

    /**
     * Returns 'true' if a bold font should be used.
     **/
    bool useBoldFont() const;

    /**
     * Indicate whether or not a bold font should be used.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setUseBoldFont( bool bold = true );

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This method may throw exceptions, for example
     *   - if there is no property with that name
     *   - if the expected type and the type mismatch
     *   - if the value is out of range
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const string & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw exceptions, for example
     *   - if there is no property with that name
     **/
    virtual YPropertyValue getProperty( const string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property set upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();

    /**
     * Get the string of this widget that holds the keyboard shortcut.
     *
     * Reimplemented from YWidget.
     **/
    virtual string shortcutString() const { return label(); }

    /**
     * Set the string of this widget that holds the keyboard shortcut.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setShortcutString( const string & str )
	{ setLabel( str ); }

    /**
     * The name of the widget property that will return user input.
     * Inherited from YWidget.
     **/
    const char *userInputProperty() { return YUIProperty_Value; }


private:

    /**
     * Save the widget's user input to a macro recorder.
     * Intentionally declared as "private" so all macro recording internals are
     * handled by the abstract libyui level, not by a specific UI.
     **/
    virtual void saveUserInput( YMacroRecorder *macroRecorder );

    ImplPtr<YCheckBoxPrivate> priv;
};


#endif // YCheckBox_h
