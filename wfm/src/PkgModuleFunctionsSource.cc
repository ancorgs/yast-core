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

   File:	PkgModuleFunctionsSource.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:  Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Access to InstSrc
		Handles source related Pkg::function (list_of_arguments) calls
		from WFMInterpreter.
/-*/


#include <ycp/y2log.h>
#include <PkgModule.h>
#include <PkgModuleFunctions.h>

#include <y2util/Url.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcPtr.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/PMError.h>

#include <ycp/YCPVoid.h>
#include <ycp/YCPBoolean.h>
#include <ycp/YCPSymbol.h>
#include <ycp/YCPString.h>
#include <ycp/YCPList.h>
#include <ycp/YCPMap.h>
#include <ycp/YCPError.h>

#include <unistd.h>
#include <sys/statvfs.h>

using std::string;

InstSrcManager::ISrcId
PkgModuleFunctions::getSourceByArgs (YCPList args, int pos)
{
    InstSrcManager::ISrcId id;

    if ((args->size() <= pos)
	|| !(args->value(pos)->isInteger()))
    {
	y2error ("source id must be int");
	return id;
    }

    unsigned int source_slot = args->value(pos)->asInteger()->value();
    if (source_slot < 0
	|| source_slot >= _sources.size()
	|| _sources[source_slot] == 0)
    {
	y2error ("Source not active");
	return id;
    }

    return _sources[source_slot];
}
// ------------------------------------------------------------------
// source related

/**
 * @builtin Pkg::SourceInit (string url) -> integer
 *
 * initializes a *NEW* package source under the given url
 * This is only needed at initial installation or in
 * the source manager. Normal code should rely on the
 * cached sources.
 * @see SourceList
 */
YCPValue
PkgModuleFunctions::SourceInit (YCPList args)
{
    if ((args->size() != 1)
	|| !(args->value(0)->isString()))
    {
	return YCPError ("Bad args to Pkg::SourceInit");
    }

    const Url url( args->value(0)->asString()->value() );

    // check if url already known

    unsigned int number_of_known_sources = _sources.size();
    if (number_of_known_sources > 0)
    {
	for (unsigned int i = 0; i < number_of_known_sources; ++i)
	{
	    constInstSrcDescrPtr source_descr = _sources[i]->descr();
	    if ((source_descr)
		&& (source_descr->url().asString() == url.asString()))
	    {
		y2milestone ("Source '%s' already known", url.asString().c_str());
		return YCPInteger (i);
	    }
	}
    }

    InstSrcManager& MGR = _y2pm.instSrcManager();
    InstSrcManager::ISrcIdList nids;

    // check url for products

    PMError err = MGR.scanMedia( nids, url );

    if ( nids.size() )
    {
#warning PkgModuleFunctions::SourceInit supports only one product per source

	InstSrcManager::ISrcId source_id = *nids.begin();
	int new_slot = -1;

	if (_first_free_source_slot < number_of_known_sources)
	{
	    new_slot = _first_free_source_slot;
	    _sources[_first_free_source_slot] = source_id;

	    // find next free slot
	    while (++_first_free_source_slot < number_of_known_sources)
	    {
		if (_sources[_first_free_source_slot] == 0)
		    break;
	    }
	}
	else		// add a new slot
	{
	    new_slot = _sources.size();
	    _sources.push_back (source_id);
	}

	err = MGR.enableSource( source_id );
	y2milestone ("enable: %d: %s", new_slot, err.errstr().c_str());
	return YCPInteger (new_slot);
    }
    return YCPError ("No source data found");
}


/**
 * @builtin Pkg::SourceList (boolean enabled_only) -> list of integer
 *
 * return list of known (and enabled) source id's
 * This starts the packagemanager from it's cache of known
 * sources
 */
YCPValue
PkgModuleFunctions::SourceList (YCPList args)
{
    if ((args->size() != 1)
	|| !(args->value(0)->isBoolean()))
    {
	return YCPError ("Bad args to Pkg::SourceList");
    }

    YCPList sources;
    bool enabled_only ( args->value(0)->asBoolean()->value() );

    InstSrcManager::ISrcIdList nids;

    _y2pm.instSrcManager().getSources (nids, enabled_only);
    if (nids.size() > 0)
    {
	unsigned int number_of_known_sources = _sources.size();
	int new_slot = -1;

	for (InstSrcManager::ISrcIdList::const_iterator it = nids.begin();
	     it != nids.end(); ++it)
	{
	    number_of_known_sources = _sources.size();

	    if (_first_free_source_slot < number_of_known_sources)
	    {
		new_slot = _first_free_source_slot;
		_sources[_first_free_source_slot] = *it;

		// find next free slot
		while (++_first_free_source_slot < number_of_known_sources)
		{
		    if (_sources[_first_free_source_slot] == 0)
			break;
		}
	    }
	    else		// add a new slot
	    {
		new_slot = _sources.size();
		_sources.push_back (*it);
	    }
	}

	for (unsigned int i = 0; i < _sources.size(); ++i)
	{
	    sources->add (YCPInteger (i));
	}
	return sources;

    } // nids > 0

    return YCPError ("No source data found", sources);
}

