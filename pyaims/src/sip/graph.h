/* Copyright (c) 1995-2006 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef PYAIMS_GRAPH_GRAPH_H
#define PYAIMS_GRAPH_GRAPH_H

inline PyObject* pyaimsConvertFrom_VertexP( void * a )
{
  return sipConvertFromInstance( a, sipClass_Vertex, 0 );
}


inline void* pyaimsConvertTo_VertexP( PyObject * o )
{
  int isErr = 0;
  return sipConvertToInstance( o, sipClass_Vertex, 0, 0, 0,
                               &isErr );
}


inline int pyaimsVertexP_Check( PyObject* o )
{
  return sipIsSubClassInstance( o, sipClass_Vertex );
}


inline PyObject* pyaimsConvertFrom_EdgeP( void * a )
{
  return sipConvertFromInstance( a, sipClass_Edge, 0 );
}


inline void* pyaimsConvertTo_EdgeP( PyObject * o )
{
  int isErr = 0;
  return sipConvertToInstance( o, sipClass_Edge, 0, 0, 0,
                               &isErr );
}


inline int pyaimsEdgeP_Check( PyObject* o )
{
  return sipIsSubClassInstance( o, sipClass_Edge );
}


#endif

