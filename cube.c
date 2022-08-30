#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


float a, b, c;	//angles
float x, y, z;	//positions
float ooz;
int xp, yp, idx;

const float distanceFromCam = 100;		//camera offset
const int dimCube = 10;					//dimensions of the cube
const int height = 44;					//window height
const int width = 60;					//window width
const float incrementSpeed = 0.6;		//value to increment angles in loop
const int backgroundASCIICode = ' ';	//background representation
const float K1 = 40;					//value for projection to console


char buffer[height * width];	//Array used to store the the printable elements.
float zBuffer[height * width];	//Array used to store the z-coordinates of the elements at the front and the back of the cube.


float calculateX(int i, int j, int k){
	/*
	Returns the x-coordinate for a general rotation R = Rz(c)*Ry(b)*Rx(a)
	*/
	return 	i * (cos(b) * cos(c)) +
			j * (sin(a) * sin(b) * cos(c) - cos(a) * sin(c)) +
			k * (cos(a) * sin(b) * cos(c) + sin(a) * sin(c));
}

float calculateY(int i, int j, int k){
	/*
	Returns the y-coordinate for a general rotation R = Rz(c)*Ry(b)*Rx(a)
	*/
	return	i * (cos(b) * sin(c)) +
			j * (sin(a) * sin(b) * sin(c) + cos(a) * cos(c)) +
			k * (cos(a) * sin(b) * sin(c) - sin(a) * cos(c));
}

float calculateZ(int i, int j, int k){
	/*
	Returns the z-coordinate for a general rotation R = Rz(c)*Ry(b)*Rx(a)
	*/
	return	i * (-sin(b)) +
			j * sin(a) * cos(b) +
			k * cos(a) * cos(b);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch){
	/*
	Map the respective surface to the buffer for printing
	*/
	
	x = calculateX(cubeX, cubeY, cubeZ);
	y = calculateY(cubeX, cubeY, cubeZ);
	z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;
	
	
	//scale indices due to distanceFromCam
	ooz = 1 / z;
	xp = (int)(width / 2 + x * ooz * K1 * 2);	//horizontal
	yp = (int)(height / 2 + y * ooz * K1);		//vertical
	
	//get respective index of the buffer
	idx = xp + yp * width;
	if(idx >= 0 && idx < width * height){
		if(ooz > zBuffer[idx]){
			zBuffer[idx] = ooz;
			buffer[idx] = ch;
		}
	}
}


/*******************************************************************************
********************************    main    ************************************
*******************************************************************************/


int main(){
	printf("\x1b[2J");
	
	// main loop
	unsigned int iter = 0;
	while(iter < 100){
		// initialize/reset buffers
		memset(buffer, backgroundASCIICode, width * height);
		memset(zBuffer, 0, width * height * 4);
		for(float cubeY = -dimCube; cubeY < dimCube; cubeY += incrementSpeed){
			for(float cubeX = -dimCube; cubeX < dimCube; cubeX += incrementSpeed){
				calculateForSurface(-cubeX, cubeY, dimCube, '#');
				calculateForSurface(cubeX, cubeY, -dimCube, '%');
				calculateForSurface(dimCube, cubeY, cubeX, '$');
				calculateForSurface(-dimCube, cubeY, -cubeX, '&');
				calculateForSurface(cubeX, dimCube, cubeY, ';');
				calculateForSurface(cubeX, -dimCube, -cubeY, '*');
			}
		}
		printf("\x1b[H");

		for(int k = 0; k < width * height; k++){
			putchar(k % width ? buffer[k] : 10);
		}
		
		//increment angles
		a += 0.05;
		b += 0.05;
		c += 0.01;
		
		//sleep
		usleep(16000);
		iter++;
	}
	return 0;
}