#include "framework.hpp"

#include "Bin.hpp"

#include <chrono>
#include <filesystem>

struct Mark
{
	const char* name;
	std::chrono::steady_clock::time_point time;

	Mark(const char* name) : time(std::chrono::high_resolution_clock::now()), name(name) { }
	Mark() : Mark("") { }

	static void printElapsedTime(Mark& start, Mark& end)
	{
		std::chrono::duration<double, std::milli> ms_double = end.time - start.time;
		std::cout << ms_double.count() << "ms\n";
	}
};

int main()
{
	std::filesystem::path p = std::filesystem::current_path() / "resources" / "test.bin";
	
	{
		Bin::Writer writer(p.string());

		std::vector<uint16_t> a;
		std::vector<uint16_t> b;
		std::vector<std::vector<uint16_t>> v;

		size_t test = 1000000;

		for (size_t i = 0; i < test / 4; i++)
			a.push_back(i);

		for (size_t i = 0; i < test / 4; i++)
			b.push_back(i);

		v.push_back(a);
		v.push_back(b);

		writer.write(a);
		writer.write(b);
		writer.write(v);

		Mark start;
		writer.flush();
		Mark end;

		Mark::printElapsedTime(start, end);
	}

	{
		Bin::Reader reader(p.string());

		std::vector<std::vector<int>> vec;
		uint8_t test = 0;

		reader.read([](Bin::Parser& parser)
		{
			std::vector<uint16_t> aa = std::vector<uint16_t>();
			std::vector<uint16_t> bb = std::vector<uint16_t>();
			std::vector<std::vector<uint16_t>> vv = std::vector<std::vector<uint16_t>>();

			Mark start;
			parser.read(&aa);
			parser.read(&bb);
			parser.read(&vv);
			Mark end;

			Mark::printElapsedTime(start, end);

			printf("vec a size: %zu\n", aa.size());
			printf("vec b size: %zu\n", bb.size());
			printf("vec c size: %zu\n", vv.size());

			// size_t i = 0;
			// for (const auto& vec : v)
			// {
			// 	printf("\n%zu: \n", i++);
			// 	for (const auto& n : vec)
			// 	{
			// 		printf("%u \t", n);
			// 	}
			// }
		});
	}

	return EXIT_SUCCESS;
}