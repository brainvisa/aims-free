
numtypes = [ 'unsigned char', 'short', 'unsigned short', 'int',
             'unsigned', 'float', 'double' ]
basetypes = numtypes + [ 'AimsRGB', 'AimsRGBA' ]
matrix = []
for z in [ map( lambda y: (x,y), basetypes ) for x in basetypes ]:
  matrix += z

todo = {
			'moment' : numtypes,
			'resampler' : [ ( 'short' ), ( 'float') ],
			'meshresampler' : [ 'AimsData<uint32_t>',
				'aims::BucketMap<Void>'],
			'sampler' : [ ( 'float', '3' ) ],
			'polynomial' : [ ( 'float', '3' ) ],
			'distancemap' : [ 'short', 'float' ],
                        'diffusionsmoother' : [ 'short', 'float' ],
			'smoothing' : [ 'float', 'double' ],
			'gradient' : [ 'float'],
       }

