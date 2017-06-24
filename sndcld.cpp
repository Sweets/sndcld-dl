
#include <stdio.h>
#include <fstream>
#include <regex>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <json11.hpp>

void print(std::string string) {
    std::cout << string << std::endl;
} // BECAUSE I'M LAZY

std::string GET(std::string url) {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;

    request.setOpt(new curlpp::options::Url(url));

    std::list<std::string> headers;
    headers.push_back("User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36");

    request.setOpt(new curlpp::options::HttpHeader(headers));

    std::stringstream data;
    data << request;
    const std::string response = data.str();
    return response;
}

std::string replaceAll(std::string string, std::string substring, std::string replacement, int index) {
    if (string.find(substring, index) == std::string::npos) {
        return string;
    } else {
        return replaceAll(string.replace(string.find(substring, index), replacement.length(), replacement), substring, replacement, string.find(substring, index));
    }
}

std::string concat(std::vector<std::string> vector, int start, int end, std::string concatenator) {
    if (start < 0 || vector.size() - 1 < end)
        return "";
    std::string string;
    for (int index = start; index <= end; index++) {
        string += vector.at(index) + concatenator;
    }
    return string;
}

void download(std::string path, std::string url) {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;

    request.setOpt(new curlpp::options::Url(url));

    std::list<std::string> headers;
    headers.push_back("User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36");

    request.setOpt(new curlpp::options::HttpHeader(headers));

    std::ofstream file;
    print(" [♥] Path opened, streaming file ");
    file.open(path);
    file << request;
    file.close();
    print(" [♥] Path closed. MP3 dowloaded. ");
}

std::string getMalformedURL(std::string playlistData) {

    print(" [♥] Getting mp3 size ");
    std::stringstream playlistStringStream(playlistData);

    std::vector<std::string> lines;
    std::string line;
    while (playlistStringStream >> line)
        lines.push_back(line);

    std::stringstream urlStream(replaceAll(lines.at(lines.size() - 2), "/", "\n", 0));
    std::vector<std::string> urlParts;
    std::string part;
    while (urlStream >> part)
        urlParts.push_back(part);

    print(" [♥] Concatenating URL parts ");
    std::string malformedUrl = concat(urlParts, 0, 2, "/") + "0/" + urlParts.at(4) + "/" + urlParts.at(5);

    return malformedUrl;
}

std::string getPlaylist(long int trackId) {
    print(" [♥] Sending stream GET ");
    const std::string url = "https://api.soundcloud.com/i1/tracks/" + std::to_string(trackId) +
        "/streams?client_id=" +
        "2t9loNQH90kzJcsFCODdigxfp325aq4z" + // client ID
        "&app_version=" +
        std::to_string(
            1496423089
        ); // appversion

    // If you yell at me for concatenating client ID, I will kill you so hard you die to death.
    // It's concatenated rather than hardcoded into the string after the track ID because it makes it
    // easier to edit.Gettig stream playlist

    const std::string response = GET(url);
    print(" [♥] Parsing stream data ");
    std::string JSONError;
    const json11::Json data = json11::Json::parse(response, JSONError);
    print(" [♥] Data parsed. Constructing playlist URL ");
    const std::string playlisturl = data["hls_mp3_128_url"].string_value();
    print(" [♥] Sending playlist GET ");
    const std::string playlist = GET(playlisturl);
    print(" [♥] Got playlist ");

    return playlist;
}

int main(int argc, char *argv[]) {
    std::string url(argv[1]);
    std::string path(argv[2]);

    print(" [♥] sndcld v2, written by Sweets ");

    print(" [♥] Sending GET request ");
    const std::string response = GET(url);
    print(" [♥] Searching for track ID ");

    std::regex json("publisher_metadata\":\\{(.*?)\\},\"purchase_title");
    std::smatch match;

    if (std::regex_search(response, match, json)) {
        std::string strMatch(match[0]);
        print(" [♥] Parsing publisher_metadata JSON for ID ");
        std::string JSONError;
        const json11::Json metadata = json11::Json::parse(strMatch.substr(20, strMatch.length() - 36), JSONError);
        long int id = std::stol(metadata["id"].dump());
        print(" [♥] ID found => " + std::to_string(id));
        print(" [♥] Gettig stream playlist ");
        std::string playlist = getPlaylist(id);
        print(" [♥] Constructing malformed URL for mp3 file ");
        std::string mp3url = getMalformedURL(playlist);
        download(path, mp3url);
    }

    return 0;
}
