#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "SOIL.h"

/* PREDEFINED VALUES */ 
#define TIMER (18) 
#define LOOK_AT_MACRO (800)
#define WALL_SPEED (4) /* Modifer which increases value of acceleration variable. */
#define POINT_SPEED (6) /* Modifer which determines points life expectancy if the player hasn't collected the point in certain period. */
#define BULLET_SPEED (6) /* Modifer which increases value of bullets translation parameter variable. */
#define WALL_SPAWN_LOCATION (425) /* Starting location of the wall. */
#define GLOBAL_TRANSLATE (50) /* Global translation parameter. */
#define BOUNDARY_R (325) /* Right in-game boundary for player movement. */
#define BOUNDARY_L (-325) /* Left in-game boundary for player movement. */
#define BOUNDARY_T (300) /* Top in-game boundary for player movement. */
#define BOUNDARY_B (-300) /* Bottom in-game boundary for player movement. */
#define GLOBAL_SCALE_Z (0.6) /* Wall scaling predifined value */
#define UPPER_WALL (0) /* ID for upper segment of the wall. */
#define MIDDLE_WALL (1) /*  ID for middle segment of the wall. */
#define LOWER_WALL (2) /*  ID for lowersegment of the wall. */
#define POINT (20) /*  ID for collecatble object "point". */ 
#define POINTS_OBJECT_SIZE (20) /* Cube's edge size. */
#define RADIUS (30) /* Player's radius. */
#define DIAMETAR (60) /* Player's diametar. */
#define PLAYER (10) /*  ID for the player. */
#define BULLET (66) /*  ID for the bullet. */
#define SHIELD (77) /*  ID for the shield. */
#define COLOR_NUMBER (3) /* Number of wall colors. */
#define PI (3.1415926535) 
#define EPSILON (0.01) 
#define MAX_BULLET_NUMBER (9) /* Maximum number for an array of bullets. */
#define DIST_BULLETS (450) /* Distance between bullet in an array. */

/* STRUCTURES */

/*Bullet structure has two coordinates. */
typedef struct {
	double X;
	double Y;
} Bullet;

/**
*Point structure has fields for: three coordinates, lifetime which is in-game period of objects life and indicator
*if the object is collected. 
**/
typedef struct {
	double X;
	double Y;
	double Z;
	double lifetime;
	int collected;
} Point;

/**
* Shield structure has fields for: two coordinates and 
* indicator if the shield exists in-game or not.
**/
typedef struct {
	double X;
	double Y;
	int exist;
} Shield;

/**
* Player structure has fields for: two coordinates, two previous coordinates,
* previousColor for remembering which color was previous,
* indicator if the player has shield and indicator if the player is alive/dead.
**/
typedef struct {
	double X;
	double Y;
	double previousX;
	double previousY;
	int previousColor;
	int hasShield;
	int dead;
}Player;

/**
* Wall structure has fields for: color, so we can track and manipulate with 
* the wall's colors. 
**/
typedef struct {
	int color;
} Wall;

/* FUNCTION DEFINITIONS */

void colisionPlayerGoldenPointFunc();
void colisionPlayerWallFunc();
void changeColor();
void colisionPlayerBulletFunc(int bulletID);
void colisionPlayerShieldFunc();
void drawOscillatingBullet(int bulletID);
void drawSimpleBullet(int bulletID);
void setColor(unsigned objectId);
void generateCoordinates(int ID);
void drawWall();
void drawPlayer();
void drawPoint();
void drawArea();
void drawShield();
void displayEnd();
void displayScore();
void displayLivesLeft();
void resetGame();
void initializeGameData();
void resetLifeLost();
static void initializeTextures();
static void initializeMain(int argc, char** argv);
static void initializeLights();
static void onKeyboard(unsigned char key, int x, int y);
static void onMotion(int x, int y);
static void onReshape(int width, int height);
static void onTimer(int value);
static void onDisplay(void);

/* VARIABLES */

