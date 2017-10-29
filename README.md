# Original research

https://en.wikipedia.org/wiki/Wikipedia:No_original_research


# Raspberry Pi 3 with Debian Buster (after Stretch)

https://wiki.debian.org/RaspberryPi3

https://github.com/Debian/raspi3-image-spec


# Raspberry Pi 3 Bluetooth with bluealsa

https://raspberrypi.stackexchange.com/questions/73659/bluetooth-disconnect

https://jp.raspberrypi.org/forums/viewtopic.php?t=193776

No pulseaudio in Raspbian Lite Stretch, use bluealsa

sudo systemctl disable bluealsa

sudo nano /lib/systemd/system/bluealsa.service

{and change it to WantedBy=multi-user.target - then save}

sudo systemctl enable bluealsa

This way, bluealsa will start up on boot in Lite

https://www.raspberrypi.org/forums/viewtopic.php?f=28&t=138145&start=125

https://raspberrypi.stackexchange.com/questions/71333/bluetooth-does-not-work-with-raspbian-stretch-and-raspberry-pi-3/71384

https://github.com/Arkq/bluez-alsa

https://www.raspberrypi.org/forums/viewtopic.php?t=147919&p=1034908


# Intel Management Engine

https://wiki.gentoo.org/wiki/Sakaki%27s_EFI_Install_Guide/Disabling_the_Intel_Management_Engine

https://puri.sm/posts/deep-dive-into-intel-me-disablement/


# gtk-recordmydesktop sound work

https://ubuntuforums.org/showthread.php?t=2362044

rename in Advanced -> Sound: DEFAULT to default

pavucontrol -> select Monitor ... to record computer output, then close it or recording can't start


# Debian live wrapper

https://debian-live.alioth.debian.org/live-wrapper-doc/quickstart.html

https://debian-live.alioth.debian.org/live-manual/stable/manual/html/live-manual.en.html#examples


# MPV (best player KISS principle) configuration in ~/.config/mpv/mpv.conf

hwdec=vdpau

save-position-on-quit


# Emacs tutorials

https://www.youtube.com/watch?v=49kBWM3RQQ8


# Ryzen Threadripper as software video card

https://askubuntu.com/questions/908381/desktop-stuck-on-software-rendering-libgl-always-software-set

https://www.reddit.com/r/lowendgaming/comments/4mlqwi/i_finally_figured_out_how_to_play_any_game_on/



# Real functional examples of open source hardware and software

https://github.com/librespacefoundation

http://dl.illwieckz.net/b/linux-gpu-compatibility-matrix/


# Turn an old laptop into a Raspberry Pi laptop

https://codeandunicorns.com/acer-travelmate-4150-retrofitting-raspberry-pi-laptop-part-1-lcd-cleanup/


# Debian Live with amdgpu fully supported

https://cdimage.debian.org/images/unofficial/non-free/images-including-firmware/


# Ubuntu boot options on live cd and after install

https://ubuntuforums.org/showthread.php?t=1613132

https://help.ubuntu.com/community/BootOptions


# Best CPU upgrades for motherboards, in my opinion

https://en.wikipedia.org/wiki/List_of_Intel_Xeon_microprocessors = Older

http://www.digitimes.com/news/a20170419PD207.html

https://hardforum.com/threads/coffee-lake-lga-1151-6c-12t-launching-in-august-several-k-models-planned.1930226/


# Driver for Creative Sonud Blaster Audigy

http://support.creative.com/Products/ProductDetails.aspx?prodID=4915


# User autologin if you forgot at install, in /target/...

https://ubuntu-mate.community/t/auto-login-to-the-desktop/60


# Raspberry Pi best Sound Card (requires VGA)

Ebay.co.uk item number: 261997177225


# Play MIDI files in Ubuntu:

sudo -s

apt-get install -y audacious fluid-soundfont-gm fluid-soundfont-gs timidity timidity-interfaces-extra

Audacious -> File -> Settings -> Plugins -> Input -> AMIDI Plugin -> Soundfont

/usr/share/sounds/sf2/FluidR3_GM.sf2


# Youtube-dl download at maximum possible quality:

