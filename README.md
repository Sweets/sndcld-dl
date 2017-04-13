# sndcld-dl
SoundCloud downloader.

src: https://eternal.is/thread/soundcloud-mp3-downloader

Normally, SoundCloud splits up the mp3 file into 9 seconds parts (or less, but never more, for the beginning and end of the song), presumably to prevent people from downloading the MP3 file as a whole. Just a night of reverse engineering, and I found a major flaw that allows this to be exploited, a forged URL to be created, which then points you to the full MP3.

This was more of a project to learn a little ofthe inner workings of SoundCloud than actually taking MP3 files, but I figured I'd share this script I made from learning about SC.

Usage:

To use the downloader, save it to a location of your choice. Keep in mind that files it downloads will always be placed in a directory within /tmp/, more on that later. Assume, for this post, that you named it "sndcld.js", and placed the script in your home directory.

Simply go to a songs page on soundcloud, copy the URL, and paste it into your commandline. There can be only one argument supplied (excluding the filename for nodejs), and that is the URL itself. Anymore are ignored, any less (0) causes an error. The scheme for the URL should look like this: https://soundcloud.com/[accountname]/[trackname]

`node ~/sndcld.js https://soundcloud.com/ohwondermusic/all-we-do`

This is written specifically for \*nix machines with node installed (and a /tmp/ directory, if for some odd reason your filesystem doesn't consist of one).

Once it finds the MP3 file, forges the URL to download, and begins downloading, it will create a new directory in /tmp/ which is the account name of who or whom uploaded the file. It is only created if it doesn't exist. The name is from the URL, and not scraped for. Same with the song name. The song is then placed within that directory. So going back to the usage example, "All We Do" by Oh Wonder will create /tmp/ohwondermusic/ (if it doesn't exist), then download all-we-do.mp3 into that directory.
