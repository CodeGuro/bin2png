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
		printf( "%s", "USE: bin2png [in] [out]\n" );
		return 1;
	}

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
		if( ( png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ) ) == NULL )
			throw std::exception( ( std::string() + "png_create_struct" ).c_str() );
		if( ( png_info = png_create_info_struct( png_ptr ) ) == NULL )
			throw std::exception( ( std::string() + "png_create_info_struct" ).c_str() );

		fseek( infile, 0, SEEK_END );
		png_int_32 data_size = ftell( infile );
		fseek( infile, 0, SEEK_SET );

		png_init_io( png_ptr, outfile );

		std::string str_data_size = std::to_string(data_size);

		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "size";
		title_text.text_length = str_data_size.length();
		title_text.text = (png_charp)str_data_size.c_str();
		png_set_text( png_ptr, png_info, &title_text, 1 );


		png_uint_32 val = (png_uint_32)ceil( sqrt( (float)data_size / 3.f ) );
		png_uint_32 width = val;
		png_uint_32 height = val;
		png_set_IHDR( png_ptr, png_info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
		png_write_info( png_ptr, png_info );
		png_int_32 row_size = 3 * width;

		rowp = new png_byte[ row_size ];

		for( png_uint_32 i = 0; i < height; ++i )
		{
			fread( rowp, row_size, 1, infile );
			png_write_row( png_ptr, rowp );
		}

		png_write_flush( png_ptr );
		png_write_end( png_ptr, png_info );

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
	if( png_ptr ) png_destroy_write_struct( &png_ptr, &png_info );
	if( outfile ) fclose( outfile );
	if( infile ) fclose( infile );

	return 0;
}