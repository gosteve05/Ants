//above 3 lines are for debugging
//Steven Oslan 01396237
//Resubmission: commented out every instance of "delete" except for the deconstructor.
//Added functionality: Prints the number of turns and the number of ants and doodlebugs.
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;
const int X_SIZE = 20;
const int Y_SIZE = 20;
const float SQUARE_PIXELS = 20.0;
const int ANT_BREEDTIME = 3;
const int DOODLEBUG_BREEDTIME = 8;
const int DOODLEBUG_STARVETIME = 3;
const int POINT_COUNT = 30;
const int NUM_SWAPS = 10000;
const int NUM_RED_BLOBS = 5; //should be 5
const int NUM_GREEN_BLOBS = 100; //should be 100
class Index_out_of_bounds {};
enum Tile_type { EMPTY, GREEN_BLOB, RED_BLOB, NUMBER_OF_TILES };
enum Direction { UP, DOWN, LEFT, RIGHT, NUMBER_OF_DIRECTIONS };
class World;
class Tile
{
public:
	Tile() : x(0), y(0), shape() {  }
	Tile(int x = 0, int y = 0, float radius = SQUARE_PIXELS, size_t point_count = POINT_COUNT) : x(x), y(y), shape(radius, point_count) { }
	virtual void display(sf::RenderWindow& window) = 0;
	static void tile_swap(Tile*& pTile1, Tile*& pTile2);
	virtual Tile_type who() = 0;
	virtual void turn(World& w) { cout << "Somehow called Tile turn"; };
	friend vector<Direction> findAdjacent(const World& w, const Tile& tile, Tile_type type);
	//void replace(World& w, const Tile& tile, Direction d);
protected:
	int x;
	int y;
	sf::CircleShape shape;
	void fix_shape_position();
};
class Blob : public Tile
{
public:
	Blob(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, POINT_COUNT)
	{
		shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
	}
	void display(sf::RenderWindow& window) = 0;
	virtual void move(World& w);
	virtual void breed(World& w);
protected:
	int breedCountdown;
};
class Green_blob : public Blob
{
public:
	Green_blob(int x = 0, int y = 0) : Blob(x, y)
	{
		shape.setFillColor(sf::Color::Green);
	}
	void display(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual Tile_type who() { return GREEN_BLOB; }
	virtual void turn(World& w);
	virtual void move(World& w);
	virtual void breed(World& w);
private:
	int breedCountdown = ANT_BREEDTIME;
};
class Red_blob : public Blob
{
public:
	Red_blob(int x = 0, int y = 0) : Blob(x, y)
	{
		shape.setFillColor(sf::Color::Red);
	}
	void display(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual Tile_type who() { return RED_BLOB; }
	virtual void turn(World& w);
	virtual void move(World& w);
	virtual void breed(World& w);
private:
	int breedCountdown = DOODLEBUG_BREEDTIME;
	int starveCountdown = DOODLEBUG_STARVETIME;
};
class Empty : public Tile
{
public:
	//Empty() : Tile(0, 0) { cout << "Calling default Empty constructor." << endl; }
	Empty(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 4)
	{
		shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256, 255));
		//tell the shape where it lives START HERE
		shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
	}
	void display(sf::RenderWindow& window)
	{
		//window.draw(shape);
	}
	virtual Tile_type who() { return EMPTY; }
};
class World
{
public:
	World(int x = X_SIZE, int y = Y_SIZE, int green_blobs = NUM_GREEN_BLOBS, int red_blobs = NUM_RED_BLOBS);
	~World();
	//write the big 5
	void display(sf::RenderWindow& window);
	void run_simulation();
	void turn();
	vector<Tile*>& operator[](int index);
	const vector<Tile*>& operator[](int index) const;
	void setAnts(int a) { ants = a; };
	void setDoodlebugs(int d) { doodlebugs = d; };
	int getAnts() { return ants; };
	int getDoodlebugs() { return doodlebugs; };
private:
	vector<vector<Tile*>> grid;
	int ants;
	int doodlebugs;
};
int main()
{
	srand(time(0));
	World world;
	world.run_simulation();
}