youtube-dl -f 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/bestvideo+bestaudio' --merge-output-format mp4 ...video...


# Firefox on 4k displays or too little Full HD displays

https://fedoramagazine.org/how-to-get-firefox-looking-right-on-a-high-dpi-display-and-fedora/

Enabling the high DPI mode in Firefox is actually pretty simple, it is just a matter of setting the layout.css.devPixelsPerPx value in about:config to a value of 2 (i.e. double the size).
To do this, open up a new tab, and type about:config into the address bar, and press enter.
Then use the search box in this page to find the key layout.css.devPixelsPerPx and then change the value from -1.0 to 2

However, on closer inspection you will realise that the icons in the Firefox toolbar, while now the correct proportions, are now slightly fuzzy compared to the text. This is because the default Firefox theme ships bitmap icons that don’t work well with High DPI displays.

The easiest way to fix this, and to make your Firefox fit that much better with the Fedora Workstation, is to install the GNOME Firefox theme. This theme uses scalable vector assets for the icons (SVGs) that look crisp and clean on a high DPI display.

https://addons.mozilla.org/en-us/firefox/addon/adwaita/


# Ubuntu improovements

https://ubuntuforums.org/showthread.php?t=2213631&p=13262506#post13262506 - iso on fat32 USB stick

https://wiki.ubuntu.com/LiveUsbPendrivePersistent

http://askubuntu.com/questions/2194/how-can-i-improve-ubuntu-overall-system-performance

https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout

http://askubuntu.com/questions/276669/how-to-monitor-disk-activity

https://help.ubuntu.com/community/DiskPerformance

https://bigacid.wordpress.com/2008/12/08/jm20337-read-data-corruption-solution/

http://www.mikejonesey.co.uk/linux/optimisation/linux-io-optimisation

http://wiki.linuxaudio.org/wiki/system_configuration

https://fritshoogland.wordpress.com/2012/12/15/throttling-io-with-linux/

https://www.kernel.org/doc/Documentation/cgroup-v1/blkio-controller.txt

http://unix.stackexchange.com/questions/48138/how-to-throttle-per-process-i-o-to-a-max-limit

https://www.reddit.com/r/linux/comments/4a87g1/is_linuxs_completely_fair_scheduler_ready_for_the/?st=izn2b9eq&sh=9e33d9af

https://wiki.archlinux.org/index.php/mpv

https://forums.linuxmint.com/viewtopic.php?f=42&t=163126


# Test stereo speakers:

speaker-test -c 2 -t wav

http://www.unix.com/os-x-apple-/249893-bash-audio-sweep-generator.html


# Test surround 5.1 speakers:

speaker-test -c 6 -t wav


# Wine installation for 3d games:

http://davidgis.fr/amdgaming/

https://www.reddit.com/r/linuxquestions/comments/6goiso/trouble_with_gallium_nine_in_wine_on_debian/

https://aur.archlinux.org/packages/wine-gaming-nine/?comments=all

https://www.reddit.com/r/archlinux/comments/5uo5nr/wine_gaming_on_amdgpu/

https://launchpad.net/~commendsarnex/+archive/ubuntu/winedri3

https://launchpad.net/~oibaf/+archive/ubuntu/graphics-drivers

https://www.reddit.com/r/linux_gaming/comments/4rezum/amdgpu_terrible_wine_support/

https://wiki.ixit.cz/d3d9

sudo -s

add-apt-repository ppa:commendsarnex/winedri3

#Nu merg live:
add-apt-repository ppa:oibaf/graphics-drivers

dpkg --add-architecture i386 && apt-get update && apt-get install -y wine2.0 xserver-xorg-video-amdgpu mesa-vdpau-drivers

WINEARCH=win32 winecfg

