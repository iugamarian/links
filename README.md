# Ryzen 5 decoding information

Debian 10.2 does not have "driver" in amdgpu for Ryzen 5 Picasso. Need to use Ubuntu 19.10.

https://www.ebay.com/itm/Acer-Aspire-3-A315-41-Hard-Drive-HDD-Connector-Cable-50-GY9N2-003/123470616803?hash=item1cbf6c18e3:g:ySEAAOSwOkxb4EgY:rk:1:pf:0

https://www.notebookcheck.net/Acer-Aspire-3-A315-41-Ryzen-3-2200U-Vega-3-SSD-FHD-Laptop-Review.306442.0.html

https://en.wikipedia.org/wiki/Unified_Video_Decoder#VCN_1

https://forums.plex.tv/t/hardware-decoding-not-working-win-10-radeon-vega-8-gpu/231752/3

https://www.notebookcheck.net/UHD-Graphics-630-vs-Vega-8-vs-Vega-10_8126_8144_8142.247598.0.html

https://superuser.com/questions/1324323/how-to-disable-secure-boot-on-an-acer-aspire-3-laptop

Specific steps
Completely shut down your laptop.

Power on the system and as soon as the first logo screen appears, immediately press F2 to enter the BIOS.

Use the right arrow key to select Security tab.

Use the down arrow key to highlight Set Supervisor Password and press Enter.

Type some password and press Enter. Don't forget the password you enter!

Retype the password to confirm and press Enter again.

Use the right arrow key to select Boot tab.

Press the down arrow key to select Secure Boot and press Enter.

With the arrow key, highlight Disabled and press Enter.

Press the F10 key and select Yes to save the changes and exit the BIOS.

https://askubuntu.com/questions/326662/which-partition-to-select-as-device-for-boot-loader-in-an-efi-mode-install

While with BIOS/MBR systems you install to the MBR and almost never to a partition, with UEFI you always install to the efi partition. It actually should default to install to that partition anyway and you can only have one efi partition (with boot flag) per drive.

In your case installing grub to sda3 the efi partition is correct. You should see multiple folders for each system you have installed, in Ubuntu they are mounted at /boot/efi and on the drive you have ubuntu & Microsoft folders with boot files:

/boot/efi/EFI/ubuntu/grubx64.efi /boot/efi/EFI/Microsoft/Boot/bootmgfw.efi

Grub2's os-prober has a bug and only creates BIOS boot entries which do not work with UEFI. You do not chainload to the Windows install like BIOS, but chain load to the efi partition. Boot-Repair can automatically create entries in 25_custom or you can manually add entries as shown in bug report to your 40_custom.

grub2's os-prober creates wrong style (BIOS) chain boot entry https://bugs.launchpad.net/ubuntu/+source/grub2/+bug/1024383 type of entry from Boot-Repair that should work. menuentry "Windows UEFI bkpbootmgfw.efi" { menuentry "Windows Boot UEFI loader" { Type of entry that does not work: 'Windows ...) (on /dev/sdXY)' Some info in Post #3 on cleaning up menus, if desired. http://ubuntuforums.org/showthread.php?t=2085530

# Raspberry Pi VPN and iptables

https://www.comparitech.com/blog/vpn-privacy/raspberry-pi-vpn/


# Best available router for OpenWRT having no multiple risky versions

https://openwrt.org/toh/asus/asus_rt-ac57u


# Raspberry Pi Raspbian has Real VNC

https://www.deviceplus.com/how-tos/how-to-make-a-vnc-server-on-raspberry-pi/


# Power supply for TP-Link SG1016 that outputs 12V 2A

https://www.yoycart.com/Product/524147033928/

https://itbbs.pconline.com.cn/diy/15217927.html


# Windows 10 disable fast boot and clone drive on same PC, Legacy and UEFI

https://somoit.net/windows/windows-cannot-shrink-volume-unmovable-files

http://wiki.christophchamp.com/index.php?title=Dd_(command)

https://en.wikipedia.org/wiki/Dd_(Unix)

dd if=/dev/zero of=path/to/file bs=512 count=1 conv=notrunc

The notrunc conversion option means do not truncate the output file — that is, if the output file already exists, just replace the specified bytes and leave the rest of the output file alone. Without this option, dd would create an output file 512 bytes long. 

https://www.linux.com/tutorials/gnu-ddrescue-best-damaged-drive-rescue/

https://help.uaudio.com/hc/en-us/articles/213195423-How-To-Disable-Fast-Startup-in-Windows-10

https://win10faq.com/how-to-clone-windows-10-with-clonezilla/

https://pureinfotech.com/clone-windows-10-drive-clonezilla/

https://www.tecmint.com/migrate-windows-10-from-hdd-to-ssd-using-clonezilla/

https://www.quora.com/If-you-clone-the-OS-hard-drive-will-Windows-require-a-new-license

https://www.tigraine.at/2014/01/04/how-to-upgrade-to-an-ssd-with-minimal-effort

https://www.technibble.com/forums/threads/gnu-ddrescue-ridiculous-learning-curve.34594/

For harddisk to 240 GB SSD we know it actually has about 220 GB and we should leave at least 10 % free

for wear levelling so 200 GB and we should not stop or shorten the output if bad sector read error,