double acceleration = 1.0; // Wall's acceleration modifer which increases over time.
double wallObjectTimer = WALL_SPAWN_LOCATION; // Timer for wall translation.
double newObjectTimer = 0; // Timer for redrawing golden object.
double newObjectSignal = 0; // Timer for redrawing golden object after colision with the player.
double bulletX = 400; // Starting X coordinate for all bullets.
double bulletY = 0; //Starting Y coordinate for all bullets. 
double createShieldTimer = 0; // Creates shield after certain time. Aprox. every 10s. Shield exists without player 5s.
double shieldAnimationParametar = 0; // This adds a litte bit animation when you pick up shield.
int window_height = 600; 
int window_width = 800;
int colisionNumber = 0; // Number of points collected.
int pointObjectRotationTimer = 0; //Timer for point object rotation.
int timerActive = 0; // Game stop/active timer.
int colisionPlayerPoint = 0; // Indicator if colision between player and point happened.
int playerColorTimer = 0; // Indicator for switching player's color.
int wallColor = 0; // Simple random integer that chooses color of wall.
int bulletYNegation[MAX_BULLET_NUMBER]; // Vector of randomly generated numbers that translates bullets on Y axis.
int bulletChooser[MAX_BULLET_NUMBER]; // Vector of randomly generated numbers that helps us choose bullet type.
int colisionPlayerShield = 0; // Indicator if colision between player and shield happened.
int newShieldSignal = 0; // Time for redrawing shield.
int playerHasShieldTimer = 0; // We don't want player to have shield infinitely, so we add a constraint for time. Player has shield for 5s.
int color = 0; // Determines the color of the wall.
int livesLeft = 3; //Number of lives player has. Game ends when player loses all of his lives.
int score = -0; // Current score.
GLuint texture[2]; // Array of textures, one texture is used for three sides.
Bullet bullets[MAX_BULLET_NUMBER]; // Vector of bullets. Obviously.
Player player; // You.
Point point; // Collectable ordinary points with value of 1. Golden object has value of 3.
Wall walls[3]; // Vector of wall's parts. 
Shield shield; // One shield to protect you.

int main(int argc, char** argv) {

	initializeMain(argc,argv);
	glutMainLoop();

	return 0;
}


static void onKeyboard(unsigned char key, int x, int y)
{
	
	switch(key){

		/* You can exit the game with pressing ESC button. */
		case 27: exit(1);
			break;

		/*Animations starts with pressing 'g' or 'G'. */
		case 'g':
		case 'G': if(!timerActive && livesLeft > 0) {
				glutWarpPointer(400,300);
				glutTimerFunc(TIMER,onTimer,0);
				player.dead = 0;
				timerActive = 1;

		}
			break;
		/*Animations stops with pressing 's' or 'S'. */
		case 's':
		case 'S': timerActive = 0;; break;
		/*Program goes to fullscreen mode with pressing 'f' or 'F'. */
		case 'f':
		case 'F': glutFullScreen(); break; 
		/*User resets games's parameters with pressing 'r' or 'R'. */
		case 'r':
		case 'R': resetGame();
				  if(!timerActive){
				  	glutTimerFunc(TIMER,onTimer,0);
					player.dead = 0;
					timerActive = 1;
				  }
				  break;
		default:;
	}
}

static void onMotion(int x, int y)
{	

	glutSetCursor(GLUT_CURSOR_NONE);

	/**
	* We're calculating player's in-game position using previous and current position.
	**/
	player.previousX = player.X;
	player.previousY = player.Y;
	int dx =  x-player.previousX - window_width/2;
	int dy = -y-player.previousY + window_height/2;
	player.X += dx;
	player.Y += dy;
	 /**
	 *To avoid any kind of arithmetic operation, we are goint to use predefined values for bounderies.
	 *If player is out of bounderies on the right side, fix the X coordinate. 
	 **/
	if(player.X + RADIUS >= BOUNDARY_R) {
		player.X = BOUNDARY_R - RADIUS;
	}
	/* If player is out of bounderies on the left side, fix the X coordinate. */
	else if ( player.X - RADIUS <= BOUNDARY_L) { 
		player.X = BOUNDARY_L + RADIUS;	
	}
	/* If player is out of bounderies on the top side, fix the Y coordinate. */
	if(player.Y + RADIUS >= BOUNDARY_T) {
		player.Y =  BOUNDARY_T - RADIUS;
	}
	/* If player is out of bounderies on the bottom side, fix the Y coordinate. */
	else if ( player.Y - RADIUS <= BOUNDARY_B){
		player.Y = BOUNDARY_B + RADIUS;
		
	}

}

