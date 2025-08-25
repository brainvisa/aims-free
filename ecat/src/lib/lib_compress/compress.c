#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <ecat/lib_compress/compress.h>
#include <ecat/lib_compress/dclz.h>
#include <ecat/lib_compress/uvlc.h>

#if defined(unix) || defined(__osf__) || defined(__APPLE__)
#include <unistd.h>
#else /* WIN32 */
#include <io.h>
#define R_OK 4
#define stat _stat
#define access _access
#endif

#if defined(__alpha) || defined(_WIN32)
#define  _LITTLE_ENDIAN 1
#else
#define _LITTLE_ENDIAN 0
#endif

static char *z_infile=NULL;
static int fd_in = 0;
static char *z_outfile=NULL;
static int fd_out = 0;

#ifdef ultrix
static char *strdup(line)
char *line;
{
	char *ret = malloc(strlen(line)+1);
	strcpy(ret,line);
	return ret;
}
#endif

#ifdef _WIN32
// On mingw, mkstemp does not exist

#include <errno.h>
#include <fcntl.h>
static const char letters[] =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

/* Generate a temporary file name based on TMPL.  TMPL must match the
   rules for mk[s]temp (i.e. end in "XXXXXX").  The name constructed
   does not exist at the time of the call to mkstemp.  TMPL is
   overwritten with the result.  */
int
mkstemp (char *tmpl)
{
  int len;
  char *XXXXXX;
  static unsigned long long value;
  unsigned long long random_time_bits;
  unsigned int count;
  int fd = -1;
  int save_errno = errno;

  /* A lower bound on the number of temporary files to attempt to
     generate.  The maximum total number of temporary file names that
     can exist for a given template is 62**6.  It should never be
     necessary to try all these combinations.  Instead if a reasonable
     number of names is tried (we define reasonable as 62**3) fail to
     give the system administrator the chance to remove the problems.  */
#define ATTEMPTS_MIN (62 * 62 * 62)

  /* The number of times to attempt to generate a temporary file.  To
     conform to POSIX, this must be no smaller than TMP_MAX.  */
#if ATTEMPTS_MIN < TMP_MAX
  unsigned int attempts = TMP_MAX;
#else
  unsigned int attempts = ATTEMPTS_MIN;
#endif

  len = strlen (tmpl);
  if (len < 6 || strcmp (&tmpl[len - 6], "XXXXXX"))
    {
      errno = EINVAL;
      return -1;
    }

/* This is where the Xs start.  */
  XXXXXX = &tmpl[len - 6];

  /* Get some more or less random data.  */
  {
    SYSTEMTIME      stNow;
    FILETIME ftNow;

    // get system time
    GetSystemTime(&stNow);
    stNow.wMilliseconds = 500;
    if (!SystemTimeToFileTime(&stNow, &ftNow))
    {
        errno = -1;
        return -1;
    }

    random_time_bits = (((unsigned long long)ftNow.dwHighDateTime << 32)
                        | (unsigned long long)ftNow.dwLowDateTime);
  }
  value += random_time_bits ^ (unsigned long long)GetCurrentThreadId ();

  for (count = 0; count < attempts; value += 7777, ++count)
    {
      unsigned long long v = value;

      /* Fill in the random bits.  */
      XXXXXX[0] = letters[v % 62];
      v /= 62;
      XXXXXX[1] = letters[v % 62];
      v /= 62;
      XXXXXX[2] = letters[v % 62];
      v /= 62;
      XXXXXX[3] = letters[v % 62];
      v /= 62;
      XXXXXX[4] = letters[v % 62];
      v /= 62;
      XXXXXX[5] = letters[v % 62];

      fd = open (tmpl, O_RDWR | O_CREAT | O_EXCL, _S_IREAD | _S_IWRITE);
      if (fd >= 0)
    {
      errno = save_errno;
      return fd;
    }
      else if (errno != EEXIST)
    return -1;
    }

  /* We got out of the loop because we ran out of combinations to try.  */
  errno = EEXIST;
  return -1;
}
#endif

static int init_tmp_files() {
	char lineIn[80], lineOut[80];
	strcpy( lineIn, "ecatXXXXXX" );
	strcpy( lineOut, "ecatXXXXXX" );
	if (z_infile == NULL) {
		    int f = mkstemp( lineIn );
        if (f < 0) return 0;
        z_infile = strdup(lineIn);
				fd_in = f;
	}
	if (z_outfile == NULL) {
		    int f = mkstemp( lineOut );
        if (f < 0) return 0;
        z_outfile = strdup(lineOut);
				fd_out = f;
	}
	return 1;
}

static void short_order(data,data_size,data_type)
char *data;
int data_size,data_type;
{
	switch( data_type)
	{
		case VAX_Ix2:
			if (!_LITTLE_ENDIAN) swab( data, data, data_size);
			break;
		case SunShort:
			if (_LITTLE_ENDIAN) swab(data, data, data_size);
			break;
	}
}

static void int_order(data,data_size,data_type)
char *data;
int data_size,data_type;
{
	switch( data_type)
	{
		case VAX_Ix2:
			if (!_LITTLE_ENDIAN) {
				swab( data, data, data_size);
				swaw( (short*)data, (short*)data, data_size/2);
			}
			break;
		case SunShort:
			if (_LITTLE_ENDIAN) {
				swab(data, data, data_size);
				swaw((short*)data, (short*)data, data_size/2);
			}
		  break;
	}
}

