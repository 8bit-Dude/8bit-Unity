# 8bit-Unity
A cross-platform game development kit for CC65, targetting 6502 based 8bit-era computers: Apple //, Atari 8bit, Atari Lynx, Commodore 64/128, NES/Famicom, Oric 1/Atmos
	
To get started:

WINDOWS  
run _builder_.bat and load any *.builder file from the folders in /projects/. After clicking "generate scripts", batch files will be added in /build/ that you can run to produce DISKS/ROMS for the various platforms.

LINUX (UBUNTU)  
Install some pre-requesites:  
sudo add-apt-repository universe  
sudo apt install cc65  
sudo apt install python2  
sudo apt install python-tk  
sudo python2 get-pip.py  
pip2 install pillow  
pip2 install pygubu  
run _builder_.sh and load any *.builder file from the folders in /projects/. After clicking "generate scripts", bash files will be added in /build/ that you can run to produce DISKS/ROMS for the various platforms.

NOTE  
Asset production tools like SpritePad, Gimp and Trackers are not included here. You can find those packaged in "release" downloads from wwww.8bit-unity.com.

