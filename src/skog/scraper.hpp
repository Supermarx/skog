#pragma once

#include <functional>

#include <supermarx/message/product_base.hpp>

#include <supermarx/raw.hpp>
#include <supermarx/util/cached_downloader.hpp>

namespace supermarx
{
	class scraper
	{
	public:
		using problems_t = std::vector<std::string>;
		using callback_t = std::function<void(const std::string&, boost::optional<std::string> const&, const message::product_base&, datetime, confidence, problems_t)>;

	private:
		callback_t callback;
		cached_downloader dl;

	public:
		scraper(callback_t _callback, size_t ratelimit, bool cache = false);
		scraper(scraper&) = delete;
		void operator=(scraper&) = delete;

		void scrape();
		raw download_image(const std::string& uri);
	};
}
