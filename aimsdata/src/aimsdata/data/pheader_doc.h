/*
 *  AttributedHeader class
 */
#ifndef AIMS_DATA_PHEADER_DOC_H
#define AIMS_DATA_PHEADER_DOC_H

/*!	\class aims::PythonHeader <aims/data/pheader.h>
  \brief Attributed python-like header, stores all needed information about an 
  object, currently used for volumes (AimsData).

    This will allows to use python dictionaries file format.
 
    Attributes currently stored:

    Attribute name     type             Comment

  - file_type          string           SPM, DICOM, ...
  - data_type          string           U8, U16, S16,...
  - disk_data_type     string           U8... (disk file original format)
  - possible_data_types vector< string >  list of compatible data types 
					allowed for reading, first is 
					default
  - ascii              int              0 or not present : binary
  - byte_swapping      int              0:not needed
  - spm_normalized     bool
  - spm_origin         vector< float >  (cx,cy,cz) in voxels in SPM orientation
  - origin             vector< float >  (cx,cy,cz) in memory orientation
  - transfer_syntax    string
  - manufacturer       string
  - modality           string
  - patient_id         string
  - study_id           string
  - series_number      int
  - volume_dimension   vector< int >      (dimx,dimy,dimz,dimt)
  - voxel_size         vector< float >  (sx,sy,sz) (sz=slice thickness)
  - time_resolution    float            sizeT, can be equal to tr
  - bits_allocated     int              8, 16, ...
  - te                 float            echo time (in seconds)
  - tr                 float            repetition time (in seconds)
  - ti                 float            inversion time (in seconds)
  - flip_angle         float            flip angle (=tip angle)
  - slice_thickness    float
  - inter_slices       float            spacing between slices
  - nb_t_pos           int              number of temporal positions 
					(=dimT)
  - start_time         vector< int >    start time for dynamic series
  - duration_time      vector< int >    duration time for dynamic series
  - isotope            string
  - radiopharamaceutical  string
  - isotope_halflife   float
  - image_unit         string           either Bq/CC ECAT Count/sec Unknown
					or labels
  - process_code       int
  - process_list       string           "Norm", "Atten_Meas", "Atten_Calc",
					"Smooth_X", "Smooth_Y", "Smooth_Z",
					"Scat2d", "Scat3d", "ArcPrc",
					"DecayPrc", "OnCompPrc", "Random",
  - category	       string
  - zero_start_time    int
  - original_filename  string
  - filter_code        int
  - filter_type        string
  - filter_param       vector< float >  6 float numbers 
  - polygon_dimension  int              mesh: num of vertices per polygon
  - vertex_number      int              mesh: num of vertices (1st time pos)
  - b_value            float            B value of MR diffusion sequence
  - scale_factor       float            scale factor.
  - scale_factor_applied bool           Is SPM scale factor already applied ?
  - vox_units		string 		SPM parameter
  - cal_units		string		SPM parameter
  - db_name		string		SPM parameter
  - aux_file		string		SPM parameter
  - orient		char		SPM parameter
  - generated		string		SPM parameter
  - scannum		string		SPM parameter
  - patient_id		string		SPM parameter
  - exp_date		string		SPM parameter
  - exp_time		string		SPM parameter
  - views		int		SPM parameter
  - start_field		int		SPM parameter
  - field_skip		int		SPM parameter
  - omax		int		SPM parameter
  - omin		int		SPM parameter
  - smax		int		SPM parameter
  - smin		int		SPM parameter
  - SPM_data_type	string		SPM parameter
  - minimum		int		SPM parameter
  - maximum		int		SPM parameter
  - history		string_vector

  - arg_syntax		string		syntaxic attribute for a graph file
*/

#endif
