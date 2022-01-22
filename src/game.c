#include <stdio.h>
#include <stdlib.h>

#include "game.h"

sfEvent           event;

enum gamestate gs;
sfRenderWindow   *window;
sfVector2u        ws;
sfView           *gameview;
double            fallspeed;
double            flyspeed;


struct {
	sfCircleShape    *circle;
	sfText           *circle_text;
	sfFont           *circle_textfont;
} Circle;

struct {
	sfRectangleShape *background;
	sfRectangleShape *bottomrect;
	sfRectangleShape *toprect;
} Background;
struct {
	sfTexture        *triangleTexture;
	sfSprite         *triangleSprite;
	sfSprite         *btris[7];
	sfSprite         *ttris[7];
} Triangles;

struct {
	double            menuxpos;
	sfRectangleShape *menubg;
	sfRectangleShape *menubgborder;
	sfRectangleShape *resumebutton;
	sfRectangleShape *resumebuttonborder;
	sfText           *resumebutton_text;
	sfRectangleShape *helpbutton;
	sfRectangleShape *helpbuttonborder;
	sfText           *helpbutton_text;
} Startmenu;

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

	Game.bird_x = 0;
	Game.bird_y = 0;
	Game.score = 0;
	Startmenu.menuxpos = 0;

	gs = GAMESTATE;
	//  Create the main window
	sfVideoMode mode = {WINDOW_WIDTH, WINDOW_HEIGHT, 32};
	window = sfRenderWindow_create(mode, "Simple Don't Touch The Spikes Clone", sfClose | sfResize, NULL);
	ws = sfRenderWindow_getSize(window);

	// Initialize the Game.timer
	Game.timer = sfClock_create();

	// Create the view
	gameview = sfView_create();
	// update the view to the new size of the window
	Game.ratio = (float)ws.y / (float)ws.x * 0.5f;
	// update the view to the new size of the window
	Game.visibleArea = sfFloatRect_fromFloat(0.f, 0.f, ws.y * 0.5, ws.y);
	sfView_reset(gameview, Game.visibleArea);
	sfView_setViewport(gameview, sfFloatRect_fromFloat(0.5f - (Game.ratio / 2.f), 0, Game.ratio, 1.0f));
	sfRenderWindow_setView(window, gameview);

	// Create the Circle.circle
	Circle.circle = sfCircleShape_create();
	sfCircleShape_setFillColor(Circle.circle, sfColor_fromRGB(250,250,250));
	
	// Initialize the Circle.circle text and font
	Circle.circle_text = sfText_create();
	Circle.circle_textfont = sfFont_createFromFile("res/Terminus.ttf");
	sfText_setFont(Circle.circle_text, Circle.circle_textfont);
	sfText_setColor(Circle.circle_text, sfColor_fromRGB(50, 50, 50));

	// Create the Background.background
	Background.background = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Background.background, sfColor_fromRGB(229,229,229));

	// Create the bottom and top rectangle
	Background.bottomrect = sfRectangleShape_create();
	Background.toprect = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Background.bottomrect, sfColor_fromRGB(50,50,50));
	sfRectangleShape_setFillColor(Background.toprect, sfColor_fromRGB(50,50,50));

	// Initialize the triangle texture 
	const sfIntRect textureArea = {0, 0, 11, 11};
	Triangles.triangleTexture = sfTexture_createFromFile("res/gem/spikes/spike.1/spike.png", &textureArea);
	sfTexture_setSmooth(Triangles.triangleTexture, 0);

	// Create the top and bottom triangles
	for (int i = 0; i < 7; i++)
	{
		Triangles.ttris[i] = sfSprite_create();
		Triangles.btris[i] = sfSprite_create();
	}

	// Create the side triangles
	for (int i = 0; i < 10; i++)
	{
		Game.stris[i] = sfSprite_create();
	}

	//Initialize the start elements
	Startmenu.menubg = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Startmenu.menubg, sfColor_fromRGB(255,50,50));
	Startmenu.menubgborder = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Startmenu.menubgborder, sfColor_fromRGB(0,0,0));
	Startmenu.resumebutton = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Startmenu.resumebutton, sfColor_fromRGB(255,0,0));
	Startmenu.resumebuttonborder = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Startmenu.resumebuttonborder, sfColor_fromRGB(0,0,0));
	Startmenu.resumebutton_text = sfText_create();
	sfText_setFont(Startmenu.resumebutton_text, Circle.circle_textfont);
	sfText_setColor(Startmenu.resumebutton_text, sfColor_fromRGB(255, 255, 255));
	Startmenu.helpbutton = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Startmenu.helpbutton, sfColor_fromRGB(255,0,0));
	Startmenu.helpbuttonborder = sfRectangleShape_create();
	sfRectangleShape_setFillColor(Startmenu.helpbuttonborder, sfColor_fromRGB(0,0,0));
	Startmenu.helpbutton_text = sfText_create();
	sfText_setFont(Startmenu.helpbutton_text, Circle.circle_textfont);
	sfText_setColor(Startmenu.helpbutton_text, sfColor_fromRGB(255, 255, 255));

	// Initialize the bird textures
	const sfIntRect textureArea2 = {0, 0, 13, 10};
	Game.birdTexture1 = sfTexture_createFromFile("res/brd0.png", &textureArea2);
	Game.birdTexture2 = sfTexture_createFromFile("res/brd1.png", &textureArea2);
	sfTexture_setSmooth(Game.birdTexture1, 0);
	sfTexture_setSmooth(Game.birdTexture2, 0);

	// Initialize the bird sprite
	Game.birdSprite = sfSprite_create();
	sfSprite_setTexture(Game.birdSprite, Game.birdTexture1, 0);
	sfSprite_setOrigin(Game.birdSprite,                                      \
	sfVector2f_fromFloat(sfSprite_getLocalBounds(Game.birdSprite).width / 2, \
	                     sfSprite_getLocalBounds(Game.birdSprite).height / 2));
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
		Game.ratio = (float)ws.y / (float)ws.x * 0.5f;
		// Update the view to the new size of the window
		Game.visibleArea = sfFloatRect_fromFloat(0.f, 0.f, ws.y * 0.5, ws.y);
		sfView_reset(gameview, Game.visibleArea);
		sfView_setViewport(gameview, sfFloatRect_fromFloat(0.5f - (Game.ratio / 2.f), 0, Game.ratio, 1.0f));
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
update(int ms) // ms - tick Game.duration
{
	sfTime t = sfClock_getElapsedTime(Game.timer);
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
		sfClock_restart(Game.timer);
	} 
}

