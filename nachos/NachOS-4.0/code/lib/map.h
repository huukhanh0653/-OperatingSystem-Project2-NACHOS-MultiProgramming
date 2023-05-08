#ifndef MAP_H
#define MAP_H

/*
    This class is similarly defined to bitmap but simpler
    because I've not understood the bit operation just like: AND, OR, etc.
    Therefore I used this instead.
    Map is used to manage kinds of linear array object.
*/
class Map
{
private:
    bool *_map;
    size_t _size;
    size_t _free;
public:
    Map(size_t size)
    {
        this->_size = size;
        this->_free = size;
        this->_map = new bool[this->_size];
        for (size_t i = 0; i < this->_size;i++)
            this->_map[i] = true; //* Set as FREE status 
    }
    ~Map()
    {
        delete[] this->_map;
    }
    size_t findFree();
    size_t numFree();
    bool isFree(size_t which);
    void clear(size_t which);
    void mark(size_t which);
};

void Map::mark(size_t which)
{
    if (!this->_map[which]) return;
    this->_map[which] = false;
    this->_free--;
}

bool Map::isFree(size_t which)
{
    return (this->_map[which]==true? true:false);
}

size_t Map::findFree()
{
    size_t i;
    for (i = 0; i < this->_size;i++)
        if (this->isFree(i)) 
        {
            this->mark(i);
            return i;
        }
    return -1;
}

void Map::clear(size_t which)
{
    this->_map[which] = true;
}

size_t Map::numFree()
{
    return this->_free;
}

#endif