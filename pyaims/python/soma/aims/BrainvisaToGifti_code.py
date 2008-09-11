# here we first try a conversion from MESH to gii ASCII
# other encodings or texture will come after with a more structured program

import sys
from xml.sax.saxutils import XMLGenerator
from xml.sax.xmlreader import AttributesImpl
#from xmlindent import XMLIndentGenerator
from struct import pack
import operator, zlib
import soma.aims
import time
from soma.uuid import Uuid
import binascii
import base64

typeConversion={'U8': 'NIFTI_TYPE_UINT8', 'U32': 'NIFTI_TYPE_INT32', 'FLOAT': 'NIFTI_TYPE_FLOAT32'}
dataFormat={ 'NIFTI_TYPE_FLOAT32':'f', 'NIFTI_TYPE_INT32':'l', 'NIFTI_TYPE_UINT8':'B' }


def insertCDATA(hd, text):
     hd.fOut.write('<![CDATA[')
     hd.characters(text)
     hd.fOut.write(']]>')


def  insertMD(hd, md):
     hd.startElement('MetaData',{})
     hd.characters('\n')
     for name in md.keys():
          hd.startElement('MD',{})
          hd.characters('\n')
          value=md[name]
          hd.startElement('Name',{})
          insertCDATA(hd, name)
          hd.endElement('Name')
          hd.characters('\n')
          hd.startElement('Value', {})
          insertCDATA(hd, value)
          hd.endElement('Value')
          hd.characters('\n')
          hd.endElement('MD')
          hd.characters('\n')
     hd.endElement('MetaData')
     hd.characters('\n')



def insertCoordinatesASCII(hd, vertices):
     hd.startElement('Data',{})
     for vert in vertices :
          point='%f %f %f\n' %(vert[0], vert[1], vert[2])
          hd.characters(point)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertCoordinatesBase64(hd, vertices, fmt):
     hd.startElement('Data',{})
     point=''
     fmt='3'+fmt
     for vert in vertices :
          point = point + pack(fmt, vert[0], vert[1], vert[2])
	#'%f %f %f ' %(vert[0], vert[1], vert[2])
#    point64=binascii.b2a_base64(point)
     #point64=base64.encodestring(point)
     point64=base64.b64encode(point)
     hd.characters(point64)
     hd.endElement('Data')
     hd.characters('\n')

def insertCoordinatesGzipBase64(hd, vertices, fmt):
     hd.startElement('Data',{})
     point=''
     fmt='3'+fmt
     for vert in vertices :
          point = point + pack(fmt, vert[0], vert[1], vert[2])
     pointzip=zlib.compress(point)
     point64=base64.b64encode(pointzip)
     hd.characters(point64)
     hd.endElement('Data')
     hd.characters('\n')
    
def insertTrianglesASCII(hd, triangles):
     hd.startElement('Data',{})
     for poly in triangles :
          tri='%d %d %d\n' %(poly[0], poly[1], poly[2])
          hd.characters(tri)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertTrianglesBase64(hd, triangles, fmt):
     hd.startElement('Data',{})
     tri=''
     fmt='3'+fmt
     for poly in triangles :
          tri = tri + pack(fmt, poly[0], poly[1], poly[2])
     #tri64=base64.encodestring(tri)
     tri64=base64.b64encode(tri)
     hd.characters(tri64)
     hd.endElement('Data')
     hd.characters('\n')

