#include <stdio.h>
#include "libgdbc.h"

int main()
{
	libgdbc_t con;
	libgdbc_create_instance(&con);
	if(libgdbc_connect(&con, "localhost", 1234) == -1)
	{
		printf("Error connecting to target\n");
		return 0;
	}
	libgdbc_regread(&con);
	libgdbc_continue(&con);
	libgdbc_delete_instance(&con);	
	return 0;
}