/**
 * @builtin Pkg::SourceFinish (integer id) -> bool
 *
 * disables source
 *
 */
YCPValue
PkgModuleFunctions::SourceFinish (YCPList args)
{
    if ((args->size() != 1)
	|| !(args->value(0)->isInteger()))
    {
	return YCPError ("Bad args to Pkg::SourceFinish");
    }

    unsigned int source_slot = args->value(0)->asInteger()->value();
    if ((source_slot < 0)
	|| source_slot >= _sources.size())
    {
	return YCPError ("Bad source id", YCPBoolean (false));
    }

    if (_sources[source_slot] == 0)
    {
	return YCPError ("Source not active", YCPBoolean (true));
    }

    PMError err = _y2pm.instSrcManager().disableSource( _sources[source_slot]);
    _sources[source_slot] = 0;
    if (source_slot < _first_free_source_slot)
	_first_free_source_slot = source_slot;
    y2milestone ("disable: %d: %s", source_slot, err.errstr().c_str());
    return YCPBoolean (true);
}


/**
 * @builtin Pkg::SourceGeneralData (integer id) -> map
 *
 * returns general data about the source as a map:
 *   $[ "default_activate" : bool,
 *	"product_dir" : string,
 *	"url" : string,			// also in SourceMediaData
 *	"type" : string
 * ];
 *
 */
YCPValue
PkgModuleFunctions::SourceGeneralData (YCPList args)
{
    InstSrcManager::ISrcId source_id =  getSourceByArgs (args, 0);
    if (!source_id)
	return YCPVoid();

    YCPMap data;
    constInstSrcDescrPtr source_descr = source_id->descr();

    if (!source_descr)
	return YCPError ("No description for source", data);

    data->add (YCPString ("default_activate"), YCPBoolean (source_descr->default_activate()));
    data->add (YCPString ("product_dir"), YCPString (source_descr->product_dir().asString()));
    data->add (YCPString ("type"), YCPString (InstSrc::toString (source_descr->type())));
    data->add (YCPString ("url"), YCPString (source_descr->url().asString ()));
    return data;
}


/**
 * @builtin Pkg::SourceMediaData (integer id) -> map
 *
 * returns media data about the source as a map:
 *   $[ "media_count" : string,
 *	"media_id" : bool,
 *	"media_vendor" : string,
 *	"url" : string			// also in SourceGeneralData
 * ];
 *
 */
YCPValue
PkgModuleFunctions::SourceMediaData (YCPList args)
{
    InstSrcManager::ISrcId source_id =  getSourceByArgs (args, 0);
    if (!source_id)
	return YCPVoid();

    YCPMap data;
    constInstSrcDescrPtr source_descr = source_id->descr();
    if (!source_descr)
	return YCPError ("No description for source", data);

    data->add (YCPString ("media_count"), YCPInteger (source_descr->media_count()));
    data->add (YCPString ("media_id"), YCPString (source_descr->media_id()));
    data->add (YCPString ("media_vendor"), YCPString (source_descr->media_vendor()));
    data->add (YCPString ("url"), YCPString (source_descr->url().asString ()));

    return data;
}


/**
 * @builtin Pkg::SourceProductData (integer id) -> map
 *
 * returns product data about the source as a map:
 *   $[ "productname" : string,
 *	"productversion" : string,
 *	"baseproductname" : string,
 *	"baseproductversion" : string,
 *	"vendor" : string,
 *	"defaultbase" : string,
 *	"architectures" : list (string),
 *	"requires" : string,
 *	"linguas" : list (string),
 *	"label" : string,
 *	"labelmap" : map (string lang, string label),
 *	"language" : string,
 *	"timezone" : string,
 *	"descrdir" : string,
 *	"datadir" : string
 * ];
 *
 */
