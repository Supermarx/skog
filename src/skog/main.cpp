#include <supermarx/scraper/scraper_cli.hpp>
#include <skog/scraper.hpp>

int main(int argc, char** argv)
{
	return supermarx::scraper_cli<supermarx::scraper>::exec(4, "skog", "Spar", argc, argv);
}
