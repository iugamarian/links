# Old motherboard not recognising new GPU card

https://forum-en.msi.com/index.php?topic=308673.0

https://imgur.com/a/bYYux

https://www.cclonline.com/product/265744/11289-01-20G/Graphics-Cards/Sapphire-Radeon-RX-590-NITRO-Special-Edition-8GB-Graphics-Card/VGA5403/

https://community.amd.com/community/gaming/blog/2016/04/05/radeon-r9-fury-nano-uefi-firmware

http://www.sapphiretech.com/productdetial.asp?pid=D5EDC78F-6920-4DDE-9EFF-6F35F8A68E27&lang=eng

It's hit and miss if both are not old or both are not new.

https://www.techpowerup.com/vgabios/


# Old sound card not recognised by Linux

https://help.ubuntu.com/community/HowToSetupSoundCards

BIOS - ADVANCED - PCIPnP - IRQ Manual - IRQ Auto - Save


# Slower mouse speed solution for Ubuntu 18.04

xinput --list

⎡ Virtual core pointer                    	id=2	[master pointer  (3)]

⎜   ↳ Virtual core XTEST pointer              	id=4	[slave  pointer  (2)]

⎜   ↳ Telink Wireless Receiver                	id=9	[slave  pointer  (2)]

⎜   ↳ Barcode Reader                          	id=12	[slave  pointer  (2)]

⎣ Virtual core keyboard                   	id=3	[master keyboard (2)]

    ↳ Virtual core XTEST keyboard             	id=5	[slave  keyboard (3)]
    

xinput --list-props 9

xinput --list-props 9 |grep Accel

xinput --list-props 9 |grep Speed


libinput Accel Speed (283):	0.000000

libinput Accel Speed Default (284):	0.000000

Avoid using Default.

Determine name of device "Telink Wireless Receiver" and property Accel Speed which

must be set to -1 for slowest, up to 1 for fastest with the following contraption:

As README.md hides some backspaces from this command it is in this repository as file:

mouseslower

And can be made permanent in Ubuntu Mate by putting it in /usr/bin as root and

sudo chmod +x /usr/bin/mouseslower

and creating preferences -> startup applications -> add as:

========================

Mouse Slower

mate-terminal -e "/usr/bin/mouseslower"

Slower mouse speed.

========================

And similarly for other desktop environments.


# scp convincing to copy from remote server with spaces and parantheses

https://unix.stackexchange.com/questions/64195/how-to-replace-a-left-parenthesis-with-sed

https://serverfault.com/questions/692364/scp-remote-file-to-local-machine

https://superuser.com/questions/1310766/scp-from-remote-linux-to-local-windows-with-spaces-in-local-path

https://stackoverflow.com/questions/19858176/how-to-escape-spaces-in-path-during-scp-copy-in-linux

https://www.funtoo.org/Sed_by_Example,_Part_2

https://stackoverflow.com/questions/6744006/can-i-use-sed-to-manipulate-a-variable-in-bash

Original name as presented by GUI:

aaabbb='A file with spaces (and parantheses).txt'

Add escape characters because client bash must be convinced to send correctly to server bash

bbbccc=$(echo $aaabbb |sed -e "s/(/\\\(/g"|sed -e "s/)/\\\)/g"|sed -r "s/ /\\\ /g")

Result becomes $bbbccc:

echo $bbbccc

A\ file\ with\ spaces\ \(and\ parantheses\).txt

Insert $bbbccc into scp request

scp -r user@example.com:"$bbbccc" ~/


# Arch Linux UEFI

https://wiki.archlinux.org/index.php/User:Soloturn/Quick_Installation_guide_UEFI


# USB good quality audio soundcards

http://raspberrypimaker.com/cheap-quality-audio-raspberry-pi/

I have the 3d sound, very good but has a red led near usb port that is annoying
and can be removed, works the same without the light after.

https://www.ebay.co.uk/itm/USB-To-3-5mm-Headphone-Jack-Stereo-Headset-3D-Sound-Card-Audio-Adapter-For-PC/311563006618?epid=1261136772&hash=item488a9a329a:g:QxoAAOSwP~tW39-l

Datasheets:
https://www.semiconductorstore.com/cart/pc/viewPrd.asp?idproduct=48472


# Debian to Windows USB stick

https://askubuntu.com/questions/599746/how-do-i-burn-the-windows-10-iso-to-a-usb

https://www.linuxbabe.com/ubuntu/easily-create-windows-10-bootable-usb-ubuntu

https://itsfoss.com/bootable-windows-usb-linux/


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

https://www.raspberrypi.org/forums/viewtopic.php?f=66&p=1224052


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

https://mpv.io/manual/master/

https://spit.mixtape.moe/view/73ac8ca5

# MPV 2018 best with AMD Polaris, configuration in ~/.config/mpv/mpv.conf:

# sudo apt install -y mesa-va-drivers

hwdec=vaapi

vo=vaapi

save-position-on-quit

audio-normalize-downmix=yes

volume=100

# MPV 2017 medium with AMD Polaris, configuration in ~/.config/mpv/mpv.conf:

# sudo apt install -y mesa-va-drivers

opengl-hwdec-interop=vaapi-copy

hwdec=vaapi-copy

vo=opengl

profile=opengl-hq

save-position-on-quit

audio-normalize-downmix=yes

volume=100

# MPV 2016 low with AMD, better with Nvidia, configuration in ~/.config/mpv/mpv.conf:

# install Nvidia proprietary driver

hwdec=vdpau

