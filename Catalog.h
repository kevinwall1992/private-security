#ifndef EBR_CATALOG
#define EBR_CATALOG

#include "Math.h"
#include "Container.h"
#include "Named.h"

#include <mutex>
#include <atomic>

template<class T, class NameType>
class Catalog;

template<class T, class NameType>
class CatalogIterator : public Iterator<T *>, public Where<T, Named<NameType>>
{
	Catalog<T, NameType> *catalog;

	int block_index;
	int element_index;
	int front_index;

public:
	CatalogIterator(Catalog<T, NameType> *catalog)
	{
		this->catalog= catalog;
		
		Reset();
	}

	void Reset()
	{
		block_index= 0;
		element_index= -1;
		front_index= catalog->GetBlockFrontIndex(block_index);
	}

	T * GetNext();
};

class Team;

template<class T, class NameType>
class Catalog : public Container<T *>
{
public:
	Iterator<T *> * GetIterator()
	{
		return new CatalogIterator<T, NameType>(this);
	}
private:

	const int first_block_size= 10;
	const int max_block_count= 100;

	T ***blocks;
	int block_count;
	int element_count;

	std::mutex mutex;

	int GetBlockSize(int block_index)
	{
		if(block_index== 0)
			return first_block_size;

		return first_block_size* Math::Pow(2, block_index);
	}

	int GetBlockIndex(int global_index)
	{
		global_index/= first_block_size;

		int count= 2;
		int block_index= 0;
		while(global_index>= (count- 1))
		{
			count*= 2;
			block_index++;
		}

		return block_index;
	}

	int GetCumulativeElementCount(int block_count)
	{
		if(block_count== 0)
			return 0;

		return first_block_size* (Math::Pow(2, block_count)- 1);
	}

	int GetElementIndex(int global_index)
	{
		if(global_index< first_block_size)
			return global_index;

		return global_index- GetCumulativeElementCount(GetBlockIndex(global_index));
	}

	int GetBlockFrontIndex(int block_index)
	{
		return (block_index == (block_count- 1)) ?  element_count- GetCumulativeElementCount(block_index) : GetBlockSize(block_index);
	}

	T * FindElement(NameType &name)
	{
		for(int block_index= 0; block_index< block_count; block_index++)
		{
			int block_front_index= GetBlockFrontIndex(block_index);

			for(int element_index= 0; element_index< block_front_index; element_index++)
			{
				T *element= blocks[block_index][element_index];

				if(element->IsNamed(name))
					return element;
			}
		}

		return nullptr;
	}

	//Not thread safe, should be called from within critical section
	T * AppendElement(T *element)
	{
		int block_index= GetBlockIndex(element_count);
		int element_index= GetElementIndex(element_count);
		
		if(block_index>= block_count)
		{
			if(block_index>= max_block_count)
			{
				assert(false && "I don't know how you did it, but you ran out of blocks." 
					             "Either something weird happened or human technology has "
								 "surpassed my wildest dreams. Hello Future!");
				return nullptr;
			}


			blocks[block_index]= new T*[GetBlockSize(block_index)];
			block_count++;
		}

		blocks[block_index][element_index]= element;
		element_count++;

		return element;
	}

	T * CreateElement(NameType &name)
	{
		mutex.lock();

		T *element= FindElement(name);

		if(element== nullptr)
			element= AppendElement(new T(name));

		mutex.unlock();

		return element;
	}

public:
	Catalog()
	{
		blocks= new T**[max_block_count];
		blocks[0]= new T*[first_block_size];

		block_count= 1;
		element_count= 0;
	}

	~Catalog()
	{
		Iterator<T *> *iterator= GetIterator();

		T *element;
		while((element= iterator->GetNext())!= nullptr)
			delete element;

		delete iterator;

		delete blocks;
	}

	T * GetElement(NameType &name)
	{
		T *element= FindElement(name);
		if(element== nullptr)
			element= CreateElement(name);

		return element;
	}

	friend class CatalogIterator<T, NameType>;
};

template<class T, class NameType>
T * CatalogIterator<T, NameType>::GetNext()
{
	if(++element_index>= front_index)
	{
		element_index= 0;
			
		if(++block_index>= catalog->block_count)
			return nullptr;

		front_index= catalog->GetBlockFrontIndex(block_index);
	}

	return catalog->blocks[block_index][element_index];
}

#include "Threading.h"

#endif