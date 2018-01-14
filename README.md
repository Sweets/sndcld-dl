# sndcld-dl
SoundCloud downloader.

# Usage

sndcld-dl works both as a module and standalone program. There is only one dependency required that is not packaged with node.js by default, and that is `request`.

To use sndcld-dl from the command line, simply execute `$ /path/to/sndcld-dl.js https://soundcloud.com/iameden/r-r /tmp/eden.mp3`.

If you would like to use it as a node module, refer to the code snippet below.

```

let sndcld_dl = require("./sndcld-dl");

sndcld_dl("https://soundcloud.com/iameden/r-r", "/tmp/eden.mp3");

```
