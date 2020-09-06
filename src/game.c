#include <stdio.h>
#include <stdlib.h>
/* #include <math.h> */

#include "game.h"

sfEvent           event;

enum gamestate gs;
sfRenderWindow   *window;
sfVector2u        ws;
sfView           *gameview;
double            fallspeed;
double            flyspeed;



// Game elements
struct Game_t {
	sfTexture        *birdTexture1;
	sfTexture        *birdTexture2;
	sfSprite         *birdSprite;
	sfClock          *timer;
	sfSprite         *stris[10];
	float             bird_x;
	float             bird_y;
	int               score;
	sfFloatRect       visibleArea;
	float             ratio;
} Game;

sfVector2f
sfVector2f_fromFloat(float x, float y)
{
	sfVector2f vec = {x, y};
	return vec;
}

sfFloatRect
sfFloatRect_fromFloat(float left, float top, float width, float height)
{
	sfFloatRect rect = {left, top, width, height};
	return rect;
}

sfIntRect
sfIntRect_fromInt(int left, int top, int width, int height)
{
	sfIntRect rect = {left, top, width, height};
	return rect;
}

int
run(void)
{
	init();
	loop();
	return 0;
}

int
init(void)
{
	gs = GAMESTATE;
	//  Create the main window
	sfVideoMode mode = {WINDOW_WIDTH, WINDOW_HEIGHT, 32};
	window = sfRenderWindow_create(mode, "Simple Don't Touch The Spikes Clone", sfClose | sfResize, NULL);
	ws = sfRenderWindow_getSize(window);

	// Initialize the timer
	timer = sfClock_create();

	// Create the view
	gameview = sfView_create();
	// update the view to the new size of the window
	ratio = (float)ws.y / (float)ws.x * 0.5f;
	// update the view to the new size of the window
	visibleArea = sfFloatRect_fromFloat(0.f, 0.f, ws.y * 0.5, ws.y);
	sfView_reset(gameview, visibleArea);
	sfView_setViewport(gameview, sfFloatRect_fromFloat(0.5f - (ratio / 2.f), 0, ratio, 1.0f));
	sfRenderWindow_setView(window, gameview);

	// Create the circle
	circle = sfCircleShape_create();
	sfCircleShape_setFillColor(circle, sfColor_fromRGB(250,250,250));
	
	// Initialize the circle text and font
	circle_text = sfText_create();
	circle_textfont = sfFont_createFromFile("res/Terminus.ttf");
	sfText_setFont(circle_text, circle_textfont);
	sfText_setColor(circle_text, sfColor_fromRGB(50, 50, 50));

	// Create the background
	background = sfRectangleShape_create();
	sfRectangleShape_setFillColor(background, sfColor_fromRGB(229,229,229));

	// Create the bottom and top rectangle
	bottomrect = sfRectangleShape_create();
	toprect = sfRectangleShape_create();
	sfRectangleShape_setFillColor(bottomrect, sfColor_fromRGB(50,50,50));
	sfRectangleShape_setFillColor(toprect, sfColor_fromRGB(50,50,50));

	// Initialize the triangle texture 
	const sfIntRect textureArea = {0, 0, 11, 11};
	triangleTexture = sfTexture_createFromFile("res/gem/spikes/spike.1/spike.png", &textureArea);
	sfTexture_setSmooth(triangleTexture, 0);

	// Create the top and bottom triangles
	for (int i = 0; i < 7; i++)
	{
		ttris[i] = sfSprite_create();
		btris[i] = sfSprite_create();
	}

	// Create the side triangles
	for (int i = 0; i < 10; i++)
	{
		stris[i] = sfSprite_create();
	}

	//Initialize the start elements
	menubg = sfRectangleShape_create();
	sfRectangleShape_setFillColor(menubg, sfColor_fromRGB(255,50,50));
	menubgborder = sfRectangleShape_create();
	sfRectangleShape_setFillColor(menubgborder, sfColor_fromRGB(0,0,0));
	resumebutton = sfRectangleShape_create();
	sfRectangleShape_setFillColor(resumebutton, sfColor_fromRGB(255,0,0));
	resumebuttonborder = sfRectangleShape_create();
	sfRectangleShape_setFillColor(resumebuttonborder, sfColor_fromRGB(0,0,0));
	resumebutton_text = sfText_create();
	sfText_setFont(resumebutton_text, circle_textfont);
	sfText_setColor(resumebutton_text, sfColor_fromRGB(255, 255, 255));
	helpbutton = sfRectangleShape_create();
	sfRectangleShape_setFillColor(helpbutton, sfColor_fromRGB(255,0,0));
	helpbuttonborder = sfRectangleShape_create();
	sfRectangleShape_setFillColor(helpbuttonborder, sfColor_fromRGB(0,0,0));
	helpbutton_text = sfText_create();
	sfText_setFont(helpbutton_text, circle_textfont);
	sfText_setColor(helpbutton_text, sfColor_fromRGB(255, 255, 255));

	// Initialize the bird textures
	const sfIntRect textureArea2 = {0, 0, 13, 10};
	birdTexture1 = sfTexture_createFromFile("res/brd0.png", &textureArea2);
	birdTexture2 = sfTexture_createFromFile("res/brd1.png", &textureArea2);
	sfTexture_setSmooth(birdTexture1, 0);
	sfTexture_setSmooth(birdTexture2, 0);

	// Initialize the bird sprite
	birdSprite = sfSprite_create();
	sfSprite_setTexture(birdSprite, birdTexture1, 0);
	sfSprite_setOrigin(birdSprite,                                      \
	sfVector2f_fromFloat(sfSprite_getLocalBounds(birdSprite).width / 2, \
	                     sfSprite_getLocalBounds(birdSprite).height / 2));
	flyspeed = -sfView_getSize(gameview).x * 0.3;
	
	if (!window)
		game_close();

	return 0;
}


