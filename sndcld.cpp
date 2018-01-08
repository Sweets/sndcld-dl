#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "json11/json11.hpp"

// HERE BE DRAGONS

namespace sndcld_util {

    const std::string user_agent = "User-Agent: Mozilla/5.0";

    std::string _replaceAll(std::string string, std::string substring, std::string replacement, unsigned int index) {

        if (string.find(substring, index) == std::string::npos) {
            return string;
        } else {
            return _replaceAll(
                string.replace(
                    string.find(substring, index),
                    replacement.length(),
                    replacement
                ),
                substring,
                replacement,
                string.find(
                    substring, index
                )
            );
        }

    }

    std::string replaceAll(std::string string, std::string substring, std::string replacement) {

        return _replaceAll(
            string, substring, replacement, 0
        );

    }

    std::string GET(std::string url, std::string output_path) {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url));

        std::list<std::string> headers;
        headers.push_back(user_agent);

        request.setOpt(new curlpp::options::HttpHeader(headers));

        if (output_path == "") {

            std::stringstream data;
            data << request;

            return data.str();

        } else {

            std::ofstream file;
            file.open(output_path);
            file << request;
            file.close();

            return output_path;

        }

        return ""; // Never reached

    }

}

namespace sndcld {

    std::string client_id;

    void set_client_id(std::string id) {
        client_id = id;
    }

    namespace track_id_methods {

        long int fallback_1(std::string url) {

            return -1; // -1 means it can't be found, even after any fallbacks.

        }

        long int main(std::string url) {

            const std::string response = sndcld_util::GET(url, "");

            std::regex json("\"uri\":\"https://api.soundcloud.com/tracks/(\\d+)\"");
            std::smatch matches;

            if (std::regex_search(response, matches, json)) {
                long int id = std::stoi(matches[1]);
                return id;
            } else {
                return fallback_1(url);
            }

        }

    } // End track id methods

    long int get_track_id(std::string url) {

        return track_id_methods::main(url);

    }

    std::string get_playlist(long int track_id) {

        if (track_id == -1)
            return "";

        const std::string streams_url = "https://api.soundcloud.com/i1/tracks/" + std::to_string(track_id) +
            "/streams?client_id=" + client_id;

        const std::string response = sndcld_util::GET(streams_url, "");
        std::string ERROR = "";
        const json11::Json JSON = json11::Json::parse(response, ERROR);

        if (ERROR != "")
            return "";

        const std::string playlist_url = JSON["hls_mp3_128_url"].string_value();
        const std::string playlist = sndcld_util::GET(playlist_url, "");

        return playlist;

    }

    std::string get_malformed_url(std::string playlist_str) {

        if (playlist_str == "")
            return "";

        std::stringstream playlist(playlist_str);

        std::vector<std::string> lines;
        std::string line;

        while (playlist >> line)
            lines.push_back(line);

        std::stringstream url(

            sndcld_util::replaceAll(
                lines.at(lines.size() - 2),
                "/", "\n"
            )

        );

        std::vector<std::string> url_parts;
        std::string url_part;

        while (url >> url_part)
            url_parts.push_back(url_part);

        std::string malformed_url = url_parts.at(0) + "//" + url_parts.at(1) + "/" + url_parts.at(2) + "/0/" + url_parts.at(4) + "/" + url_parts.at(5);

        return malformed_url;

    }

} // End sndcld

void usage(void) {

    printf("%s\n", "sndcld v3 by Sweets");
    printf("%s\n", " Usage: sndcld [artist] [title] [output path]");
    printf("%s\n", " Example: sndcld iameden r-r ~/Music/eden-rock-and-roll.mp3");
    printf("%s\n", " URL for song in example: https://soundcloud.com/iameden/r-r");

}

int main(int argc, char **argv) {

    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4] != NULL) {
        usage();
        return 1;
    }

    std::string artist(argv[1]);
    std::string title(argv[2]);
    std::string output_path(argv[3]);
    
    printf("%s\n", " [❤] sndcld v3 by Sweets");

    std::string url = "https://soundcloud.com/" + artist + "/" + title;

    printf("%s\n", " [❤] Setting client_id and appversion.");
    
    sndcld::set_client_id("JAgPhXap7XK0g8dUOtklbE7UnF05W8AH");
    
    printf("%s\n", " [❤] Getting track_id.");

    try {
        long int track_id = sndcld::get_track_id(url);
        printf("%s\n", " [❤] Got track_id.");

        printf("%s\n", " [❤] Getting playlist data.");
        std::string playlist = sndcld::get_playlist(track_id);
        printf("%s\n", " [❤] Got playlist data.");

        printf("%s\n", " [❤] Creating malformed URL.");
        std::string mp3url = sndcld::get_malformed_url(playlist);
        printf("%s\n", " [❤] Malformed URL created.");

        printf("%s\n", " [❤] Downloading song.");
        sndcld_util::GET(mp3url, output_path);
        printf("%s\n", " [❤] Sond downloaded.");

        return 0;

    } catch (curlpp::LibcurlRuntimeError) {
        printf("%s\n", " [(' ^ ';)] An error occured (curl).");

        return 1;
    }

}
