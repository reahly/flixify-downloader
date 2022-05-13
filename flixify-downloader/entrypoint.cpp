#include <iostream>
#include <Windows.h>
#include <vector>
#include <nlohmann/json.hh>
#include <cpr/cpr.h>
#include <regex>
#include <filesystem>
#include "utils/utils.hh"

int main( ) {
back:
	std::string choosen_movie;
	printf( "what movie do you want to watch? \n" );
	std::getline( std::cin, choosen_movie );

	if ( choosen_movie.find( ' ' ) != std::string::npos )
		choosen_movie = std::regex_replace( choosen_movie, std::regex( " " ), "-" );

	auto searched_movie = utils::search_movie( choosen_movie );
	if ( searched_movie.empty( ) ) {
		printf( "no movies found with that name \n" );
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		system( "cls" );
		goto back;
	}

	printf( "Movies found with that name: \n" );

	std::map<std::string, std::string> serie_list;
	for ( const auto& a : searched_movie ) {
		printf( std::format( "{} \t\n {} ({}) \n", a.second.second ? "Film" : "Serie", a.second.first, a.first.first ).c_str( ) );
		if ( !a.second.second )
			serie_list[a.first.first] = a.first.second;
	}

	std::string choosen_movie_id, choosen_serie_id;
	printf( "\nchoose the movie id \n" );
	std::getline( std::cin, choosen_movie_id );

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	system( "cls" );

	if ( !serie_list[choosen_movie_id].empty( ) ) {
		printf( "Episodes found: \n" );
		for ( const auto& episodes : utils::episode_info( serie_list[choosen_movie_id] ) ) {
			printf( std::format( "- Season {}, Episode {} ({}) \n", episodes.first, episodes.second.first, episodes.second.second ).c_str( ) );
		}

		printf( "choose the episode id \n" );
		std::getline( std::cin, choosen_serie_id );

		const auto download_link = utils::movie_download_link( choosen_serie_id, quality::P1080 );
		utils::download_from_url( download_link, std::ofstream( "C:\\downloaded_serie.mp4", std::ios::out | std::ios::binary ) );
		
		system( "pause" );
		return 1;
	}

	const auto download_link = utils::movie_download_link( choosen_movie_id, quality::P1080 );
	utils::download_from_url( download_link, std::ofstream( "C:\\downloaded_video.mp4", std::ios::out | std::ios::binary ) );

	system( "pause" );
	return 1;
}
