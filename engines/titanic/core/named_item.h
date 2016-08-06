/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TITANIC_NAMED_ITEM_H
#define TITANIC_NAMED_ITEM_H

#include "titanic/core/tree_item.h"

namespace Titanic {

class CViewItem;
class CNodeItem;
class CRoomItem;

class CNamedItem: public CTreeItem {
	DECLARE_MESSAGE_MAP;
public:
	CString _name;
public:
	CLASSDEF;

	/**
	 * Dump the item
	 */
	virtual CString dumpItem(int indent) const;

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent);

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);

	/**
	 * Gets the name of the item, if any
	 */
	virtual const CString getName() const { return _name; }

	/**
	 * Compares the name of the item to a passed name
	 */
	virtual int compareTo(const CString &name, int maxLen) const;

	/**
	 * Find a parent node for the item
	 */
	virtual CViewItem *findView() const;

	/**
	 * Find a parent node for the item
	 */
	virtual CNodeItem *findNode() const;

	/**
	 * Find a parent room item for the item
	 */
	virtual CRoomItem *findRoom() const;
};

} // End of namespace Titanic

#endif /* TITANIC_NAMED_ITEM_H */