static void onTimer(int value)
{
 	if (value != 0)
        return;

   	/* Updating timer for many, many things. */

    /* Point rotation timer. */
   	pointObjectRotationTimer = (pointObjectRotationTimer + 6) % 360;  
    
   	/* Wall movement and wall color. Wall is speeding up every frame by increasing value of global variable called 'acceleration'. */
    wallObjectTimer -= WALL_SPEED*acceleration;  
    if(wallObjectTimer <= -WALL_SPAWN_LOCATION + GLOBAL_TRANSLATE) {
    	wallObjectTimer = WALL_SPAWN_LOCATION; 
    	wallColor = rand() % COLOR_NUMBER;
    	acceleration += 0.05;
    }
    /**
    * Point life timer. If the player doesn't collect the point in certein period, 
    * it gives a signal for new coordinates.
    **/
    newObjectTimer += POINT_SPEED; 
    if (newObjectTimer >= 1000){
    	newObjectSignal = 1;
    	newObjectTimer = 0;
    }
    /* If score is above 30, we introduce bullets into the game. */
    if ( score > 30 ) {
	    bulletX -= BULLET_SPEED;
	    if ( bulletX <= -(MAX_BULLET_NUMBER*DIST_BULLETS)){
	    	bulletX = 400;
	    	/* It's not fun if Y value is always 0 at the begining of bullet's journey, so we randomize the Y value. */
	    	for(int i = 0; i < MAX_BULLET_NUMBER; i++) {
	    		bulletChooser[i] = rand();
	    		bulletYNegation[i] = ((bulletYNegation[i] = rand()) % 2 == 0? bulletYNegation[i] % 100: (bulletYNegation[i] % 100)*(-1));  	
	    	}
	    }
	}

	/*If player has a shield, we create simple animation and removing shield after 3.5 seconds. */
  	if ( player.hasShield == 1 ) {
	    shieldAnimationParametar += 1.5;
	    if ( shieldAnimationParametar >= 2*(RADIUS + 2.5) )
	    	shieldAnimationParametar = 2*(RADIUS + 2.5);
	    /** 
	    * Removing shield after 3.5 seconds and informing that the player
	    * no longer has the shield. 
	    **/
	    playerHasShieldTimer += 1;
		if ( playerHasShieldTimer >= 210){
			player.hasShield = 0;
			shield.exist = 0;
			playerHasShieldTimer = 0;
		} 
	}
	else {
		shieldAnimationParametar = -(RADIUS + 2.5);
	}

	/**
	* Shield spawns after some time and disappears if not collected. 
	* If there is already a shield, game won't draw it again.
	**/
    createShieldTimer += 1;
    if ( createShieldTimer >= 600 && createShieldTimer <= 900) {
    	if ( shield.exist == 0) 
    		newShieldSignal = 1;
    	shield.exist = 1;

	}
	else if ( createShieldTimer > 900 && playerHasShieldTimer == 0){
		createShieldTimer = 0;
		shield.exist = 0;
	}

    glutPostRedisplay();

    if (timerActive)
        glutTimerFunc(TIMER, onTimer, 0);
}

static void onReshape(int width, int height)
{
	window_height = height;
	window_width = width;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0, width, height);
    gluPerspective(45, (float) width / height, 1, 800);
    glutPostRedisplay();
}

static void onDisplay(void)
{	

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, LOOK_AT_MACRO, 
	 	 	  0, 0, 0,
	 		  0, 1, 0);

	glTranslatef(GLOBAL_TRANSLATE,0,0);	
	initializeLights();	
	/* Drawing wall. Player has to avoid the wall. */
	drawWall();
	/* Drawing player. Player controls sphere. */
    drawPlayer();
    /* Drawing shield. If collected, player becomes invincable. */
	drawShield();
	/* Drawing point. Player collects them as points. */
	drawPoint();
	/* Displaying current score. */
	displayScore();
	/* Displaying lives left. */
	displayLivesLeft();
	/* Drawing playground in which player and objects are showing up. */
	drawArea();
	/**
	* If score is above 30, we introduce bullets into the game. Player has to avoid bullets to stay alive.
	**/
	if ( score > 30 ) { 
	
		for ( int i = 0; i < MAX_BULLET_NUMBER; i++) {
			if ( bulletChooser[i] % 2 )
				drawSimpleBullet(i);
			else
				drawOscillatingBullet(i);
			if ( player.hasShield == 0)
				colisionPlayerBulletFunc(i);
		}
	}
	/**
	* If the player has the shield, neither bullets nor wall can take his lives.
	**/
	if ( player.hasShield == 0)
		colisionPlayerWallFunc(); 
	/* We're checking if there's a collision between in-game friendly objects */
	colisionPlayerGoldenPointFunc();
	colisionPlayerShieldFunc();	
	/**
	* If the player is dead, we display end game message, else we're reseting the game parameters except score and lives left.
	**/
	if ( player.dead == 1) {
		if ( livesLeft < 1) 
			displayEnd();	
		else
			resetLifeLost();
		timerActive = 0;
	}
	
		
	glFlush();
	glutSwapBuffers();
}