void
loop(void)
{
	// Start the game loop 
	while (sfRenderWindow_isOpen(window))
	{
		ratio = (float)ws.y / (float)ws.x * 0.5f;
		// Update the view to the new size of the window
		visibleArea = sfFloatRect_fromFloat(0.f, 0.f, ws.y * 0.5, ws.y);
		sfView_reset(gameview, visibleArea);
		sfView_setViewport(gameview, sfFloatRect_fromFloat(0.5f - (ratio / 2.f), 0, ratio, 1.0f));
		sfRenderWindow_setView(window, gameview);

		// Update whe window size varible
		ws = sfRenderWindow_getSize(window);

		// Process events 
		while (sfRenderWindow_pollEvent(window, &event))
		{
			handleinput();
		}
		// Update
		update(17);
		// Render the objects
		render();
	}
}

void
update(int ms) // ms - tick duration
{
	sfTime t = sfClock_getElapsedTime(timer);
	if (ms<=sfTime_asMilliseconds(t))
	{
		switch (gs) 
		{
		case start:
			start_update();
			break;
		case game:
			game_update();
			break;
		case pause:
			pause_update();
		default:
			break;
		}
		background_update();
		sfClock_restart(timer);
	} 
}

void
game_update(void)
{
	menuxpos = -sfView_getSize(gameview).x;
	bird_update();
	stris_animation();
	colcheck_update();

	// Position the circle text
	char scorestr[12];
	sprintf(scorestr, "%d", score); 
	sfText_setString(circle_text, scorestr);
	sfText_setCharacterSize(circle_text, sfView_getSize(gameview).x * 0.3);
	sfText_setPosition(circle_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 - sfText_getLocalBounds(circle_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(circle_text).height));
}

void
bird_update(void)
{
	if (flyspeed < 0)
	{
		sfSprite_setScale(birdSprite, \
		sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		flyspeed = -(double)sfView_getSize(gameview).x * 0.01;
	}
	if (flyspeed > 0)
	{
		sfSprite_setScale(birdSprite, \
		sfVector2f_fromFloat((float)-sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		flyspeed = (double)sfView_getSize(gameview).x * 0.01;
	}
	sfSprite_setPosition(birdSprite, \
	                     sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + bird_x, \
	                                          sfView_getSize(gameview).y / 2 + bird_y));

	// Change bird sprite
	if (fallspeed > 0)
	{
		sfSprite_setTexture(birdSprite, birdTexture2, 0);
	}
	else
	{
		sfSprite_setTexture(birdSprite, birdTexture1, 0);
	}
	
	// Change fly direction
	if (sfSprite_getPosition(birdSprite).x - sfSprite_getGlobalBounds(birdSprite).width / 2 < 0)
	{
		flyspeed = sfView_getSize(gameview).x * 0.01;
		score++;
		stris_update();
	}
	else if (sfSprite_getPosition(birdSprite).x + sfSprite_getGlobalBounds(birdSprite).width / 2 > sfView_getSize(gameview).x)
	{
		flyspeed = -sfView_getSize(gameview).x * 0.01;
		score++;
		stris_update();
	}
	fallspeed += (double)sfView_getSize(gameview).x * 0.004;
	bird_y += fallspeed;
	bird_x += flyspeed;
}

void
stris_update(void)
{
// <<->>
	if(flyspeed > 0.f)
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_setTexture(stris[i], triangleTexture, 0);
			sfSprite_setScale(stris[i],                                     \
			sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
			(float)sfView_getSize(gameview).x * 0.01));
			sfSprite_setRotation(stris[i], -90.f);
	
			sfSprite_setPosition(stris[i],                      \
			sfVector2f_fromFloat(sfView_getSize(gameview).x  \
			- sfSprite_getGlobalBounds(stris[i]).width * 0.0, \
			0 + sfRectangleShape_getSize(toprect).y * 3 * (rand() % 10)));
		
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_setTexture(stris[i], triangleTexture, 0);
			sfSprite_setScale(stris[i],                                     \
			sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
			(float)sfView_getSize(gameview).x * 0.01));
			sfSprite_setRotation(stris[i], 90.f);
	
			sfSprite_setPosition(stris[i],                      \
			sfVector2f_fromFloat(sfSprite_getGlobalBounds(stris[i]).width * 0.0, \
			0 + sfRectangleShape_getSize(toprect).y + sfRectangleShape_getSize(toprect).y * 4 * (rand() % 10)));
		
		}
	}
	
}

