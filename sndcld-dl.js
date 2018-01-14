
let request = require("request");
let filesystem = require("fs");

let sndcld_dl = function(url, path) {

    this.url = url;
    this.path = path;

    let sndcld = function() {

        this.user_agent = "Mozilla/5.0";

        this.client_id = "JAgPhXap7XK0g8dUOtklbE7UnF05W8AH";

    };

    sndcld.prototype.get_track_id = function(url, callback)
    {
        request({
            url: url,
            headers: {
                "User-Agent": this.user_agent
            }
        }, function(error, response, body){
            let pattern = /"uri":"https:\/\/api\.soundcloud\.com\/tracks\/(\d+)"/;
            let matches = body.match(pattern);
            let track_id = Number(matches[1]);

            callback(track_id);
        });

    };

    sndcld.prototype.get_playlist = function(track_id, callback)
    {
        let url = "https://api.soundcloud.com/i1/tracks/" +
            track_id + "/streams?client_id=" + this.client_id;

        let user_agent = this.user_agent;

        request({
            url: url,
            headers: {
                "User-Agent": user_agent
            }
        }, function(error, response, body)
        {
            let json = JSON.parse(body);
            let playlist_url = json.hls_mp3_128_url;

            request({
                url: playlist_url,
                headers: {
                    "User-Agent": user_agent
                }
            }, function(error, response, body){
                callback(body);
            });
        });

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
            console.log(" [❤] Got playlist");
            console.log(" [❤] Creating malformed URL");
            sndcld.get_malformed_url(playlist,
                get_malformed_url_callback);
        };

        let get_track_id_callback = function(track_id) {
            console.log(" [❤] Track ID found =>", track_id);
            console.log(" [❤] Getting playlist");
            sndcld.get_playlist(track_id,
                get_playlist_callback);
        };

        console.log(" [❤] Getting track ID");
        sndcld.get_track_id(this.url,
            get_track_id_callback);

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

