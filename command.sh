ffmpeg -i input.mp4 \
-map 0:v -map 0:a \
-c:v libx264 -s:v:0 1920x1080 -b:v:0 5000k -maxrate:v:0 6000k -bufsize:v:0 12000k \
-c:v libx264 -s:v:1 1280x720 -b:v:1 2500k -maxrate:v:1 3000k -bufsize:v:1 6000k \
-c:v libx264 -s:v:2 640x360 -b:v:2 1000k -maxrate:v:2 1200k -bufsize:v:2 2400k \
-profile:v high -level:v 4.0 -preset:v medium -keyint_min:v 60 -g:v 60 -sc_threshold:v 0 \
-c:a aac -b:a:0 128k -ac:a:0 2 -ar:a:0 44100 \
-c:a aac -b:a:1 64k -ac:a:1 2 -ar:a:1 44100 \
-strict -2 -f dash output.mpd