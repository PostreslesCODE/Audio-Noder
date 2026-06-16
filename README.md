# Audio-Noder
Audio Noder written in C and C++

# THIS ONLY WORKS ON LINUX WITH PIPEWIRE AND WAYLAND ANY OTHER CONFIGURATIONS HAVE NOT BEEN CONSIDERED
# IMPORTANT, MOST OF THE CODE IS LEARNT AND PROVIDED BY LLMs SO DONT BLAME ME FOR PC KABOOM
(That is because I had no idea of using how to use the pipewire/qt libraries and code with them)


This is an app that allows/generates static permanent(stays on reboot and apps reapearing) audio node links.

### This is writen for my personal use but anyone can use it if you want to i guess

I made this so that I can arrange the connections as I please and to fix certain programs bugged ahh inicial connections, think of this as a work around.

To build this masterpiece:

in your megaterminal choose where you want to store this awsome program, then type:

git clone https://github.com/PostreslesCODE/Audio-Noder.git
cd Audio-Noder
mkdir build && cd build
cmake ..
make
./AudioNoderC