void
game_update(void)
{
	Startmenu.menuxpos = -sfView_getSize(gameview).x;
	bird_update();
	stris_animation();
	colcheck_update();

	// Position the Circle.circle text
	char scorestr[12];
	sprintf(scorestr, "%d", Game.score); 
	sfText_setString(Circle.circle_text, scorestr);
	sfText_setCharacterSize(Circle.circle_text, sfView_getSize(gameview).x * 0.3);
	sfText_setPosition(Circle.circle_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 - sfText_getLocalBounds(Circle.circle_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(Circle.circle_text).height));
}

void
bird_update(void)
{
	if (flyspeed < 0)
	{
		sfSprite_setScale(Game.birdSprite, \
		sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		flyspeed = -(double)sfView_getSize(gameview).x * 0.01;
	}
	if (flyspeed > 0)
	{
		sfSprite_setScale(Game.birdSprite, \
		sfVector2f_fromFloat((float)-sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		flyspeed = (double)sfView_getSize(gameview).x * 0.01;
	}
	sfSprite_setPosition(Game.birdSprite, \
	                     sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Game.bird_x, \
	                                          sfView_getSize(gameview).y / 2 + Game.bird_y));

	// Change bird sprite
	if (fallspeed > 0)
	{
		sfSprite_setTexture(Game.birdSprite, Game.birdTexture2, 0);
	}
	else
	{
		sfSprite_setTexture(Game.birdSprite, Game.birdTexture1, 0);
	}
	
	// Change fly direction
	if (sfSprite_getPosition(Game.birdSprite).x - sfSprite_getGlobalBounds(Game.birdSprite).width / 2 < 0)
	{
		flyspeed = sfView_getSize(gameview).x * 0.01;
		Game.score++;
		stris_update();
	}
	else if (sfSprite_getPosition(Game.birdSprite).x + sfSprite_getGlobalBounds(Game.birdSprite).width / 2 > sfView_getSize(gameview).x)
	{
		flyspeed = -sfView_getSize(gameview).x * 0.01;
		Game.score++;
		stris_update();
	}
	fallspeed += (double)sfView_getSize(gameview).x * 0.004;
	Game.bird_y += fallspeed;
	Game.bird_x += flyspeed;
}

void
stris_update(void)
{
// <<->>
	if(flyspeed > 0.f)
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_setTexture(Game.stris[i], Triangles.triangleTexture, 0);
			sfSprite_setScale(Game.stris[i],                                     \
			sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
			(float)sfView_getSize(gameview).x * 0.01));
			sfSprite_setRotation(Game.stris[i], -90.f);
	
			sfSprite_setPosition(Game.stris[i],                      \
			sfVector2f_fromFloat(sfView_getSize(gameview).x  \
			- sfSprite_getGlobalBounds(Game.stris[i]).width * 0.0, \
			0 + sfRectangleShape_getSize(Background.toprect).y * 3 * (rand() % 10)));
		
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_setTexture(Game.stris[i], Triangles.triangleTexture, 0);
			sfSprite_setScale(Game.stris[i],                                     \
			sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
			(float)sfView_getSize(gameview).x * 0.01));
			sfSprite_setRotation(Game.stris[i], 90.f);
	
			sfSprite_setPosition(Game.stris[i],                      \
			sfVector2f_fromFloat(sfSprite_getGlobalBounds(Game.stris[i]).width * 0.0, \
			0 + sfRectangleShape_getSize(Background.toprect).y + sfRectangleShape_getSize(Background.toprect).y * 4 * (rand() % 10)));
		
		}
	}
	
}

