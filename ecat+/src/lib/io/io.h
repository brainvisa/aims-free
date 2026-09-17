/****************************************************************************
 * PROJECT     : E.C.A.T.S.H.F.J. Library
 * MODULE NAME : io.h                    * TYPE     : Source
 * AUTHOR      : FROUIN V.               * CREATION : 19/05/2000
 * VERSION     : 0.1                     * REVISION :
 * LANGUAGE    : C++                     * EXAMPLE  :
 * DEVICE      : 
 ****************************************************************************
 *
 * DESCRIPTION : Ecat6-7 data reader class
 *               Version d'apres codes les codes de C. Comtat.
 *
 ****************************************************************************
 * REVISIONS :  DATE  |    AUTHOR    |       DESCRIPTION
 *--------------------|--------------|---------------------------------------
 *       07/09/2000   | COMTAT C.    |   Ajout de quatre nouvelles fonctions:
 *                    |              |
 *                    |              |   ecatOpen.c :
 *                    |              |                 EcatVersion
 *                    |              |                 EcatUeiCopy
 *                    |              |
 *                    |              |   ecatWrite.c :
 *                    |              |                 EcatWriteVolume_S16
 *                    |              |                 EcatWriteVolume_FLOAT
 *--------------------|--------------|---------------------------------------
 *                    |              |
 *       15/01/2013   | RASTELLO.F   |   Use V72 for version value when ecat7
 *                    |              |   format, for biograph reads
 *                    |              |
 ****************************************************************************/
#ifndef ECATSHFJ_IO_IO_H
#define ECATSHFJ_IO_IO_H


#include <ecat+/io/general.h>

//for biograph reads, use version 7.2 for ecat7 format
#define V72 72

#ifdef __cplusplus
  extern "C" {
#endif

/****************************************************************************/
UnifiedEcatInfo*      EcatOpen(
			       char *fname,
			       char *mode
			       );
/****************************************************************************/


/****************************************************************************/
int                   EcatClose(
				UnifiedEcatInfo *uei
				);
/****************************************************************************/

/****************************************************************************/
int                  EcatVersion(
				char *fname
				);
/****************************************************************************/

/****************************************************************************/
int                  EcatUeiCopy(
				UnifiedEcatInfo *uei_in,
				UnifiedEcatInfo *uei_out
				);
/****************************************************************************/

/****************************************************************************/
float*                EcatReadVolume_FLOAT(
				UnifiedEcatInfo *uei,
				int t
				);
/****************************************************************************/

/****************************************************************************/
short*                EcatReadVolume_S16(
				UnifiedEcatInfo *uei,
				int t
				);
/****************************************************************************/

/****************************************************************************/
int                  EcatWriteVolume_FLOAT(
				UnifiedEcatInfo *uei,
				float *volume,
				int t
				);
/****************************************************************************/

/****************************************************************************/
int                  EcatWriteVolume_S16(
				UnifiedEcatInfo *uei,
				short *volume,
				int t
				);
/****************************************************************************/


/****************************************************************************/

void EcatSinoFree(
  UnifiedEcatSinoInfo   *uesi
  );


/****************************************************************************/


/****************************************************************************/
UnifiedEcatSinoInfo*      EcatSinoOpen(
			       char *fname,
			       char *mode
			       );
/****************************************************************************/


/****************************************************************************/
int                   EcatSinoClose(
				UnifiedEcatSinoInfo *uesi
				);
/****************************************************************************/

/****************************************************************************/
short*                EcatSinoRead_S16(
				UnifiedEcatSinoInfo *uesi,
				int                  segment,
				int bed, 
				int gate, 
				int frame
				);
/****************************************************************************/

/****************************************************************************/
float*                EcatSinoRead_FLOAT(
				UnifiedEcatSinoInfo *uesi,
				int                  segment,
				int bed, 
				int gate, 
				int frame
				);
/****************************************************************************/

/****************************************************************************/
int                  EcatWriteSino_S16(
				UnifiedEcatSinoInfo *uesi,
				short               indata[],
				int                  segment,
				int bed, 
				int gate, 
				int frame
				);
/****************************************************************************/
/****************************************************************************/
int                  EcatWriteSino_FLOAT(
				UnifiedEcatSinoInfo *uesi,
				float               indata[],
				int                  segment,
				int bed, 
				int gate, 
				int frame
				);
/****************************************************************************/


#define EcatSizeX( u )            ((u)->size.x)
#define EcatSizeY( u )            ((u)->size.y)
#define EcatSizeZ( u )            ((u)->size.z)
#define EcatSizeT( u )            ((u)->size.t)
#define EcatVoxSizeX( u )         ((u)->voxelsize.x)
#define EcatVoxSizeY( u )         ((u)->voxelsize.y)
#define EcatVoxSizeZ( u )         ((u)->voxelsize.z)
#define EcatVoxSizeT( u )         ((u)->voxelsize.t)

#define EcatCalib( u )            ((u)->calib)

#define EcatVolScale( u, n )     *((u)->scale + *((u)->sortuniqlist +(n)) )
#define EcatVolStartTime( u, n ) *((u)->start_time + *((u)->sortuniqlist +(n)))
#define EcatVolDurTime( u, n )   *((u)->duration_time + *((u)->sortuniqlist +(n)) )
#define EcatRealFrameGate( u, n) *((u)->rawlist + *((u)->sortuniqlist + (n)))
#define EcatFramed( u )           ((u)->dyn_type == ECATSHFJ_FRAMED ? 1 : 0)
#define EcatGated( u )            ((u)->dyn_type == ECATSHFJ_GATED ? 1 : 0)

#define EcatCalibUnit( u )            ((u)->calibUnits)
#define EcatRadiopharmaceutical( u )  ((u)->radiopharmaceutical)
#define EcatDataCalibrated( u  )      ((u)->dataCalibrated)
#define EcatProcCode( u)              ((u)->proc_code)
#define EcatFileType( u )             ((u)->trueFileType)

#ifdef __cplusplus
  }
#endif

#endif
