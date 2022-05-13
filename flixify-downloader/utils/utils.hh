#pragma once
#include <vector>
#include <string>

enum class quality {
	P480,
	P720,
	P1080
};

namespace utils {
	std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, bool>>> search_movie( const std::string& );
	std::string movie_download_link( const std::string& , quality );
	std::vector<std::pair<std::string, std::pair<std::string, std::string>>> episode_info( const std::string& );
	void download_from_url( const std::string&, std::ofstream );
}