Extra (no more Wine bug reports if you don't stop with install here):

apt-get install -y mono-complete

#apt-get install -y mono-runtime

apt-get install -y winetricks

winetricks d3dx9_36

winetricks ddr=opengl multisampling=enabled orm=fbo videomemorysize=1024 strictdrawordering=enabled

winetricks dotnet20

winetricks dotnet40

winetricks corefonts vcrun2005 vcrun2008 vcrun2010 vcrun2012 vcrun2015

For Wine 2.0 and onwards, DirectX 11 is getting supported. More on that later...


# Windows or DOS first if it is found by grub:

apt-cache search dpkg-divert

sudo -s

apt-get install -y config-package-dev

dpkg-divert --local --divert /etc/grub.d/09_os-prober --rename --add /etc/grub.d/30_os-prober

update-grub


# Remove this divert (works to remove if bad name given to diverted file), go back to default:

sudo -s

dpkg-divert --rename --remove /etc/grub.d/30_os-prober

update-grub


# MPV better configuration in ~/.config/mpv/mpv.conf

hwdec=vdpau

profile=opengl-hq

debrand=no

save-position-on-quit

af=drc


# Avrusb500v2 avr programmer has worked for me, I recommend it:

http://www.tuxgraphics.org/electronics/200705/article07052.shtml

http://helpful.knobs-dials.com/index.php/Electronics_project_notes/Avrusb500v2_notes

http://letsmakerobots.com/blog/nuumio/first-experiences-avrusb500-v2-programmer


# Using a ft232bl instead of ft232rl works, tested, but requires a 1k5 resistor and 6mhz quartz and two 27pF capacitors and a 470r resistor and another 100nF capacitor added, like in this article (seems even faster):

http://vonkonow.com/wordpress/2012/03/usbserial-pcb/


# ... fixing a bricked one from a pc without another programmer by using a Linux live cd (repeat the make commands a few times if they give error, bitbang is hard to get):

http://tuxgraphics.org/common/src2/article07052/avrusb500-upd.html

http://images.tuxgraphics.org/iso/sysresccd.org/systemrescuecd-x86-2.8.1.iso

http://www.tuxgraphics.org/common/src2/article07052/avrusb500v2-1.5.tar.gz


# ... configuring the programmer in Ubuntu Linux 14.04 and newer (changed default parallel and "" between type):

gedit /etc/avrdude.conf

default_parallel     = "/dev/ttyUSB0";

default_serial     = "/dev/ttyUSB0";


# ... and further down:

programmer

  id    = "avrusb500";
  
  desc  = "Atmel AVR ISP V2 programmer from tuxgraphics";
  
  type  =  "stk500v2";
  
;

#

# ... create a script to program fast (usage = sudo compavr (file without .c extension)):
touch compavr && chmod +x compavr && gedit compavr

avr-gcc -mmcu=atmega32 -Wall -Os -o $1.elf $1.c

avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex

avrdude -c avrusb500 -p atmega32 -u -U hfuse:w:0xc9:m -U lfuse:w:0xe3:m

avrdude -c avrusb500 -p atmega32 -U flash:w:$1.hex


# Usbasp has failed for me, found a way to fix it:

http://visionhack.de/2012/09/23/installation-usap-programmer-with-ubuntu/

Short JP3 slow sck.

Or...

https://forum.arduino.cc/index.php?topic=363772.75

It always keeps reset on the target chip - after program, to test, you need to remove connection and power it from somewhere else.

Compavr for atmega8 chip programmed with usbasp:

avr-gcc -mmcu=atmega8 -Wall -Os -o $1.elf $1.c

avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex

# Fuses are for fast external oscilator like 12MHz !!! Not internal.

avrdude -c usbasp -p atmega8 -u -U hfuse:w:0xc9:m -U lfuse:w:0xef:m

avrdude -c usbasp -p atmega8 -U flash:w:$1.hex


0. Verify that you have a USBasp V2.0, and that it has a 12MHz crystal and an ATMEGA8 or ATMEGA8A MCU onboard. DO NOT CONNECT IT TO THE USB PORT OF YOUR COMPUTER.

1. Short the JP2 (self-programming) jumper.

2. Connect the USBasp V2.0 to the USBtinyISP using a 10-pin ribbon cable

Picture of pins:
http://www.scienceprog.com/building-and-installing-usbasp-usb-programmer-of-avr-microcontrollers/

Firmware:

http://blog.lincomatic.com/wp-content/uploads/2014/05/usbasp.atmega8.2011-05-28.zip

Commands that work:

avrdude -c avrusb500 -p atmega8 -u -U hfuse:w:0xc9:m -U lfuse:w:0xef:m

avrdude -c avrusb500 -p atmega8 -U flash:w:usbasp.atmega8.2011-05-28.hex

# other links

https://eagleup.wordpress.com/tutorial-v4/     - google sketchup allows many 3d board models in eagle

# connect to vnc with tunneling (the server behaves as running on the client)

First open port 22 and 5900 (or all of them) on the router on the network to enter.

x11vnc connects to already open Desktop, doesn not make a new one, I like that

On the server make a vnc password, avoid anyone entering vnc server without a password (port 5900):

x11vnc -storepasswd

on the client (not ssh-ed in) in separate terminals or with an & after the first one do:

ssh -t -L 5900:localhost:5900 root@IPTARGET 'x11vnc -forever -localhost -display :0 -ultrafilexfer -auth /home/pi/.Xauthority'

or with more caching that reduces network usage (but introduces vertically down images used for cache):

ssh -t -L 5900:localhost:5900 pi@IPTARGET 'x11vnc -forever -localhost -ncache 2 -ncache_cr -ncache_no_rootpixmap -display :0 -ultrafilexfer -auth /home/pi/.Xauthority'

xtightvncviewer -encodings "copyrect tight hextile" -quality 5 -compresslevel 5 localhost:0

(maximum 400 kB/s when there are a lot of image changes, 5 kB/s at a static image with blinking cursor)

(Raspberry Pi 2 CPU at 34 % at maximum, but that's also because of the program changing the image)

# Copy files securely on the internet over ssh:

From here to there a directory (-r means recursive):

scp -r /media/user/datadrive/videos root@IPTARGET:/media/pi/250gbdate/pi/

From there to here, a file:

scp root@IPTARGET:/media/pi/250gbdate/pi/file.tar.gz /media/user/ 

# Autostart a program in LXDE

Right click - add on desktop

file manager - ctrl + h

cd ./config

mkdir autostart

Drag and drop from the desktop in autostart.


# Working Samba share

# Also chech the newer samba2016 file in this folder

sudo -s
apt-get install -y samba samba-common python-glade2 system-config-samba
cp -pf /etc/samba/smb.conf /etc/samba/smb.conf.bak
cat /dev/null  > /etc/samba/smb.conf
systemctl stop apparmor
systemctl disable apparmor
firewall-cmd --add --service=samba

a) In terminal run (set password that Windows asks):
sudo pbedit -a pi

b) In terminal run:
mkdir /media/pi/EXT4DATE/dan/sambasharefolder

