#define PNG_NO_SETJMP
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <png.h>
#include <exception>


int main( int argc, const char **argv )
{
	if( argc < 3 )
	{
		printf( "%s", "USE: png2bin [in] [out]\n" );
		return 1;
	}
	const char *in_txt = argv[ 1 ];
	const char *out_txt = argv[ 2 ];

	FILE *infile = NULL;
	FILE *outfile = NULL;
	png_structp png_ptr = NULL;
	png_infop png_info = NULL;
	png_bytep rowp = NULL;

	try
	{
		if( ( infile = fopen( argv[ 1 ], "rb" ) ) == NULL )
			throw std::exception( ( std::string() + "problem opening: " + argv[ 1 ] ).c_str() );
		if( ( outfile = fopen( argv[ 2 ], "wb" ) ) == NULL )
			throw std::exception( ( std::string() + "problem opening: " + argv[ 2 ] ).c_str() );
		if( ( png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ) ) == NULL )
			throw std::exception( ( std::string() + "png_create_struct" ).c_str() );
		if( ( png_info = png_create_info_struct( png_ptr ) ) == NULL )
			throw std::exception( ( std::string() + "png_create_info_struct" ).c_str() );

		png_init_io( png_ptr, infile );
		png_read_info( png_ptr, png_info );

		png_textp title_textp= NULL;
		int placeholder = 0;
		png_get_text( png_ptr, png_info, &title_textp, &placeholder );
		png_int_32 data_size = atoi( title_textp->text );

		png_uint_32 width = png_get_image_width( png_ptr, png_info );
		png_uint_32 height = png_get_image_height( png_ptr, png_info );
		png_uint_32 bit_depth = png_get_bit_depth( png_ptr, png_info );
		png_uint_32 size_row = 3 * width;

		rowp = new png_byte[ size_row ];

		for( png_int_32 data_read = 0; data_read < data_size;)
		{
			png_int_32 data_diff = data_size - data_read;
			png_int_32 reading_size = data_diff > size_row ? size_row : data_diff;
			png_read_row( png_ptr, rowp, NULL );
			fwrite( rowp, reading_size, 1, outfile );
			data_read += reading_size;
		}

	}
	catch( std::exception exc )
	{
		printf( "%s", exc.what() );
	}
	catch( ... )
	{
		printf( "%s", "caught unexpected exception\n" );
	}

	if( rowp ) delete[] rowp;
	if( png_info ) png_destroy_info_struct( png_ptr, &png_info );
	if( png_ptr ) png_destroy_read_struct( &png_ptr, &png_info, NULL );
	if( outfile ) fclose( outfile );
	if( infile ) fclose( infile );

	return 0;
}