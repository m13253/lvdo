LVDO
========

LVDO is a video steganography implementation by StarBrilliant using DCT
coefficients.


What can it achieve?
--------------------

You can convert a file into video and upload it to YouTube or many other video
sites. The receiver downloads the video and is able to extract the file using
the same parameter you encode it.


Usage
-----

This is a possible combination of parameters:

```bash
# Compile it
git clone https://github.com/m13253/lvdo.git
cd lvdo/src
make

# Encode
cat secret.mp3 | ./lvdoenc -s 640x480 -q 5 --qmin 1 --qmax 4 | x264 --input-res 640x480 --fps 1 --profile high --level 5.1 --tune stillimage --crf 22 --colormatrix bt709 --me dia --merange 0 -o public.mkv

# Decode
ffmpeg -i public.mkv -f rawvideo - | ./lvdodec -s 640x480 -q 5 --qmin 1 --qmax 4 | mplayer -
```


Issues
------

Some parameter combination causes undecodable file.

Due to rounding errors, input file and output file is not precisely the same.
You should add some mechanism to keep file integrity.


License
-------

This program is licensed under GNU General Public License version 3 or later,
the author does not provide any warranties. Users should be responsible for any
potential damage caused by this software.


Abuse
-----

I hope you will not abuse this program to do something evil.

I suggest you **not** try to encode porn with LVDO and upload it to YouTube.
You would be punished then.

