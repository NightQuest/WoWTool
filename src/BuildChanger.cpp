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
	FILE *file;
	char *text[] = {
		"MAJOR_VERSION",
		"MINOR_VERSION",
		"HOTFIX_VERSION",
		"BUILD_NO",
	};
	int value[4] = { 0 };

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
		value[3] = (value[3] + 1);
		printf("File version: %u.%u.%u.%u\n", value[0], value[1], value[2], value[3]);
		fprintf(file,	"#define MAJOR_VERSION 			%u\n"
						"#define MINOR_VERSION 			%u\n"
						"#define HOTFIX_VERSION			%u\n"
						"#define BUILD_NO				%u\n\n"
						"#define VERSION MAJOR_VERSION##,##MINOR_VERSION##,##HOTFIX_VERSION##,##BUILD_NO\n"
						"#define STRVERSION \"%u, %u, %u, %u\"\n",
						value[0], value[1], value[2], value[3],
						value[0], value[1], value[2], value[3]);
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