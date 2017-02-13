#include "Space.h"

Space * Space::space= new Space();

Vec3i Space::GetTilePosition(Tile *tile)
{
	for(unsigned int i= 0; i< 10; i++)
		for(unsigned int j= 0; j< 10; j++)
			for(unsigned int k= 0; k< 1; k++)
				if(&tiles[i][j][k]== tile)
					return Vec3i(i, j, k);

	return Vec3i(-1, -1, -1);
}

Vec3i Space::GetObjectPosition(Object *object)
{
	for(unsigned int i= 0; i< 10; i++)
		for(unsigned int j= 0; j< 10; j++)
			for(unsigned int k= 0; k< 1; k++)
				if(tiles[i][j][k].Contains(object))
					return Vec3i(i, j, k);

	return Vec3i(-1, -1, -1);
}

Space::Space()
{

}

vector<Perceptible*> Space::GetPerceptibles()
{
	vector<Perceptible *> perceptibles;

	for(unsigned int i= 0; i< 10; i++)
		for(unsigned int j= 0; j< 10; j++)
			perceptibles.push_back(&(tiles[i][j][0]));

	return perceptibles;
}

/*void Space::Draw()
{
	for(unsigned int i= 0; i< 10; i++)
		for(unsigned int j= 0; j< 10; j++)
			tiles[i][j][0].Draw();
}*/