void
stris_animation(void)
{
	if(sfSprite_getPosition(Game.stris[0]).x < sfView_getCenter(gameview).x)
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_move(Game.stris[i], sfVector2f_fromFloat((( sfSprite_getGlobalBounds(Game.stris[i]).width * 0.8) - sfSprite_getPosition(Game.stris[i]).x) * 0.2, 0));
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			sfSprite_move(Game.stris[i], sfVector2f_fromFloat(((sfView_getSize(gameview).x - sfSprite_getGlobalBounds(Game.stris[i]).width * 0.8) - sfSprite_getPosition(Game.stris[i]).x) * 0.2, 0));
		}
	}
}

void
colcheck_update(void)
{
	// Bottom spikes
	if (sfSprite_getPosition(Triangles.btris[0]).y <= sfSprite_getPosition(Game.birdSprite).y)
	{
		game_close();
	}

	// Top spikes
	if (sfSprite_getPosition(Triangles.ttris[0]).y >= sfSprite_getPosition(Game.birdSprite).y)
	{
		game_close();
	}

	// Side spikes
	for (int i = 0; i < 10; i++)
	{
		if(sfView_getSize(gameview).x - sfSprite_getGlobalBounds(Game.stris[i]).width * 0.4<\
		sfSprite_getGlobalBounds(Game.birdSprite).left + sfSprite_getGlobalBounds(Game.birdSprite).width)
		{
			if((sfSprite_getGlobalBounds(Game.birdSprite).top > sfSprite_getGlobalBounds(Game.stris[i]).top\
			-  sfSprite_getGlobalBounds(Game.stris[i]).height) &&
			(sfSprite_getGlobalBounds(Game.birdSprite).top - sfSprite_getGlobalBounds(Game.birdSprite).height\
			< sfSprite_getGlobalBounds(Game.stris[i]).top) &&\
			sfSprite_getGlobalBounds(Game.birdSprite).left<\
			sfSprite_getGlobalBounds(Game.stris[i]).left)
			game_close();
		}
		else if(sfSprite_getGlobalBounds(Game.stris[i]).width * 0.4 >\
		sfSprite_getGlobalBounds(Game.birdSprite).left)
		{
			if((sfSprite_getGlobalBounds(Game.birdSprite).top > sfSprite_getGlobalBounds(Game.stris[i]).top\
			-  sfSprite_getGlobalBounds(Game.stris[i]).height) &&
			(sfSprite_getGlobalBounds(Game.birdSprite).top - sfSprite_getGlobalBounds(Game.birdSprite).height\
			< sfSprite_getGlobalBounds(Game.stris[i]).top) &&\
			sfSprite_getGlobalBounds(Game.birdSprite).left + sfSprite_getGlobalBounds(Game.birdSprite).width >\
			sfSprite_getGlobalBounds(Game.stris[i]).left)
			game_close();
		}
	}
}

