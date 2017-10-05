# sndcld-dl
SoundCloud downloader.

src: https://eternal.is/thread/soundcloud-mp3-downloader

Requires curlpp and json11. You'll have to figure out how to get these yourself. I simply installed them from the AUR (arch user repository).

Normally, SoundCloud splits up the mp3 file into 9 seconds parts (or less, but never more, for the beginning and end of the song), presumably to prevent people from downloading the MP3 file as a whole. Just a night of reverse engineering, and I found a major flaw that allows this to be exploited, a forged URL to be created, which then points you to the full MP3.

This was more of a project to learn a little ofthe inner workings of SoundCloud than actually taking MP3 files, but I figured I'd share this script I made from learning about SC.

Compilation:

```
mkdir build && cd build
cmake ..
make
```

Usage:

Say we wanted soundcloud.com/iameden/r-r

`$ ./sndcld iameden r-r /tmp/music.mp3`
