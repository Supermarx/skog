#include <skog/scraper.hpp>

#include <iostream>
#include <deque>
#include <fstream>

#include <boost/locale.hpp>
#include <jsoncpp/json/json.h>

#include <supermarx/util/stubborn.hpp>

#include <skog/parsers/product_parser.hpp>
#include <skog/parsers/category_parser.hpp>
#include <skog/parsers/category_id_parser.hpp>

namespace supermarx
{
	Json::Value parse_json(std::string const& src)
	{
		Json::Value root;
		Json::Reader reader;

		if(!reader.parse(src, root, false))
			throw std::runtime_error("Could not parse json feed");

		return root;
	}

	scraper::scraper(product_callback_t _product_callback, tag_hierarchy_callback_t, size_t _ratelimit, bool _cache, bool)
	: product_callback(_product_callback)
	, dl("supermarx skog/1.0", _ratelimit, _cache ? boost::optional<std::string>("./cache") : boost::none)
	{}

	void scraper::scrape()
	{
		static const std::string base_uri("http://derks.spar.nl"); // Derks has been chosen at random

		category_id_parser cidp([&](category_id_parser::category_id_t const& _cid)
		{
			for(size_t offset = 0; offset < 100000;)
			{
				std::stringstream puri_ss;
				puri_ss << base_uri;
				puri_ss << "/xhr/getProducts";
				puri_ss << "?offset=" << offset;
				puri_ss << "&limit=40";
				puri_ss << "&cat=" << _cid;

				std::string puri = puri_ss.str();
				Json::Value root(parse_json(dl.fetch(puri).body));
				if(root["status"].asString() != "success")
					throw std::runtime_error("Server does not report success");

				if(root["data"]["noProducts"].asBool())
					return;

				size_t newOffset = root["data"]["newOffset"].asUInt64();
				assert(offset < newOffset); // Hard progress
				offset = newOffset;

				product_parser pp([&](const message::product_base& p, boost::optional<std::string> const& _image_uri, datetime retrieved_on, confidence conf, problems_t probs)
				{
					product_callback(puri, _image_uri, p, retrieved_on, conf, probs);
				});
				pp.parse(root["data"]["html"].asString());
			}

			throw std::logic_error("Product limit overflow");
		});

		category_parser cp([&](category_parser::category_uri_t const& _curi)
		{
			cidp.parse(dl.fetch(_curi).body);
		});

		cp.parse(dl.fetch(base_uri).body);
	}

	raw scraper::download_image(const std::string& uri)
	{
		std::string buf(dl.fetch(uri).body);
		return raw(buf.data(), buf.length());
	}
}
