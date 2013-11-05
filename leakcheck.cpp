// -*- C++ -*-
/*
  Copyright (c) 2012, University of Massachusetts Amherst.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*
 * @file   leakcheck.cpp
 * @brief  Detecting leakage usage case.
           Basic idea:
           We first traverse the heap to get an alive list (not freed objects) and verify whether
           these objects are reachable from stack, registers and global variables or not.
           If an object is not freed and it is not reachable, then it is a memory leak. 

           In order to verify whether an object is reachable, we start from the root list (those 
           possible reachable objects).

           However, it is much easier for the checking in the end of a program. We basically think
           that every object should be freed. Thus, we only needs to search the heap list to find
           those unfreed objects. If there is someone, then we reported that and rollback.  

           In order to detect those callsites for those memory leakage, we basically maintain 
           a hashtable. Whenever there is memory allocation, we check whether this object is a 
           possible memory leakage. If yes, then we update corresponding list about how many leakage
           happens on each memory allocation site. 

 * @author Tongping Liu <http://www.cs.umass.edu/~tonyliu>
 */
#include <set>
#include <ucontext.h>
#include <internalheap.h>
#include "leakcheck.h"
#include "threadmap.h"
#include "xmemory.h"

void leakcheck::searchHeapPointersInsideGlobals(void) {
  int totalRegions;

  totalRegions = xmemory::getInstance().getGlobalRegionsNumb();
  unsigned long begin, end;

  for(int i = 0; i < totalRegions; i++) {
    xmemory::getInstance().getGlobalRegion(i, &begin, &end);
    if(begin != 0) {
      searchHeapPointers(begin, end);
    } 
  }
}
