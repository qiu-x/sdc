#include <stdio.h>
#include "inputhandler.h"


void
handleinput(void)
{
	if (event.type == sfEvtClosed)
	{
		game_close();
	}

	if (event.type == sfEvtKeyPressed)
	{
		if (event.key.code == sfKeyQ)
		{
			game_close();
		}
	}

	switch (gs) 
	{
	case start:
		start_handleinput();
		break;
	case game:
		game_handleinput();
		break;
	case pause:
		pause_handleinput();
	default:
		break;
	}
}

void
start_handleinput(void)
{
	if (event.type == sfEvtKeyPressed)
	{
		if (event.key.code == sfKeySpace)
		{
			gs = game;
			fallspeed = -sfView_getSize(gameview).x * 0.060;
		}
		
	}
}

void
game_handleinput(void)
{
	if (event.type == sfEvtKeyPressed)
	{
		if (event.key.code == sfKeySpace)
		{
			fallspeed = -sfView_getSize(gameview).x * 0.060;
		}
		if (event.key.code == sfKeyP)
				gs = pause;
	}
}

void
pause_handleinput(void)
{
	if (event.type == sfEvtKeyPressed)
	{
		if (event.key.code == sfKeyP)
				gs = game;
	}
	if (event.type == sfEvtMouseMoved)
	{
		//if (event.mouseMove.x < ws.x/2)
		//{
			printf("%f\n", (double)event.mouseMove.x/ws.x);
		//}
	}
}