harddisk being larger first we shrink Windows C drive to 190 GB, then start Clonezilla, enter command prompt:

lsblk

#/dev/sda is large but partitions on it are under 200GB seen in Windows Disk Management, /deb/sdb is about 220GB

dd iflag=fullblock if=/dev/sda of=/dev/sdb bs=1M count=204800 conv=noerror,sync status=progress


# Raspbian Raspberry Pi BTRFS with compression

https://hackmd.io/FP-7sHiPTJGaJvzSa3nw8A

https://hackmd.io/@yOoDLyRxTU2S4S-obb7NpQ/HJOFQphWW?type=view

https://hackaday.com/2017/06/18/btrfs-for-the-pi/

https://syonyk.blogspot.com/2018/03/project-pi3desk-building-awesome-pi3.html

https://github.com/sakaki-/genpi64-overlay/blob/master/sys-apps/rpi3-init-scripts/files/init.d_autoexpand_root-4

https://github.com/sakaki-/gentoo-on-rpi-64bit

https://github.com/sakaki-/gentoo-on-rpi-64bit/blob/master/configs/bcmrpi3_defconfig

# Gentoo Raspberry Pi fix keyboard issues ( UK to US )

https://linoxide.com/linux-how-to/fix-error-locale-cannot-set-lc_all-to-default-locale-no-such-file-or-directory/

https://wiki.gentoo.org/wiki/Handbook:AMD64/Installation/System#Init_and_boot_configuration

su -

echo "Europe/Bucharest" > /etc/timezone

emerge --config sys-libs/timezone-data

nano -w /etc/conf.d/keymaps

=====================

keymap="us"

=====================

export LC_ALL=en_US.UTF-8

nano  /etc/locale.gen

=====================

en_US.UTF-8 UTF-8

=====================

locale-gen

sync

locale -a

reboot


# Gentoo Raspberry Pi graphical login VNC

https://github.com/sakaki-/gentoo-on-rpi-64bit/wiki/Run-a-Remote-VNC-Desktop-on-your-RPi

https://wiki.gentoo.org/wiki//etc/local.d

genpi64 instead of rpi3 for repository

DO NOT ADD tigervnc service to default runlevel - it gets unstable because it is starting too fast !!!

Configure ssh to be more permissive on the client and on the Raspberry Pi

http://www.uptimemadeeasy.com/networking/connect-blocked-ports-ssh-tunneling/

https://superuser.com/questions/346971/ssh-tunnel-connection-refused

https://wiki.gentoo.org/wiki/SSH_tunneling

https://askubuntu.com/questions/539937/how-may-i-do-a-ssh-tunneling

nano /etc/ssh/sshd_config

=====================

AllowTcpForwarding yes

GatewayPorts yes

X11Forwarding yes

X11UseLocalhost yes

PermitUserEnvironment yes

=====================

reboot

START GRAPHICAL LOGIN AFTER RASPBERRY PI IS TURNED OFF:

1) START RASPBERRY PI

2) WAIT 4 MINUTES - starting all services

ssh demouser@iprpi

sudo rc-service tigervnc start

3) WAIT 3 MINUTES - starting a second X session for display :1 in the background

sync

exit

4) Do not exit this console after the command to maintain tunelling link:

ssh -L 5902:localhost:5901 demouser@iprpi

ssh -L 5902:127.0.0.1:5901 demouser@iprpi         <<<<<<<   not working !!!

5) Open a second terminal, leave first one open, then in the second terminal:

xtightvncviewer 127.0.0.1:5902

=====================

Performing standard VNC authentication

Password: 

Authentication successful

=====================

6) WINDOW OPENS - COMPLETED GRAPHICAL LOGIN

7) Access Raspberry Pi Share folder:

ON THE RASPBERRY PI

mkdir -p /home/demouser/share

ON THE CLIENT

mkdir -p /home/pi/sshfsmnt

sshfs demouser@iprpi:/home/demouser/share /home/pi/sshfsmnt


In case of problems: they are created by apparmor or ssh tunneling, tigervnc is ok - test with no tunelling.

On server troubleshoot:

1) Uninstall apparmor if available on client and Raspberry Pi

2) Linux cmdline apparmor=0 on client and Raspberry Pi

3) Disable tigervnc service

sudo rc-update del tigervnc default

reboot

ps -ef |grep 5901

=====================

only bash shell, no VNC server

=====================

4) update OS on client that accesses Raspberry Pi because maybe security fix for keys

5) change tunnel port to target port to plus a few because maybe old port has old key configured


6) On Raspberry Pi:

sudo rc-service tigervnc start

sudo ps -ef |grep 5901

7) On client:

ssh -L 5902:localhost:5901 demouser@iprpi

xtightvncviewer 127.0.0.1:5902

NOT OK ?

On Raspberry Pi:

sudo rc-service tigervnc start

sudo ps -ef |grep 5901

On client:

ssh -L 5903:localhost:5901 demouser@iprpi

xtightvncviewer 127.0.0.1:5903

5904

5905

.

.

.

590n

9) If it is fixed, on Raspberry Pi do not enable tigervnc service:

sudo rc-update add tigervnc default      < < < < < <  D O   N O T

sudo reboot


# Wayland on Debian, make Synaptic work (not tested yet)

