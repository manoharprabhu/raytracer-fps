/*
 * Main.cpp
 *
 *  Created on: Apr 3, 2019
 *      Author: mprabhu
 */

#include<stdio.h>
#include <SFML/Graphics.hpp>
#include "math.h"
#include <vector>
#include<map>
#include <iostream>
const int mapSize = 8;
const int windowWidth = 1000;
const int windowHeight = 1000;
int mapData[mapSize][mapSize];
double playerAngle = 9.50332f;
double playerPositionX = 5.5073;
double playerPositionY = 5.58162f;
const double fov = M_PI / 4.0f;
std::map<long, double> sineMap;
std::map<long, double> cosineMap;

double cosine(double val) {
	return cos(val);
//	std::map<long, double>::iterator it = cosineMap.find(((long) (val * 10000)));
//	// printf("cos %d\n", cosineMap.size());
//	if (it == cosineMap.end()) {
//		double ans = cos(val);
//		cosineMap[((long) (val * 10000))] = ans;
//		return ans;
//	} else {
//		return ((it->second) / 10000.0);
//	}
}

double sine(double val) {
	return sin(val);
//	std::map<long, double>::iterator it = sineMap.find(((long) (val * 10000)));
//	// printf("sin %d\n", sineMap.size());
//	if (it == sineMap.end()) {
//		double ans = sin(val);
//		sineMap[((long) (val * 10000))] = ans;
//		return ans;
//	} else {
//		return ((it->second) / 10000.0);
//	}
}

void printPlayerPosition() {
	printf("Player position: %lf %lf\n", playerPositionX, playerPositionY);
}

void turnPlayerLeft() {
	playerAngle += (M_PI / 80.0f);
}

void movePlayerForwards() {
	double unitX = cosine(playerAngle);
	double unitY = sine(playerAngle);
	playerPositionX = playerPositionX + (unitX * 0.1f);
	playerPositionY = playerPositionY + (unitY * 0.1f);
}

void movePlayerBackwards() {
	double unitX = cosine(playerAngle);
	double unitY = sine(playerAngle);
	playerPositionX = playerPositionX - (unitX * 0.1f);
	playerPositionY = playerPositionY - (unitY * 0.1f);
}

void turnPlayerRight() {
	playerAngle -= (M_PI / 20.0f);
}

void initMap() {
	mapData[4][4] = 1;
	mapData[3][4] = 1;
}

int get_line_intersection(double p0_x, double p0_y, double p1_x, double p1_y,
		double p2_x, double p2_y, double p3_x, double p3_y, double *outX,
		double *outY) {
	double s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;
	s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;
	s2_y = p3_y - p2_y;

	double s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y))
			/ (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x))
			/ (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		// Collision detected
		if (outX != NULL)
			*outX = p0_x + (t * s1_x);
		if (outY != NULL)
			*outY = p0_y + (t * s1_y);
		return 1;
	}

	return 0; // No collision
}

void hit(double testX, double testY, int blockX, int blockY, double rayAngle,
		std::vector<double> *v) {
	double intersectX, intersectY;
	double minDistance = 999;
	// printf("testing for %lf : %lf vs block %d : %d\n", testX, testY, blockX, blockY);
	if (get_line_intersection(playerPositionX, playerPositionY, testX, testY,
			blockX, blockY, blockX + 1, blockY, &intersectX, &intersectY)) {
		double dist = sqrt(
				pow((intersectX - playerPositionX), 2)
						+ (pow(intersectY - playerPositionY, 2)))
				* cosine(abs(playerAngle - rayAngle));
		if (dist < minDistance) {
			minDistance = dist;
		}
	}
	if (get_line_intersection(playerPositionX, playerPositionY, testX, testY,
			blockX, blockY, blockX, blockY + 1, &intersectX, &intersectY)) {
		double dist = sqrt(
				pow((intersectX - playerPositionX), 2)
						+ (pow(intersectY - playerPositionY, 2)))
				* cosine(abs(playerAngle - rayAngle));
		if (dist < minDistance) {
			minDistance = dist;
		}
	}
	if (get_line_intersection(playerPositionX, playerPositionY, testX, testY,
			blockX, blockY + 1, blockX + 1, blockY + 1, &intersectX,
			&intersectY)) {
		double dist = sqrt(
				pow((intersectX - playerPositionX), 2)
						+ (pow(intersectY - playerPositionY, 2)))
				* cosine(abs(playerAngle - rayAngle));
		if (dist < minDistance) {
			minDistance = dist;
		}
	}
	if (get_line_intersection(playerPositionX, playerPositionY, testX, testY,
			blockX + 1, blockY, blockX + 1, blockY + 1, &intersectX,
			&intersectY)) {
		double dist = sqrt(
				pow((intersectX - playerPositionX), 2)
						+ (pow(intersectY - playerPositionY, 2)))
				* cosine(abs(playerAngle - rayAngle));
		if (dist < minDistance) {
			minDistance = dist;
		}
	}
	v->push_back(minDistance);
}

