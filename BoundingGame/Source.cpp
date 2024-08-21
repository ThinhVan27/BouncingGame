#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <cmath>
#include <random>
#include <vector>
using namespace std;

struct Ball;

const sf::Color Orange(184, 134, 11);
mt19937_64 rd(chrono::steady_clock::now().time_since_epoch().count());

const unsigned int FPS = 60;
const unsigned int w = 900;
const unsigned int h = 900;

static long long Rand(long long l, long long r) {
	return l + rd() % (r - l + 1);
}
sf::RenderWindow window(sf::VideoMode(w, h), "BouncingGame");
//videomode(width, height)

float start_arc = 330.f*std::_Pi_val/180.f;
float end_arc = 30.f * std::_Pi_val / 180.f;
const float triangle_edge = 260.f / (std::cos(30.f * std::_Pi_val / 180.f));
const sf::Vector2f g(0,0.1);

float spinning_step = 0.01;
static sf::Color randColor() {
	return sf::Color(Rand(0, 255), Rand(0, 255), Rand(0, 255));
}

sf::ConvexShape triangle;
void drawTriangle() {
	triangle.setPointCount(3);
	triangle.setFillColor(sf::Color::Black);
	triangle.setPoint(0, sf::Vector2f(450, 450));
	triangle.setPoint(1, sf::Vector2f(450 + triangle_edge * std::cos(start_arc), 450 - triangle_edge * std::sin(start_arc)));
	triangle.setPoint(2, sf::Vector2f(450 + triangle_edge * std::cos(end_arc), 450 - triangle_edge * std::sin(end_arc)));

	start_arc += spinning_step;
	end_arc += spinning_step;

	if (start_arc >= 2 * std::_Pi_val) start_arc -= 2 * std::_Pi_val;
	if (end_arc >= 2 * std::_Pi_val) end_arc -= 2 * std::_Pi_val;

	window.draw(triangle);
}

sf::CircleShape c;
void drawCircle() {
	c.setPosition(sf::Vector2f(200, 200));
	c.setRadius(250);
	c.setFillColor(sf::Color::Transparent);
	c.setOutlineColor(Orange);
	c.setOutlineThickness(10);

	window.draw(c);


}

float dotProduct(sf::Vector2f v1, sf::Vector2f v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

bool isCollision(sf::Vector2f pos) {
	return sqrt((pos.x-450)*(pos.x-450) + (pos.y-450)*(pos.y-450)) >=243;
}
vector<Ball> balls;

struct Ball {
	sf::CircleShape b;
	sf::Vector2f veloc;
	bool isInside;

	Ball() {
		veloc = sf::Vector2f(Rand(-10,10), Rand(0,5));
		b.setPosition(sf::Vector2f(450, 260));
		b.setRadius(7);
		b.setFillColor(randColor());
		isInside = true;

	}

	sf::Vector2f centerPos() const {
		return b.getPosition() + sf::Vector2f(7, 7);
	}

};

bool visible(sf::Vector2f pos) {
	float X = pos.x;
	float Y = pos.y;
	return X >= -10 && X <= 1000 && Y >= -10 && Y <= 1000;
}

int add = 0;
static void draw_and_update() {
	int sz = balls.size();
	add = 0;
	for (int i = 0; i < sz;i++) {
		Ball& ball = balls[i];
		sf::Vector2f center(ball.centerPos());
		if (!visible(center)) continue;
		//cout << center.x << " " << center.y << "\n";
		if (isCollision(center) && ball.isInside) {
			sf::Vector2f unit_normal(center.x - 450, center.y - 450);
			sf::Vector2f unit_tangent(center.y - 450, -center.x + 450);
			float len = std::sqrt(unit_tangent.x * unit_tangent.x + unit_tangent.y * unit_tangent.y);
			unit_normal /= len;
			unit_tangent /= len;

			float alpha = std::acos(dotProduct(unit_normal, sf::Vector2f(1, 0)));
			if (unit_normal.y > 0) alpha = (2 * std::_Pi_val - alpha);
			if (alpha >= 2 * std::_Pi_val) alpha -= 2 * std::_Pi_val;

			if (ball.isInside && !(alpha >= start_arc && alpha <= end_arc) && !(start_arc > end_arc && (alpha <= end_arc || alpha >= start_arc))) {
				ball.veloc = -1 * dotProduct(ball.veloc, unit_normal) * unit_normal + dotProduct(ball.veloc, unit_tangent) * unit_tangent;
				float move = len - 250 + 7;
				ball.b.setPosition((center - move * unit_normal) - sf::Vector2f(7, 7));
				ball.veloc += (7 * spinning_step) * unit_tangent;

			}
			else {

				//cout << i << "\n";
				if (ball.isInside) {
					add += Rand(1,2);
					//balls.push_back(Ball());
					//balls.push_back(Ball());
				}
				float X = ball.b.getPosition().x;
				float Y = ball.b.getPosition().y;

				if (visible (center)) {

					ball.isInside = false;
					//ball.veloc += (sf::Vector2f(center.x - 450, center.y - 450)) / 3000.f;
					//ball.veloc += sf::Vector2f(center.y - 450, -center.x + 450) / 2000.f;
					//cout << "v" << balls.size();
				}
			}
		}
		window.draw(ball.b);
		if (visible(center)) {
			ball.b.setPosition(ball.b.getPosition() + ball.veloc);
			ball.veloc += g;
		}
	}
}

int main() {
	window.setFramerateLimit(FPS);

	Ball b;
	balls.push_back(b);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		drawCircle();
		drawTriangle();
		draw_and_update();

		while (add--) balls.push_back(Ball());
		window.display();
	}
}