c) In terminal run:
chmod -R 0755 /media/pi/EXT4DATE/dan/sambasharefolder/
chown -R pi:pi /media/pi/EXT4DATE/dan/sambasharefolder/
chown pi:pi /media/pi/EXT4DATE/dan

d) nano /etc/samba/smb.conf
[global]
    workgroup = WORKGROUP
    dos charset = cp850
    unix charset = ISO-8859-1
    force user = pi
    force group = pi
    read only = no
    wins support = yes
    name resolve order = bcast host lmhosts wins

[sambasharefolder]
    path = /media/pi/EXT4DATE/dan/sambasharefolder
    writable = yes
    browseable = yes
    guest ok = no

e) Restart sharing service:
systemctl restart smbd


# Another smb.conf tested and it but bcast must be first (again) for it to work

[global]
	workgroup = WORKGROUP
	client code page = 850
	character set = ISO8859-1
	force user = pi
	force group = pi
	read only = no
	os level = 99
	wins support = yes
	name resolve order = bcast lmhosts hosts wins
	domain master = yes
	preferred master = yes
	dns proxy = yes
	time service = yes
	dos filetimes = yes
	dos filetime resolution = yes
	delete readonly = yes
	keep alive = 10
	max xmit = 4096
	status = no

[sambasharefolder]
	path = /media/pi/EXT4DATE/dan/sambasharefolder
	writable = yes
	browseable = yes
	guest ok = no
	

