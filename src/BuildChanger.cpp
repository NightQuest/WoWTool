////////////////////////////////////////////////////////////////
// BuildChanger
// Copyright © 2010 Recelate Studios
// DATE: March 18th, 2010
////////////////////////////////////////////////////////////////

#include <iostream>

int main(int argc, char **argv)
{
	long fsize = 0L;
	char *buffer, *ptr, *ptr2;
	unsigned int revision = 0;
	FILE *file;
	char *text[] = {
		"MAJOR_VERSION",
		"MINOR_VERSION",
		"BUILD_NO",
		"REVISION_NO",
	};
	int value[4] = { 0 };

	file = fopen("..\\.svn\\entries", "r");
	if( file )
	{
		fseek( file, 0, SEEK_END );
		fsize = ftell( file );
		fseek( file, 0, SEEK_SET );

		buffer = new char[fsize];
		if( buffer != NULL )
		{
			fread( buffer, fsize, 1, file );

			ptr = strstr( buffer, "dir" );
			if( ptr != NULL )
			{
				while( !isdigit( *ptr ) )
					++ptr;

				ptr2 = ptr;
				while( isdigit( *ptr2 ) )
					++ptr2;

				*ptr2 = NULL;

				revision = atoi( ptr );
			}

			delete[] buffer;
		}

		fclose(file);
	}

	file = fopen("..\\src\\VersionNo.h", "r");
	if( file )
	{
		fseek(file, 0, SEEK_END);
		fsize = ftell(file);
		rewind(file);
		buffer = new char[fsize];
		if( buffer != NULL )
		{
			memset(buffer, NULL, fsize);
			fread( buffer, fsize, 1, file );

			ptr = buffer;
			for( int x = 0; x < 4; x++ )
			{
				ptr = strstr( ptr, text[x] );

				if( ptr != NULL )
				{
					while( !isdigit( *ptr ) )
						++ptr;

					ptr2 = ptr;
					while( isdigit( *ptr2 ) )
						++ptr2;

					*ptr2 = NULL;
					value[x] = atoi( ptr );

					ptr++;
					ptr++;
				}
			}

			delete[] buffer;
		}
		fclose(file);
	}

	file = fopen("..\\src\\VersionNo.h", "w");
	if( file )
	{
		value[2] = (value[2] + 1);
		printf("File version: %u.%u.%u.%u\n", value[0], value[1], revision, value[2]);
		fprintf(file,	"#define MAJOR_VERSION 	%u\n"
						"#define MINOR_VERSION 	%u\n"
						"#define BUILD_NO		%u\n"
						"#define REVISION_NO 	%u\n\n"
						"#define VERSION MAJOR_VERSION##,##MINOR_VERSION##,##REVISION_NO##,##BUILD_NO\n"
						"#define STRVERSION \"%u, %u, %u, %u\"\n",
						value[0], value[1], value[2], revision,
						value[0], value[1], revision, value[2]);
		fflush(file);
		fclose(file);
	}

	file = fopen("..\\src\\WoWCTool.ver", "w");
	if( file )
	{
		fprintf(file, "%u", value[2]);
		fflush(file);
		fclose(file);
	}

	return 0;
}