void compute(std::vector<double> *traceData) {
	traceData->clear();
	for (int i = 0; i < windowWidth; i++) {
		const double rayAngle = (playerAngle - (fov / 2))
				+ ((double) i / (double) windowWidth) * fov;
		double testDistance = 0.1;
		// TODO: Replace this while loop with DDA
		while (true) {
			const double unitX = cosine(rayAngle);
			const double unitY = sine(rayAngle);

			const double testX = playerPositionX + (unitX * testDistance);
			const double testY = playerPositionY + (unitY * testDistance);

			const int blockX = (int) testX;
			const int blockY = (int) testY;

			if (blockX >= mapSize - 1 || blockY >= mapSize - 1 || blockX <= 0
					|| blockY <= 0) {
				// Hit the map boundaries. Draw boundary walls here.
				hit(testX, testY, blockX, blockY, rayAngle, traceData);
				break;
			}

			if (mapData[blockX][blockY] != 0) {
				// Got map block
				hit(testX, testY, blockX, blockY, rayAngle, traceData);
				break;
			}
			// printf("block %d %d\n", blockX, blockY);
			testDistance += 0.1;
		}
	}
}

void printPlayerDetails() {
	std::cout << "player position: " << playerPositionX << " : " << playerPositionY << ", player angle: " << playerAngle << std::endl;
}

int main(int arc, char **argv) {
	initMap();
	std::vector<double> traceData;
	compute(&traceData);
//	for (std::vector<double>::iterator x = traceData.begin();
//			x != traceData.end(); x++) {
//		printf("%lf , ", *x);
//	}
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "RT");
	window.setKeyRepeatEnabled(true);
	while (window.isOpen()) {
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				movePlayerForwards();
				printPlayerDetails();
				compute(&traceData);
			} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				movePlayerBackwards();
				printPlayerDetails();
				compute(&traceData);
			} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				turnPlayerRight();
				printPlayerDetails();
				compute(&traceData);
			} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				turnPlayerLeft();
				printPlayerDetails();
				compute(&traceData);
			}

			window.clear(sf::Color::Black);

			for (int loop = 0; loop < windowWidth; loop++) {
				const double columnHeight = windowHeight / traceData[loop];
				sf::RectangleShape rectangle(sf::Vector2f(1, columnHeight));

				double multiplier = 1.0;
				if (traceData[loop] < 2)
					multiplier = 1.0;
				else if (traceData[loop] < 2.2)
					multiplier = 0.8;
				else if (traceData[loop] < 2.4)
					multiplier = 0.4;
				else if (traceData[loop] < 2.8)
					multiplier = 0.2;
				else
					multiplier = 0.1;

				rectangle.setFillColor(sf::Color(0, 0, 255 * multiplier));
				rectangle.setPosition(loop,
						(windowHeight / 2) - (columnHeight / 2));
				sf::RectangleShape floor(
						sf::Vector2f(1,
								(windowHeight / 2) + (columnHeight / 2)));
				floor.setFillColor(sf::Color(200, 200, 200));
				floor.setPosition(loop,
						(windowHeight / 2) + (columnHeight / 2));
				window.draw(rectangle);
				window.draw(floor);
			}
			window.display();
		}
	}
	printf("Finished program");
}
