/*****************************************************************************
 *   Copyright (C) 2004-2014 The PyKEP development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   http://keptoolbox.sourceforge.net/index.html                            *
 *   http://keptoolbox.sourceforge.net/credits.html                          *
 *                                                                           *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

#ifndef PLANET_TLE_H
#define PLANET_TLE_H

#include "serialization.h"
#include "planet.h"
#include "config.h"
#include "third_party/libsgp4/SGP4.h"
#include "third_party/libsgp4/Tle.h"

namespace kep_toolbox{

/// A planet from TLE format
/**
 * This class derives from the planet class and allows to instantiate Earth-orbiting
 * satellites from their Two Line Element format. The ephemerides will then be computed
 * using SGP4/SDP4 orbital model. The third party C++ library SGP4 Satellite Library is
 * used (source code in tp/libsgp4)
 *
 * NOTE: the constant used in the satellite data memebr initialization are not the pykep ones, rather the 
 * constants defined in the sgp4lib are used (tp/libsgp4/Globals.h)
 *
 * @see http://celestrak.com/columns/v04n03/#FAQ01
 * @see http://www.danrw.com/sgp4/
 *
 * @author Dario Izzo (dario.izzo _AT_ googlemail.com)
 */

class __KEP_TOOL_VISIBLE planet_tle : public planet
{
public:
	/**
	 * Construct a planet_tle from two strings containing the two line elements
	 * \param[in] line1 first line
	 * \param[in] line2 second line
	 */
	planet_tle(const std::string & = "1 23177U 94040C   06175.45752052  .00000386  00000-0  76590-3 0    95", const std::string & = "2 23177   7.0496 179.8238 7258491 296.0482   8.3061  2.25906668 97438");
	planet_ptr clone() const;
	/// Computes the planet/system position and velocity w.r.t the Sun
	/**
		* \param[in] when Epoch in which ephemerides are required
		* \param[out] r Planet position at epoch (SI units)
		* \param[out] v Planet velocity at epoch (SI units)
		*/
	void get_eph(const epoch& when, array3D &r, array3D &v) const;
private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<planet>(*this);
		ar & const_cast<std::string& >(m_line1);
		ar & const_cast<std::string& >(m_line2);
		boost::serialization::split_member(ar, *this, version);
	}

	template <class Archive>
		void save(Archive &, const unsigned int) const
		{}

	template <class Archive>
		void load(Archive &, const unsigned int)
		{
			// NOTE: the Tle and SGP4 data members are not saved during serialization. Hence, upon loading,
			// we are going to build them again from data. This set up was chosen to avoid implementing
			// serialization of the third-party library libsgp4 objects
			m_tle = Tle("TLE satellite", m_line1, m_line2);
			m_sgp4_propagator = SGP4(m_tle);
		}

		const std::string m_line1;
		const std::string m_line2;
		Tle m_tle;
		SGP4 m_sgp4_propagator;
	};


} /// End of namespace kep_toolbox

BOOST_CLASS_EXPORT_KEY(kep_toolbox::planet_tle)

#endif // PLANET_TLE_H