https://bugs.launchpad.net/ubuntu/+source/synaptic/+bug/1731324/comments/4

Wayland is designed to not allow elevated (sudo -H, gksu ...) permissions with GUI application programs.

There is a workaround to allow graphical application programs owned by other users than the logged in user in Wayland:

xhost +si:localuser:root

Take a look at: https://wiki.ubuntu.com/Wayland/gks


# Nvidia Chromium vdpau accelerated h264 decoding on Arch Linux

 You need to install linux-lts linux-lts-headers linux-lts-docs and run:

grub-mkconfig -o /boot/grub/grub.cfg

 You need to compile as user and install as root some AUR packages with these steps:
 
 - creade a folder
 
 - put a PKGBUILD text file inside
 
 - run:
 
 cd
 
 cd folder
 
 makepkg -Acs
 
 - see what files are missing - patches, you can see them at Sources section on the AUR site for the package
 
 - create text files for the missing patches with the name in the Sources section and fill them
 
 makepkg -Acs
 
 pacman -U *.xz
 
 This are the AUR packages required:
 
 https://aur.archlinux.org/packages/chromium-vaapi-bin/ or without -bin
 
 https://aur.archlinux.org/packages/libva-vdpau-driver-chromium/
 
 This are the required "main repository" packages:
 
 pacman -Sy
 
 pacman -S libva-mesa-driver mesa-vdpau vdpauinfo libvdpau-va-gl 
 
 With X server off:
 
 pacman -R lxdm
 
 pacman -R gdm
 
 Install for Nvidia Geforce 8800GT and 9600GSO this:
 
 wget http://us.download.nvidia.com/XFree86/Linux-x86_64/340.107/NVIDIA-Linux-x86_64-340.107-no-compat32.run
 
 chmod +x NV*
 
 ./NV*
 
 Start Chromium, add extension h264ify
 
 https://askubuntu.com/questions/336302/gpu-accelerated-chromium
 
 
    Open the about:flags in your address bar
    
    Look for Override software redendering list
    
    Click "Enable"
    
    Restart Chrome/ium
 
 https://wiki.archlinux.org/index.php/pacman
 
 The nvidia.ko module installed with .run file will need the exact kernel to be kept, not updated
 
nano /etc/pacman.conf
    
IgnorePkg=linux

IgnorePkg=linux-headers

IgnorePkg=linux-docs

IgnorePkg=linux-lts

IgnorePkg=linux-lts-headers

IgnorePkg=linux-lts-docs

 
 That's it. Tested and it works. To test that games work well:
 
 pacman -S warzone2100
 
 warzone2100


# Nvidia proprietary Linux driver get bigger fonts

sudo -s

nvidia-xconfig --no-use-edid-dpi

nano /etc/X11/xorg.conf

[...]

Section "Monitor"

Identifier "Monitor0"

VendorName "Unknown"

ModelName "Unknown"

HorizSync 28.0 – 33.0

VertRefresh 43.0 – 72.0

Option "DPI" "96 x 96"     or    Option "DPI" "128 x 128"     be careful about the quotes on copy-paste

[...]


# Hacking Google FRP on Vodafone phones

Working:

1 - install "password libraries"

https://www.apkmirror.com/apk/google-inc/google-account-manager/google-account-manager-6-0-1-release/google-account-manager-6-0-1-android-apk-download/

2 - install "password invoker"

https://frpbypass.download/frp-bypass-apk/

2.5 - (maybe) in Settings - Security - Device administrators - Enable permissions for Google Account Manager

3 - open FRP Bypass

4 - press three dots, Browser sign-in

5 - login with a real Google accont, accept, next, etc.

6 - don't write in the "Email" box, instead restart the phone

7 - after internet you have message "Account added" - press Next, wait "Just a sec..."

8 - Not now, next, put your settings, Don't restore, next, next, next, next

9 - The phone considers you it's master now. Across factory resets also. That's it. 

Working video:

https://www.youtube.com/watch?v=cG16UFq7ViQ


For documentation, but each not actually working:

https://www.youtube.com/watch?v=BGcomr3z75Y

first link: https://www.dropbox.com/s/sd2w2ecq26nzy6y/apk.apk?dl=0

second link: https://www.apkmirror.com/apk/sika524/quickshortcutmaker/quickshortcutmaker-2-4-0-release/quickshortcutmaker-2-4-0-android-apk-download/

from second link app I deduced (something) Google Account Manager was not installed

https://www.youtube.com/watch?v=Yfzu_nMBKMk

https://apex-launcher.en.uptodown.com/android/download


# Video 5 minutes 2 seconds - 2 x 24 hour to 2 x 9 minutes and 36 seconds

https://drive.google.com/open?id=1q9QpxAYUR4zCtuv1vRMk-Ihi6gXvwV2b


# Debian experimental for select packages

https://www.phoronix.com/forums/forum/phoronix/latest-phoronix-articles/1111742-debian-11-bullseye-cycle-prepares-to-begin-long-journey/page2

Just to be clear though, experimental isn't a complete distribution. It's just an extra repository where maintainers can upload the latest stuff without immediately pushing it into unstable. It's meant for users of unstable who wants to pull in the latest of only some specific package. Recently I've used it for getting the latest mesa while Debian has been frozen these last few months, and it's great to have such a simple way to do that. Just don't try going into your apt sources list and change unstable to experimental. That won't work because experimental isn't an entire distribution in itself the way unstable and stable is. The way you do it is you have both unstable and experimental in your sources list, and whenever you want to install an experimental package you append '-t experimental' to apt, otherwise experimental packages will be ignored.

