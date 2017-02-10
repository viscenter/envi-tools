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

// argv == et_PointGenerator ymin ymax xmin xmax
int main(int argc, char** argv) {
	if (argc < 5) {
		std::cout << " Usage: " << argv[0];
		std::cout << " ymin ymax xmin xmax" << std::endl;
		return EXIT_FAILURE;
	}
	
	int num_points = 500;
	int xmin = [3];
	int ymin = [1];
	int xmax = [4];
	int ymax = argv[2];

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
