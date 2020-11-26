daytime: daytime_server.c daytime_client.c
	gcc -o daytime_server daytime_server.c -I.
	gcc -o daytime_client daytime_client.c -I.