void
stris_animation(void)
{
	if(sfSprite_getPosition(stris[0]).x < sfView_getCenter(gameview).x)
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_move(stris[i], sfVector2f_fromFloat((( sfSprite_getGlobalBounds(stris[i]).width * 0.8) - sfSprite_getPosition(stris[i]).x) * 0.2, 0));
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_move(stris[i], sfVector2f_fromFloat(((sfView_getSize(gameview).x - sfSprite_getGlobalBounds(stris[i]).width * 0.8) - sfSprite_getPosition(stris[i]).x) * 0.2, 0));
		}
	}
}

void
colcheck_update(void)
{
	for (int i = 0; i < 10; i++)
	{
		if(sfSprite_getGlobalBounds(stris[i]).width * 0.4 >\
		sfSprite_getGlobalBounds(birdSprite).left)
		{
			if((sfSprite_getGlobalBounds(birdSprite).top > sfSprite_getGlobalBounds(stris[i]).top\
			-  sfSprite_getGlobalBounds(stris[i]).height) &&
			(sfSprite_getGlobalBounds(birdSprite).top - sfSprite_getGlobalBounds(birdSprite).height\
			< sfSprite_getGlobalBounds(stris[i]).top) &&\
			sfSprite_getGlobalBounds(birdSprite).left<\
			sfSprite_getGlobalBounds(stris[i]).left)
			printf("l\n");
		}
		else if(sfView_getSize(gameview).x - sfSprite_getGlobalBounds(stris[i]).width * 0.4<\
		sfSprite_getGlobalBounds(birdSprite).left + sfSprite_getGlobalBounds(birdSprite).width)
		{
			if((sfSprite_getGlobalBounds(birdSprite).top > sfSprite_getGlobalBounds(stris[i]).top\
			-  sfSprite_getGlobalBounds(stris[i]).height) &&
			(sfSprite_getGlobalBounds(birdSprite).top - sfSprite_getGlobalBounds(birdSprite).height\
			< sfSprite_getGlobalBounds(stris[i]).top) &&\
			sfSprite_getGlobalBounds(birdSprite).left + sfSprite_getGlobalBounds(birdSprite).width >\
			sfSprite_getGlobalBounds(stris[i]).left)
			printf("r\n");
		}
	}
}

