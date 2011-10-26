// $Id: Projection.h,v 1.1 2005/11/15 02:55:57 hpereira Exp $
#ifndef Projection_h
#define Projection_h

/*!
	\file Projection.h
	\brief simple class to handle an histogram projection of a tree
*/

#include <string>
#include <list>
#include <TCut.h>
#include <TH1.h>

//! simple class to handle an histogram projection of a tree
class Projection
{
	public:
			
	//! constructor
	Projection( 
			const std::string& h_name,
			const std::string& var_name,
			const TCut& cut ):
		_h_name( h_name ),
		_var_name( var_name ),
		_cut( cut ),
		_h( 0 )
		{}
		
	//! define bins
	void set_bins( unsigned int bins, double min, double max )
	{
		if( _h  ) delete _h;
		_h = new TH1F( _h_name.c_str(), _h_name.c_str(), bins, min, max );
	}

	//! histogram name	
	std::string _h_name;
	
	//! variable name
	std::string _var_name;
	
	//! cut used to project histogram
	TCut _cut;
	
	//! projected histogram
	TH1 *_h;
	
};

typedef std::list<Projection> ProjectionList;

#endif	 
	