/**
* Function contains many glut functions and initializes game's parameters
**/
static void initializeMain(int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(200, 200);
	glutCreateWindow(argv[0]);
	glutKeyboardFunc(onKeyboard);
	glutPassiveMotionFunc(onMotion);
	glutReshapeFunc(onReshape);
	glutDisplayFunc(onDisplay);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));
	initializeTextures();
	initializeGameData();


}
/* Function initializes light's ambiental, diffusel, specular colors and light's position. */
static void initializeLights(){
	
	GLfloat light_ambient[] = { 0, 0, 0, 1 };
   	GLfloat light_diffuse[] = { 1, 1, 1, 1 };
    GLfloat light_specular[] = { 1, 1, 1, 1 };
    GLfloat light_position[] = { 0, 0, LOOK_AT_MACRO, 1};

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);	
}
/**
* All textures were made by me.
* I mistekenly gave to ceil and floor texture number 2 and I liked the way it looked, so I kept that.
* These textures give that 70's or 80's vibe to the game. 
**/
static void initializeTextures(){
	
	texture[0] = SOIL_load_OGL_texture
    (
        "./textures/levocartoony.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
       	SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
    );
	texture[1] = SOIL_load_OGL_texture
    (
        "./textures/desnocartoony.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
    );
	
    /* If the game fails to load textures, exits with a failure. */
    if(texture[0] == 0 || texture[1] == 0 ) {
    	fprintf(stderr, "Failed to initialize textures!\n");
    	exit(EXIT_FAILURE);
    }
    
}



/* We want to set up normals for cylinder. */
void set_normal_and_vertex(float u, float v)
{
	glNormal3f(
	            35*sin(v),
	            0,
	            35*cos(v)
	            );
	    glVertex3f(
	            sin(v),
	            u,
	            cos(v)
	            );
}

/* Function drawArea() draws floor, ceil and side walls and two decorative polygons. */
void drawArea()
{
	glColor3f(1.0,1.0,1.0);
	glPushMatrix();
		glDisable(GL_LIGHTING);
		/* Floor */
		glPushMatrix();
			glTranslatef(0,-335,25);
			glRotatef(180,0,0,1);
			glRotatef(90,1,0,0);
			glScalef(7,0.5,1);
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, texture[1]);
	        glBegin(GL_QUADS);
	            glNormal3f(0.0f, 1.0f, 0.0f);
	            glTexCoord2f(1.0f, 0.0f); 
	            glVertex3f(-50.0f,  50.0f, -50.0f);
	            glTexCoord2f(1.0f, 1.0f); 
	            glVertex3f(-50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 1.0f); 
	            glVertex3f( 50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 0.0f); 
	            glVertex3f( 50.0f,  50.0f, -50.0f); 
	        glEnd();
	        glDisable(GL_TEXTURE_2D);  
	        
		glPopMatrix();
		
		/* Side walls */
		/* Left */
		glPushMatrix();
			
			glTranslatef(-370,0,100);		
			glRotatef(180,0,0,1);
			glRotatef(90,1,0,0);
			glScalef(1.5,0.1,6);
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, texture[0]);
	        glBegin(GL_QUADS);
	            glNormal3f(0.0f, 1.0f, 0.0f);
	            glTexCoord2f(1.0f, 0.0f); 
	            glVertex3f(-50.0f,  50.0f, -50.0f);
	            glTexCoord2f(1.0f, 1.0f); 
	            glVertex3f(-50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 1.0f); 
	            glVertex3f( 50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 0.0f); 
	            glVertex3f( 50.0f,  50.0f, -50.0f); 
	        glEnd();
	        glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		/* Right */
		glPushMatrix();
			
			glTranslatef(310,0,100); 
			glRotatef(180,0,0,1);
			glRotatef(90,1,0,0);
			glScalef(0.5,0.1,6);
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, texture[1]);
	        glBegin(GL_QUADS);
	            glNormal3f(0.0f, 1.0f, 0.0f);
	            glTexCoord2f(1.0f, 0.0f); 
	            glVertex3f(-50.0f,  50.0f, -50.0f);
	            glTexCoord2f(1.0f, 1.0f); 
	            glVertex3f(-50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 1.0f); 
	            glVertex3f( 50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 0.0f); 
	            glVertex3f( 50.0f,  50.0f, -50.0f); 
	        glEnd();
	        glDisable(GL_TEXTURE_2D); 
	        
		glPopMatrix();
		glPushMatrix();
			
			glTranslatef(400,0,20);  
			glRotatef(180,0,0,1);
			glRotatef(90,1,0,0);
			glScalef(2,1,10);
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, texture[1]);
	        glBegin(GL_QUADS);
	            glNormal3f(0.0f, 1.0f, 0.0f);
	            glTexCoord2f(1.0f, 0.0f); 
	            glVertex3f(-50.0f,  50.0f, -50.0f);
	            glTexCoord2f(1.0f, 1.0f); 
	            glVertex3f(-50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 1.0f); 
	            glVertex3f( 50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 0.0f); 
	            glVertex3f( 50.0f,  50.0f, -50.0f); 
	        glEnd();
	        glDisable(GL_TEXTURE_2D); 
	        
			
		glPopMatrix();

				glPushMatrix();
			
			glTranslatef(-520,0,20); 
			glRotatef(180,0,0,1);
			glRotatef(90,1,0,0);
			glScalef(2,1,10);
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, texture[1]);
	        glBegin(GL_QUADS);
	            glNormal3f(0.0f, 1.0f, 0.0f);
	            glTexCoord2f(1.0f, 0.0f); 
	            glVertex3f(-50.0f,  50.0f, -50.0f);
	            glTexCoord2f(1.0f, 1.0f); 
	            glVertex3f(-50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 1.0f); 
	            glVertex3f( 50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 0.0f); 
	            glVertex3f( 50.0f,  50.0f, -50.0f); 
	        glEnd();
	        glDisable(GL_TEXTURE_2D); 
	        
		glPopMatrix();

		/* Ceil */
		glPushMatrix();
			glTranslatef(0,335,25);
			glRotatef(180,0,0,1);
			glRotatef(90,1,0,0);
			glScalef(7,0.5,1);
			glEnable(GL_TEXTURE_2D);
	        glBindTexture(GL_TEXTURE_2D, texture[1]);
	        glBegin(GL_QUADS);
	            glNormal3f(0.0f, 1.0f, 0.0f);
	            glTexCoord2f(1.0f, 0.0f); 
	            glVertex3f(-50.0f,  50.0f, -50.0f);
	            glTexCoord2f(1.0f, 1.0f); 
	            glVertex3f(-50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 1.0f); 
	            glVertex3f( 50.0f,  50.0f,  50.0f);
	            glTexCoord2f(0.0f, 0.0f); 
	            glVertex3f( 50.0f,  50.0f, -50.0f); 
	        glEnd();
	        glDisable(GL_TEXTURE_2D);  
	        
		glPopMatrix();

		glEnable(GL_LIGHTING);
	glPopMatrix();
	return;
}