void
pause_update(void)
{
	// Position the pause background border
	sfRectangleShape_setSize(menubgborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/7.5, \
	                     sfView_getSize(gameview).y / 3.7));
	sfRectangleShape_setOrigin(menubgborder,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(menubgborder).x / 2, \
	                     sfRectangleShape_getSize(menubgborder).y / 2));
	sfRectangleShape_setPosition(menubgborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + menuxpos,\
	                     sfView_getSize(gameview).y / 2 ));

	// Position the pause background
	sfRectangleShape_setSize(menubg,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/6, \
	                     sfView_getSize(gameview).y / 4));
	sfRectangleShape_setOrigin(menubg,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(menubg).x / 2, \
	                     sfRectangleShape_getSize(menubg).y / 2));
	sfRectangleShape_setPosition(menubg,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + menuxpos,\
	                     sfView_getSize(gameview).y / 2 ));

	// Position the resumebutton
	sfRectangleShape_setSize(resumebutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/4.8, \
	                     sfView_getSize(gameview).y / 18));
	sfRectangleShape_setOrigin(resumebutton,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(resumebutton).x / 2, \
	                     sfRectangleShape_getSize(resumebutton).y / 2));
	sfRectangleShape_setPosition(resumebutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     - sfRectangleShape_getSize(resumebutton).y ));

	// Position the resumebutton border
	sfRectangleShape_setSize(resumebuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/5.15, \
	                     sfView_getSize(gameview).y / 15));
	sfRectangleShape_setOrigin(resumebuttonborder,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(resumebuttonborder).x / 2, \
	                     sfRectangleShape_getSize(resumebuttonborder).y / 2));
	sfRectangleShape_setPosition(resumebuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     - sfRectangleShape_getSize(resumebutton).y ));

	// Position the resumebutton text
	sfText_setCharacterSize(resumebutton_text, sfView_getSize(gameview).x * 0.1);
	sfText_setPosition(resumebutton_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 + menuxpos - sfText_getLocalBounds(resumebutton_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(resumebutton_text).height - sfRectangleShape_getSize(resumebutton).y));
	sfText_setString(resumebutton_text, "RESUME");

	// Position the helpbutton
	sfRectangleShape_setSize(helpbutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/4.8, \
	                     sfView_getSize(gameview).y / 18));
	sfRectangleShape_setOrigin(helpbutton,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(helpbutton).x / 2, \
	                     sfRectangleShape_getSize(helpbutton).y / 2));
	sfRectangleShape_setPosition(helpbutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     + sfRectangleShape_getSize(helpbutton).y ));

	// Position the helpbutton border
	sfRectangleShape_setSize(helpbuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/5.15, \
	                     sfView_getSize(gameview).y / 15));
	sfRectangleShape_setOrigin(helpbuttonborder,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(helpbuttonborder).x / 2, \
	                     sfRectangleShape_getSize(helpbuttonborder).y / 2));
	sfRectangleShape_setPosition(helpbuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     + sfRectangleShape_getSize(helpbutton).y ));

	// Position the helpbutton text
	sfText_setCharacterSize(helpbutton_text, sfView_getSize(gameview).x * 0.1);
	sfText_setPosition(helpbutton_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 + menuxpos- sfText_getLocalBounds(helpbutton_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(helpbutton_text).height + sfRectangleShape_getSize(helpbutton).y));
	sfText_setString(helpbutton_text, "HELP");
	pause_animation();
}

void
pause_animation(void)
{
	menuxpos+=(double)(sfView_getSize(gameview).x / 2 - sfRectangleShape_getPosition(menubgborder).x) * 0.07;
}

void
start_update(void)
{
	// Position the circle text
	sfText_setCharacterSize(circle_text, sfView_getSize(gameview).x * 0.1);
	sfText_setPosition(circle_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 - sfText_getLocalBounds(circle_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(circle_text).height));
	sfText_setString(circle_text, "Press\nSpace\n");

	sfSprite_setPosition(birdSprite, \
	                     sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + bird_x, \
	                                          sfView_getSize(gameview).y / 2 + bird_y));
	sfSprite_setScale(birdSprite, \
	sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
	(float)sfView_getSize(gameview).x * 0.01));
}