Example if I want latest gnome-shell and mutter and already have added the repo to my sources
Code:

sudo apt -t experimental install gnome-shell mutter

That way, nothing but gnome-shell and mutter will be from experimental. Everything else in my distribution will still be from unstable. 

# android block phones

https://www.androidauthority.com/how-to-block-a-phone-number-246484/

http://andauth.co/fdmcms


# btrfs raid 1 for devices with some checksum errors

https://btrfs.wiki.kernel.org/index.php/Using_Btrfs_with_Multiple_Devices#Adding_new_devices

https://wiki.archlinux.org/index.php/Btrfs#Preparation

https://wiki.gentoo.org/wiki/Btrfs/System_Root_Guide

https://askubuntu.com/questions/32418/is-there-an-easier-way-to-chroot-than-bind-mounting

https://bartsimons.me/ubuntu-linux-chroot-guide/

-boot (netinst or cd or dvd) Debian GNU/Linux

-Enter rescue mode drop to shell root

-connect second device of similar size (not required to be exact size)

-run this commands as root

fdisk /dev/sda1

o n ENTER ENTER ENTER t 83 w

fdisk /dev/sdb1

o n ENTER ENTER ENTER t 83 w

mkfs.btrfs -d raid1 -m raid1 /dev/sda1 /dev/sdb1

-reboot and begin installing on the raid

-grub installation will fail - select continue without boot loader

-Debian boot - advanced - rescue mode

-Open a shell in the installer enviroment

cat /proc/partitions

mount /dev/sda1 /mnt

-if that has an error: mount /dev/sdb1 /mnt

-if that has an error: mount /dev/sdc1 /mnt

-if still error stop here - failed install

for f in proc sys dev ; do mount --bind /$f /mnt/$f ; done

chroot /mnt

apt install -y grub2

grub-install /dev/sda

update-grub

nano /etc/fstab

...   btrfs  defaults,noatime,autodefrag   ...

sync

exit

poweroff

===========================================

On already existing non raid install high risk of not working:

btrfs device add -f /dev/sdb1 /

btrfs balance start -dconvert=raid1 -mconvert=raid1 /

ERROR: error during balancing '/': Input/output error
There may be more info in syslog - try dmesg | tail

BTRFS warning (device sda1): csum failed root

===========================================


# p5ad2-e Premium fix sound

su -s

nano /etc/modprobe.d/alsa-base.conf

At the end:

options snd-hda-intel model=6stack-digout

options snd-hda-intel power_save=0

Then set pulseaudio to avoid HDMI audio output:

https://bbs.archlinux.org/viewtopic.php?id=180102

echo "set-card-profile 1 output:analog-stereo+input:analog-stereo" >> /etc/pulse/default.pa

sync

Restart.

If still not working, try:

apt remove --purge pulseaudio

# Old PC hardware accelerated decoding with capable video card in Chromium web browser (needs tweaking)

https://forums.fedoraforum.org/showthread.php?260198-Compile-FFMPEG-with-VDPAU-support

https://superuser.com/questions/250437/how-to-pass-custom-options-to-configure-when-building-a-package-with-debuild

https://www.pcsuggest.com/chromium-hardware-accelerated-video-decoding-linux/

https://www.linuxuprising.com/2019/01/ubuntu-testing-chromium-snap-with-vaapi.html

https://askubuntu.com/questions/336302/gpu-accelerated-chromium

https://wiki.archlinux.org/index.php/Chromium#Hardware_video_acceleration

https://bbs.archlinux.org/viewtopic.php?id=244031

https://askubuntu.com/questions/336302/gpu-accelerated-chromium


# Better than FAT32 for TV's and Android: exFAT allows more than 4 GB for a file

https://appuals.com/how-to-format-a-drive-as-exfat-on-linux/

wipefs -a /dev/sdc

fdisk /dev/sdc

n ENTER ENTER ENTER ENTER ENTER t 7 w

mkfs.exfat -n hardDisk /dev/sdc1


# Debian easy install some desktop from minimal installation

https://wiki.debian.org/tasksel

https://packages.debian.org/buster/task-mate-desktop

su -

tasksel

tasksel --list-tasks

tasksel --task-packages task-mate-desktop


# Debian run graphical applications as root

https://help.ubuntu.com/community/RootSudo#Graphical_sudo

You can use pkexec on those flavours that support this option. As of 18.04, only Xubuntu supports this option by default, as shown in the following examples:

    pkexec thunar
    pkexec mousepad /etc/fstab

https://unix.stackexchange.com/questions/203136/how-do-i-run-gui-applications-as-root-by-using-pkexec

https://www.linuxquestions.org/questions/linux-desktop-74/linux-debian-9-gui-don%27t-accept-sudo-and-asks-for-root-password-4175611640/


# Unlock locked phone and make it work on any other network(s) with 4G and calls

01 - unlock phone for 0.99 pounds at

https://www.ebay.co.uk/sch/mobunlockingmaster/m.html?_nkw=&_armrs=1&_ipg=&_from=