/** 
* Function drawPoints() draws collectable in-game object. 
* The more you collect, the more you progress. 
* When the player collects the object 
* or fails to collect it in certain time, a brand new object is drawn.
**/
void drawPoint()
{
	double pointObjectRotation = pointObjectRotationTimer;
	glPushMatrix();
		generateCoordinates(POINT);
		glTranslatef(point.X,point.Y, POINTS_OBJECT_SIZE/2);
		glRotatef(pointObjectRotation,1,0,0);
		glRotatef(pointObjectRotation,0,1,0);
		setColor(POINT);
		glutSolidCube(POINTS_OBJECT_SIZE);
	glPopMatrix();
	return;
}

/**
* Function drawSimpleBullet draws a bullet which has a constant projectory. 
* Player has to avoid bullets, or he will lose his lives.
**/

void drawSimpleBullet(int bulletID){
	glPushMatrix();
		glTranslatef(bullets[bulletID].X = bulletX + bulletID*DIST_BULLETS,bullets[bulletID].Y = bulletYNegation[bulletID]*2.5,0); // We multiply with 2.5 because we want our bullet to spawn all over the Y axis
		glRotatef(-90,0,1,0);
		setColor(BULLET);
		glutSolidCone(POINTS_OBJECT_SIZE/2, 40, 50, 50);
	glPopMatrix();

}


/**
* Function drawSimpleBullet draws a bullet which has an oscilatting projectory. 
* Player has to avoid bullets, or he will lose his lives.
**/

void drawOscillatingBullet(int bulletID){
	glPushMatrix();
		glTranslatef(bullets[bulletID].X = bulletX + bulletID*DIST_BULLETS,bullets[bulletID].Y = sin((bulletX + bulletID*DIST_BULLETS)/96)*96 + bulletYNegation[bulletID],0); 
		glRotatef(-90,0,1,0);
		setColor(BULLET);
		glutSolidCone(POINTS_OBJECT_SIZE/2, 40, 50, 50);
	glPopMatrix();
	
	return;
}

/**
* Function draw a shield which is a collectable object. When the player takes the shield,
* he becomes invincible. Shield lasts 3.5 seconds and it spawns periodically.
* There's a simple animation which starts when there's a collison between the player and the shield.
**/

