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

#ifndef AIMS_IO_GIFTIMESHFORMAT_D_H
#define AIMS_IO_GIFTIMESHFORMAT_D_H

#include <aims/io/giftiformat.h>
#include <aims/io/giftiheader.h>
#include <aims/io/giftiutil.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>
#include <aims/io/reader.h>
extern "C" {
#include <gifti_io.h>
}

namespace aims {

namespace {
template<int D, typename T>
void giftiReadTexture(AimsTimeSurface<D, T> & vol, int texnum,
		giiDataArray *da, GiftiHeader & hdr) {
	int vnum = da->dims[0];
	int j;
	std::vector<T> & tex = vol[texnum].texture();
	tex.clear();
	tex.reserve(vnum);
	for (j = 0; j < vnum; ++j) {
		tex.push_back(convertedNiftiValue<T> (da->data, j, da->datatype));
	}
}

class GiftiReadExternalTexture: public Process {
public:
	GiftiReadExternalTexture(carto::Object textures, giiDataArray* da, int nt,
			int nts) :
		Process(), textures(textures), da(da), nt(nt), nts(nts) {
	}

	carto::Object textures;
	giiDataArray *da;
	int nt;
	int nts;
};

template<typename T>
bool giftiReadExternalTexture(Process & p, const std::string &, Finder &) {
	GiftiReadExternalTexture & gp = static_cast<GiftiReadExternalTexture &> (p);
	giiDataArray *da = gp.da;
	int j, vnum = da->dims[0], t, nt = gp.nt, nts = gp.nts;
	carto::Object textures = gp.textures;
	carto::Object o;
	if ((int) textures->size() < nt)
		o = textures->getArrayItem(nt);
	else {
		o = carto::Object::value(TimeTexture<T> ());
		textures->insertArrayItem(-1, o);
	}
	TimeTexture<T> & ttex = o->carto::GenericObject::value<TimeTexture<T> >();
	ttex[ttex.size()]; // force inserting a new timepoint
	std::vector<T> & tex = ttex[ttex.size() - 1].data();
	tex.reserve(vnum);
	for (j = 0; j < vnum; ++j) {
		tex.push_back(convertedNiftiValue<T> (da->data, j, da->datatype));
	}

	return true;
}

template<int D>
void giftiReadTexture(AimsTimeSurface<D, Void> & /*vol*/, int /*texnum*/,
		giiDataArray *da, GiftiHeader & hdr, int nts) {
	carto::Object textures;
	try {
		textures = hdr.getProperty("textures");
	} catch (...) {
		textures = carto::Object::value(carto::ObjectVector());
	}
	// get data type
	int ndim = da->num_dim;
	int nt = 1;
	std::string dtype = giftiTextureDataType(da->datatype, ndim, da->dims,
			da->intent, nt);
	// std::cout << "reading texture of: " << dtype << std::endl;

	GiftiReadExternalTexture p(textures, da, nt, nts);
	p.registerProcessType("Texture", "FLOAT", &giftiReadExternalTexture<float> );
	p.registerProcessType("Texture", "POINT2DF", &giftiReadExternalTexture<
			Point2df> );
	p.registerProcessType("Texture", "S16", &giftiReadExternalTexture<int16_t> );
	p.registerProcessType("Texture", "S32", &giftiReadExternalTexture<int32_t> );
	p.registerProcessType("Texture", "U32", &giftiReadExternalTexture<uint32_t> );
	p.registerProcessType("Texture", "VECTOR_OF_2_S16",
			&giftiReadExternalTexture<Point2d> );
	// TODO: etc for other types...
	Finder f;
	f.setObjectType("Texture");
	f.setDataType(dtype);
	if (p.execute(f, "")) {
		hdr.setProperty("textures", textures);
	}
}

}

template<int D, typename T>
bool GiftiMeshFormat<D, T>::read(const std::string & filename, AimsTimeSurface<
		D, T> & vol, const carto::AllocatorContext & /*context*/,
		carto::Object options) {
	GiftiHeader hdr(filename);

	setOptions(options);

	if (!hdr.read())
		carto::io_error::launchErrnoExcept(hdr.name());

	gifti_image *gim = gifti_read_image(hdr.name().c_str(), 1);
	if (!gim) {
		throw carto::format_error("could not re-read GIFTI file", hdr.name());
	}

	int nda = gim->numDA, i;
	int tmesh = -1, ttex = -1, tnorm = -1, tpoly = -1, nts = 0;
	for (i = 0; i < nda; ++i) {
		giiDataArray *da = gim->darray[i];

		switch (da->intent) {
		case NIFTI_INTENT_POINTSET: {
			++tmesh;
			char *ts = gifti_get_meta_value(&da->meta, "Timestep");
			if (ts)
				sscanf(ts, "%d", &tmesh);
			int vnum = da->dims[0];
			int j;
			std::vector<Point3df> & vert = vol[tmesh].vertex();
			vert.clear();
			vert.reserve(vnum);
			for (j = 0; j < vnum; ++j)
				vert.push_back(Point3df(convertedNiftiValue<float> (da->data, j
						* 3, da->datatype), convertedNiftiValue<float> (
						da->data, j * 3 + 1, da->datatype),
						convertedNiftiValue<float> (da->data, j * 3 + 2,
								da->datatype)));
		}
			break;
		case NIFTI_INTENT_VECTOR: {
			++tnorm;
			char *ts = gifti_get_meta_value(&da->meta, "Timestep");
			if (ts)
				sscanf(ts, "%d", &tnorm);
			else if (tnorm < tmesh)
				tnorm = tmesh;
			int vnum = da->dims[0];
			int j;
			std::vector<Point3df> & norm = vol[tnorm].normal();
			norm.clear();
			norm.reserve(vnum);
			for (j = 0; j < vnum; ++j)
				norm.push_back(Point3df(convertedNiftiValue<float> (da->data, j
						* 3, da->datatype), convertedNiftiValue<float> (
						da->data, j * 3 + 1, da->datatype),
						convertedNiftiValue<float> (da->data, j * 3 + 2,
								da->datatype)));
			break;
		}
		case NIFTI_INTENT_TRIANGLE: {
			++tpoly;
			char *ts = gifti_get_meta_value(&da->meta, "Timestep");
			if (ts)
				sscanf(ts, "%d", &tpoly);
			else if (tpoly < tmesh)
				tpoly = tmesh;
			int vnum = da->dims[0];
			int j, k;
			std::vector<AimsVector<unsigned, D> > & poly = vol[tpoly].polygon();
			poly.clear();
			poly.reserve(vnum);
			for (j = 0; j < vnum; ++j) {
				poly.push_back(AimsVector<unsigned, D> ());
				AimsVector<unsigned, D> &p = poly[j];
				for (k = 0; k < D; ++k)
					p[k] = convertedNiftiValue<unsigned> (da->data, j * D + k,
							da->datatype);
			}
			break;
		}
		case NIFTI_INTENT_TIME_SERIES:
			if (nts == 0)
				++ttex;
			giftiReadTexture(vol, ttex, da, hdr, nts);
			++nts;
			break;
		default: {
			// texture
			++ttex;
			nts = 0;
			giftiReadTexture(vol, ttex, da, hdr, 0);
		}
		}
	}

	++tmesh;
	++tnorm;
	++tpoly;
	++ttex;
	if (tmesh > tpoly)
		tmesh = tpoly;
	/*    if( vol.size() > (unsigned) tmesh )
	 {
	 std::cout << "warning: some incomplete meshes; truncating "
	 << vol.size() << " instead of " << tmesh << "elements" << std::endl;
	 while( vol.size() > (unsigned) tmesh )
	 static_cast<std::map<int,AimsSurface<D,T> > &>(vol).erase(
	 (int) vol.size() - 1 );
	 }*/
	// verify polygons are all in the vertices range
	bool broken = false;
	for (i = 0; i < tmesh; ++i) {
		AimsSurface<D, T> & surf = vol[i];
		std::vector<Point3df> & vert = surf.vertex();
		std::vector<AimsVector<uint, D> > & poly = surf.polygon();
		typename std::vector<AimsVector<uint, D> >::iterator ip;
		unsigned nvertex = vert.size();
		for (ip = poly.begin(); ip != poly.end(); ++ip) {
			AimsVector<uint, D> & pol = *ip;
			for (int j = 0; j < D; ++j)
				if (pol[j] >= nvertex) {
					if (!broken) {
						broken = true;
						std::cerr << "Broken mesh: polygon pointing to a "
								<< "vertex out of range" << std::endl;
						// std::cout << pol[j] << " / " << nvertex << std::endl;
					}
					poly.erase(ip);
					--ip;
					break;
				}
		}

		if (surf.normal().size() != vert.size())
			surf.updateNormals();
	}

	vol.setHeader(hdr);
	gifti_free_image(gim);

	std::cout << "gifti mesh read OK\n";
	return true;
}

template<int D, typename T>
bool GiftiMeshFormat<D, T>::write(const std::string & filename,
		const AimsTimeSurface<D, T> & thing, bool ascii) {
	std::cout << "gifti mesh write\n";
	try {
		const PythonHeader & thdr = thing.header();
		GiftiHeader hdr(filename);
		hdr.copy(thdr);

		if (hdr.hasProperty("nb_t_pos"))
			hdr.removeProperty("nb_t_pos");
		gifti_image *gim = hdr.giftiImageBase();
		std::string fname = hdr.name();

		int hdrmeshda = 0, hdrnormda = 0, hdrpolyda = 0, hdrtexda = 0;

		bool normal = false;
		int encoding = 2;

		carto::Object da_info;
		try {
			da_info = thdr.getProperty("GIFTI_dataarrays_info");

			carto::Object it = da_info->objectIterator();
			for (; it->isValid(); it->next()) {
				carto::Object el = it->currentValue();
				if (el->getProperty("encoding", encoding))
					break;
			}

		} catch (...) {
			//std::cout << "error GIFTI_dataarrays_info\n";
		}

		if (!options().isNull()) {
			hdr.setOptions(options());
			try {
				carto::Object n = options()->getProperty("normal");
				normal = (bool) n->getScalar();
				carto::Object a = options()->getProperty("encoding");
				if ((int) a->getScalar() != 0) {
					encoding = (int) a->getScalar();
				}

		}
		catch( ... )
		{
		}
	}

	int nda = 0, t = 0;
	typename AimsTimeSurface<D, T>::const_iterator is, es=thing.end();

	for( is=thing.begin(); is!=es; ++is, ++t )
	{
		const AimsSurface<D, T> & surf = is->second;
		// write vertices
		{
			const std::vector<Point3df> & vert = surf.vertex();
			nda = gim->numDA;
			gifti_add_empty_darray( gim, 1 );
			giiDataArray * da = gim->darray[nda];
			gifti_set_DA_defaults( da );
			da->intent = NIFTI_INTENT_POINTSET;
			da->datatype = NIFTI_TYPE_FLOAT32;
			da->num_dim = 2;
			da->dims[0] = vert.size();
			da->dims[1] = 3;
			da->dims[2] = 0;
			da->dims[3] = 0;
			da->dims[4] = 0;
			da->dims[5] = 0;
			da->nvals = vert.size() * 3;
			da->nbyper = 4;

			da->encoding = encoding;
			//GIFTI_ENCODING_ASCII     1      /* human readable ASCII data  */
			//GIFTI_ENCODING_B64BIN    2      /* base64 encoded binary data */
			//GIFTI_ENCODING_B64GZ     3      /* base64 compressed binary   */

			gifti_alloc_DA_data( gim, &nda, 1 );
			unsigned i, n = vert.size();

			float *buf = reinterpret_cast<float *>( da->data );
			for( i=0; i<n; ++i )
			{
				buf[i*3] = vert[i][0];
				buf[i*3+1] = vert[i][1];
				buf[i*3+2] = vert[i][2];
			}

			if( t != is->first )
			{
				// store timestep
				std::ostringstream ts;
				ts << is->first;
				gifti_add_to_meta( &da->meta, "Timestep", ts.str().c_str(), 1 );
			}

			//TO DO : write that BV is in left hand referential
			//            gifti_add_empty_CS (da);
			//
			//			giiCoordSystem * csnew = da->coordsys[da->numCS-1];
			//			int              r, c;
			//
			//			csnew->dataspace  = gifti_strdup("Brainvisa Referential");
			//			csnew->xformspace = gifti_strdup("indirect reference");
			//
			//			for( r = 0; r < 4; r++ )
			//			  for( c = 0; c < 4; c++ )
			//			    csnew->xform[r][c] = 0;
			//
			//			 csnew->xform[0][0] = -1;
			//			 csnew->xform[1][1] = 1;
			//			 csnew->xform[2][2] = 1;
			//			 csnew->xform[3][3] = 1;

			// metadata
			carto::Object dainf
			= GiftiHeader::giftiFindHdrDA( hdrmeshda, da_info,
					"NIFTI_INTENT_POINTSET" );
			if( !dainf.isNone() )
			{
				++hdrmeshda;
				GiftiHeader::giftiCopyMetaToGii( dainf, da );
			}
		}

		// write normals
		if (normal)
		{
			const std::vector<Point3df> & norm = surf.normal();
			if( !norm.empty() )
			{
				nda = gim->numDA;
				gifti_add_empty_darray( gim, 1 );
				giiDataArray * da = gim->darray[nda];
				gifti_set_DA_defaults( da );
				da->intent = NIFTI_INTENT_VECTOR;
				da->datatype = NIFTI_TYPE_FLOAT32;
				da->num_dim = 2;
				da->dims[0] = norm.size();
				da->dims[1] = 3;
				da->dims[2] = 0;
				da->dims[3] = 0;
				da->dims[4] = 0;
				da->dims[5] = 0;
				da->nvals = norm.size() * 3;
				da->nbyper = 4;

				da->encoding = encoding;

				gifti_alloc_DA_data( gim, &nda, 1 );
				unsigned i, n = norm.size();
				float *buf = reinterpret_cast<float *>( da->data );
				for( i=0; i<n; ++i )
				{
					buf[i*3] = norm[i][0];
					buf[i*3+1] = norm[i][1];
					buf[i*3+2] = norm[i][2];
				}

				if( t != is->first )
				{
					// store timestep
					std::ostringstream ts;
					ts << is->first;
					gifti_add_to_meta( &da->meta, "Timestep", ts.str().c_str(), 1 );
				}

				// metadata
				carto::Object dainf
				= GiftiHeader::giftiFindHdrDA( hdrnormda, da_info,
						"NIFTI_INTENT_VECTOR" );
				if( !dainf.isNone() )
				{
					++hdrnormda;
					GiftiHeader::giftiCopyMetaToGii( dainf, da );
				}
			}
		}

		// write polygons
		{
			const std::vector<AimsVector<unsigned, D> > & poly
			= surf.polygon();
			nda = gim->numDA;
			gifti_add_empty_darray( gim, 1 );
			giiDataArray * da = gim->darray[nda];
			gifti_set_DA_defaults( da );
			da->intent = NIFTI_INTENT_TRIANGLE;
			da->datatype = NIFTI_TYPE_INT32;
			da->num_dim = 2;
			da->dims[0] = poly.size();
			da->dims[1] = D;
			da->dims[2] = 0;
			da->dims[3] = 0;
			da->dims[4] = 0;
			da->dims[5] = 0;
			da->nvals = poly.size() * D;
			da->nbyper = 4;

			da->encoding = encoding;

			gifti_alloc_DA_data( gim, &nda, 1 );
			unsigned i, j, n = poly.size();
			int *buf = reinterpret_cast<int *>( da->data );
			for( i=0; i<n; ++i )
			for( j=0; j<D; ++j )
			buf[i*D+j] = poly[i][j];

			if( t != is->first )
			{
				// store timestep
				std::ostringstream ts;
				ts << is->first;
				gifti_add_to_meta( &da->meta, "Timestep", ts.str().c_str(), 1 );
			}

			// metadata
			carto::Object dainf
			= GiftiHeader::giftiFindHdrDA( hdrpolyda, da_info,
					"NIFTI_INTENT_TRIANGLE" );
			if( !dainf.isNone() )
			{
				++hdrpolyda;
				GiftiHeader::giftiCopyMetaToGii( dainf, da );
			}
		}
		// write texture
		const std::vector<T> & tex = surf.texture();
		hdr.giftiAddTexture( gim, tex );

		// metadata
		carto::Object dainf
		= GiftiHeader::giftiFindHdrDA( hdrtexda, da_info, "" );
		if( !dainf.isNone() )
		{
			++hdrtexda;
			GiftiHeader::giftiCopyMetaToGii( dainf,
					gim->darray[gim->numDA-1] );
		}
	}

	// add external textures
	hdr.giftiAddExternalTextures( gim, hdrtexda, da_info );

	// labels table
	hdr.giftiAddLabelTable( gim );

	// global referential
	if( thdr.hasProperty( "transformations" )
			&& thdr.hasProperty( "referentials" ) )
	{
		// set it on the first mesh, and in any other which does not have
		// coordinates systems information
		nda = gim->numDA;
		bool first = true;
		for( t=0; t<nda; ++t )
		{
			giiDataArray *da = gim->darray[t];
			if( da->intent == NIFTI_INTENT_POINTSET
					&& ( first || da->numCS == 0 ) )
			{
				first = false;
				hdr.giftiSetTransformations( carto::Object::reference( thdr ),
						da );
			}
		}
	}

	gifti_write_image( gim, fname.c_str(), 1 );

	gifti_free_image( gim );
	// .minf header
	if( hdr.hasProperty( "GIFTI_metadata") )
	hdr.removeProperty( "GIFTI_metadata" );
	if( hdr.hasProperty( "GIFTI_version" ) )
	hdr.removeProperty( "GIFTI_version" );
	if( hdr.hasProperty( "GIFTI_dataarrays_info" ) )
	hdr.removeProperty( "GIFTI_dataarrays_info" );
	if( hdr.hasProperty( "file_type" ) )
	hdr.removeProperty( "file_type" );
	hdr.writeMinf( fname + ".minf" );
	return true;
}
catch( std::exception & e )
{
	return false;
}

return true;
}

}

#endif
