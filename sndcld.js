String.prototype.py_format = function() {
	    /*
	     *         Python-like format method on strings
	     *                 does NOT account for escaped braces
	     *                     */
	    var str = this;
	    var matches = str.match(/(\{[\d]\})/g);
	    if (matches) {
		            for (var index = 0; index < matches.length; index++) {
				                var match = matches[index];
				                str = str.replace(match, arguments[index]);
				            }
		        }
	    return str;
};

var request = require("request");
var nightmare = require("nightmare")({show: false});
var filesystem = require("fs");

var save_as = {
	    "name": "",
	    "dir": ""
};

var get_track_id = function(url, cb) {
	    var id = -1;
	    save_as.name = url.split("/").slice(-2)[1];
	    save_as.dir = url.split("/").slice(-2)[0];

	    console.log(" [♥] Fetching track id ");
	    nightmare
	        .goto(url)
	        .evaluate(function(){
			            return document.body.innerHTML;
			        })
	        .end()
	        .then(function(html){
			            var matches = html.match(/"publisher_metadata":\{(.*?)\},"purchase_title"/i);
			            if (matches) {
					                    var data = JSON.parse("{" + matches[1] + "}");
					                    id = data.urn.split(":")[2];
					                    if (data.artist === undefined)
						                        data.artist = save_as.dir;

					                    if (data.album_title === undefined)
						                        data.album_title = "_unknown_";

					                    if (data.release_title === undefined)
						                        data.release_title = save_as.name;

					                    console.log(" [♥] Showing scraped data\n")
					                    console.log(" [♥] ID:", id);
					                    console.log(" [♥] Artist:", data.artist);
					                    console.log(" [♥] Album:", data.album_title);
					                    console.log(" [♥] Name:", data.release_title, "\n");
					                    cb(id);
					                } else {
								                console.log(" [x] Uh-oh. Track id not found");
								            }
			        });
	    
};

var get_file = function(extension) {
	    console.log(" [♥] Forging extension url");
	    var extpart = extension.split("/");
	    var new_file_url = [
		            extpart.slice(0, 4).join("/"),
		            0,
		            extpart.slice(5).join("/")
		        ].join("/");
	    var download = function() {
		        console.log(" [♥] Requesting file");
		            request(
				                new_file_url
				            ).pipe(
						                filesystem.createWriteStream(
									                "/tmp/" + save_as.dir + "/" + save_as.name + ".mp3"
									            )
						            );
		            console.log(" [♥] Song saving as: " + save_as.name + ".mp3 (in /tmp/" + save_as.dir + ")");
		        };

	    if (!filesystem.existsSync("/tmp/" + save_as.dir)) {
		            console.log(" [♥] Making new directory " + save_as.dir + " in /tmp/")
		            filesystem.mkdir("/tmp/" + save_as.dir, function() {
				                download();
				            });
		        } else {
				        download();
				    }
};

var get_stream = function(track_id) {
	    var client_id = "2t9loNQH90kzJcsFCODdigxfp325aq4z";
	    var app_version = 1496423089;
	    var scheme = "https://api.soundcloud.com/i1/tracks/{0}/streams?client_id={1}&app_version={2}";
	    var url = scheme.py_format(track_id, client_id, app_version);
	    console.log(" [♥] Fetching streams for filesize ");
	    request(url, function(error, response, body){
		            try {
                  var data = JSON.parse(body);
                } catch (e) {
                  console.log("SoundCloud issued update since last run. Update client id and app version.");
                  process.exit(1);
                }		 
		            var new_url = data.hls_mp3_128_url;
		            console.log(" [♥] Playlist found from JSON ");
		            request(new_url, function(error, response, body){
				                console.log(" [♥] Stream fetched ");
				                var body = body.split("\n");
				                var last_ext = body.slice(body.length - 3, body.length - 2)[0];
				                console.log(" [♥] Fetching file from extension url ");
				                get_file(last_ext);
				            });
		        });
};

var get_mp3 = function(url) {
	    get_track_id(url, get_stream);
};

get_mp3(process.argv[2]);