World::World(int x, int y, int green_blobs, int red_blobs)
{
	vector<Tile*> column(y);
	for (int i = 0; i < x; i++)
		grid.push_back(column);
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
		{
			if (green_blobs > 0)
			{
				grid[i][j] = new Green_blob(i, j);
				green_blobs--;
			}
			else if (red_blobs > 0)
			{
				grid[i][j] = new Red_blob(i, j);
				red_blobs--;
			}
			else
				grid[i][j] = new Empty(i, j);
		}
	int x1, x2;
	int y1, y2;
	for (int i = 0; i < NUM_SWAPS; i++)
	{
		x1 = rand() % x;
		x2 = rand() % x;
		y1 = rand() % y;
		y2 = rand() % y;
		Tile::tile_swap(grid[x1][y1], grid[x2][y2]);
	}
}
World::~World()
{
	for (int i = 0; i < grid.size(); i++)
		for (int j = 0; j < grid[i].size(); j++)
			delete grid[i][j];
}
void World::display(sf::RenderWindow& window)
{
	for (int i = 0; i < grid.size(); i++)
		for (int j = 0; j < grid[i].size(); j++)
			grid[i][j]->display(window);
}

void World::turn()
{
	vector<Tile*> red_blobs;
	for (int i = 0; i < grid.size(); i++)
		for (int j = 0; j < grid[i].size(); j++)
			if (grid[i][j]->who() == RED_BLOB)
				red_blobs.push_back(grid[i][j]);
	for (auto red_blob : red_blobs)
		red_blob->turn(*this);

	vector<Tile*> green_blobs;
	for (int i = 0; i < grid.size(); i++)
		for (int j = 0; j < grid[i].size(); j++)
			if (grid[i][j]->who() == GREEN_BLOB)
				green_blobs.push_back(grid[i][j]);
	for (auto green_blob : green_blobs)
		green_blob->turn(*this);
	//garbage below


}
void World::run_simulation()
{
	int turnNum = 0;
	cout << "Press Enter to go forward one time step. Current turn: " << turnNum << ". Ants remaining: " << NUM_GREEN_BLOBS << ". Doodlebugs remaining: " << NUM_RED_BLOBS << endl;
	sf::RenderWindow window(sf::VideoMode(400, 400), "World of blobs!");
	bool keyHeld = true;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			//adding garbage
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
			{
				exit(1);
			}
			if (keyHeld)
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
					keyHeld = false;
			if (!keyHeld)
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				{
					keyHeld = true;
					turn();
					turnNum++;
					vector<Tile*> red_blobs1;
					for (int i = 0; i < grid.size(); i++)
						for (int j = 0; j < grid[i].size(); j++)
							if (grid[i][j]->who() == RED_BLOB)
								red_blobs1.push_back(grid[i][j]);
					doodlebugs = red_blobs1.size();
					vector<Tile*> green_blobs1;
					for (int i = 0; i < grid.size(); i++)
						for (int j = 0; j < grid[i].size(); j++)
							if (grid[i][j]->who() == GREEN_BLOB)
								green_blobs1.push_back(grid[i][j]);
					ants = green_blobs1.size();
					cout << "Press Enter to go forward one time step. Current turn: " << turnNum << ". Ants remaining: " << ants << ". Doodlebugs remaining: " << doodlebugs << endl;
				}
		}
		window.clear();
		display(window);
		//shape.move(sf::Vector2f(rand()%3 -1, rand()%3 - 1));
		//shape.rotate(1);
		//window.draw(shape);		
		window.display();
	}
}
void Tile::tile_swap(Tile*& pTile1, Tile*& pTile2)
{
	swap(pTile1->x, pTile2->x);
	swap(pTile1->y, pTile2->y);
	swap(pTile1, pTile2);
	pTile1->fix_shape_position();
	pTile2->fix_shape_position();
}
void Tile::fix_shape_position()
{
	shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
}
void Green_blob::move(World& w)
{
	Blob::move(w);
	breed(w);
}

