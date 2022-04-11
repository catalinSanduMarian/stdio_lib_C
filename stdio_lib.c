#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "header_f.h"

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
 	int open_file_mode;
 	SO_FILE *stream;
 	stream = alocare_structura();
	open_file_mode = check_mode(mode);

 	switch (open_file_mode)
 	{
 		case 1:
 		 	stream -> descriptor = open(pathname,  O_RDWR);
 		 	get_location(stream, 0);
 			break;
 		case 2:
 			stream-> descriptor = open(pathname, O_CREAT | O_WRONLY | O_TRUNC, 0644);
 			get_location(stream, 0);
 			break;
 		case 3:
 			 stream-> descriptor = open(pathname, O_CREAT | O_WRONLY | O_APPEND , 0644);
 		 	get_location(stream, 1);
 			break;

 		case 11:
 			 stream-> descriptor = open(pathname,  O_RDWR);
 		 	get_location(stream, 0);
 			break;
 		case 12:

 			break;
 		case 13:
 			stream-> descriptor = open(pathname, O_CREAT | O_RDWR | O_APPEND , 0644);
 		 	get_location(stream, 1);
 			break;

 		default:
 			free_structura(stream);
 			return NULL;
 	}

 	if (stream-> descriptor == -1){
 		free_structura (stream);
 		return NULL;
 	}
 	return stream;
}

int so_fileno(SO_FILE *stream)
{
	return stream -> descriptor;
}

int so_fgetc(SO_FILE *stream)
{	
	unsigned char character_read = 0;
	int bytes_read;

	if (stream -> de_citit == 0)
	{
		bytes_read = read( stream -> descriptor, stream -> r_buffer, LENGHT);
		if (bytes_read == -1)
		{
			stream -> ok=0;
			return SO_EOF;
		}
		stream -> de_citit =1;
		stream -> r_offset = stream -> r_offset +1;
		character_read = stream -> r_buffer[0];
	}
	else 
	{
		character_read = stream -> r_buffer[stream -> r_offset];
		if (stream -> r_offset <LENGHT-1)
		{
			stream -> r_offset = stream -> r_offset +1;
		}
		else
		{
			stream -> r_offset =0;
			stream -> de_citit =0;
		}		
	}
	stream -> locatie = stream -> locatie +1;
	if (stream -> file_end < stream -> locatie)
	{
		stream -> ok=0;
		return -1;
	}
	return character_read;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i,character_read,j;
	char *buffer;
	buffer = (char*) ptr;

	for (i = 0; i < nmemb; i++) {
		for(j =0 ;j <size;j++){

			character_read = so_fgetc(stream);
			if (stream -> ok == 0){
				return 0;
			}
			buffer[i*size + j] = character_read;
		}
	}
	return i;
}

int so_fputc(int c, SO_FILE *stream)
{
	int bytes_written;

	if (stream -> de_scris == 0)
	{
		stream -> w_buffer[stream -> w_offset] = c;
		
		stream -> w_offset = stream -> w_offset +1;
		if (stream -> w_offset > LENGHT)
		{
			stream -> de_scris = 1;
		}
	}
	else{
			bytes_written = write_to_file(stream);
			if (bytes_written == SO_EOF)
			{
				stream -> w_ok =0;
				return SO_EOF;
			}
		stream -> w_buffer[stream -> w_offset] = c;
		stream -> w_offset = stream -> w_offset +1;
	}
	stream -> locatie = stream -> locatie +1;
	return c;
}
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i,j;
	char *buffer;
	buffer = (char*) ptr;
	for (i = 0; i < nmemb; i++) {
		for(j =0 ;j <size;j++)
		{
			so_fputc(buffer[i*size + j], stream);
			if (stream -> w_ok == 0)
			{
				return 0;
			}
		}	
	}
	return i;
}

long so_ftell(SO_FILE *stream)
{
	return stream -> locatie;
}

int so_fflush(SO_FILE *stream)
{
	int bytes_written;
	bytes_written = write_to_file (stream);
	return bytes_written;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
	stream -> r_offset =0;
	stream -> de_citit =0;
	int bytes_written;
	bytes_written = write_to_file(stream);
	stream -> locatie =lseek(stream -> descriptor,offset, whence );
	if (stream -> locatie == -1)
		return -1;
	return 0;
}

int so_feof(SO_FILE *stream)
{
	if (stream -> file_end < stream -> locatie)
		return -1;
	return 0;
}

int so_ferror(SO_FILE *stream)
{
	if (stream == NULL)
		return 0;
	if (stream -> ok == 1 && stream -> w_ok == 1)
		return 0;
	return -1;
}

SO_FILE *so_popen(const char *command, const char *type)
{
	int rc;
	int mode;
	int file_pipe[2];
	SO_FILE *stream;
	mode = check_mode(type);
	stream =  alocare_structura();
	if (stream == NULL)
		return NULL;

	rc = pipe(file_pipe);

	if (rc != 0) {
		free(stream);
		return NULL;
	}
	if (mode == 1)
		stream->descriptor = file_pipe[0];
	else
		stream->descriptor = file_pipe[1];

	pid_t pid = fork();
	switch (pid) {
	case 0:
		if (mode == 1) {
			close(file_pipe[0]);
			dup2(file_pipe[1], STDOUT_FILENO);

		} else {
			close(file_pipe[1]);
			dup2(file_pipe	[0], STDIN_FILENO);
		}
		rc = execlp("/bin/sh", "sh", "-c", command,NULL);
		if (rc)
			return NULL;
		break;
	case -1:
		free_structura(stream);
		return NULL;
	default:
		stream->process_id = pid;
		if (mode == 1) {
			close(file_pipe[1]);
		}
		else{
			close(file_pipe[0]);
		}

	}

	stream -> file_end = 16000;
	return stream;
}	

int so_pclose(SO_FILE *stream)
{
	int status,rc,bytes_written;	
	bytes_written = write_to_file(stream);
	close(stream -> descriptor);
	rc = waitpid(stream -> process_id, &status, 0);
	free_structura (stream);
	if (rc < 0)
		return -1;
	return 0;
}

int so_fclose(SO_FILE *stream)
{
	int bytes_written,close_value;
	
	if (stream == NULL)
		return SO_EOF;

	bytes_written = write_to_file(stream);
	close_value = close (stream->descriptor);
	free_structura (stream);

	if (bytes_written == SO_EOF || close_value == -1)
		return SO_EOF;	
	return 0;
}