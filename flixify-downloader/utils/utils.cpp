#include "utils.hh"
#include <iostream>
#include <magic_enum.hh>
#include <regex>
#include <cpr/cpr.h>
#include <nlohmann/json.hh>

constexpr std::string session_token = "SESSIONCOOKIE";

std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, bool>>> utils::search_movie( const std::string& name ) {
	const auto request = Get( cpr::Url{ std::format( "https://raptus.site/search?_t=5&_u=5&p=1&postersize=poster&q={}", name ) }, cpr::Header{ { "accept", "application/json" }, { "cookie", std::format( R"(pip=0; promo_id=0; auid=0; session={}; si2=0; profile_id=0; ds8=1; _vjs_volume=0; test_cookie=0; pxid=0)", session_token ) } } ).text;
	if ( !nlohmann::json::accept( request ) )
		return { };

	std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, bool>>> ret;
	const auto parsed_request = nlohmann::json::parse( request );
	for ( const auto& entries : parsed_request["items"].items( ) ) {
		ret.emplace_back( std::make_pair( std::make_pair( entries.value( )["id"].get<std::string>( ), entries.value( )["url"].get<std::string>( ) ), std::make_pair( entries.value( )["title"].get<std::string>( ), entries.value( )["type"].get<std::string>( ).find( "movie" ) != std::string::npos ) ) );
	}

	return ret;
}

std::string utils::movie_download_link( const std::string& id, const quality q ) {
	auto q_str = std::string( magic_enum::enum_name( q ) );
	q_str = std::regex_replace( q_str, std::regex( "\\P" ), "" );

	const auto request = Get( cpr::Url{ std::format( "https://raptus.site/media/links/{}?_t=5&_u=5", id ) }, cpr::Header{ { "accept", "application/json" }, { "cookie", std::format( R"(pip=0; promo_id=0; auid=0; session={}; si2=0; profile_id=0; ds8=1; _vjs_volume=0; test_cookie=0; pxid=0)", session_token ) } } ).text;
	if ( !nlohmann::json::accept( request ) )
		return "";

	const auto parsed_request = nlohmann::json::parse( request );
	return parsed_request["media"][std::string( q_str )].get<std::string>( );
}

std::vector<std::pair<std::string, std::pair<std::string, std::string>>> utils::episode_info( const std::string& serie_url ) {
	const auto request = Get( cpr::Url{ std::format( "https://raptus.site/{}?_t=5&_u=5", serie_url ) }, cpr::Header{ { "accept", "application/json" }, { "cookie", std::format( R"(pip=0; promo_id=0; auid=0; session={}; si2=0; profile_id=0; ds8=1; _vjs_volume=0; test_cookie=0; pxid=0)", session_token ) } } ).text;
	if ( !nlohmann::json::accept( request ) )
		return {};

	std::vector<std::string> seasons_list;
	const auto parsed_request = nlohmann::json::parse( request );
	for ( const auto& season : parsed_request["seasons"].items( ) ) {
		seasons_list.emplace_back( season.value( )["url"].get<std::string>( ) );
	}

	std::vector<std::pair<std::string, std::pair<std::string, std::string>>> episodes_list;
	for ( const auto& season_list : seasons_list ) {
		auto season_idx = season_list;
		season_idx.erase( 0, season_idx.find_last_of( '-' ) + 1 );

		const auto request_episodes = Get( cpr::Url{ std::format( "https://raptus.site{}?_t=5&_u=5", season_list ) }, cpr::Header{ { "accept", "application/json" }, { "cookie", std::format( R"(pip=0; promo_id=0; auid=0; session={}; si2=0; profile_id=0; ds8=1; _vjs_volume=0; test_cookie=0; pxid=0)", session_token ) } } ).text;
		if ( !nlohmann::json::accept( request_episodes ) )
			continue;

		const auto parsed_request_episodes = nlohmann::json::parse( request_episodes );
		for ( const auto& episodes : parsed_request_episodes["episodes"].items( ) ) {
			auto episode_idx = episodes.value( )["url"].get<std::string>( );
			episode_idx.erase( 0, episode_idx.find_last_of( '-' ) + 1 );

			episodes_list.emplace_back( std::make_pair( season_idx, std::make_pair( episode_idx, episodes.value( )["id"].get<std::string>( ) ) ) );
		}
	}

	return episodes_list;
}

void utils::download_from_url( const std::string& url, std::ofstream path ) {
	Download( path, cpr::Url{ url }, cpr::ProgressCallback( [&]( const size_t download_total, const size_t download_now, size_t, size_t ) -> bool {
		if ( download_total && download_now ) {
			const auto total_downloaded_percentage = static_cast<float>( download_now ) / static_cast<float>( download_total ) * 100.f;
			std::cout << "Downloaded " << total_downloaded_percentage << "%" << std::endl;
		}
		return true;
	} ) );
}