/*
void Tile::replace(World& w, const Tile& tile, Direction d) //UP DOWN LEFT RIGHT
{ //0: dx=0,dy=-1 1: dx=0,dy=1 2:dy=0,dx=-1 3:dy=0,dx=1

	int x1 = x;
	int y1 = y;
	int dy = d>1 ? 0 : 2*d-1;
	int dx = d>1 ? 2*d-5 : 0;
	Tile::tile_swap(w[x1][y1],w[x1+dx][y1+dy]);
	x1+=dx;
	y1+=dy;
	delete(w[x1][y1]);
	w[x1][y1] = new Empty(x1, y1);
}
*/
void Red_blob::move(World& w)
{
	int x1 = x;
	int y1 = y;
	if (starveCountdown <= 0)
	{
		delete(w[x1][y1]); //Charlie
		w[x1][y1] = new Empty(x1, y1);
		//x = x1;
		//y = y1;
		return;
	}
	breedCountdown--;
	vector<Direction> edibleDirections = findAdjacent(w, *this, GREEN_BLOB);
	if (edibleDirections.size() > 0)
	{
		starveCountdown = DOODLEBUG_STARVETIME;
		Direction i = edibleDirections[rand() % edibleDirections.size()];
		switch (i)
		{
		case UP:
			Tile::tile_swap(w[x1][y1], w[x1][y1 - 1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		case DOWN:
			Tile::tile_swap(w[x1][y1], w[x1][y1 + 1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		case LEFT:
			Tile::tile_swap(w[x1][y1], w[x1 - 1][y1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		case RIGHT:
			Tile::tile_swap(w[x1][y1], w[x1 + 1][y1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		}
	}
	else
	{
		starveCountdown--;
		Blob::move(w);
	}
}
void Red_blob::turn(World& w)
{
	//adding stuff
	int x1 = x;
	int y1 = y;
	if (starveCountdown <= 0)
	{
		delete(w[x1][y1]); //test
		w[x1][y1] = new Empty(x1, y1);
		//x = x1;
		//y = y1;
		return;
	}
	breedCountdown--;
	vector<Direction> edibleDirections = findAdjacent(w, *this, GREEN_BLOB);
	if (edibleDirections.size() > 0)
	{
		starveCountdown = DOODLEBUG_STARVETIME;
		Direction i = edibleDirections[rand() % edibleDirections.size()];
		switch (i)
		{
		case UP:
			Tile::tile_swap(w[x1][y1], w[x1][y1 - 1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		case DOWN:
			Tile::tile_swap(w[x1][y1], w[x1][y1 + 1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		case LEFT:
			Tile::tile_swap(w[x1][y1], w[x1 - 1][y1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		case RIGHT:
			Tile::tile_swap(w[x1][y1], w[x1 + 1][y1]);
			delete(w[x1][y1]);
			w[x1][y1] = new Empty(x1, y1);
			break;
		}
	}
	else
	{
		starveCountdown--;
		Blob::move(w);
	}
	//added stuff

	//Red_blob::move(w);
	if (breedCountdown <= 0)
	{
		breedCountdown = DOODLEBUG_BREEDTIME;
		vector<Direction> emptyAdjacent = findAdjacent(w, *this, EMPTY);
		if (emptyAdjacent.size() <= 0)
			return;
		switch (emptyAdjacent[rand() % emptyAdjacent.size()])
		{
		case UP:
			w[x][y - 1] = new Red_blob(x, y - 1);
			break;
		case DOWN:
			w[x][y + 1] = new Red_blob(x, y + 1);
			break;
		case LEFT:
			w[x - 1][y] = new Red_blob(x - 1, y);
			break;
		case RIGHT:
			w[x + 1][y] = new Red_blob(x + 1, y);
			break;
		}
	}
}
void Blob::move(World& w)
{
	int i = rand() % NUMBER_OF_DIRECTIONS;
	switch (i)
	{
	case UP:
		if (y > 0)
			if (w[x][y - 1]->who() == EMPTY)
				Tile::tile_swap(w[x][y], w[x][y - 1]);
		break;
	case DOWN:
		if (y < Y_SIZE - 1)
			if (w[x][y + 1]->who() == EMPTY)
				Tile::tile_swap(w[x][y], w[x][y + 1]);
		break;
	case LEFT:
		if (x > 0)
			if (w[x - 1][y]->who() == EMPTY)
				Tile::tile_swap(w[x][y], w[x - 1][y]);
		break;
	case RIGHT:
		if (x < X_SIZE - 1)
			if (w[x + 1][y]->who() == EMPTY)
				Tile::tile_swap(w[x][y], w[x + 1][y]);
		break;
	}
}
void Blob::breed(World& w) {}
void Green_blob::breed(World& w)
{
	breedCountdown--;//decrement breedCountdown.
	if (breedCountdown <= 0)
	{
		breedCountdown = ANT_BREEDTIME;
		vector<Direction> emptyAdjacent = findAdjacent(w, *this, EMPTY);
		if (emptyAdjacent.size() <= 0)
			return;
		switch (emptyAdjacent[rand() % emptyAdjacent.size()])
		{
		case UP:
			w[x][y - 1] = new Green_blob(x, y - 1);
			break;
		case DOWN:
			w[x][y + 1] = new Green_blob(x, y + 1);
			break;
		case LEFT:
			w[x - 1][y] = new Green_blob(x - 1, y);
			break;
		case RIGHT:
			w[x + 1][y] = new Green_blob(x + 1, y);
			break;
		}
	}
}
void Red_blob::breed(World& w)
{
	breedCountdown--;
	if (breedCountdown <= 0)
	{
		breedCountdown = DOODLEBUG_BREEDTIME;
		vector<Direction> emptyAdjacent = findAdjacent(w, *this, EMPTY);
		if (emptyAdjacent.size() <= 0)
			return;
		switch (emptyAdjacent[rand() % emptyAdjacent.size()])
		{
		case UP:
			w[x][y - 1] = new Red_blob(x, y - 1);
			break;
		case DOWN:
			w[x][y + 1] = new Red_blob(x, y + 1);
			break;
		case LEFT:
			w[x - 1][y] = new Red_blob(x - 1, y);
			break;
		case RIGHT:
			w[x + 1][y] = new Red_blob(x + 1, y);
			break;
		}
	}
}
void Green_blob::turn(World& w)
{
	move(w);
}
vector<Tile*>& World::operator[](int index)
{
	if (index >= grid.size())
		throw Index_out_of_bounds();
	return grid[index];
}
const vector<Tile*>& World::operator[](int index) const
{
	if (index >= grid.size())
	{
		cout << "Crashing because index is: " << index << endl;
		throw Index_out_of_bounds();
	}
	return grid[index];
}
//findAdjacent is a friend function of Tile that returns a vector of Directions based on the location and the type we're looking for.
vector<Direction> findAdjacent(const World& w, const Tile& tile, Tile_type type)
{
	int x = tile.x;
	int y = tile.y;
	vector<Direction> adjacents;
	if (x > 0)
		if (w[x - 1][y]->who() == type)
			adjacents.push_back(LEFT);
	if (x < X_SIZE - 1)
		if (w[x + 1][y]->who() == type)
			adjacents.push_back(RIGHT);
	if (y > 0)
		if (w[x][y - 1]->who() == type)
			adjacents.push_back(UP);
	if (y < Y_SIZE - 1)
		if (w[x][y + 1]->who() == type)
			adjacents.push_back(DOWN);
	return adjacents;
}