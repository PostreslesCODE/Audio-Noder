<p align="center">
  <img src="AudioNoderC/icons/logo.png" alt="Audio-Noder logo" width="200"/>
</p>

# Audio-Noder
Audio Noder written in C and C++

## THIS ONLY WORKS ON LINUX WITH PIPEWIRE AND WAYLAND ANY OTHER CONFIGURATIONS HAVE NOT BEEN CONSIDERED
## IMPORTANT, MOST OF THE CODE IS LEARNT AND PROVIDED BY LLMs SO DONT BLAME ME FOR PC KABOOM
#### (That is because I had no idea of using how to use the pipewire/qt libraries and code with them)

# Description

This is an app that allows/generates static permanent (while the app is running) audio node links that stay on reboot and apps reapearing.

Also has a mixer and virtual mics that you can route to if you need them.

### This is writen for my personal use but anyone can use it if you want to i guess

I made this so that I can arrange the connections as I please and to fix certain programs bugged ahh iniｔial connections, think of this as a work around.

## Building

To build this masterpiece:

in your megaterminal choose where you want to store this awesome program, then type:

```
git clone https://github.com/PostreslesCODE/Audio-Noder.git
cd Audio-Noder/AudioNoderC
mkdir build && cd build
cmake ..
make
```

Then in the /Audio-Noder/AudioNoderC/build/ folder you will find the actual executable AudioNoderC

## Usage

Place your now built executable in some folder where it is alone with a conifg.json file that will contain your connections, start the executable and you should have the connections set up like you specified in the config.json

Example:


<img width="1155" height="519" alt="image" src="https://github.com/user-attachments/assets/7035ddd2-3338-436c-aa45-d063dee2ecaa" />

This translates to: 


<img width="932" height="528" alt="image" src="https://github.com/user-attachments/assets/dcaae2b2-0091-41b2-af46-550c825b44e6" />




## Config
The config has this structure and it has to be followed correctly unless you want a non functioning program: 
```
{
  "pairs": [
    { "out_node": "OriginNodeName1",         "out_port": "OriginPortName1",       "in_node": "DestinationNodeName1", "in_port": "DestinationPortName2" },
    { "out_node": "OriginNodeName2",         "out_port": "OriginPortName2",       "in_node": "DestinationNodeName1", "in_port": "DestinationPortName2" }
  ]
}


```