save-position-on-quit

audio-normalize-downmix=yes

volume=100

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

# Also check the newer samba2016 file in this folder

sudo -s

apt-get install -y samba samba-common python-glade2 system-config-samba

cp -pf /etc/samba/smb.conf /etc/samba/smb.conf.bak

cat /dev/null  > /etc/samba/smb.conf

systemctl stop apparmor

systemctl disable apparmor

firewall-cmd --add --service=samba

a) In terminal run (set password that Windows asks):

sudo pdbedit -a pi

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

# OpenWRT/LEDE Project - Samba Server 2018 install and setup

http://diantokam.blogspot.ro/2012/11/openwrt-samba-file-server.html

Install samba36-server and luci-app-samba

# opkg install samba36-server

# opkg install luci-app-samba

Edit /etc/config/samba

# vi /etc/config/samba
config samba

option name 'OpenWrt'

option workgroup 'WORKGROUP'

option description 'OpenWrt'

option homes '0'

config sambashare

option name 'dlna'

option read_only 'no'

option users 'diantokam'

option create_mask '0777'

option dir_mask '0777'

option path '/home/dlna'

option guest_ok 'yes'

config sambashare

option name 'hidden$'

option users 'diantokam'

option read_only 'no'

option create_mask '0777'

option dir_mask '0777'

option path '/home/old'

option guest_ok 'yes'

Edit /etc/passwd and add user diantokam,copy paste user nobody configuration, leave everything same as user nobody, except for user ID and group name.

# vi /etc/passwd

root:$1$nvFplbd8$l05HR0mdTHcGprNaMg8QA1:0:0:root:/root:/bin/ash

daemon:*:1:1:daemon:/var:/bin/false

network:*:101:101:network:/var:/bin/false

nobody:*:65534:65534:nobody:/var:/bin/false

diantokam:*:65535:65534:diantokam:/var:/bin/false

Create password for user diantokam

# passwd diantokam

Adding diantokam to samba user

# smbpasswd -a diantokam

Enable samba service on startup

# /etc/init.d/samba enable

To start samba service

# /etc/init.d/samba start

To stop samba service

# /etc/init.d/samba stop

To accesses samba share from windows type \\samba_server_name on address bar, in my case it is \\openwrt

# OpenWRT/LEDE Project - Samba Server 2018 firewalled only one ip allowed, also ssh and http (LuCI)

Combined from the following (each not working individually, only after merging all and testing):

http://mattventura.net/2009/08/17/a-mostly-complete-openwrt-tutorial/ - input_rule instead of INPUT

https://www.garron.me/en/bits/iptables-open-port-for-specific-ip.html - port to one IP

http://www.chiark.greenend.org.uk/~peterb/network/drop-vs-reject - drop saves upload speed

https://superuser.com/questions/924662/openwrt-iptables-rules-will-be-deleted-automatically - /etc/firewall.user

http://troy.jdmz.net/samba/fw/ - the ports used by Samba

Allow only one ip to ssh and samba no matter wan or lan, edit /etc/firewall.user

iptables -A input_rule -p tcp -s 192.168.1.101/32 --dport 22 -j ACCEPT

iptables -A input_rule -p tcp -s 192.168.1.101/32 --dport 80 -j ACCEPT

iptables -A input_rule -p udp -s 192.168.1.101/32 --dport 137 -j ACCEPT

iptables -A input_rule -p udp -s 192.168.1.101/32 --dport 138 -j ACCEPT

iptables -A input_rule -p tcp -s 192.168.1.101/32 --dport 139 -j ACCEPT

iptables -A input_rule -p tcp -s 192.168.1.101/32 --dport 445 -j ACCEPT

iptables -A input_rule -p tcp -s 0.0.0.0/0 --dport 22 -j DROP

iptables -A input_rule -p tcp -s 0.0.0.0/0 --dport 80 -j DROP

iptables -A input_rule -p udp -s 0.0.0.0/0 --dport 137 -j DROP

iptables -A input_rule -p udp -s 0.0.0.0/0 --dport 138 -j DROP

iptables -A input_rule -p tcp -s 0.0.0.0/0 --dport 139 -j DROP

iptables -A input_rule -p tcp -s 0.0.0.0/0 --dport 445 -j DROP


# Mounting samba shares

sudo apt install cifs-utils

sudo nano /etc/group

add your user name to the users line users:x:100:abc

sudo nano /etc/fstab

//192.168.1.50/500gbhdd /media/500gbhdd cifs username=abc,password=def 0 1

https://askubuntu.com/questions/33480/how-do-i-navigate-to-a-samba-location-ive-mounted-in-nautilus-on-the-command-li

https://askubuntu.com/questions/33480/how-do-i-navigate-to-a-samba-location-ive-mounted-in-nautilus-on-the-command-li

Upon adding myself to this group (sudo nano /etc/group, adding my name to the users line users:x:100:pi), I am now able to access the remote directories. My /etc/fstab file has these lines:

//xxx.220.131.233/user  /mnt/rua_user  cifs  username=abc,password=def 0 1
//xxx.220.131.233/all   /mnt/rua_all   cifs  username=abc,password=def 0 1

The rights on the folders look like this (ls -lh /mnt):

drwxrwx---  8 1016 users 0 2011-03-08 11:14 rua_all
drwxrwx--- 25 1016 users 0 2011-05-25 18:21 rua_user

# Using switches ensures less or no sniffing of packets

https://blog.packet-foo.com/2016/07/how-to-use-wireshark-to-steal-passwords/