void
pause_update(void)
{
	// Position the pause Background.background border
	sfRectangleShape_setSize(Startmenu.menubgborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/7.5, \
	                     sfView_getSize(gameview).y / 3.7));
	sfRectangleShape_setOrigin(Startmenu.menubgborder,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Startmenu.menubgborder).x / 2, \
	                     sfRectangleShape_getSize(Startmenu.menubgborder).y / 2));
	sfRectangleShape_setPosition(Startmenu.menubgborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Startmenu.menuxpos,\
	                     sfView_getSize(gameview).y / 2 ));

	// Position the pause Background.background
	sfRectangleShape_setSize(Startmenu.menubg,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/6, \
	                     sfView_getSize(gameview).y / 4));
	sfRectangleShape_setOrigin(Startmenu.menubg,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Startmenu.menubg).x / 2, \
	                     sfRectangleShape_getSize(Startmenu.menubg).y / 2));
	sfRectangleShape_setPosition(Startmenu.menubg,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Startmenu.menuxpos,\
	                     sfView_getSize(gameview).y / 2 ));

	// Position the Startmenu.resumebutton
	sfRectangleShape_setSize(Startmenu.resumebutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/4.8, \
	                     sfView_getSize(gameview).y / 18));
	sfRectangleShape_setOrigin(Startmenu.resumebutton,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Startmenu.resumebutton).x / 2, \
	                     sfRectangleShape_getSize(Startmenu.resumebutton).y / 2));
	sfRectangleShape_setPosition(Startmenu.resumebutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Startmenu.menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     - sfRectangleShape_getSize(Startmenu.resumebutton).y ));

	// Position the Startmenu.resumebutton border
	sfRectangleShape_setSize(Startmenu.resumebuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/5.15, \
	                     sfView_getSize(gameview).y / 15));
	sfRectangleShape_setOrigin(Startmenu.resumebuttonborder,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Startmenu.resumebuttonborder).x / 2, \
	                     sfRectangleShape_getSize(Startmenu.resumebuttonborder).y / 2));
	sfRectangleShape_setPosition(Startmenu.resumebuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Startmenu.menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     - sfRectangleShape_getSize(Startmenu.resumebutton).y ));

	// Position the Startmenu.resumebutton text
	sfText_setCharacterSize(Startmenu.resumebutton_text, sfView_getSize(gameview).x * 0.1);
	sfText_setPosition(Startmenu.resumebutton_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 + Startmenu.menuxpos - sfText_getLocalBounds(Startmenu.resumebutton_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(Startmenu.resumebutton_text).height - sfRectangleShape_getSize(Startmenu.resumebutton).y));
	sfText_setString(Startmenu.resumebutton_text, "RESUME");

	// Position the Startmenu.helpbutton
	sfRectangleShape_setSize(Startmenu.helpbutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/4.8, \
	                     sfView_getSize(gameview).y / 18));
	sfRectangleShape_setOrigin(Startmenu.helpbutton,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Startmenu.helpbutton).x / 2, \
	                     sfRectangleShape_getSize(Startmenu.helpbutton).y / 2));
	sfRectangleShape_setPosition(Startmenu.helpbutton,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Startmenu.menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     + sfRectangleShape_getSize(Startmenu.helpbutton).y ));

	// Position the Startmenu.helpbutton border
	sfRectangleShape_setSize(Startmenu.helpbuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x - sfView_getSize(gameview).x/5.15, \
	                     sfView_getSize(gameview).y / 15));
	sfRectangleShape_setOrigin(Startmenu.helpbuttonborder,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Startmenu.helpbuttonborder).x / 2, \
	                     sfRectangleShape_getSize(Startmenu.helpbuttonborder).y / 2));
	sfRectangleShape_setPosition(Startmenu.helpbuttonborder,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Startmenu.menuxpos,\
	                     sfView_getSize(gameview).y / 2 \
	                     + sfRectangleShape_getSize(Startmenu.helpbutton).y ));

	// Position the Startmenu.helpbutton text
	sfText_setCharacterSize(Startmenu.helpbutton_text, sfView_getSize(gameview).x * 0.1);
	sfText_setPosition(Startmenu.helpbutton_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 + Startmenu.menuxpos- sfText_getLocalBounds(Startmenu.helpbutton_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(Startmenu.helpbutton_text).height + sfRectangleShape_getSize(Startmenu.helpbutton).y));
	sfText_setString(Startmenu.helpbutton_text, "HELP");
	pause_animation();
}