def insertTrianglesGzipBase64(hd, triangles, fmt):
     hd.startElement('Data',{})
     tri=''
     fmt='3'+fmt
     for poly in triangles :
          tri = tri + pack(fmt, poly[0], poly[1], poly[2])
     trizip=zlib.compress(tri)
     tri64=base64.b64encode(trizip)
     hd.characters(tri64)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertNormalsASCII(hd, normals):
     hd.startElement('Data',{})
     for norm in normals :
          vnorm='%f %f %f\n' %(norm[0], norm[1], norm[2])
          hd.characters(vnorm)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertNormalsBase64(hd, normals, fmt):
     hd.startElement('Data',{})
     vnorm=''
     fmt='3'+fmt
     for norm in normals :
          vnorm = vnorm + pack(fmt, norm[0], norm[1], norm[2])
     #vnorm64=base64.encodestring(vnorm)
     vnorm64=base64.b64encode(vnorm)
     hd.characters(vnorm64)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertNormalsGzipBase64(hd, normals, fmt):
     hd.startElement('Data',{})
     vnorm=''
     fmt='3'+fmt
     for norm in normals :
          vnorm = vnorm + pack(fmt, norm[0], norm[1], norm[2])
     vnormzip=zlib.compress(vnorm)
     vnorm64=base64.b64encode(vnormzip)
     hd.characters(vnorm64)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertTextureASCII(hd, texture):
     hd.startElement('Data',{})
     for i in xrange(texture.size()):
          val=texture.item(i)
          valeur='%f\n' %(val)
          hd.characters(valeur)
     hd.endElement('Data')
     hd.characters('\n')
     
def insertTextureBase64(hd, texture, fmt):
     hd.startElement('Data',{})
     valeur=''
     for i in xrange(texture.size()):
          val=texture.item(i)
          valeur = valeur + pack(fmt, val)
     #valeur64=base64.encodestring(valeur)
     valeur64=base64.b64encode(valeur)
     hd.characters(valeur64)
     hd.endElement('Data')
     hd.characters('\n')

def insertTextureGzipBase64(hd, texture, fmt):
     hd.startElement('Data',{})
     valeur=''
     for i in xrange(texture.size()):
          val=texture.item(i)
          valeur = valeur + pack(fmt, val)
     valeurzip=zlib.compress(valeur)
     valeur64=base64.b64encode(valeurzip)
     hd.characters(valeur64)
     hd.endElement('Data')
     hd.characters('\n')

def insertCoordinateArray(hd, attributs, v):
     attrArray=AttributesImpl(attributs)
     hd.startElement('DataArray',attrArray)
     hd.characters('\n')
     
     unique=Uuid()
     fmt=dataFormat[attributs['DataType']]
     meta={'AnatomicalStructurePrimary':structurePrimary, 'AnatomicalStructureSecondary':structureSecondary, 'GeometricType':geometricType, 'UniqueID':str(unique)}
     insertMD(hd, meta)
     
     hd.startElement('CoordinateSystemTransformMatrix',{})
     hd.characters('\n')
     hd.startElement('DataSpace',{})
     insertCDATA(hd, 'NIFTI_XFORM_UNKNOWN')
     hd.endElement('DataSpace')
     hd.characters('\n')
     hd.startElement('TransformedSpace',{})
     insertCDATA(hd, 'NIFTI_XFORM_UNKNOWN')
     hd.endElement('TransformedSpace')
     hd.characters('\n')
     hd.startElement('MatrixData',{})
     hd.characters('\n')
     un=1.00000
     zero=0.00000
     matrix='%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n' %(un, zero, zero, zero, zero, un, zero, zero, zero, zero, un, zero, zero, zero, zero, un)
     hd.characters(matrix)
     hd.endElement('MatrixData')
     hd.characters('\n')
     hd.endElement('CoordinateSystemTransformMatrix')
     hd.characters('\n')
     if attributs['Encoding']=='ASCII':
          insertCoordinatesASCII(hd, v)
     elif attributs['Encoding']=='Base64Binary':
          insertCoordinatesBase64(hd,v,fmt)
     elif attributs['Encoding']=='GZipBase64Binary':
          insertCoordinatesGzipBase64(hd,v,fmt)
     else :
          raise RuntimeError( 'Encoding %s not supported (yet)' % attributs['Encoding'])
     hd.endElement('DataArray')
     hd.characters('\n')

