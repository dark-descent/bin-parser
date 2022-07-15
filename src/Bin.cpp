#include "Bin.hpp"


Bin::TypeInfo::TypeInfo() : TypeInfo(0, 0, nullptr) { }
Bin::TypeInfo::TypeInfo(size_t type, size_t size) : TypeInfo(type, size, nullptr) { }
Bin::TypeInfo::TypeInfo(size_t type, size_t size, char* ptr) : type(type), size(size), ptr(ptr) { }

Bin::Writer::Writer(std::string path) : file_(path) { }

void Bin::Writer::addPrimitiveType(Type type, char* ptr)
{
	header_.push_back(Bin::TypeInfo(type, typeSizes[type], ptr));
}

void Bin::Writer::flushHeader(std::ofstream& os)
{
	size_t size = header_.size() * sizeof(Bin::TypeInfo);
	os.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
	os.write(reinterpret_cast<const char*>(header_.data()), size);
}

void Bin::Writer::flush()
{
	std::ofstream os = std::ofstream(file_, std::ofstream::binary);

	flushHeader(os);

	size_t infoIndex = 0;

	const size_t il = header_.size();

	TypeInfo info = header_[infoIndex];

	while (infoIndex < il)
	{
		info = header_[infoIndex];
		if ((info.type & Type::END) == Type::END)
		{
			infoIndex++;
		}
		else
		{
			switch (info.type)
			{
			case Type::VECTOR:
				infoIndex = flushVector(os, infoIndex + 1, info.size);
				break;
			default:
				os.write(info.ptr, info.size);
				infoIndex++;
				break;
			}
		}
	}
}

size_t Bin::Writer::flushVector(std::ofstream& os, size_t nextIndex, size_t size)
{
	TypeInfo& next = header_.at(nextIndex);

	if (next.type == Type::VECTOR)
	{
		return flushVector(os, nextIndex + 1, next.size);
	}
	os.write(next.ptr, size * next.size);
	return nextIndex + 1;
}

Bin::Reader::Reader(std::string file) : file_(file) { }

Bin::Parser::Parser(std::ifstream& is) : is(is), header()
{
	size_t size = 0;
	is.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	header.resize(size);
	is.read(reinterpret_cast<char*>(header.data()), size);
}

void Bin::Parser::read(char* ptr)
{
	if (infoIndex >= header.size())
		return;

	TypeInfo& info = header.at(infoIndex);

	if ((info.type & Type::END) == Type::END)
	{
		infoIndex++;
		return read(ptr);
	}
	else if (info.type == Type::VECTOR)
	{
		infoIndex = readVector(ptr, infoIndex + 1, info.size);
	}
	else
	{
		is.read(ptr, info.size);
		infoIndex++;
	}
}

size_t Bin::Parser::readVector(char* ptr, size_t nextIndex, size_t size)
{
	TypeInfo& next = header.at(nextIndex);
	
	while((next.type & Type::END) == Type::END)
	{
		nextIndex++;
		if(nextIndex >= header.size())
			return nextIndex;
		next = header.at(nextIndex);
	}

	if (next.type == Type::VECTOR)
	{
		std::vector<std::vector<char>>* vec = reinterpret_cast<std::vector<std::vector<char>>*>(ptr);
		vec->resize(size);
		for (size_t i = 0; i < size; i++)
		{
			vec->at(i) = std::vector<char>();
			nextIndex = readVector(reinterpret_cast<char*>(&vec->at(i)), nextIndex + 1, header.at(nextIndex).size);
		}
		return nextIndex + 1;
	}
	else
	{
		const size_t blockSize = size * next.size;
		std::vector<char>* vec = reinterpret_cast<std::vector<char>*>(ptr);
		vec->resize(blockSize);
		is.read(vec->data(), blockSize);
		return nextIndex + 2;
	}

}