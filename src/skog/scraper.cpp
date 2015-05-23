#include <skog/scraper.hpp>

#include <iostream>
#include <deque>
#include <fstream>
#include <boost/locale.hpp>

#include <supermarx/util/stubborn.hpp>

#include <skog/parsers/product_parser.hpp>
#include <skog/parsers/category_parser.hpp>
#include <skog/parsers/category_id_parser.hpp>

namespace supermarx
{
	scraper::scraper(callback_t _callback, size_t _ratelimit, bool _cache)
	: callback(_callback)
	, dl("supermarx skog/1.0", _ratelimit, _cache ? boost::optional<std::string>("./cache") : boost::none)
	{}

	void scraper::scrape()
	{
		category_id_parser cidp([&](category_id_parser::category_id_t const& _cid)
		{
			std::cerr << _cid << std::endl;
		});

		category_parser cp([&](category_parser::category_uri_t const& _curi)
		{
			std::cerr << _curi << std::endl;
			cidp.parse(dl.fetch(_curi));
		});

		cp.parse(dl.fetch("http://derks.spar.nl"));
	}

	raw scraper::download_image(const std::string& uri)
	{
		std::string buf(dl.fetch(uri));
		return raw(buf.data(), buf.length());
	}
}