def insertTriangleArray(hd, attributs, p):
     attrArray=AttributesImpl(attributs)
     hd.startElement('DataArray',attrArray)
     hd.characters('\n')
     unique=Uuid()
     meta={'TopologicalType':topologicalType, 'UniqueID':str(unique)}
     insertMD(hd, meta)
     fmt=dataFormat[attributs['DataType']]

     
     if attributs['Encoding']=='ASCII':
          insertTrianglesASCII(hd, p)
     elif attributs['Encoding']=='Base64Binary':
	     insertTrianglesBase64(hd, p, fmt)
     elif attributs['Encoding']=='GZipBase64Binary':
	     insertTrianglesGzipBase64(hd, p, fmt)
     else :
          raise RuntimeError( 'Encoding %s not supported (yet)' % attributs['Encoding'])
     hd.endElement('DataArray')
     hd.characters('\n')

def insertNormalArray(hd, attributs, n):
     attrArray=AttributesImpl(attributs)
     fmt=dataFormat[attributs['DataType']]
     hd.startElement('DataArray',attrArray)
     hd.characters('\n')
     unique=Uuid()
     meta={'UniqueID':str(unique)}
     insertMD(hd, meta)
     if attributs['Encoding']=='ASCII':
          insertNormalsASCII(hd, n)
     elif attributs['Encoding']=='Base64Binary':
          insertNormalsBase64(hd, n, fmt)
     elif attributs['Encoding']=='GZipBase64Binary':
          insertNormalsGzipBase64(hd, n, fmt)
     else :
          raise RuntimeError( 'Encoding %s not supported (yet)' % attributs['Encoding'])
     hd.endElement('DataArray')
     hd.characters('\n')
     
def insertTextureArray(hd, attributs, texture):
     attrArray=AttributesImpl(attributs)
     fmt=dataFormat[attributs['DataType']]
     hd.startElement('DataArray',attrArray)
     hd.characters('\n')
     unique=Uuid()
     meta={'UniqueID':str(unique)}
     insertMD(hd, meta)
     if attributs['Encoding']=='ASCII':
          insertTextureASCII(hd,texture)
     elif attributs['Encoding']=='Base64Binary':
          insertTextureBase64(hd,texture, fmt)
     elif attributs['Encoding']=='GZipBase64Binary':
          insertTextureGzipBase64(hd,texture, fmt)
     else:
          raise RuntimeError( 'Encoding %s not supported (yet)' % attributs['Encoding'])
     hd.endElement('DataArray')
     hd.characters('\n')


def writeMesh(hd, mesh):
     print 'timesteps:', mesh.size()
     for t in xrange( mesh.size() ):
          print 'time:', t
          v = mesh.vertex(t)
          n = mesh.normal(t)
          p = mesh.polygon(t)

     print 'The mesh contains ', v.size(), ' vertices, ' , p.size(), ' polygons and ', n.size(), ' normals'
     nbVertices='%d'%v.size()
     nbTriangles='%d'%p.size()
     nbNormals='%d'%n.size()
    
     if (nbVertices != nbNormals) :
          raise RuntimeError( 'pb : nb of Vertices is %(nbv)s and number of normals is %(nbn)s' % {
                                  'nbv': str( nbVertices ), 'nbn': str( nbNormals ) } )

     hd.startDocument()
     attr = AttributesImpl({ 'Version': giftiVersion, 'NumberOfDataArrays':'3' })
     
    #head element
     hd.startElement('GIFTI', attr)
     hd.characters('\n')
     # some information via MetaData
     date=time.strftime('%d/%m/%y %H:%M',time.localtime())
     meta={'brainvisa version':'3.1', 'Date':date}
     insertMD(hd, meta)

     #this one is optional
     hd.startElement('LabelTable',{})
     hd.characters('\n')
     hd.endElement('LabelTable')
     hd.characters('\n')

     # This one contains the actual data
    
     attributs={ "Intent":"NIFTI_INTENT_POINTSET", "DataType":"NIFTI_TYPE_FLOAT32", "ArrayIndexingOrder":"RowMajorOrder", "Dimensionality":"2", "Dim0":nbVertices, "Dim1":"3", "Encoding":encode, "Endian":endian, "ExternalFileName":"", "ExternalFileOffset":""}
     insertCoordinateArray(hd, attributs, v)

     attributs={ "Intent":"NIFTI_INTENT_TRIANGLE", "DataType":"NIFTI_TYPE_INT32", "ArrayIndexingOrder":"RowMajorOrder", "Dimensionality":"2",  "Dim0":nbTriangles, "Dim1":"3", "Encoding":encode, "Endian":endian, "ExternalFileName":"", "ExternalFileOffset":""}
     insertTriangleArray(hd, attributs, p)

     attributs={ "Intent":"NIFTI_INTENT_VECTOR", "DataType":"NIFTI_TYPE_FLOAT32", "ArrayIndexingOrder":"RowMajorOrder", "Dimensionality":"2", "Dim0":nbNormals, "Dim1":"3", "Encoding":encode, "Endian":endian, "ExternalFileName":"", "ExternalFileOffset":""}
     insertNormalArray(hd, attributs, n)

    # closing everything
     hd.endElement("GIFTI")
     hd.characters('\n')
     hd.endDocument()