If code does not work don't consume your tries, send another question about item with

"Code does not Work. Please can you give another code ?". They usually give another code that works 100%

02 - uninstall locked caller/dialer app that does not want to work on other networks

03 - install New Dialer,Ultimate Dialer,Superb Dialer, Contacts BITS INDIA

https://play.google.com/store/apps/details?id=n_com.android.dialer

04 - open Phone = New Dialer,Ultimate Dialer,Superb Dialer, Contacts BITS INDIA

05 - Download APK specified by the app - required to have call history also

06 - Settings - Apps - uninstall Phone = New Dialer,Ultimate Dialer,Superb Dialer, Contacts BITS INDIA

07 - Settings - Security - Allow Unknown Sources

08 - Install downloaded APK

-- You can now make calls and have call history and contacts

09 - Uninstall messages (SMS) app

09 - install Google Messages https://play.google.com/store/apps/details?id=com.google.android.apps.messaging 

10 - open it, update required apps, open it again and make it default for messages

-- You can now send SMS messages

11 - open Chrome, update Chrome, open it, go to Settings - Home page - https://www.google.com - Save

-- You can now browse the web fast


# Firefox in RAM only

https://wiki.archlinux.org/index.php/Firefox/Profile_on_RAM


# Raspberry Pi disable swap

http://ideaheap.com/2013/07/stopping-sd-card-corruption-on-a-raspberry-pi/

sudo -s

dphys-swapfile swapoff

dphys-swapfile uninstall

systemctl disable dphys-swapfile

reboot


# Fast Raspberry Pi remote desktop not on the same display:

# AND PROTECT IT from too many authentification failures from internet

https://www.cyberciti.biz/tips/linux-iptables-how-to-specify-a-range-of-ip-addresses-or-ports.html

https://serverfault.com/questions/161401/how-to-allow-a-range-of-ips-with-iptables

sudo -s

iptables -A INPUT -p tcp -m iprange --src-range 192.168.1.2-192.168.1.254 --dport 22 -j ACCEPT

iptables -A INPUT -p tcp -m iprange --src-range 192.168.1.2-192.168.1.254 --dport 5901 -j ACCEPT

iptables -A INPUT -p tcp -s 0.0.0.0/0 --dport 22 -j DROP

iptables -A INPUT -p tcp -s 0.0.0.0/0 --dport 5901 -j DROP

exit

https://raspberrypi.stackexchange.com/questions/28149/tightvnc-connection-refused

https://raspberrypi.stackexchange.com/questions/45914/raspbian-graphical-login-screen-stuck-cant-login

https://superuser.com/questions/696840/is-there-a-way-to-get-tightvnc-to-host-the-native-display-0

https://elinux.org/RPiconfig

https://ubuntuforums.org/showthread.php?t=1289897

On the Pi:

sudo -s

raspi-config -> Interfacing options -> SSH -> Enable SSH -> TAB -> Finish

apt install -y tightvncserver

chown -R pi /home/pi

OR

chown pi:pi .Xauthority

On the PC start tightvncserver on the Pi:

ssh pi@192.168.x.x

tightvncserver :1 -geometry 800x600 -depth 24

exit

On the PC graphical login on the Pi (server is already started):

xtightvncviewer 192.168.1.2:5901

KEEP TERMINAL OPEN

# GOOD NEWS - Snapraid also protects against bitrot in parity files not just data files

# Very good protection against bitrot

https://forum.level1techs.com/t/idea-for-a-highly-flexible-nas/108575

https://sourceforge.net/p/snapraid/discussion/1677233/thread/7167759c/

2018-04-03

Fix all the files and the parity data.

All the files and the parity data are compared with the snapshot state saved in the last "sync". If a difference is found, it's reverted to the stored snapshot.

The "fix" command doesn't differentiate between errors and intentional modifications. It unconditionally reverts the file state at the last "sync".

https://sourceforge.net/p/snapraid/discussion/1677233/thread/31bc9de8/

Down in the thread:

2013-12-15

Beta works perfect! I've modified a byte in parity, scrub found 1 error and fix -e fixed it without problems.


# Par2 maximum continuous (.tar) archive file size is 1 TB

# 1 MB block size => 32 GB maximum for Usenet posts

# 32 MB block size => 32 x 32 GB = 1024 GB maximum for backups

https://news.ycombinator.com/item?id=13615978

 Thanks for the heads up but that's not the one. I posted it in 1998 or 1999 and I tried to find the exact Usenet archive link similar to the direct link for Larry Page's famous 1996 post on comp.lang.java

To go back to the article, the author mentions posting the files to newsgroup "alt.binaries.backup". With Usenet, there isn't exactly a contractual SLA (Service Level Agreement) for that group. It's a gentlemen's agreement between those commercial Usenet providers (and non-commercials ones like universities) to replicate messages. Maybe because I posted the message to my GTE/Verizon ISP's Usenet server meant that it only got replicated to a few peers and it "died".

If my tiny text-based post which is 2 years newer than Larry Page's can't be recovered today, it doesn't give me lot of confidence to use Usenet as a backup solution. I have over 1 terabyte of photos, home videos, and tif scans of all my paper files. It's not appealing to chop that 1TB into a thousand PAR2 files with extra 20% redundant parity and posting it to alt.binaries.backup. That seems extremely fragile. Another commenter made a suggestion for Amazon's new personal "unlimited" cloud for $60/year. That seems much more reliable.