void
pause_animation(void)
{
	Startmenu.menuxpos+=(double)(sfView_getSize(gameview).x / 2 - sfRectangleShape_getPosition(Startmenu.menubgborder).x) * 0.07;
}

void
start_update(void)
{
	// Position the Circle.circle text
	sfText_setCharacterSize(Circle.circle_text, sfView_getSize(gameview).x * 0.1);
	sfText_setPosition(Circle.circle_text, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2 - sfText_getLocalBounds(Circle.circle_text).width / 2, \
	sfView_getSize(gameview).y/2 - sfText_getLocalBounds(Circle.circle_text).height));
	sfText_setString(Circle.circle_text, "Press\nSpace\n");

	sfSprite_setPosition(Game.birdSprite, \
	                     sfVector2f_fromFloat(sfView_getSize(gameview).x / 2 + Game.bird_x, \
	                                          sfView_getSize(gameview).y / 2 + Game.bird_y));
	sfSprite_setScale(Game.birdSprite, \
	sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
	(float)sfView_getSize(gameview).x * 0.01));
}

void
background_update(void)
{
	// Position the bottom rectangle
	sfRectangleShape_setSize(Background.bottomrect,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x, \
	                     sfView_getSize(gameview).y / 20));
	sfRectangleShape_setOrigin(Background.bottomrect,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Background.bottomrect).x / 2, \
	                     sfRectangleShape_getSize(Background.bottomrect).y / 2));
	sfRectangleShape_setPosition(Background.bottomrect,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2, \
	                     sfView_getSize(gameview).y      \
	                     - sfRectangleShape_getSize(Background.bottomrect).y / 2));
	
	// Position the top rectangle
	sfRectangleShape_setSize(Background.toprect,                \
	sfVector2f_fromFloat(sfView_getSize(gameview).x, \
	                     sfView_getSize(gameview).y/20));
	sfRectangleShape_setOrigin(Background.toprect,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Background.toprect).x / 2, \
	                     sfRectangleShape_getSize(Background.toprect).y / 2));
	sfRectangleShape_setPosition(Background.toprect,                 \
	sfVector2f_fromFloat(sfView_getSize(gameview).x / 2,  \
	                     sfView_getSize(gameview).y /2    \
	                     - sfView_getSize(gameview).y / 2 \
	                     + sfRectangleShape_getSize(Background.toprect).y / 2));

	// Initialize the Background.background
	sfRectangleShape_setSize(Background.background, \
	sfVector2f_fromFloat(ws.x * 100, ws.y * 100));
	sfRectangleShape_setOrigin(Background.background,                           \
	sfVector2f_fromFloat(sfRectangleShape_getSize(Background.background).x / 2, \
	sfRectangleShape_getSize(Background.background).y / 2));
	sfRectangleShape_setPosition(Background.background, \
	sfVector2f_fromFloat(ws.x/2, ws.y/2));

	// Position the Circle.circle
	sfCircleShape_setRadius(Circle.circle, sfView_getSize(gameview).x / 3);
	sfCircleShape_setPosition(Circle.circle, \
	sfVector2f_fromFloat(sfView_getSize(gameview).x/2-sfCircleShape_getRadius(Circle.circle), \
	sfView_getSize(gameview).y/2-sfCircleShape_getRadius(Circle.circle)));
	

	// Position the bottom triangles
	for (int i = 0; i < 7; i++)
	{
		sfSprite_setTexture(Triangles.btris[i], Triangles.triangleTexture, 0);
		sfSprite_setScale(Triangles.btris[i], \
		sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		sfSprite_setRotation(Triangles.btris[i], 0.f);
		if( i % 2 == 0)
		{
			sfSprite_setPosition(Triangles.btris[i],                      \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			- sfSprite_getGlobalBounds(Triangles.btris[i]).width / 2      \
			- sfSprite_getGlobalBounds(Triangles.btris[i]).width / 2 * i, \
			sfView_getSize(gameview).y - sfRectangleShape_getSize(Background.toprect).y * 2));
		}
		else
		{
			sfSprite_setPosition(Triangles.btris[i],                      \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			+ sfSprite_getGlobalBounds(Triangles.btris[i]).width / 2 * i, \
			sfView_getSize(gameview).y - sfRectangleShape_getSize(Background.toprect).y * 2));
		}
	
	}

	// Position the top triangles
	for (int i = 0; i < 7; i++)
	{
		sfSprite_setTexture(Triangles.ttris[i], Triangles.triangleTexture, 0);
		sfSprite_setScale(Triangles.ttris[i],                                     \
		sfVector2f_fromFloat((float)sfView_getSize(gameview).x * 0.01, \
		(float)sfView_getSize(gameview).x * 0.01));
		sfSprite_setRotation(Triangles.ttris[i], -180.f);

		if( i % 2 == 0)
		{
			sfSprite_setPosition(Triangles.ttris[i], \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			+ sfSprite_getGlobalBounds(Triangles.ttris[i]).width / 2      \
			+ sfSprite_getGlobalBounds(Triangles.ttris[i]).width / 2 * i, \
			0 + sfRectangleShape_getSize(Background.toprect).y * 2)) ;
		}
		else
		{
			sfSprite_setPosition(Triangles.ttris[i],                      \
			sfVector2f_fromFloat(sfView_getCenter(gameview).x  \
			- sfSprite_getGlobalBounds(Triangles.ttris[i]).width / 2 * i, \
			0 + sfRectangleShape_getSize(Background.toprect).y * 2));
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

	sfRenderWindow_drawSprite(window, Game.birdSprite, NULL);
}

