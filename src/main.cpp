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

		std::vector<std::string> strings;

		strings.emplace_back("helloa A :D");
		strings.emplace_back("helloa B :D");
		strings.emplace_back("helloa C :D");

		writer.write(strings);

		writer.flush();
	}


	{
		Bin::Reader reader(p.string());

		reader.read([](Bin::Parser& parser)
		{
			std::vector<std::string> test;
			parser.read(test);
			for (const auto& str : test)
				printf("%s\n", str.c_str());
		});
	}

	return EXIT_SUCCESS;
}