> It's not appealing to chop that 1TB into a thousand PAR2 files with extra 20% redundant parity and posting it to alt.binaries.backup.

For a 1 TB archive with 20% redundancy, you're looking at a block size of at least 32 MB in each par2 file (due to the maximum block count of 32767 in the official implementation). Given that the article size limit for many news servers is roughly 1 MB, you're looking at even a single block getting split into 32 article posts. par2 programs typically will generate a series of files where the smallest files contain a single block and the largest files contain 50 or more blocks. The 50 block files will each get split into 1600 articles.

For par2 recovery to work even when articles are missing, you really want the recovery block size to be less than the article size limit, so that even if one or more articles are missing, the par2 archiver program can still read a subset of blocks from the incomplete recovery file and still use them for recovery. That means that the maximum archive size would be roughly 32 GB to keep the block size under the article size limit.

Going beyond that size means that it's less likely that the recovery file would be usable if some of the articles are missing. At 32 GB, if one article is missing from a 3 block recovery file, the software will still be able find 2 blocks in that file. But, if the archive size was 100 GB, then the block size would be a minimum of 3 MB and just missing 3 out of 9 articles that make up a 3 block recovery file would make the recovery file unusable.


# e4defrag segfaults strlen.S on Debian Buster 10 i386 so use XFS or BTRFS instead for /

# XFS defragmentation

https://www.rootusers.com/how-to-defragment-an-xfs-file-system/

http://fibrevillage.com/storage/673-how-to-defragment-an-xfs-filesystem

https://linuxhint.com/defragment_xfs_file_system/

https://wiki.archlinux.org/index.php/XFS

https://www.thegeekdiary.com/running-repairs-on-xfs-filesystems/

http://xfs.org/index.php/XFS_FAQ

https://opensource.com/article/18/4/ext4-filesystem

https://arstechnica.com/information-technology/2014/01/bitrot-and-atomic-cows-inside-next-gen-filesystems/

https://www.funtoo.org/BTRFS_Fun

xfs_check /dev/sda1

xfs_repair /dev/sda1

xfs_db -c frag -r /dev/sda1

xfs_fsr /dev/sda1

xfs_db -c frag -r /dev/sda1

# BTRFS defragmentation (for not encountering up to about year 2022 BTRFS bugs avoid doing snapshots, reflinks, compression and RAID 5/6)

https://stackoverflow.com/questions/26429360/crc32-vs-crc32c

BTRFS uses CRC32C with polynomial 0x1EDC6F41, hardware accelerated in newer CPUs and quite good for bitrot detection, if you also have RAID1 mirror to repair automatically after detection even if it's a whole 4 KB sector missing which BTRFS does.

https://news.ycombinator.com/item?id=8304217

http://noahdavids.org/self_published/CRC_and_checksum.html

https://www.youtube.com/watch?v=W3QRWUfBua8&feature=youtu.be&t=17m51s

https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=840248

https://btrfs.wiki.kernel.org/index.php/Changelog

https://lwn.net/Articles/577961/

https://lwn.net/Articles/579009/

https://github.com/egara/arch-btrfs-installation

https://btrfs.wiki.kernel.org/index.php/Status

https://chrisirwin.ca/posts/btrfs-presentation/

https://askubuntu.com/questions/229770/how-do-i-show-the-current-amount-of-fragmentation-on-a-btrfs-filesystem

https://serverfault.com/questions/896535/possible-to-see-btrfs-defrag-progress

# BTRFS has complete toolset including defragmentation in Debian Install netinst Rescue Mode, XFS and e4defrag lack defragmentation and many others in Rescue Mode

https://wiki.archlinux.org/index.php/Btrfs#Defragmentation

https://wiki.debian.org/Btrfs

https://patchwork.kernel.org/patch/3554661/

https://www.spinics.net/lists/linux-btrfs/msg78524.html

https://borgbackup.readthedocs.io/en/stable/

Swap files in Btrfs are supported since Linux 5.0

Displaying used/free space

General linux userspace tools such as df will inaccurately report free space on a Btrfs partition. It is recommended to use btrfs filesystem usage to query Btrfs partitions. For example:

btrfs filesystem usage /

Note: The btrfs filesystem usage command does not currently work correctly with RAID5/RAID6 RAID levels.

Btrfs supports online defragmentation through the mount option autodefrag. To manually defragment your root, use:

btrfs filesystem defragment -r /

Using the above command without the -r switch will result in only the metadata held by the subvolume containing the directory being defragmented. This allows for single file defragmentation by simply specifying the path.

Defragmenting a file which has a COW copy (either a snapshot copy or one made with cp --reflink or bcp) plus using the -c switch with a compression algorithm may result in two unrelated files effectively increasing the disk usage.

Parity RAID (RAID 5/6) code has multiple serious data-loss bugs in it.

Bitrot protection (repair) available if you use well tested RAID 1 (mirror) or RAID 10 (mirror striped)

Bitrot protection (repair) can be done if you use two partitions of the same size on the same one harddisk / ssd and put them in RAID 1 (mirror) or maybe even one partition with RAID1 made just inside it:

# BTRFS No space left on device

From live rescue mode enter a shell withowt chroot:

cat /proc/partitions

mount /dev/sdxx /mnt

cd /mnt

btrfs fi balance start -dusage=5 /mnt

rm /mount/point/var/cache/apt/archives/*.deb

btrfs fi balance start --full-balance /mnt

btrfs filesystem defragment -r /mnt


# But you use mirrored so half space half performance

https://btrfs.wiki.kernel.org/index.php/Using_Btrfs_with_Multiple_Devices

https://www.complang.tuwien.ac.at/anton/btrfs-raid1.html

https://seravo.fi/2016/perfect-btrfs-setup-for-a-server

Installing RAID1 for BTRFS
Debian does not support Btrfs RAID out of the box, so the way to go is to start to install BTRFS without RAID on one of the disk drives, leave the same space on a partition on the other drive(s), and then do

btrfs device add /dev/sdb3 /
btrfs balance start -dconvert=raid1 -mconvert=raid1 /

We also add "degraded" as file system option in fstab; e.g.:

UUID=3d0ce18b-dc2c-4943-b765-b8a79f842b88 /               btrfs   degraded,strictatime        0       0

btrfs device delete missing tells btrfs to remove the first device that is described by the filesystem metadata but not present when the FS was mounted.
...
For example if you have a raid1 layout with two devices, and a device fails, you must:

    mount in degraded mode.
    add a new device.
    remove the missing device.

The UUID (check with blkid) is the same for both partitions in our RAID, so no need to specify devices.
EFI and RAID1
There is no support for RAID1 and EFI in Linux or Debian, so what we did was to have one EFI system partition (ESP) on each drive, let the Debian installer install the the EFI stub of grub on one of them, and then use dd to copy the contents of the ESP to the other ESP partition(s):

dd if=/dev/sda1 of=/dev/sdb1

This has to be repeated every time the EFI partition changes, but it seems that this normally does not change, even when running update-grub. OTOH, it does not hurt to do the dd more often than necessary.

We also needed to change /etc/grub.d/10_linux in different places than "The perfect Btrfs setup for a server" (which seems to be written for a BIOS/MBR system) indicates: Search for " ro " (two occurences), and prepend "rootflags=degraded". One of these lines becomes

	linux	${rel_dirname}/${basename}.efi.signed root=${linux_root_device_thisversion} rootflags=degraded ro ${args}

In order for that to take effect, we had to

update-grub

What to do on a failed disk
We disconnected one of the disks (while the system was offline, online would have been an interesting variant) to simulate a disk failure. Due to a bug in BTRFS, it degrades nicely on the first boot, but then becomes irreversibly read-only on the second boot. If you get there, the best option seems to be to copy the read-only file system to a fresh and writable file system (with e.g., tar or cpio). (We actually went as far as having the read-only file system with one of the two drives we used, so the bug is still there in Linux 4.11).

You probably want to avoid these complications, and while you are still in your first boot, you can. What we did (with the other disk), is to convert it back from RAID1 to a single profile, then remove the failed device (which complains that it does not know the device, but still removes it).

btrfs balance start -v -mconvert=dup -dconvert=single /
btrfs device remove /dev/sdb3
#now check that it has worked
btrfs device usage /
btrfs fi show
btrfs fi usage

We then shut down the system, plugged the replacement disk in (actually the disk we had earlier ruined by double degraded booting, after wiping the BTRFS partition), booted and then did the usual dance to turn the now-single BTRFS into a RAID1 again:

btrfs device add /dev/sdb3 /
btrfs balance start -dconvert=raid1 -mconvert=raid1 /

As a result, we had a RAID1 again.

If you wonder why we did not use btrfs replace: We would have to connect the new disk before the second reboot, which is not always practical. With the method above, once we have rebalanced the file system to a single one, we can reboot as often as we like to get the new drive online. 

# BTRFS scrub stopped half way not running but says it's running bug

http://marc.merlins.org/perso/btrfs/post_2014-04-26_Btrfs-Tips_-Cancel-A-Btrfs-Scrub-That-Is-Already-Stopped.html

btrfs scrub start -d /dev/mapper/dshelf1

ERROR: scrub is already running.

To cancel use 'btrfs scrub cancel /dev/mapper/dshelf1'.

btrfs scrub status  /dev/mapper/dshelf1

scrub status for 6358304a-2234-4243-b02d-4944c9af47d7

scrub started at Tue Apr  8 08:36:18 2014, running for 46347 seconds

total bytes scrubbed: 5.70TiB with 0 errors

btrfs scrub cancel  /dev/mapper/dshelf1

ERROR: scrub cancel failed on /dev/mapper/dshelf1: not running

# Fix:

perl -pi -e 's/finished:0/finished:1/' /var/lib/btrfs/*

btrfs scrub status  /dev/mapper/dshelf1

scrub status for 6358304a-2234-4243-b02d-4944c9af47d7

scrub started at Tue Apr  8 08:36:18 2014 and finished after 46347 seconds

total bytes scrubbed: 5.70TiB with 0 errors

btrfs scrub start -d /dev/mapper/dshelf1

scrub started on /dev/mapper/dshelf1, fsid 6358304a-2234-4243-b02d-4944c9af47d7 (pid=24196)

# BTRFS deal with databases and virtual harddisks without NOCOW

marc.merlins.org/linux/talks/Btrfs-LC2014-JP/Btrfs.pdf

If you have a virtual disk image, or a database file that gets written randomly in the middle,

Copy On Write is going to cause many fragments since each write is a new fragment.

My virtualbox image grew 100,000 fragments quickly.

You can turn off COW for specific files

and directories with chattr +C /path (new files will inherit this)

Or you can defragment the file:

btrfs filesystem defragment vbox.vdi

(could take hours)

cp –reflink=never vbox.vdi vbox.vdi.new; rm vbox.vdi

mv vbox.vdi.new vbox.vdi

(is much faster)


# Spectre and Meltdown mitigation checker

https://salsa.debian.org/debian/spectre-meltdown-checker/blob/master/spectre-meltdown-checker.sh


# Reduce new (Polaris onwards) AMD graphics card power usage to maximum 50 W, tested

https://wiki.archlinux.org/index.php/AMDGPU#Overclocking

https://bitcointalk.org/index.php?topic=1543701.40

https://www.youtube.com/watch?v=X7i9vR4s8iQ

https://forums.anandtech.com/threads/rx-series-undervolt-how-much-have-you-undervolted-it.2541872/

echo 50000000 > /sys/class/drm/card0/device/hwmon/hwmon0/power1_cap

# or

echo 50000000 > /sys/class/drm/card0/device/hwmon/hwmon1/power1_cap

Not required to boot with amdgpu.ppfeaturemask=0xffffffff in cmdline in this case.


# Ubuntu 18.04.2 (all graphical desktops not loading after lightdm or sddm) for arm64 Raspberry Pi 3B and 3B+

https://wiki.ubuntu.com/ARM/RaspberryPi


# Gentoo for arm64 Raspberry Pi 3B and 3B+ and 4B with hardware decoding support through v4l2 framework

https://github.com/sakaki-/gentoo-on-rpi-64bit

https://github.com/sakaki-/bcmrpi3-kernel

https://github.com/TheNextLVL/RPi.GPIO.64

# and autobuild repository so that emerge does not need to compile, just to download

https://isshoni.org/pi64pie


# Laptop with defective main LCD and external LCD monitor broken band down side resize display

```bash
#!/bin/bash
xrandr --output LVDS-1 --off
xrandr --output VGA-1 --panning 1280x1800
xrandr --output VGA-1 --fb 1280x850
```

Last command triggers errors but the resize takes place as required.


# Battery protection script for Puppy Linux and UpupBB

CTRL+ALT+F2

root

woofwoof

./batt-low.sh

ALT+F4

or in /root/Startup

```bash
#!/bin/bash
while true
do
battery_level=`acpi -b|grep -P -o '[0-9]+(?=%)'`
battery_discharging=`acpi -b|grep Discharging|wc -l`
if [ $battery_level -le 1 ] && [ $battery_discharging -eq 1 ]
then
wmpoweroff
fi
sleep 30
done
```


# Mount NTFS and USB

https://askubuntu.com/questions/113733/how-do-i-correctly-mount-a-ntfs-partition-in-etc-fstab

https://unix.stackexchange.com/questions/326791/cannot-boot-because-missing-external-disk

For USB drives: nofail Do not report errors for this device if it does not exist.

https://askubuntu.com/questions/9939/what-do-the-last-two-fields-in-fstab-mean

sudo -s

blkid

/etc/fstab

UUID="XXX" /450gbusb     ext4    defaults,nofail         0       3

UUID="YYY" /750gbw7     ntfs-3g   defaults,nls=utf8,umask=000,dmask=027,fmask=137,uid=1000,gid=1000,windows_names 0 0


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


# MPV 2019 best with AMD Polaris, can select on timeline, configuration in ~/.config/mpv/mpv.conf:

# sudo apt install -y mesa-va-drivers

https://github.com/mpv-player/mpv/issues/5711

hwdec=vaapi

gpu-context=x11egl

save-position-on-quit

audio-normalize-downmix=yes

volume=100


# MPV 2018 best with AMD Polaris, can't select on timeline,  configuration in ~/.config/mpv/mpv.conf:

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

https://github.com/rg3/youtube-dl/issues/16819

nano .bashrc
alias ytdlbst="youtube-dl -f 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/bestvideo+bestaudio' --merge-output-format mkv"
. .bashrc

# an entire channel

youtube-dl -f best -ciw -o %(title)s.%(id)s.%(ext)s -v https://www.youtube.com/ubuntupodcast

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

winetricks ddr=opengl multisampling=enabled orm=fbo videomemorysize=2048 strictdrawordering=enabled

winetricks dotnet20

winetricks dotnet40

winetricks corefonts vcrun2005 vcrun2008 vcrun2010 vcrun2012 vcrun2015

Run game with this export to avoid black or invisible textures and surfaces:

https://askubuntu.com/questions/140161/dark-textures-on-intel-hd-3000-sandy-bridge-in-games-via-wine

export MESA_EXTENSION_OVERRIDE="-GL_EXT_texture_sRGB_decode -GL_ARB_draw_elements_base_vertex -GL_ARB_map_buffer_range"

cd .wine/drive_c/Program\ Files/gamedir

wine game

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
