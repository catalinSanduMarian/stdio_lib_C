#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "so_stdio.h"

#define LENGHT 4095
#define read_pipe 0
#define PIPE_WRITE 1

typedef struct _so_file{
	int descriptor;
	int de_citit;
	int de_scris;
	unsigned char *r_buffer;
	char *w_buffer;
	int r_offset;
	int w_offset;
	int ok;
	int w_ok;
	long locatie;
	long file_end;
	int process_id;
}SO_FILE;


int check_mode (const char *mode)
{
	int type = strlen(mode);
	if (type == 1)
	{
		if (mode[0] == 'r')
			return 1;

		else if (mode[0] == 'w')
				return 2;

		else if (mode[0] == 'a')
				return 3;	
	}
	else if (type == 2)
	{
		if (mode[0] == 'r')
		{
			if (mode[1] == '+')
			return 11;
		}
		else if (mode[0] == 'w')
		{
			
				if (mode[1] == '+')
				return 12;
		}
		else if (mode[0] == 'a')
		{
			
				if (mode[1] == '+')
				return 13;
		}
	}	
	return 0;
}

SO_FILE *alocare_structura()
{
	SO_FILE* ee;
	ee = (SO_FILE*) malloc (sizeof (SO_FILE));
	ee -> r_buffer = (unsigned char *) calloc (sizeof (char), LENGHT+1); 
	ee -> w_buffer = (char *) calloc (sizeof (char), LENGHT+1); 
	ee -> de_citit = 0;
	ee -> r_offset = 0;
	ee -> ok=1;
	ee -> de_scris = 0;
	ee -> w_offset =0;
	ee -> w_ok =1;
	ee -> locatie =0;
	ee -> file_end =0;
}

void free_structura (SO_FILE* stream)
{
	free(stream -> r_buffer);
	free(stream -> w_buffer);
	free(stream);
}

void get_location (SO_FILE* stream, int to_append)
{
	stream -> file_end = lseek(stream -> descriptor,0, SEEK_END);
	if (to_append == 0)
		{
			lseek(stream -> descriptor, 0, SEEK_SET);
		}
}

int write_to_file (SO_FILE* stream)
{
	int bytes_written;
	stream -> de_scris =0;
	if (stream -> w_offset ==0 )
	{
		return 0;
	}
	bytes_written = write (stream -> descriptor,stream -> w_buffer,stream -> w_offset);

	if (bytes_written == -1)
	{
		return SO_EOF;	
	
	}
	stream -> w_offset = 0;
	return 0;
}