int compression_method(str)
char *str;
 {
	char *p=NULL;
	char *comp_env = str;

	if (comp_env == NULL)	/* if not specified environment variable*/
		comp_env = getenv("MATRIX_COMPRESSION_METHOD");
	if (comp_env == NULL) return 1;		/* if none specified use unix */
										/* force lower case */
	for (p=comp_env; *p != 0; p++) *p = tolower(*p);
	if (strcmp(comp_env,"none") == 0) return 0;
	if (strcmp(comp_env,"unix") == 0) return 1;
	if (strcmp(comp_env,"dclz") == 0) return 2;
	if (strcmp(comp_env,"uvlc") == 0) return 3;
	return 1;
}

int compress(MatrixData *matrix_data, int data_type, char *z_data, int mode)
{
	char line[80];
	FILE *fp=NULL;
	struct stat st;
	int  z_data_size,data_size;
	int skip_dpcm = 0, code_length = 1024;
	char *data = matrix_data->data_ptr;
	int xdim =  matrix_data->xdim;
	int ydim =  matrix_data->ydim;

										/* compress only short matrices */
	if (data_type != VAX_Ix2 && data_type != SunShort) return 0;
	data_size = xdim*ydim*matrix_data->zdim*sizeof(short);
	if (!init_tmp_files()) return 0;
	if ( (fp=fdopen(fd_in,"w")) == NULL) return 0;
	if (mode != 3)	{	/* byte based method, swap before compress  */
		short_order(data,data_size,data_type);
		fwrite(data,1,data_size,fp);
		short_order(data,data_size,data_type);
	} else fwrite(data,1,data_size,fp);
	fclose(fp);
	switch(mode) {
	case 1:				/* unix */
		if (access(z_outfile,R_OK)==0) unlink(z_outfile);	/* already exists */
		sprintf(line,"compress < %s > %s",z_infile,z_outfile);
		if ( system(line) < 0 )
		  return 0;
		break;
	case 2:				/* dclz */
		dclz(z_infile, z_outfile, 'c');
		break;
	case 3:				/* uvlc */
		pcpet(skip_dpcm, code_length, z_infile, z_outfile,xdim,ydim);
		break;
	default:
		unlink(z_infile);
		return 0;
	}
	unlink(z_infile);
	if (stat(z_outfile, &st) < 0) return 0;
	z_data_size = ((st.st_size+511)/512)*512;		/* align to block */
	if ((fp=fdopen(fd_out,"r")) == NULL) return 0;
	if (z_data == NULL) z_data = calloc(z_data_size,1);
	if ( fread(z_data,1,st.st_size,fp) != (size_t)st.st_size )
	{
	  fclose(fp);
		return 0;
	}
	fclose(fp);
	unlink(z_outfile);
	if (mode == 3)	{	/* int based method, swap after compress  */
		int_order(z_data,z_data_size,data_type);
	}
	return z_data_size;
}
	
int uncompress(char *z_data, int z_data_size, MatrixData *matrix_data,
	       int mode)
{
	FILE *fp=NULL;
	char line[80];
	int data_size;
	int skip_dpcm = 0, code_length = 1024;
	int data_type = matrix_data->data_type;
	int xdim =  matrix_data->xdim;
	int ydim =  matrix_data->ydim;

	if (data_type != VAX_Ix2 && data_type != SunShort) return 0;
	data_size = xdim*ydim*matrix_data->zdim*sizeof(short);
	if (!init_tmp_files()) return 0;
	if ( (fp=fdopen(fd_in,"w")) == NULL) return 0;
	if (mode == 3)	{	/* int based method, swap before uncompress  */
		int_order(z_data,z_data_size,data_type);
	}
	fwrite(z_data,1,z_data_size,fp);
	fclose(fp);
	switch(mode) {
	case 1:				/* unix */
		if (access(z_outfile,R_OK)==0) unlink(z_outfile);	/*already exists */
		sprintf(line,"uncompress < %s > %s",z_infile,z_outfile);
		if ( system(line) < 0 )
		  return 0;
		break;
	case 2:				/* dclz */
		dclz(z_infile, z_outfile, 'd');
		break;
	case 3:				/* ulvc */
		pdpet(skip_dpcm, code_length, z_infile, z_outfile,xdim,ydim);
		break;
	default:
		unlink(z_infile);
		return 0;
	}
	unlink(z_infile);
	if ((fp=fdopen(fd_out,"r")) ==NULL) return 0;
	if (matrix_data->data_ptr == NULL)
		matrix_data->data_ptr = calloc((data_size+511)/512,512);
														/* align to block */
	if (fread(matrix_data->data_ptr,1,data_size,fp) != (size_t)data_size)
		 data_size = 0;
	fclose(fp);
	unlink(z_outfile);
	if (data_size == 0) return 0;
	if (mode != 3)	{	/* byte based method, swap after compress  */
		short_order(matrix_data->data_ptr,data_size,data_type);
	}
	return data_size;
}