void drawShield(){
	glPushMatrix();
		/* If the player has collected the shield */
		if ( player.hasShield == 1 ) {
			
			glTranslatef(player.X, player.Y, 0);
			setColor(SHIELD);
			
			double clip_plane[] = {-1, 0, 0, shieldAnimationParametar};
		    glClipPlane(GL_CLIP_PLANE0, clip_plane);
		    
		    glEnable(GL_CLIP_PLANE0);
			glEnable(GL_CULL_FACE);
		    
		    glCullFace(GL_FRONT);
		    glutSolidSphere(RADIUS + 2.5, 50, 50);

		    glCullFace(GL_BACK);
		    glutSolidSphere(RADIUS + 2.5, 50, 50);

		    glDisable(GL_CULL_FACE);
		    glDisable(GL_CLIP_PLANE0);
			}
		/* If shield exists, but the player hasn't collected it. */
		else if ( shield.exist == 1 && player.hasShield == 0 ) {
			generateCoordinates(SHIELD);
			glTranslatef(shield.X,shield.Y,0);
			setColor(SHIELD);
			glutSolidSphere(RADIUS + 2.5,50,50);
		}
	glPopMatrix();
	return;
}

/**
* Function drawPlayer() draws controlable object also known as "the player" 
**/
void drawPlayer()
{
	glPushMatrix();
		glTranslatef(player.X,player.Y,0);
		setColor(PLAYER);
		glutSolidSphere(RADIUS,50,50);
	glPopMatrix();
	return;

}
/**
* Function drawWalls() draws a three-segment wall, with all segments having a different color. 
* Each time the wall spawns, it's segments change their color. The wall is in shape of a cylinder
* so it can provide some nice 3D effect.
**/
void drawWall()
{
	/*Drawing upper wall.*/
	double wallObjectTranslation = wallObjectTimer;
	glPushMatrix();
		glTranslatef(wallObjectTranslation,0,0);
	   	float u, v;
	    glPushMatrix();
		    setColor(UPPER_WALL);
		    glTranslatef(0,100,0);
		    glScalef(35,200/PI,200/PI*GLOBAL_SCALE_Z);
		    for (u = 0; u < PI; u += PI / 20) {
		        glBegin(GL_TRIANGLE_STRIP);
		        for (v = 0; v <= PI*2 + EPSILON; v += PI / 20) {
		            set_normal_and_vertex(u, v);
		            set_normal_and_vertex(u + PI / 20, v);
		        }
		        glEnd();
		    }

	    glPopMatrix();

		/*Drawing middle wall.*/
	    glPushMatrix();
		    setColor(MIDDLE_WALL);
		    glTranslatef(0,-100,0);
		    glScalef(35,200/PI,200/PI*GLOBAL_SCALE_Z);
		    for (u = 0; u < PI; u += PI / 20) {
		        glBegin(GL_TRIANGLE_STRIP);
		        for (v = 0; v <= PI*2 + EPSILON; v += PI / 20) {
		            set_normal_and_vertex(u, v);
		            set_normal_and_vertex(u + PI / 20, v);
		        }
		        glEnd();
		    }

	    glPopMatrix();

		/*Drawing lower wall.*/
	    glPushMatrix();
		    setColor(LOWER_WALL);
		    glTranslatef(0,-300,0);
		    glScalef(35,200/PI,200/PI*GLOBAL_SCALE_Z);
		    for (u = 0; u < PI; u += PI / 20) {
		        glBegin(GL_TRIANGLE_STRIP);
		        for (v = 0; v <= PI*2 + EPSILON; v += PI / 20) {
		            set_normal_and_vertex(u, v);
		            set_normal_and_vertex(u + PI / 20, v);
		        }
		        glEnd();
		    }

	    glPopMatrix();
    glPopMatrix();

	return;
}