void
game_render(void)
{
	// Draw the bird
	sfRenderWindow_drawSprite(window, Game.birdSprite, NULL);
	for (int i = 0; i < 10; i++)
	{
		sfRenderWindow_drawSprite(window, Game.stris[i], NULL);
	}

}

void
pause_render(void)
{
	game_render();
	sfRenderWindow_drawRectangleShape(window, Startmenu.menubgborder, NULL);
	sfRenderWindow_drawRectangleShape(window, Startmenu.menubg, NULL);
	sfRenderWindow_drawRectangleShape(window, Startmenu.resumebuttonborder, NULL);
	sfRenderWindow_drawRectangleShape(window, Startmenu.resumebutton, NULL);
	sfRenderWindow_drawText(window, Startmenu.resumebutton_text, NULL);
	sfRenderWindow_drawRectangleShape(window, Startmenu.helpbuttonborder, NULL);
	sfRenderWindow_drawRectangleShape(window, Startmenu.helpbutton, NULL);
	sfRenderWindow_drawText(window, Startmenu.helpbutton_text, NULL);
}

void
background_render(void)
{
	sfRenderWindow_drawRectangleShape(window, Background.background, NULL);
	sfRenderWindow_drawCircleShape(window, Circle.circle, NULL);
	sfRenderWindow_drawText(window, Circle.circle_text, NULL);

	sfRenderWindow_drawRectangleShape(window, Background.toprect, NULL);
	sfRenderWindow_drawRectangleShape(window, Background.bottomrect, NULL);

	for (int i = 0; i < 7; i++)
	{
		sfRenderWindow_drawSprite(window, Triangles.ttris[i], NULL);
		sfRenderWindow_drawSprite(window, Triangles.btris[i], NULL);
	}
}

void
game_close(void)
{
	sfRenderWindow_close(window);
	sfRenderWindow_destroy(window);
	sfCircleShape_destroy(Circle.circle);
	exit(0);
}