YCPValue
PkgModuleFunctions::SourceProductData (YCPList args)
{
    InstSrcManager::ISrcId source_id =  getSourceByArgs (args, 0);
    if (!source_id)
	return YCPVoid();

    YCPMap data;
    constInstSrcDescrPtr source_descr = source_id->descr();
    if (!source_descr)
	return YCPError ("No description for source", data);

    data->add (YCPString ("productname"), YCPString ((const std::string &)(source_descr->content_product().name)));
    data->add (YCPString ("productversion"), YCPString (PkgEdition::toString (source_descr->content_product().edition)));
    data->add (YCPString ("baseproductname"), YCPString ((const std::string &)(source_descr->content_baseproduct().name)));
    data->add (YCPString ("baseproductversion"), YCPString (PkgEdition::toString (source_descr->content_baseproduct().edition)));
    data->add (YCPString ("vendor"), YCPString (source_descr->content_vendor ()));
    data->add (YCPString ("defaultbase"), YCPString (source_descr->content_defaultbase ()));
    InstSrcDescr::ArchMap::const_iterator it1 = source_descr->content_archmap().find (_y2pm.baseArch());
    if (it1 != source_descr->content_archmap().end())
    {
	YCPList architectures;
	for (std::list<PkgArch>::const_iterator it2 = it1->second.begin();
	     it2 != it1->second.end(); ++it2)
	{
	    architectures->add (YCPString ((const std::string &)(*it2)));
	}
	data->add (YCPString ("architectures"), architectures);
    }
    data->add (YCPString ("requires"), YCPString (source_descr->content_requires().asString()));
    YCPList linguas;
    for (InstSrcDescr::LinguasList::const_iterator it = source_descr->content_linguas().begin();
	it != source_descr->content_linguas().end(); ++it)
    {
	linguas->add (YCPString (*it));
    }
    data->add (YCPString ("linguas"), linguas);
    data->add (YCPString ("label"), YCPString (source_descr->content_label ()));
    YCPMap labelmap;
    for (InstSrcDescr::LabelMap::const_iterator it = source_descr->content_labelmap().begin();
	it != source_descr->content_labelmap().end(); ++it)
    {
	labelmap->add (YCPString ((const std::string &)(it->first)), YCPString (it->second));
    }
    data->add (YCPString ("labelmap"), labelmap);
    data->add (YCPString ("language"), YCPString (source_descr->content_language ()));
    data->add (YCPString ("timezone"), YCPString (source_descr->content_timezone ()));
    data->add (YCPString ("descrdir"), YCPString (source_descr->content_descrdir().asString()));
    data->add (YCPString ("datadir"), YCPString (source_descr->content_datadir().asString()));
    return data;
}


/**
 * @builtin Pkg::SourceProvideFile (integer source, integer medianr, string file) -> string path
 * provide file from source to local path
 * @param source	source id from SourceList()
 * @param medianr	should be 0 (let the packagemanager decide about the media)
 * @param file		filename relative to source (i.e. CD) root path
 */
YCPValue
PkgModuleFunctions::SourceProvideFile (YCPList args)
{
    InstSrcManager::ISrcId source_id =  getSourceByArgs (args, 0);
    if (!source_id)
	return YCPVoid();

    if ((args->size() != 3)
	|| !(args->value(1)->isInteger())
	|| !(args->value(2)->isString()))
    {
	return YCPError ("Bad args to Pkg::SourceProvideFile");
    }

    constMediaAccessPtr media = source_id->media();
    std::string filename = args->value(1)->asString()->value();
    PMError err = media->provideFile (Pathname (filename));
    if (err)
    {
	y2error ("provideFile(%s) failed: %s", filename.c_str(), err.errstr().c_str());
	return YCPVoid();
    }
    return YCPString (media->localPath (filename).asString());
}


/**
 * @builtin Pkg::SourceCacheCopyTo (string dir) -> bool
 *
 * copy cache data of all installation sources to 'dir'
 * to be called at end of initial installation with
 * the target root dir.
 */
YCPValue
PkgModuleFunctions::SourceCacheCopyTo (YCPList args)
{
    if ((args->size() != 1)
	|| !(args->value(0)->isString()))
    {
	return YCPError ("Bad args to Pkg::SourceCacheCopyTo");
    }
    Pathname path (args->value(0)->asString()->value());
    PMError err = _y2pm.instSrcManager().cacheCopyTo (path);
    if (err != PMError::E_ok)
    {
	return YCPError (string ("SourceCacheCopyTo failed: ")+err.errstr(), YCPBoolean (false));
    }
    return YCPBoolean (true);
}