def writeTex(hd, texture, typeTexture):
     print 'Texture : timesteps =', texture.size()
     head=texture.header()
     format=typeConversion[head['data_type']]
     print 'Format :',format
     nbVertices=str(texture[0].size())
     print 'The texture contains ', nbVertices, ' points.'
     hd.startDocument()
     attr = AttributesImpl({ "Version": giftiVersion, 'NumberOfDataArrays':str(texture.size()) })
     
     #head element
     hd.startElement('GIFTI', attr)
     hd.characters('\n')
     date=time.strftime('%d/%m/%y %H:%M',time.localtime())
     meta={'brainvisa version':'3.1', 'Date':date}
     insertMD(hd, meta)

     #this one is optional
     hd.startElement('LabelTable',{})
     hd.characters('\n')
     hd.endElement('LabelTable')
     hd.characters('\n')

     # This one contains the actual data
     for t in xrange( texture.size() ):
     #new Gifti convention 
          dimension='1'
          dim0=nbVertices
          attributs={ "Intent":textureType, "DataType":format, "ArrayIndexingOrder":"RowMajorOrder", "Dimensionality":dimension, "Dim0":dim0, "Encoding":encode, "Endian":endian, "ExternalFileName":"", "ExternalFileOffset":""}
          insertTextureArray(hd, attributs, texture[t])

    # closing everything
     hd.endElement("GIFTI")
     hd.characters('\n')
     hd.endDocument()
    



def startConvert(inFile, outFile, structPrimary, structSecondary, geomType, topoType, enc, texType):
  
     global structurePrimary, structureSecondary, geometricType, topologicalType, encode, textureType, giftiVersion, endian
     structurePrimary=structPrimary
     structureSecondary=structSecondary
     geometricType=geomType
     topologicalType=topoType
     encode=enc
     textureType=texType

     if hex(43981)=='0xabcd':
          endian='LittleEndian'
     else:
          endian='BigEndian'
     giftiVersion='1.0'

     print inFile + '->' + outFile
     global fOut
     fOut=file(outFile, 'w')
     hd=XMLGenerator(fOut,'UTF-8')
     
     hd.fOut = fOut
     
#hd=XMLIndentGenerator(fOut,'UTF-8')
#hd=XMLGenerator(fOut)

     reader = soma.aims.Reader()
     object = reader.read( inFile )
#print 'file:', object
     h = object.header()

     print 'header:', h
     what=h['file_type']

     if what == 'MESH':
          print 'Converting mesh...'
          writeMesh(hd, object)
     elif what == 'TEX':
          print 'Converting texture...'
          writeTex(hd, object, 'Functional')
     else :
          print 'Unsupported type of object'

     hd.fOut.close()