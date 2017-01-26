#include <iostream>
#include <random>
#include <fstream>

// returns random int on interval [a,b]
int RandomInt(int a, int b)
{
    std::random_device genDevice;
    std::uniform_int_distribution<int> genDist(a, b);
    return genDist(genDevice);
}

int main() {
	int ymin, ymax, xmin, xmax, num_points = 500;
	xmin = 464;
	ymin = 1512;
	xmax = 487;
	ymax = 1540;

	std::ofstream myfile;
	myfile.open("papyrus_points.txt");

	std::cout << "X,Y" << std::endl;
	myfile << "X,Y" << std::endl;
	
	for (int i = 0; i < num_points; i++)
	{
		int x = RandomInt(xmin, xmax);
		int y = RandomInt(ymin, ymax);

		std::cout << x << "," << y << std::endl;
		myfile << x << "," << y << std::endl;
	}
	
	myfile.close();
	return 0;
}
