LVDO
========

LVDO is a video steganography implementation by StarBrilliant using DCT
coefficients.


What can it do?
---------------

You can convert a file into video and upload it to YouTube or many other video
sites. The receiver downloads the video and is able to extract the file using
the same parameter you encode it.


Usage
-----

This is a possible combination of parameters:

```bash
# Install dependencies (modify according to your Linux distribution)
sudo apt-get install libglib2.0-dev libfftw3-dev
sudo apt-get install git ffmpeg x264 mplayer

# Compile it
git clone https://github.com/m13253/lvdo.git
cd lvdo/src
make

# Encode
cat secret.mp3 | ./lvdoenc -s 640x480 -q 6 --qmin 1 --qmax 4 | x264 --input-res 640x480 --fps 1 --profile high --level 5.1 --tune stillimage --crf 22 --colormatrix bt709 --me dia --merange 0 -o public.mkv -

# Decode
ffmpeg -i public.mkv -f rawvideo - | ./lvdodec -s 640x480 -q 6 --qmin 1 --qmax 4 | mplayer -
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


FAQ
---

1. Why it is called LVDO?

   You Will find some secrets when you read it in Japanesu Engurishu.

2. How do I adjust those complicated parameters?

   Do not use the maximum. Do not use the minimum either. Try to decode your self
   before publishing.

3. Why the video downloaded from YouTube can not be decoded?

   Play around with those parameters please. If you could not get it work, feel
   free to contact me for help.

4. What is `clipping xxx to xxx`?

   It means the decoded file is corrupted. If it happens a lot during decoding,
   make sure you are using appropriate parameters. If it happens a lot during
   encoding, please submit an issue to me alongside with your parameters.