void
background_update(void)
{
	// Position the bottom rectangle
	sfRectangleShape_setSize(bottomrect,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x, \
	                     sfView_getSize(gameview).y / 20));
	sfRectangleShape_setOrigin(bottomrect,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(bottomrect).x / 2, \
	                     sfRectangleShape_getSize(bottomrect).y / 2));
	sfRectangleShape_setPosition(bottomrect,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2, \
	                     sfView_getSize(gameview).y      \
	                     - sfRectangleShape_getSize(bottomrect).y / 2));
	
	// Position the top rectangle
	sfRectangleShape_setSize(toprect,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x, \
	                     sfView_getSize(gameview).y/20));
	sfRectangleShape_setOrigin(toprect,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(toprect).x / 2, \
	                     sfRectangleShape_getSize(toprect).y / 2));
	sfRectangleShape_setPosition(toprect,                 \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2,  \
	                     sfView_getSize(gameview).y /2    \
	                     - sfView_getSize(gameview).y / 2 \
	                     + sfRectangleShape_getSize(toprect).y / 2));

	// Initialize the background
	sfRectangleShape_setSize(background, \
	sfVector2f_fromFloat(ws.x * 100, ws.y * 100));
	sfRectangleShape_setOrigin(background,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(background).x / 2, \
	sfRectangleShape_getSize(background).y / 2));
	sfRectangleShape_setPosition(background, \
	sfVector2f_fromFloat(ws.x/2, ws.y/2));

	// Position the circle
	sfCircleShape_setRadius(circle, sfView_getSize(gameview).x / 3);
	sfCircleShape_setPosition(circle, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2-sfCircleShape_getRadius(circle), \
	sfView_getSize(gameview).y/2-sfCircleShape_getRadius(circle)));
	

	// Position the bottom triangles
	for (int i = 0; i < 7; i++)
	{
		sfSprite_setTexture(btris[i], triangleTexture, 0);
		sfSprite_setScale(btris[i], \
		sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		sfSprite_setRotation(btris[i], 0.f);
		if( i % 2 == 0)
		{
			sfSprite_setPosition(btris[i],                      \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			- sfSprite_getGlobalBounds(btris[i]).width / 2      \
			- sfSprite_getGlobalBounds(btris[i]).width / 2 * i, \
			sfView_getSize(gameview).y - sfRectangleShape_getSize(toprect).y * 2));
		}
		else
		{
			sfSprite_setPosition(btris[i],                      \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			+ sfSprite_getGlobalBounds(btris[i]).width / 2 * i, \
			sfView_getSize(gameview).y - sfRectangleShape_getSize(toprect).y * 2));
		}
	
	}

	// Position the top triangles
	for (int i = 0; i < 7; i++)
	{
		sfSprite_setTexture(ttris[i], triangleTexture, 0);
		sfSprite_setScale(ttris[i],                                     \
		sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		sfSprite_setRotation(ttris[i], -180.f);

		if( i % 2 == 0)
		{
			sfSprite_setPosition(ttris[i], \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			+ sfSprite_getGlobalBounds(ttris[i]).width / 2      \
			+ sfSprite_getGlobalBounds(ttris[i]).width / 2 * i, \
			0 + sfRectangleShape_getSize(toprect).y * 2)) ;
		}
		else
		{
			sfSprite_setPosition(ttris[i],                      \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			- sfSprite_getGlobalBounds(ttris[i]).width / 2 * i, \
			0 + sfRectangleShape_getSize(toprect).y * 2));
		}
	
	}
}
	
void
render(void)
{
	// Clear the screen 
	sfRenderWindow_clear(window, sfBlack);
	
	background_render();
		switch (gs) 
		{
		case start:
			start_render();
			break;
		case game:
			game_render();
			break;
		case pause:
			pause_render();
			break;
		default:
			break;
		}

	// Update the window 
	sfRenderWindow_display(window);
}

void
start_render(void)
{

	sfRenderWindow_drawSprite(window, birdSprite, NULL);
}

void
game_render(void)
{
	// Draw the bird
	sfRenderWindow_drawSprite(window, birdSprite, NULL);
	for (int i = 0; i < 10; i++)
	{
		sfRenderWindow_drawSprite(window, stris[i], NULL);
	}

}

void
pause_render(void)
{
	game_render();
	sfRenderWindow_drawRectangleShape(window, menubgborder, NULL);
	sfRenderWindow_drawRectangleShape(window, menubg, NULL);
	sfRenderWindow_drawRectangleShape(window, resumebuttonborder, NULL);
	sfRenderWindow_drawRectangleShape(window, resumebutton, NULL);
	sfRenderWindow_drawText(window, resumebutton_text, NULL);
	sfRenderWindow_drawRectangleShape(window, helpbuttonborder, NULL);
	sfRenderWindow_drawRectangleShape(window, helpbutton, NULL);
	sfRenderWindow_drawText(window, helpbutton_text, NULL);
}

void
background_render(void)
{
	sfRenderWindow_drawRectangleShape(window, background, NULL);
	sfRenderWindow_drawCircleShape(window, circle, NULL);
	sfRenderWindow_drawText(window, circle_text, NULL);

	sfRenderWindow_drawRectangleShape(window, toprect, NULL);
	sfRenderWindow_drawRectangleShape(window, bottomrect, NULL);

	for (int i = 0; i < 7; i++)
	{
		sfRenderWindow_drawSprite(window, ttris[i], NULL);
		sfRenderWindow_drawSprite(window, btris[i], NULL);
	}
}

void
game_close(void)
{
	sfRenderWindow_close(window);
	sfRenderWindow_destroy(window);
	sfCircleShape_destroy(circle);
	exit(0);
}
