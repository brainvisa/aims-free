/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#include <cartobase/thread/cpu.h>
#if defined(_WIN32)
// for GetSystemInfo() on Windows
#include <windows.h>
#elif defined(__APPLE__)
// for MPProcessorsScheduled() on Mac OS X
#include <CoreServices/CoreServices.h>
#elif defined(__FreeBSD__)
// for sysctl() on FreeBSD
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__)
// for sysctl() on OpenBSD and netBSD
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(__hpux)
// for pstat_getdynamic() on HP-UX
#include <sys/param.h>
#include <sys/pstat.h>
#else
// for sysconf() on Linux, Solaris, IRIX, and Tru64
#include <unistd.h>
#endif

using namespace carto;


unsigned carto::cpuCount()
{
#if defined(_WIN32)
  // total number of processors, Windows
  SYSTEM_INFO sysinfo;
  GetSystemInfo( &sysinfo );
  unsigned nCpu = (int)sysinfo.dwNumberOfProcessors;
#elif defined(__APPLE__)
  // number of processors online, Mac OS X
  unsigned nCpu = (int)MPProcessorsScheduled();
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
  // total number of processors, FreeBSD, OpenBSD, and NetBSD
  int mib[2];
  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;
  int nCpui = 1;
  size_t len = sizeof( nCpu );
  sysctl( mib, 2, &nCpui, &len, NULL, 0 );
  unsigned nCpu = (unsigned) nCpui;
#elif defined(__hpux) 
  // number of processors online, HP-UX
  struct pst_dynamic psd; 
  pstat_getdynamic(&psd, sizeof(psd), 1, 0);
  unsigned nCpu = (unsigned)psd.psd_proc_cnt;
#elif defined(__linux) || defined(__sun) || defined(__osf)
  // number of processors online, Linux, Solaris, and Tru64
  unsigned nCpu = (unsigned)sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(__sgi)
  // number of processors online, IRIX
  unsigned nCpu = (unsigned)sysconf( _SC_NPROC_ONLN );
#else
  unsigned nCpu = 1;
#endif
  return nCpu;
}


