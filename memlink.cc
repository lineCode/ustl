// This file is part of the ustl library, an STL implementation.
// Copyright (C) 2003 by Mike Sharov <msharov@talentg.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the 
// Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
// Boston, MA  02111-1307  USA.
//
// memlink.cc
//
//	A pointer to a sized block of memory.
//

#include "mistream.h"
#include "memlink.h"
#include "uutility.h"
#include "ualgo.h"
#include <stdlib.h>
#include <string.h>

namespace ustl {

/// Initializes both links to point to NULL,0
memlink::memlink (void)
: cmemlink (),
  m_Data (NULL)
{
}

/// Initializes the const link to point to \p p, \p n. Non-const link is NULL.
memlink::memlink (const void* p, size_t n)
: cmemlink (p, n),
  m_Data (NULL)
{
}

/// Initializes both links to point to \p p, \p n
memlink::memlink (void* p, size_t n)
: cmemlink (p, n),
  m_Data (p)
{
}

/// Copies information from \p l
memlink::memlink (const cmemlink& l)
: cmemlink (l),
  m_Data (NULL)
{
}

/// Copies information from \p l
memlink::memlink (const memlink& l)
: cmemlink (l),
  m_Data (l.m_Data)
{
}

/// Does nothing
memlink::~memlink (void)
{
}

/// Copies information from \p l
const memlink& memlink::operator= (const cmemlink& l)
{
    cmemlink::operator= (l);
    m_Data = NULL;
    return (*this);
}

/// Copies information from \p l
const memlink& memlink::operator= (const memlink& l)
{
    cmemlink::operator= (l);
    m_Data = l.m_Data;
    return (*this);
}

/// Exchanges the contents with \p l
void memlink::swap (memlink& l)
{
    cmemlink::swap (l);
    ::ustl::swap (m_Data, l.m_Data);
}

/// Initializes both links to point to \p p, \p n
void memlink::link (void* p, size_t n)
{
    cmemlink::link (p, n);
    m_Data = p;
}

/// Reads the object from stream \p s
void memlink::read (istream& is)
{
    size_t n;
    is >> n;
    assert (n % elementSize() == 0 && "You are trying to read a block with different element type.");
    size_t btr = min (n, size());
    is.read (data(), btr);
    resize (btr);
    is.skip (n - btr);
    is.align();
}

/// Resets all members to 0
void memlink::unlink (void)
{
    cmemlink::unlink();
    m_Data = NULL;
}

/// Copies data from \p p, \p n to the linked block starting at \p start.
void memlink::copy (iterator start, const void* p, size_t n)
{
    assert (m_Data || !n);
    assert (p || !n);
    assert (start >= begin() && start + n <= end());
    assert (n % elementSize() == 0 && "You are trying to write an incompatible element type");
    if (p && n && p != m_Data)
	memcpy (start, p, n);
}

/// Fills the linked block with the given pattern.
/// \arg start   Offset at which to start filling the linked block
/// \arg p       Pointer to the pattern.
/// \arg elSize  Size of the pattern.
/// \arg elCount Number of times to write the pattern.
/// Total number of bytes written is \p elSize * \p elCount.
///
void memlink::fill (iterator start, const void* p, size_t elSize, size_t elCount)
{
    assert (m_Data || !elCount || !elSize);
    assert (start >= begin() && start + elSize * elCount <= end());
    assert (elSize % elementSize() == 0 && "You are trying to write an incompatible element type");
    if (elSize == 1)
	memset (start, *reinterpret_cast<const u_char*>(p), elCount);
    else {
	while (elCount--) {
	    memcpy (start, p, elSize);
	    start += elSize;
	}
    }
}

/// Shifts the data in the linked block from \p start to \p start + \p n.
/// The contents of the uncovered bytes is undefined.
void memlink::insert (iterator start, size_t n)
{
    assert (m_Data || !n);
    assert (cmemlink::begin() || !n);
    assert (start >= begin() && start + n <= end());
    assert (n % elementSize() == 0 && "You are trying to write an incompatible element type");
    assert (distance(begin(), start) % elementSize() == 0 && "You are trying to write in the middle of an element");
    u_char* first = reinterpret_cast<u_char*>(start.base());
    u_char* last = reinterpret_cast<u_char*>(end().base());
    u_char* middle = last - n;
    rotate (first, middle, last);
}

/// Shifts the data in the linked block from \p start + \p n to \p start.
/// The contents of the uncovered bytes is undefined.
void memlink::erase (iterator start, size_t n)
{
    assert (m_Data || !n);
    assert (cmemlink::begin() || !n);
    assert (start >= begin() && start + n <= end());
    assert (n % elementSize() == 0 && "You are trying to write an incompatible element type");
    assert (distance(begin(), start) % elementSize() == 0 && "You are trying to write in the middle of an element");
    u_char* first = reinterpret_cast<u_char*>(start.base());
    u_char* last = reinterpret_cast<u_char*>(end().base());
    u_char* middle = first + n;
    rotate (first, middle, last);
}

/// Override to initialize malloc'ed space, like calling constructors, for example.
void memlink::constructBlock (void* p, size_t n) const
{
    assert (n % elementSize() == 0 && "You are trying to write an incompatible element type");
    memset (p, 0, n);
}

#ifndef NDEBUG
/// Override to deinitialize malloc'ed space, like calling destructors, for example.
void memlink::destructBlock (void* p, size_t n) const
{
    assert (n % elementSize() == 0 && "You are trying to write an incompatible element type");
    memset (p, 0xCD, n);
}
#else
void memlink::destructBlock (void*, size_t) const
{
}
#endif

} // namespace ustl