/**
* Function setColor(unsigned objectID) sets color for in-game objects.
**/
void setColor(unsigned objectId){

    GLfloat ambient_coeffs[] = { 0, 0, 0, 1 };
    GLfloat diffuse_coeffs[] = { 0, 0, 0, 1 };
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
    GLfloat shininess = 128;
   
    switch (objectId) {
        case PLAYER: 
        		if ( playerColorTimer == 1 ) { 
        			/* 
					* The idea is to get the color from the spectar of colors, and then apply it to the player.
					* We want to avoid color repeating (for example: blue, blue), that's why we increment color
					* if it's same as the previous color of the player.  
        			*/
        			color = rand() % COLOR_NUMBER;
        			if (player.previousColor == color)		
        					color++;
        				
        			player.previousColor = color % COLOR_NUMBER;
        			diffuse_coeffs[color] = 1.0;
        			playerColorTimer = 0;
        		}
        		else
        			diffuse_coeffs[player.previousColor] = 1.0;
            break;
        /* 
        * We're increasing variable wallColor with different values, so we know for sure
		* that wall will have three different colors all the time.
        */
        case UPPER_WALL: 
        	walls[UPPER_WALL].color = (wallColor+1) % COLOR_NUMBER;
            diffuse_coeffs[walls[UPPER_WALL].color] = 1.0;
            break;
        case MIDDLE_WALL:
        	walls[MIDDLE_WALL].color = (wallColor+2) % COLOR_NUMBER;
            diffuse_coeffs[walls[MIDDLE_WALL].color] = 1.0;
            break;
        case LOWER_WALL:
           	walls[LOWER_WALL].color = wallColor % COLOR_NUMBER;
        	diffuse_coeffs[walls[LOWER_WALL].color] = 1.0;
        	break;
       	case POINT:
       		diffuse_coeffs[0] = 1.0;
       		diffuse_coeffs[1] = 1.0;
       		break;
       	case BULLET:
       		diffuse_coeffs[0] = diffuse_coeffs[1] = 0.5;
       		diffuse_coeffs[2] = 0.8;
       		break;
       	case SHIELD:
       		diffuse_coeffs[2] = 1.0;
       		diffuse_coeffs[0] = 0.5;
       		diffuse_coeffs[1] = 1.0;
       		break;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    return;

}

/**
* Function generates new coordinates of point object and shield sphere.
**/
void generateCoordinates(int ID)
{
	switch(ID) {
		/* We're generating new coordinates for point in two cases: 
		* if the player hasn't collected the point in certein period.
		* if there's a coliison between the player and the point.
		*/
		case POINT : if ( colisionPlayerPoint == 1 || newObjectSignal == 1) {
						float negX = 1;
						float negY = 1;

						if ( rand() % 2 == 0)
							negX = -1;
						if ( rand() % 2 == 0)
							negY = -1;

						point.X = (int)(negX * rand()) % 260;
						point.Y = (int)(negY * rand()) % 260;

						colisionPlayerPoint = 0;
						newObjectSignal = 0;
					}
					break;
		/* We're generating new coordinates for the in predefined time intervals. */
		case SHIELD: if ( newShieldSignal == 1 ) {
						float negX = 1;
						float negY = 1;

						if ( rand() % 2 == 0)
							negX = -1;
						if ( rand() % 2 == 0)
							negY = -1;

						shield.X = (int)(negX * rand()) % 260;
						shield.Y = (int)(negY * rand()) % 260;
						shield.exist = 1;
						colisionPlayerShield = 0;
						newShieldSignal = 0;
					}
					break;
				}
	return;
}


/**
* Function calculates the distance between sphere's center and cube's center.
* Avoids using square root becuause of heavy calculations.
* We're checking if there's a collision. 50^2 is the best choice from my experiance.
**/
void colisionPlayerGoldenPointFunc(){
	double dx = (player.X + player.previousX)/2 - point.X;
	double dy = (player.Y + player.previousY)/2 - point.Y;
	if ( dx*dx+dy*dy < 2000 ) { 
		colisionPlayerPoint = 1; 
		playerColorTimer = 1;
		setColor(PLAYER);
		newObjectTimer = 0;
		score++;	
	}
	return;
}


/**
* Function checks if there's a colision between the player and the wall. Player has to get through the part of the wall which 
* has the same color as the player. There's a slight correction (+5 or -5) to avoid colision which isn't visable to the player's eye.
* but the function detects it.
**/
void colisionPlayerWallFunc(){
	
	if( (player.X + RADIUS > wallObjectTimer - 37 && player.X - RADIUS < wallObjectTimer + 32)){
		if ( (player.Y + RADIUS-5 > 100 ) && player.previousColor != walls[UPPER_WALL].color) {
			livesLeft--;
			player.dead = 1;
		}
		else if ((player.Y - RADIUS+5 < -100 ) && player.previousColor != walls[LOWER_WALL].color) {
			livesLeft--;
			player.dead = 1;
		}
		else if (player.previousColor != walls[MIDDLE_WALL].color && player.Y <= 100 + RADIUS-5 && player.Y >= -100 - RADIUS+5){
			livesLeft--;
			player.dead = 1;
		}
			
	}
}

/**
* Function calculates the distance between sphere's center and bullet's center.
* Avoids using square root becuause of heavy calculations. Also, scales values so it 
* could aproximate colision a little bit better.
*  We're checking if there's a collision. 35^2 is the best choice from my experiance. 
**/
void colisionPlayerBulletFunc(int bulletID){
	double dx = (player.X + player.previousX)/2 - bullets[bulletID].X;
	double dy = (player.Y + player.previousY)/2 - bullets[bulletID].Y;

	if ( dx*dx/2+dy*dy*2 < 1225) {
		player.dead = 1;
		timerActive = 0;
		livesLeft--;
	}
}
/**
* Function calculates the distance between shield's center and player's center.
* If there's a colision, player is shielded and avoids obstacles without any interuption.
* We're checking if there's a collision. Because we have two spheres, we check distance between their centers.
* Again, we're avoiding using sqrt() function from math.h. 
**/

void colisionPlayerShieldFunc(){
	double dx = player.X - shield.X;
	double dy = player.Y - shield.Y;
	if ( dx*dx+dy*dy < DIAMETAR*DIAMETAR && shield.exist == 1) { 
		player.hasShield = 1;
		playerHasShieldTimer = 0;
	}
}

/**
* Function displayScore() displays player's current score and it is shown on the left side
* of the screen.
*/
void displayScore(){

    glPushMatrix();
        glDisable(GL_LIGHTING);
        glColor3f(0,0,0);
       	glRasterPos3f(BOUNDARY_L - 85,-25, 125);
        
        char map[20];
        sprintf(map, "SCORE:    %d", score); 
        int len = strlen(map);

        for(int i = 0; i < len; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, map[i]);
        
        glEnable(GL_LIGHTING);
    glPopMatrix();

}


/**
* Function displayScore() displays player's current number of lives and it is shown on the left side
* of the screen.
*/
void displayLivesLeft(){

    glPushMatrix();
    	
    	glDisable(GL_LIGHTING);
        glColor3f(0,0,0);
        glRasterPos3f(BOUNDARY_L - 85, 0, 125);

        char map[20]; 
        sprintf(map, "LIVES:      %d", livesLeft);
        int len = strlen(map);
        
        for(int i = 0; i < len; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, map[i]);
        
        glEnable(GL_LIGHTING);
    glPopMatrix();

}

/**
* Functions resets game parameters while the user is in-game.
**/
void resetGame(){
	initializeGameData();
	score = 0;
	livesLeft = 3;
	acceleration = 1.0; 
    pointObjectRotationTimer = 0; 
	wallObjectTimer = WALL_SPAWN_LOCATION; 
    colisionPlayerPoint = 0; 
	playerColorTimer = 0; 
    newObjectTimer = 0; 
	newObjectSignal = 0; 
	createShieldTimer = 0; 
    colisionPlayerShield = 0; 
    newShieldSignal = 0; 
	playerHasShieldTimer = 0;
	return;
}

/**
* Function resets the game parameters, except score and number of lives.
**/
void resetLifeLost(){
	initializeGameData();

	 glPushMatrix();
    	
    	glDisable(GL_LIGHTING);
        glColor3f(1.0,1.0,1.0);
        glRasterPos3f(-165, 50, 125);

        char map[256]; 
        sprintf(map, "LIFE LOST! PRESS G TO CONTINUE!");
        int len = strlen(map);
        
        for(int i = 0; i < len; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, map[i]);
        
        glEnable(GL_LIGHTING);
    glPopMatrix();
	return;
}

/**
* Function initializes game data.
**/

void initializeGameData(){
	glutWarpPointer(400,300);
	wallObjectTimer = WALL_SPAWN_LOCATION;
	bulletX = 400; 
	bulletY = 0;
	wallColor = rand() % COLOR_NUMBER;
	for(int i = 0; i < MAX_BULLET_NUMBER; i++) {
    		bulletChooser[i] = rand();
    		bulletYNegation[i] = ((bulletYNegation[i] = rand()) % 2 == 0? bulletYNegation[i] % 100: (bulletYNegation[i] % 100)*(-1));  	
    	}
    shieldAnimationParametar = -(RADIUS + 2.5);
    shield.exist = 0;   
    player.hasShield = 0;
    player.X = -window_width/2;
	player.Y = window_height/2;
	point.X = (int)rand() % 260;
	point.Y = (int)rand() % 260;
	return;

}

/**
* Function displays end message to the player and his score. It also informs him that he can reset game by pressing 'R' or 'r'.
**/

void displayEnd(){
	 glPushMatrix();
    	
    	glDisable(GL_LIGHTING);
        glColor3f(1.0,1.0,1.0);
        glRasterPos3f(-210, 50, 125);

        char map[256]; 
        sprintf(map, "GAME OVER!!! SCORE: %d! PRESS R TO PLAY AGAIN!", score);
        int len = strlen(map);
        
        for(int i = 0; i < len; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, map[i]);
        
        glEnable(GL_LIGHTING);
    glPopMatrix();

}
