# Ubuntu improovements

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

# Test stereo speakers:

speaker-test -c 2 -t wav

# Test surround 5.1 speakers:

speaker-test -c 6 -t wav

# Wine installation for 3d games:

apt-get install -y wine-stable 

apt-get install -y winetricks 

WINEARCH=win32 winecfg

apt-get install -y wine-mono0.0.8 

apt-get install -y mono-runtime

WINEARCH=win32 winecfg

winetricks d3dx9_36

winetricks ddr=opengl multisampling=enabled orm=fbo videomemorysize=1024 strictdrawordering=enabled

winetricks dotnet20

winetricks dotnet40

For Wine 2.0 and onwards, DirectX 11 is getting supported. More on that later...


# Windows or DOS first if it is found by grub:

sudo -s

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
	

