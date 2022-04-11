Sandu Marian-Catalin 334CB

Tema 2 - Biblioteca stdio

Organizare

	int descriptor - File descriptorul fisierului

	int de_citit 			Variabila ce tine cont daca trebuie facut read
	int de_scris 			Variabila ce tine cont daca trebuie facut write
	unsigned char *buffer 	Bufferul pentru citire
	char *w_buffer 			Bufferul pentru scriere
	int r_offset;			Pozitia de la care se face urmatoare citire in buffer
	int w_offset;			Pozitia de la care se face urmatoare scriere in buffer
	int ok;					Flag pentru verifciare operatiei de read
	int w_ok;				Flag pentru verifciare operatiei de write
	long locatie;			Variabila ce tine minte pozitia propiu-zisa in fisier
	long file_end;			Tine minte unde se termina fisierul
	int process_id;			pid unui proces

Implementare

Intreg enuntul este implementat

Fget si Fwrite sunt implementate prin intermediul bufferelor, astfel, la primul apel fgetc, se face operatia de read si se umple un buffer, urmand ca la urmatoarele apeluri, sa fie returnat
cate un caracter din bufer. Procesul este asemanator si pentru fputc


In cazul operatiei fseek, sunt golite bufferele. 

Pentru implementarea ftell, a fost folosita o variabila ce tine minte pozitia curenta din fisier, la apelarea functiei, variabila este returnata.

Pentru popen, am deschis un pipe anonim, dupa care am creeat un nou proccess. 
Functia pclose, goleste bufferul de scrie si inchide pipeul + dezalocare memorie

Creare biblioteca dinamica:
    Linux - make / make build;

