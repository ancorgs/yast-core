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

  File:		YRadioButton.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YRadioButton_h
#define YRadioButton_h

#include "YWidget.h"

class YRadioButtonGroup;
class YMacroRecorder;
class YRadioButtonPrivate;

using std::string;

/**
 * RadioButton: Widget for one-out-of-many selection.
 *
 * Only one RadioButton in a RadioBox (in a RadioButtonGroup) can be set to
 * "on" at the same time. Setting any RadioButton of a RadioButtonGroup to "on"
 * automatically sets all others in the same RadioButtonGroup to "off".
 *
 * RadioButtons customarily have a distinct visual appearance from CheckBoxes:
 *
 *     ( ) RadioButton 1
 *     (*) RadioButton 2
 *     ( ) RadioButton 3
 *
 *     [ ] CheckBox 1
 *     [*] CheckBox 2
 *     [*] CheckBox 3
 **/
class YRadioButton : public YWidget
{
protected:
    /**
     * Constructor.
     *
     * Creates a new RadioButton with user-visible text 'label'.
     * 'label' can and should contain a keyboard shortcut (designated with
     * '&').
     *
     * The caller has to take care to add this RadioButton to its
     * RadioButtonGroup:
     *
     * if ( radioButton->buttonGroup() )
     *     radioButton->buttonGroup()->addRadioButton( radioButton );
     *
     * This can't be done in the constructor because it would involve calling a
     * virtual function, which doesn't work yet within the constructor.
     **/
    YRadioButton( YWidget * parent, const string & label );

public:
    /**
     * Destructor: Removes the button from the radio button group.
     **/
    virtual ~YRadioButton();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     *
     * Reimplemented from YWidget.
     **/
    virtual const char * widgetClass() const { return "YRadioButton"; }

    /**
     * Get the current on/off value:
     * 'true' if checked, 'false' if unchecked.
     *
     * Derived classes are required to implement this.
     **/
    virtual bool value() = 0;

    /**
     * Set the radio button value (on/off).
     *
     * Derived classes are required to implement this.
     **/
    virtual void setValue( bool checked ) = 0;

    /**
     * Get the label (the text on the RadioButton).
     **/
    string label() const;

    /**
     * Set the label (the text on the RadioButton).
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
     * Get a pointer to the radio button group this button belongs to.
     **/
    YRadioButtonGroup * buttonGroup();

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
    const char * userInputProperty() { return YUIProperty_Value; }

protected:
    /**
     * Traverse the widget hierarchy upwards to find the corresponding
     * YRadioButtonGroup, i.e. the class that controls the radio box behaviour
     * (i.e. that makes sure that no more than one RadioButton is set to "on"
     * at the same time).
     **/
    YRadioButtonGroup * findRadioButtonGroup() const;


private:
    /**
     * Save the widget's user input to a macro recorder.
     * Intentionally declared as "private" so all macro recording internals are
     * handled by the abstract libyui level, not by a specific UI.
     **/
    virtual void saveUserInput( YMacroRecorder *macroRecorder );

    ImplPtr<YRadioButtonPrivate> priv;
};


#endif // YRadioButton_h
