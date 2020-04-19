
let request = require("request");
let filesystem = require("fs");

let sndcld_dl = function(url, path) {

    this.url = url;
    this.path = path;

    let sndcld = function() {

        this.user_agent = "Mozilla/5.0";
        this.client_id = "PANENeeumEFxeUKuTj575zguSBQI5DwE";
        this.headers = {"User-Agent": this.user_agent};

    };

    sndcld.prototype.get_playlist = function(url, callback)
    {
        let client_id = this.client_id;
        let headers = this.headers;

        let track_url_callback = function(error, response, body) {
            let json = JSON.parse(body);
            let playlist_url = json.url;

            request({
                url: playlist_url,
                headers: headers
            }, function(error, response, body) {
                callback(body);
            });
        };

        let url_callback = function(error, response, body) {
            let pattern = /"https:\/\/api\-v2\.soundcloud\.com\/media\/soundcloud:tracks:(\d+)\/([0-9a-fA-F]{8}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{12})\/stream\/hls"/;
            let matches = body.match(pattern);

            let track_id = Number(matches[1]);
            let uuid = matches[2];
            let track_url = "https://api-v2.soundcloud.com/media/soundcloud:tracks:" +
                track_id + "/" + uuid + "/stream/hls?client_id=" + client_id;

            request({url: track_url, headers: headers}, track_url_callback);
        };

        request({url: url, headers: headers}, url_callback);
    };

    sndcld.prototype.get_malformed_url = function(playlist, callback)
    {
        let url = playlist.split('\n').slice(-2, -1)[0];

        let url_parts = url.split('/');

        let malformed_url = url_parts.slice(0, 4).join('/') +
            '/0/' + url_parts.slice(5).join('/');

        callback(malformed_url);

    };

    this.sndcld = new sndcld();

    (function(){

        let sndcld = this.sndcld;
        let path = this.path;

        let get_malformed_url_callback = function(malformed_url) {
            console.log(" [❤] Malformed URL created");
            console.log(" [❤] Streaming to", path);
            request({
                url: malformed_url,
                headers: {
                    "User-Agent": sndcld.user_agent
                }
            }, function(error, response, body) {
                console.log(" [❤] Song downloaded! [❤]");
            }).pipe(
                filesystem.createWriteStream(path)
            );
        };

        let get_playlist_callback = function(playlist) {
            console.log(" [❤] Getting playlist");
            console.log(" [❤] Got playlist");
            console.log(" [❤] Creating malformed URL");
            sndcld.get_malformed_url(playlist,
                get_malformed_url_callback);
        };


        sndcld.get_playlist(this.url,
            get_playlist_callback);

    }).call(this);

};

if (require.main === module)
{
    let sndcld_url = process.argv[2];
    let path = process.argv[3];

    new sndcld_dl(sndcld_url, path);
}
else
{
    module.exports = sndcld_dl;
}
