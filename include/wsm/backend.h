/*
Wayland Security Module
Copyright (C) 2014 Martin Peres

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef WSM_BACKEND_H
#define WSM_BACKEND_H

/**
 * \file backend.h
 * \author MùPùF - Martin Peres (martin dot peres at free.fr dot fr)
 * \date 5-10-2014
 */

#ifdef __cplusplus
extern "C"
{
#endif

	/*! \brief Get the space that would be needed if we were to store the entry in a string.
	* \param entry The concerned entry.
	* \return  Return the space that would be needed if we were to store the entry in a string.*/
	const char* getModuleName(const ppk_entry* entry);

	/*! \brief Convert an entry to a string
	* \param entry The entry to be converted.
	* \param[out] returned_key The string will be copied in this buffer.
	* \param max_key_size How much character are we allowed to copy to returned_key ?
	* \return Returns PPK_TRUE if everything went fine and PPK_FALSE if there were not enough room in returned_key to fit the given entry.*/
	ppk_boolean ppk_get_key(const ppk_entry* entry, char* returned_key, size_t max_key_size);

	/*! \brief Convert a string to a ppk_entry
	* \param key The key to be converted to a ppk_entry.
	* \return Returns a ppk_entry if everything went fine, NULL otherwise.*/
	ppk_entry* ppk_entry_new_from_key(const char* key);

#ifdef __cplusplus
}
#endif

#endif //WSM_BACKEND_H
