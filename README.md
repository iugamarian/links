# BTRFS most big issue to get rid of because many think it is "very good":

# BTRFS snapshots are a complete disaster until about the year 2038, hard to disable, missing nosnapshots mount option

https://simonbasle.github.io/2019/04/linux-where-has-my-space-gone/

http://marc.merlins.org/perso/btrfs/post_2014-05-04_Fixing-Btrfs-Filesystem-Full-Problems.html

https://lwn.net/Articles/579009/

https://www.solaris-cookbook.eu/linux/btrfs-filling-filesystem-snapshots-apt-snapshot/

https://btrfs.wiki.kernel.org/index.php/SysadminGuide

BTRFS reflinks are a smaller issue, but snapshots are a very big issue - starts using space for any small file change, kills ssd drives.

This is very bad issue because IT CAN NOT BE DISABLED / KILLED ... maybe it can be by recompiling Linux with BTRFS snapshot code removed from the source.


# BTRFS news October 2020

https://www.spinics.net/lists/linux-btrfs/msg105967.html

Subject: Re: using raid56 on a private machine

From: Zygo Blaxell <ce3g8jdj@xxxxxxxxxxxxxxxxxxxxx>

Date: Mon, 5 Oct 2020 21:24:27 -0400

Cc: cryptearth <cryptearth@xxxxxxxxxxxxx>, linux-btrfs@xxxxxxxxxxxxxxx, Josef Bacik <jbacik@xxxxxx>

In-reply-to: <91a18b63-6211-08e1-6cd9-8ef403db1922@libero.it>

References: <dbf47c42-932c-9cf0-0e50-75f1d779d024@cryptearth.de> <91a18b63-6211-08e1-6cd9-8ef403db1922@libero.it>

User-agent: Mutt/1.10.1 (2018-07-13)
 
On Mon, Oct 05, 2020 at 07:57:51PM +0200, Goffredo Baroncelli wrote:

> On 10/5/20 6:59 PM, cryptearth wrote:

> > Hello there,

> > 

> > as I plan to use a 8 drive RAID6 with BtrFS I'd like to ask about

> > the current status of BtrFS RAID5/6 support or if I should go with a

> > more traditional mdadm array.

Definitely do not use a single mdadm raid6 array with btrfs.  It is

equivalent to running btrfs with raid6 metadata:  mdadm cannot recover

from data corruption on the disks, and btrfs cannot recover from

write hole issues in degraded mode.  Any failure messier than a total

instantaneous disk failure will probably break the filesystem.

> > The general status page on the btrfs wiki shows "unstable" for

> > RAID5/6, and it's specific pages mentions some issue marked as "not

> > production ready". It also says to not use it for the metadata but

> > only for the actual data.

That's correct.  Very briefly, the issues are:

	1.  Reads don't work properly in degraded mode.

	2.  The admin tools are incomplete.

	3.  The diagnostic tools are broken.

	4.  It is not possible to recover from all theoretically
	recoverable failure events.

Items 1 and 4 make raid5/6 unusable for metadata (total filesystem loss

is likely).  Use raid1 or raid1c3 for metadata instead.  This is likely

a good idea even if all the known issues are fixed--metadata access

patterns don't perform well with raid5/6, and the most likely proposals

to solve the raid5/6 problems will require raid1/raid1c3 metadata to

store an update journal.

If your application can tolerate small data losses correlated with disk

failures (i.e. you can restore a file from backup every year if required,

and you have no requirement for data availability while replacing disks)

then you can use raid5 now; otherwise, btrfs raid5/6 is not ready yet.

> > I plan to use it for my own personal system at home - and I do

> > understand that RAID is no replacement for a backup, but I'd rather

> > like to ask upfront if it's ready to use before I encounter issues

> > when I use it.

> > I already had the plan about using a more "traditional" mdadm setup

> > and just format the resulting volume with ext4, but as I asked about

> > that many actually suggested to me to rather use modern filesystems

> > like BtrFS or ZFS instead of "old school RAID".

Indeed, old school raid maximizes your probability of silent data loss by

allowing multiple disks in inject silent data loss failures and firmware

bug effects.

btrfs and ZFS store their own data integrity information, so they can

reliably identify failures on the drives.  If redundant storage is used,

they can recover automatically from failures the drives can't or won't

report.

> > Do you have any help for me about using BtrFS with RAID6 vs mdadm or ZFS?

> 
> Zygo collected some useful information about RAID5/6:

> 
> https://lore.kernel.org/linux-btrfs/20200627032414.GX10769@xxxxxxxxxxxxxx/

>

> However more recently Josef (one of the main developers), declared

> that BTRFS with RAID5/6 has  "...some dark and scary corners..."

> 

> https://lore.kernel.org/linux-btrfs/bf9594ea55ce40af80548888070427ad97daf78a.1598374255.git.josef@xxxxxxxxxxxxxx/

I think my list is a little more...concrete.  ;)

> > I also don't really understand why and what's the difference between

> > metadata, data, and system.

> > When I set up a volume only define RAID6 for the data it sets

> > metadata and systemdata default to RAID1, but doesn't this mean that

> > those important metadata are only stored on two drives instead of

> > spread accross all drives like in a regular RAID6? This would somewhat

> > negate the benefit of RAID6 to withstand a double failure like a 2nd

> > drive fail while rebuilding the first failed one.

> 

> Correct. In fact Zygo suggested to user RAID6 + RAID1C3.

> 

> I have only few suggestions:

> 1) don't store valuable data on BTRFS with raid5/6 profile. Use it if

> you want to experiment and want to help the development of BTRFS. But

> be ready to face the lost of all data. (very unlikely, but more the

> size of the filesystem is big, more difficult is a restore of the data

> in case of problem).

Losing all of the data seems unlikely given the bugs that exist so far.

The known issues are related to availability (it crashes a lot and

isn't fully usable in degraded mode) and small amounts of data loss

(like 5 blocks per TB).

The above assumes you never use raid5 or raid6 for btrfs metadata.  Using

raid5 or raid6 for metadata can result in total loss of the filesystem,

but you can use raid1 or raid1c3 for metadata instead.

> 2) doesn't fill the filesystem more than 70-80%. If you go further

> this limit the likelihood to catch the "dark and scary corners"

> quickly increases.

Can you elaborate on that?  I run a lot of btrfs filesystems at 99%

capacity, some of the bigger ones even higher.  If there were issues at

80% I expect I would have noticed them.  There were some performance

issues with full filesystems on kernels using space_cache=v1, but

space_cache=v2 went upstream 4 years ago, and other significant

performance problems a year before that.

The last few GB is a bit of a performance disaster and there are

some other gotchas, but that's an absolute number, not a percentage.

Never balance metadata.  That is a ticket to a dark and scary corner.

Make sure you don't do it, and that you don't accidentally install a

cron job that does it.

> 3) run scrub periodically and after a power failure ; better to use

> an uninterruptible power supply (this is true for all the RAID, even

> the MD one).

scrub also provides early warning of disk failure, and detects disks

that are silently corrupting your data.  It should be run not less than

once a month, though you can skip months where you've already run a

full-filesystem read for other reasons (e.g. replacing a failed disk).

> 4) I don't have any data to support this; but as occasional reader of

> this mailing list I have the feeling that combing BTRFS with LUCKS(or

> bcache) raises the likelihood of a problem.

I haven't seen that correlation.  All of my machines run at least one

btrfs on luks (dm-crypt).  The larger ones use lvmcache.  I've also run

bcache on test machines doing power-fail tests.

That said, there are additional hardware failure risks involved in

caching (more storage hardware components = more failures) and the

system must be designed to tolerate and recover from these failures.

When cache disks fail, just uncache and run scrub to repair.  btrfs

checksums will validate the data on the backing HDD (which will be badly

corrupted after a cache SSD failure) and will restore missing data from

other drives in the array.

It's definitely possible to configure bcache or lvmcache incorrectly,

and then you will have severe problems.  Each HDD must have a separate

dedicated SSD.  No sharing between cache devices is permitted.  They must

use separate cache pools.  If one SSD is used to cache two or more HDDs

and the SSD fails, it will behave the same as a multi-disk failure and

probably destroy the filesystem.  So don't do that.

Note that firmware in the SSDs used for caching must respect write

ordering, or the cache will do severe damage to the filesystem on

just about every power failure.  It's a good idea to test hardware

in a separate system through a few power failures under load before

deploying them in production.  Most devices are OK, but a few percent

of models out there have problems so severe they'll damage a filesystem

in a single-digit number of power loss events.  It's fairly common to

encounter users who have lost a btrfs on their first or second power

failure with a problematic drive.  If you're stuck with one of these

disks, you can disable write caching and still use it, but there will

be added write latency, and in the long run it's better to upgrade to

a better disk model.

> 5) pay attention that having an 8 disks raid, raises the likelihood of a

> failure of about an order of magnitude more than a single disk ! RAID6

> (or any other RAID) mitigates that, in the sense that it creates a

> time window where it is possible to make maintenance (e.g. a disk

> replacement) before the lost of data.

> 6) leave the room in the disks array for an additional disk (to use

> when a disk replacement is needed)

> 7) avoid the USB disks, because these are not reliable

> 

> 

> > 

> > Any information appreciated.

> > 

> > 

> > Greetings from Germany,

> > 

> > Matt

> 

> 

> -- 

> gpg @keyserver.linux.it: Goffredo Baroncelli <kreijackATinwind.it>

> Key fingerprint BBF5 1610 0B64 DAC6 5F7D  17B2 0EDA 9B37 8B82 E0B5

> 


# Windows update use less resources automatically

https://www.techiexpert.com/what-is-windows-modules-installer-how-to-enable-disable/

1. Go to the “Run” by pressing the Windows key with R on your system.
    
Then, type the services.msc and click on the OK button.
    
Then search for the “Windows Module Installer.”
    
Then double click on the “Windows Module Installer” and choose the manual
    
from the dropdown. It is usually set to Automatic or even workload.
    
Now find and double click on the Windows update option.
    
Now a dialogue will appear, in that select the startup type to manual or even
    
disabled whatever option you have to choose, according to your preference.
    
Then click on OK
    
Then reboot your system, and then everything will be fine.

2. Set your internet to connect as a metered connection

Most of the people would recommend this as this would not let the system to take

much of the consumption of the internet by the windows module installer worker,

and thus there are high chances that it might stop using the high-performance

resources of the CPU.

You can do this by only going on to the Start > Settings > Network & Internet > Wi-Fi.

Then click on the advanced button options.

Then, in the last, select the set metered connection as “On.”

If you are working on an ethernet connection, then it is better to set the updates with them manually.


# Ubuntu do release upgrade

https://linuxconfig.org/how-to-upgrade-ubuntu-to-20-10

$ sudo apt update

$ sudo apt upgrade

$ sudo apt dist-upgrade

$ sudo apt autoremove

/etc/update-manager/release-upgrades

Prompt=normal

$ sudo do-release-upgrade

y

$ sudo reboot


# BTRFS FI BALANCE IS DEPRECATED, PLEASE USE BTRFS BALANCE START

https://serverfault.com/questions/519852/does-btrfs-balance-also-defragment-files

balance will not defragment - if 11000 fragments still 11000 fragments better arranged

defragment will not balance so much - the actual write of the defrag goes through the

allocator which balances but will not complete the balance unless writing enough

'btrfs filesystem balance' command is deprecated, please use 'btrfs balance start' command instead.


# Alternative to the stagnant CUPS

https://www.msweet.org/pappl/

https://github.com/sponsors/michaelrsweet

https://www.youtube.com/watch?v=tIJ6FsgZE3E


# NAS SATA expander

https://conectica.ro/controlere/pci-express/pci-express-cu-8-porturi-sata-iii-delock-90391

https://conectica.ro/controlere/pci-express/pci-express-cu-5-porturi-sata-iii-delock-90395


# Open physics 4 volumes book

http://www.openculture.com/2018/04/the-feynman-lectures-on-physics.html


# OpenJDK on Windows 10

https://openjdk.java.net/

https://stackoverflow.com/questions/52511778/how-to-install-openjdk-11-on-windows

http://www.nhdeeds.com/instructions/HowToSetWindowsFileAssociationsToOpenJNLPFiles.html

https://stackoverflow.com/questions/55186796/openjdk-builds-and-javaws-bin-file

https://adoptopenjdk.net/icedtea-web.html

https://openwebstart.com/download/

https://en.wikipedia.org/wiki/Java_Web_Start

Extract the zip file into a folder, e.g. C:\Program Files\Java\ and it will create a jdk-11

folder (where the bin folder is a direct sub-folder). You may need Administrator privileges to

extract the zip file to this location.

Set a PATH:

Select Control Panel and then System.
Click Advanced and then Environment Variables.
Add the location of the bin folder of the JDK installation to the PATH variable in System Variables.

The following is a typical value for the PATH variable:

C:\WINDOWS\system32;C:\WINDOWS;"C:\Program Files\Java\jdk-11\bin"

Set JAVA_HOME:

Under System Variables, click New.

Enter the variable name as JAVA_HOME.

Enter the variable value as the installation path of the JDK (without the bin sub-folder).

Click OK.

Click Apply Changes.

Configure the JDK in your IDE (e.g. IntelliJ or Eclipse).

You are set.

To see if it worked, open up the Command Prompt and type java -version and see if it prints

your newly installed JDK.

If you want to uninstall - just undo the above steps.

Note: You can also point JAVA_HOME to the folder of your JDK installations and then set the

PATH variable to %JAVA_HOME%\bin. So when you want to change the JDK you change only the JAVA_HOME variable and leave PATH as it is.

How to Set Windows File Associations to Open .JNLP Files
Note:
Each file type on your computer is associated with an application that runs it. Files of type JNLP are run by Java Web Start. It is possible for JNLP files to become associated with another application, causing it to open incorrectly. Because Java Web Start opens the search application, when the JNLP files are associated incorrectly, the search application cannot open.

Document Details:
To modify the file association to have JNLP files open with Java Web Start on a Windows XP, Windows 7/Windows Vista, Windows 8, or Windows 10 computer, follow the steps below:

 

Windows 10:

Go to the nhdeeds.com website, then click on Search County Records.

Click to Accept Registry Disclaimer, then click the link to Launch the search application

When prompted to open or save, choose save

Navigate to the saved location to right-click on the saved .jnlp file and choose Open With

Make sure there is a check-mark in the box that says Always use this app to open .jnlp files, then click More apps

Scroll down to the bottom and select Look for another app on this PC

Select the Java folder, (If you do not see Java, go back up one level to the c:/ drive and

select Program Files(x86), then select the Java folder. If you do not have a Java folder here,

you will want to go to www.java.com and download the latest version before continuing with these steps.)

Double click the latest jre folder (example: jre7, jre1.8_191, etc)

Double click the bin folder

Double click the javaws application

Click the Close button

JNLP files will now open, by default, with Java Web Start

Windows 7 and Vista:

Click on Start and select Control Panel.

In the top right corner of the Control Panel, select Category from the view by: pull down menu.

Click the Programs link.

Under Default Programs, click the Make a file type always open in a specific program link.

Under the Name column, locate and select JNLP from the list of Extensions.

Click the Change program button.

From the Open With window, click the Browse... button..

From the Open With... dialogue window, (This will typically open in the c:\Program Files directory)

Select the Java folder, (If you do not see Java, go back up one level to the c:/ drive and select

Program Files(x86), then select the Java folder. If you do not have a Java folder here, you will

want to go to www.java.com and download the latest version before continuing with these steps.)

Double click the latest jre folder (example: jre7, jre1.8_191, etc)

Double click the bin folder

Double click the javaws application

Click the OK button.

Click the Close button.

Please turn to icedtea-web project. It is independent project, and in one way or another,

it is what all distros are packing as javaws - http://icedtea.classpath.org/wiki/IcedTea-Web

What am I supposed to do in Windows? Just install the binaries from

icedtea.wildebeest.org/download/icedtea-web-binaries/1.8/… and it will magically work with

Red Hat's OpenJDK 11?

Yes. that is exactly what you should do. Please note, icedtea-webmoved to githb, and is now

cooperating with adoptpenjdk and karakun, to provide more friendly user expereince:

github.com/adoptopenjdk/icedtea-web – judovana Jun 30 '19 at 10:30


# Microsoft Active Signal Resolution issues

https://answers.microsoft.com/en-us/windows/forum/windows_10-other_settings/incorrect-active-signal-resolution/9ff80937-8959-42bf-acc7-cca8a3bae441

Intel Craphics Control Panel

Home -> Display

Selecting "Maintain Display Scaling" and pressing Apply makes the Desktop Resolution be the same as Active Signal Resolution not different. Tested.


# Youtube awk tutorial

https://www.youtube.com/watch?v=jJ02kEETw70

https://www.youtube.com/watch?v=vcRPNhLbhoc

https://www.youtube.com/watch?v=8q8DHmA9puw


# Voice recognition offline free software supporting already 10 languages, Apache License 2.0

https://www.phoronix.com/forums/forum/phoronix/latest-phoronix-articles/1202341-mozilla-s-incredible-speech-to-text-engine-is-at-risk-following-layoffs/page2

https://github.com/alphacep/vosk-api

https://alphacephei.com/vosk/

There are some good alternatives around, even better than DeepSpeech. Vosk for example supports 10 languages,

works offline on Android and RPi with small 50Mb model and allows to configure grammar on the fly. Based on the

testing I just did with Vosk, Mozilla DeepSpeech, Google Speech to Text and Microsoft Azure, I disagree with your

arugment that SaaS has the best quality results.

Mozilla DeepSpeech was definitely trailing the bleeding edge, but Vosk using the vosk-model-en-us-daanzu-20200328

model produces very accurate results even on uncommon words, similar in performance to Google & Microsoft

(which has generally better formatting than Google's STT)


# Ultimate Boot CD having wdidle3 version 1.05

https://www.ultimatebootcd.com/download.html

Download from down in the page.


# Check MD5 built in Windows 7 and 10

https://onthefencedevelopment.com/2017/08/15/windows-10-builtin-md5-checksum-calculator/

CertUtil -hashfile path-to-file MD5

# Harddisk 8 seconds idle fix script if firmware method fails

idle3ctl can fix more drives than wdidle3, first try using idle3ctl

WD Elements white label drives sometimes stop parking the heads after

being removed from USB case and used on SATA and idle3ctl -g /dev/sdx

Tested this on a 12 TB WD Elements drive from 2020 that I use on SATA now.

https://forum.storj.io/t/anybody-using-wd-blue-drives-for-a-storagenode/5945/8

Thanks for all the answers. I tried all the suggestions, but only ioping and dd

(with and without ioflag) worked. With “touch” and “echo” the SMART value kept increasing.

How I tested:

in terminal 1 I requested the smart value every 20 seconds

while true; do smartctl -a /dev/sdf|grep Load;sleep 20;done

in terminal 2 I ran the command every 7 seconds (better make it every 6 seconds to be sure):

while true;do dd if=/dev/zero of=/mnt/backup/keep_awake bs=4k count=1 oflag=direct;sleep 7;done

oflag=direct should bypass write cache

@Itsmenh: Thanks for your stats. As you can see disk 1 and 2 have already very high values for Load_Cycle_Count.

I read somewhere that they are rated for “only” 300000.

I bought 2 of these drives as external “MyBook” drives for my backups on offer for ~£100 each. The external

cases I sold on Ebay for £20 each. So £80 for 6TB is not bad

In case someone is interested in my little script. Just fill in all the affected hard disk models you have in

your system into the “model” variable of the script separated by a space. ioping needs to be installed too.

#!/bin/bash

models="WDC_WD60EZRZ"

killall ioping 2>/dev/null

for model in $models; do

  disks=$(ls /dev/disk/by-id/*$model*|grep -v part)
  
  for disk in $disks;do
  
    ioping -i 7s -q $disk &
    
  done
  
done

Disclaimer: as always, do it at your own risk


# Western digital harddisk 8 seconds parking head idle fix

https://www.reddit.com/r/DataHoarder/comments/dzthf6/elements_12tb_emaz_seek_noise_every_5_seconds/

https://withblue.ink/2016/07/15/what-i-learnt-from-using-wd-red-disks-to-build-a-home-nas.html

https://forums.unraid.net/topic/14854-do-not-run-wdidle3-d-on-the-new-western-digital-ezrx-drives/

https://wiki.hackspherelabs.com/index.php?title=FreeBSD

https://www.linuxsecrets.com/archlinux-wiki/wiki.archlinux.org/index.php/Advanced_Format.html

https://deki.mpifr-bonn.mpg.de/Cooperations/EVN_TOG/Disk_Inventory

https://wiki.archlinux.org/index.php/Hdparm

https://packages.gentoo.org/packages/sys-apps/idle3-tools

http://www.openmutual.org/2018/02/fixing-the-western-digital-excessive-load-unload-problem-with-idle3-tools/

https://www.reddit.com/r/DataHoarder/comments/aqmr4y/do_the_wd_white_drives_8tb_10tb_shucks_have/

https://www.reddit.com/r/DataHoarder/comments/7670dg/wd_have_blocked_wdidle3_from_working/

https://www.reddit.com/r/DataHoarder/comments/7670dg/wd_have_blocked_wdidle3_from_working/

https://www.reddit.com/r/freenas/comments/91hzwt/western_digital_drives_and_idle3_time/

https://www.ixsystems.com/community/threads/hacking-wd-greens-and-reds-with-wdidle3-exe.18171/page-15

https://lukas.zapletalovi.com/2020/01/wd-idle-time-in-linux.html

https://techblog.jeppson.org/tag/zfs/

https://nikolausdulgeridis.de.tl/FestplattenIdle.htm

https://sigtar.com/tag/unraid/

New seek issue:

The 'noise' heard is due to a feature used to prevent disturbances in media lube due to prolonged dwelling

at a single location. This is common for HDD suppliers as a preventive activity for reliability. Normal

operation typically forces seek away and the seek noises heard are expected. During moments of idle, there

is less activity and the unexpected sound is more noticeable (Approximately every 5 seconds). Directly

attaching the drive to a metal chassis can amplify this occurrence to make the hard drive more audible.

Test how bad it is already:

smartctl -a /dev/sda | grep "^193"

Fix WD Red 8 seconds to 300 seconds official:

wd5741.exe -Dall

I tried this on a WD red 8tb "hdparm -S 120 /dev/sdX" (X being your drive letter) (apm must be

254 as default) and that makes the drive spin down after 17 minutes. I tried a couple of other

values, but couldnt find anything else working. I would like to spin down after 1,5 hours

This method will use a utility called idle3ctl to alter the firmware value for the idle3 timer on WD hard drives

(similar to wdidle3.exe from WD). The advantage compared to the official utility is you do not need to create a DOS

bootdisk first to change the idle3 timer value. Additionally idle3ctl might also work over USB-to-S-ATA bridges

(in some cases). The utility is provided by the idle3-tools package.

idle3ctl -g /dev/your_wd_hdd

to get the raw idle3 timer value. You can disable the IntelliPark feature completely, with:

idle3ctl -d /dev/your_wd_hdd

or set it to a different value (0-255) with (e.g. 10 seconds):

idle3ctl -s 100 /dev/your_wd_hd

The range 0-128 is in 0.1s and 129-255 in 30s. For the changes to take effect, the drive needs to

go through one powercycle, meaning powering it off and on again (on internal drives, a reboot is not sufficient).

If your WD hard drive is not recognized, you can use the --force option. For more options see:

$ idle3ctl -h

To check current status

wdidle3 /r

to disable intellipark

wdidle3 /d

to set to 300 (max) timeout

wdidle3 /s300

Modern Western Digital "Green" Drives include the Intellipark feature that stops the disk when not in use.

Unfortunately, the default timer setting is not perfect on linux/unix systems, including many NAS,

and leads to a dramatic increase of the Load Cycle Count value (SMART attribute #193).

Please deactivat it with http://idle3-tools.sourceforge.net/ (normally in the Distro)

get the Info: idle3ctl -g /dev/sd(x)

disabling :   idle3ctl -d /dev/sd(x)

The idle3 timer seems to be a power on only setting. 

That means that the drive needs to be powered OFF and then ON to use the new setting.

I turn off the Computer, plug the power off and push the start button, for a short while the fan go's on.

All power it out.

Or turn off for few minutes to take the effect.

To set idle3 timer raw value, option -s must be used. Value must be an integer between 1 and 255.

The idle3 timer is set in 0.1s for the 1-128 range, and in 30s for the 129-255 range.

Use idle3 to query current spindown status (update drive letters to suit your needs)

for drive in {a..p}; do echo /dev/sd$drive; sudo idle3ctl -g /dev/sd$drive; done

For anything that doesn’t say Idle3 timer is disabled run the following:

sudo idle3ctl -s 0 /dev/sd(DRIVE_LETTER)

No more drive spindown!

idle3ctl -s 129 /dev/sdc

which for new drives means 30 seconds ( and -s 130 would mean 60 seconds and so on).

After you set the value in the drive you must turn off the power to your computer (drive)

as the value is read at power up. So do a shutdown and then power off.

This one completely disables the timer (run as root):

idle3ctl -d /dev/sdc

And in Ultimate Boot CD or MS-DOS:

wdidle3 /d

This one sets the timer to the maximum allowed, in Linux a little over 60 minutes (run as root):

idle3ctl -s 255

And in Ultimate Boot CD or MS-DOS, 5 minutes:

wdidle3 /S300

Meaning of the returned raw value depending on the wdidle3 version

The value 0 is used when the timer is disabled.

The values 1 to 128 are reported in .1s by all wdidle3.exe versions.

The values 129 to 255 are in .1s for version 1.00, but in 30s for versions 1.03 and 1.05

The difference only affects the output, the stored timer is identical between versions. Maybe different WD drives have different beheviour.

May not work over USB - if so remove the drive from USB case and connect it to SATA port then try setting timer.

Tests on reddit and Youtube show that to use ok this needs to be done:

1) SATA Mode set from AHCI to IDE in the BIOS

2) All other SATA drives diconnected, only destination drive connected and boot from USB

3) After the setting complete the drive will behave badu until it is powered off then on

4) Tested working on the internet: 4 TB, 5 TB, 6 TB, 8TB, manufactured in 2019 or 2020.


# NFS async is 10 times faster for many small files on async then on sync

https://serverfault.com/questions/372151/nas-performance-nfs-vs-samba-vs-glusterfs

I am planning my new shared storage infrastructure for a small webserver farm.

Therefore i ran a lot of tests with a lot of NAS filesystems. By doing so, i got some

unexpected results and would like to know whether somebody here can confirm this.

In short: Samba is hugely faster than NFS and GlusterFS for small file writes.

Here what i did: I ran a simple "rsync benchmark" with a lot of files to compare the write

performance for small files. For easier reproduction, i re-ran it just now with the contents

of the current wordpress tar.gz.

    GlusterFS replicated 2: 32-35 seconds, high CPU load
    
    GlusterFS single: 14-16 seconds, high CPU load
    
    GlusterFS + NFS client: 16-19 seconds, high CPU load
    
    NFS kernel server + NFS client (sync): 32-36 seconds, very low CPU load
    
    NFS kernel server + NFS client (async): 3-4 seconds, very low CPU load
    
    Samba: 4-7 seconds, medium CPU load
    
    Direct disk: < 1 second

I am absolutely no samba guru (i think my last contact was with samba 2.x), so i did not optimize

anything here - just out-of-the-box config (debian/squeeze package). The only thing I

added "sync always = yes" which is supposed to enforce sync after write (but seeing those results..).

Without it, the tests where about 1-2 seconds faster.

All the tests where run on the same machine (self-mounted it's NAS export), so no network delays - pure protocol performance.

Side node: As file system i used ext4 and xfs. The above results are with ext4. xfs performed up to 40% (less time) better.

The machines are EC2 m1.small instances. NAS export are on EBS volumes, sources (extracted tar) on ephemeral disk.

So here we go: Can anybody explain to me why samba is so much faster?

Also: Is NFS performance with the kernel server supposed to be that horrible (outperformed by the GlusterFS NFS server)

in sync mode? Any idea how to tune this?

Thanks, L


# Glusterfs seems to use a key for data so fast encryption, but needs minimum 2 servers

https://www.gluster.org/

https://en.wikipedia.org/wiki/Gluster#GlusterFS

https://www.howtoforge.com/tutorial/high-availability-storage-with-glusterfs-on-debian-8-with-two-nodes/

This tutorial shows how to set up a high-availability storage with two storage servers (Debian Jessie)

that uses GlusterFS. Each storage server will be a mirror of the other storage server, and files will

be replicated automatically across both storage nodes. The client system (Debian 8 as well) will be able

to access the storage as if it was a local filesystem. GlusterFS is a clustered file-system capable of

scaling to several peta-bytes. It aggregates various storage bricks over Infiniband RDMA or TCP/IP

interconnect into one large parallel network file system. Storage bricks can be made of any commodity

hardware such as x86_64 servers with SATA-II RAID and Infiniband HBA.

 
1 Preliminary Note

In this tutorial, I will use three systems, two servers and a client:

    server1.example.com: IP address 192.168.1.100 (server)
    
    server2.example.com: IP address 192.168.1.101 (server)
    
    client1.example.com: IP address 192.168.1.102 (client)

All three systems should be able to resolve the other systems' hostnames. If this cannot be done through

DNS, you should edit the /etc/hosts file so that it looks as follows on all three systems:

nano /etc/hosts

127.0.0.1 localhost

192.168.1.100 server1.example.com server1

192.168.1.101 server2.example.com server2

192.168.1.102 client1.example.com client1


# The following lines are desirable for IPv6 capable hosts

::1 localhost ip6-localhost ip6-loopback

ff02::1 ip6-allnodes

ff02::2 ip6-allrouters

(It is also possible to use IP addresses instead of hostnames in the following setup. If you prefer to use

IP addresses, you don't have to care about whether the hostnames can be resolved or not.)

 
2 Setting up the GlusterFS Servers

server1.example.com/server2.example.com:

The latest GlusterFS version is available as Debian package from gluster.org. We can install it as follows:

We add the public key of the gluster.org Debian repository on both servers.

wget -O - http://download.gluster.org/pub/gluster/glusterfs/3.7/3.7.9/rsa.pub | apt-key add -

Then add the GlusterFS repository (the command is one line!)

echo deb http://download.gluster.org/pub/gluster/glusterfs/3.7/3.7.9/Debian/jessie/apt jessie main > /etc/apt/sources.list.d/gluster.list

and update the repository list.

apt-get update

Now we can install the GlusterFS server software with apt.

apt-get -y install glusterfs-server

The command

glusterfsd --version

should now show the GlusterFS version that you've just installed (3.7.9 in this case):

root@server1:/# glusterfsd --version

glusterfs 3.7.9 built on Mar 23 2016 05:24:49

Repository revision: git://git.gluster.com/glusterfs.git

Copyright (c) 2006-2013 Red Hat, Inc. <http://www.redhat.com/>

GlusterFS comes with ABSOLUTELY NO WARRANTY.

It is licensed to you under your choice of the GNU Lesser

General Public License, version 3 or any later version (LGPLv3

or later), or the GNU General Public License, version 2 (GPLv2),

in all cases as published by the Free Software Foundation.

root@server1:/#

If you use a firewall, ensure that TCP ports 111, 24007, 24008, 24009-(24009 + number of bricks

across all volumes) are open on server1.example.com and server2.example.com.

Glusterfs shall store its data in the directory /data on the servers. This location can be a normal

directory if you have a smaller installation or you use a separate hard disk partition and mount it as /data.

Run on both servers:

mkdir /data

to create the data directory.

Next, we must add server2.example.com to the trusted storage pool (please note that I'm running all

GlusterFS configuration commands from server1.example.com, but you can as well run them from

server2.example.com because the configuration is repliacted between the GlusterFS nodes - just make

sure you use the correct hostnames or IP addresses):

server1.example.com:

On server1.example.com, run

gluster peer probe server2.example.com

root@server1:/# gluster peer probe server2.example.com

peer probe: success.

root@server1:/#

The status of the trusted storage pool should now be similar to this:

gluster peer status

root@server1:/# gluster peer status

Number of Peers: 1

Hostname: server2.example.com

Uuid: 0f7ee46c-6a71-4a31-91d9-6076707eff95

State: Peer in Cluster (Connected)

root@server1:/#

Next we create the share named testvol with two replicas (please note that the number

of replicas is equal to the number of servers in this case because we want to set up mirroring)

on server1.example.com and server2.example.com in the /data/testvol directory (this will be

created if it doesn't exist):

gluster volume create testvol replica 2 transport tcp server1.example.com:/data/testvol

server2.example.com:/data/testvol force

root@server1:/# gluster volume create testvol replica 2 transport tcp server1.example.com:/data/testvol server2.example.com:/data/testvol force

volume create: testvol: success: please start the volume to access data

root@server1:/#

Start the volume:

gluster volume start testvol

root@server1:/# gluster volume start testvol

volume start: testvol: success

root@server1:/#

Our test volume has been started successfully.

It is possible that the above command tells you that the action was not successful:

root@server1:~# gluster volume start testvol

Starting volume testvol has been unsuccessful

root@server1:~#

In this case you should check the output of...

server1.example.com/server2.example.com:

netstat -tap | grep glusterfsd

on both servers.

If you get output like this...

root@server1:/# netstat -tap | grep glusterfsd

tcp 0 0 *:49152 *:* LISTEN 8007/glusterfsd

tcp 0 0 server1.example.c:65533 server1.example.c:24007 ESTABLISHED 8007/glusterfsd

tcp 0 0 server1.example.c:49152 server2.example.c:65531 ESTABLISHED 8007/glusterfsd

tcp 0 0 server1.example.c:49152 server1.example.c:65532 ESTABLISHED 8007/glusterfsd

tcp 0 0 server1.example.c:49152 server1.example.c:65531 ESTABLISHED 8007/glusterfsd

tcp 0 0 server1.example.c:49152 server2.example.c:65526 ESTABLISHED 8007/glusterfsd

root@server1:/#

... everything is fine, but if you don't get any output...

root@server2:~# netstat -tap | grep glusterfsd

root@server2:~#

... restart the GlusterFS daemon on the corresponding server (server1.example.com in this case):

server2.example.com:

service glusterfs-server restart

Then check the output of...

netstat -tap | grep glusterfsd

... again on that server - it should now look like this:Advertisement

root@server2:/# netstat -tap | grep glusterfsd

tcp 0 0 *:49152 *:* LISTEN 7852/glusterfsd

tcp 0 0 server2.example.c:49152 server2.example.c:65532 ESTABLISHED 7852/glusterfsd

tcp 0 0 server2.example.c:49152 server1.example.c:65526 ESTABLISHED 7852/glusterfsd

tcp 0 0 server2.example.c:49152 server2.example.c:65525 ESTABLISHED 7852/glusterfsd

tcp 0 0 server2.example.c:65533 server2.example.c:24007 ESTABLISHED 7852/glusterfsd

tcp 0 0 server2.example.c:49152 server1.example.c:65524 ESTABLISHED 7852/glusterfsd

root@server2:/#

Now back to server1.example.com:

server1.example.com:

You can check the status of the volume with the command

gluster volume info

root@server1:/# gluster volume info

Volume Name: testvol

Type: Replicate

Volume ID: 3fc9af57-ca56-4a72-ad54-3d2ea03e5883

Status: Started

Number of Bricks: 1 x 2 = 2

Transport-type: tcp

Bricks:

Brick1: server1.example.com:/data/testvol

Brick2: server2.example.com:/data/testvol

Options Reconfigured:

performance.readdir-ahead: on

root@server1:/#

By default, all clients can connect to the volume. If you want to grant access to client1.example.com (= 192.168.1.102) only, run:

gluster volume set testvol auth.allow 192.168.1.102

root@server1:/# gluster volume set testvol auth.allow 192.168.1.102

volume set: success

root@server1:/#

Please note that it is possible to use wildcards for the IP addresses (like 192.168.*) and

that you can specify multiple IP addresses separated by comma (e.g. 192.168.1.102,192.168.1.103).

The volume info should now show the updated status:

gluster volume info

root@server1:/# gluster volume info

Volume Name: testvol

Type: Replicate

Volume ID: 3fc9af57-ca56-4a72-ad54-3d2ea03e5883

Status: Started

Number of Bricks: 1 x 2 = 2

Transport-type: tcp

Bricks:

Brick1: server1.example.com:/data/testvol

Brick2: server2.example.com:/data/testvol

Options Reconfigured:

auth.allow: 192.168.1.102

performance.readdir-ahead: on

root@server1:/#
 
3 Setting up the GlusterFS Client

client1.example.com:

On the client system we add the public key of the gluster.org Debian repository first.

wget -O - http://download.gluster.org/pub/gluster/glusterfs/3.7/3.7.9/rsa.pub | apt-key add -

Then add the GlusterFS repository (the command is one line!)

echo deb http://download.gluster.org/pub/gluster/glusterfs/3.7/3.7.9/Debian/jessie/apt jessie main > /etc/apt/sources.list.d/gluster.list

and update the repository list.

apt-get update

Now we can install the GlusterFS client as follows.

apt-get -y install glusterfs-client

Then we create the following directory:

mkdir /mnt/glusterfs

That's it! Now we can mount the GlusterFS filesystem to /mnt/glusterfs with the following command:

mount.glusterfs server1.example.com:/testvol /mnt/glusterfs

(Instead of server1.example.com you can as well use server2.example.com in the above command!)

You should now see the new share in the outputs of...

mount

root@client1:/# mount

...

server1.example.com:/testvol on /mnt/glusterfs type fuse.glusterfs (rw,relatime,user_id=0,group_id=0,default_permissions,allow_other,max_read=131072)

root@client1:/#

... and...

df -h

root@client1:/# df -h

Filesystem Size Used Avail Use% Mounted on

...

server1.example.com:/testvol 57G 21G 34G 39% /mnt/glusterfs

root@client1:/#

Instead of mounting the GlusterFS share manually on the client, you could modify /etc/fstab

so that the share gets mounted automatically when the client boots.

Open /etc/fstab and append the following line:

nano /etc/fstab

[...]

server1.example.com:/testvol /mnt/glusterfs glusterfs defaults,_netdev 0 0

(Again, instead of server1.example.com you can as well use server2.example.com!)

To test if your modified /etc/fstab is working, reboot the client:

reboot

After the reboot, you should find the share in the outputs of...

df -h

... and...

mount

 
4 Testing GlusterFS Replication

Now let's create some test files on the GlusterFS share:

client1.example.com:

touch /mnt/glusterfs/test1

touch /mnt/glusterfs/test2

Now let's check the /data directory on server1.example.com and server2.example.com. The test1 and test2 files should be present on each node:

server1.example.com/server2.example.com:

ls -l /data/testvol

root@server1:/# ls -l /data/testvol/

total 0

-rw-r--r-- 2 root root 0 Mar 23 2016 test1

-rw-r--r-- 2 root root 0 Mar 23 2016 test2

Now we shut down server1.example.com and add/delete some files on the GlusterFS share on client1.example.com.

server1.example.com:

shutdown -h now

client1.example.com:

touch /mnt/glusterfs/test3

touch /mnt/glusterfs/test4

rm -f /mnt/glusterfs/test2

The changes should be visible in the /data/testvol directory on server2.example.com:

server2.example.com:

ls -l /data/testvol

root@server2:/# ls -l /data/testvol

total 8

-rw-r--r-- 2 root root 0 Mar 23 08:06 test1

-rw-r--r-- 2 root root 0 Mar 23 08:09 test3

-rw-r--r-- 2 root root 0 Mar 23 08:09 test4

root@server2:/#

Let's boot server1.example.com again and take a look at the /data directory:

server1.example.com:

ls -l /data/testvol

root@server1:/# ls -l /data/testvol

total 0

-rw-r--r-- 2 root root 0 Mar 23 08:06 test1

-rw-r--r-- 2 root root 0 Mar 23 08:09 test3

-rw-r--r-- 2 root root 0 Mar 23 08:09 test4

root@server1:/#

As you see, server1.example.com automatically synced the changed. In case that the change

has not been synced yet, then this is easy to fix, all we need to do is invoke a read command

on the GlusterFS share on client1.example.com, e.g.:

client1.example.com:

ls -l /mnt/glusterfs/

root@client1:/# ls -l /mnt/glusterfs/

total 0

-rw-r--r-- 1 root root 0 Mar 23 08:06 test1

-rw-r--r-- 1 root root 0 Mar 23 08:09 test3

-rw-r--r-- 1 root root 0 Mar 23 08:09 test4

root@client1:/#

# Glusterfs with 1 server meaning one brick

https://docs.gluster.org/en/latest/glossary/

https://docs.gluster.org/en/latest/Administrator%20Guide/Setting%20Up%20Volumes/

https://www.digitalocean.com/community/tutorials/how-to-create-a-redundant-storage-pool-using-glusterfs-on-ubuntu-18-04

https://hexadix.com/setup-network-shared-folder-using-glusterfs-ubuntu-servers-backup-server-array-auto-mount/

https://docs.gluster.org/en/latest/Administrator%20Guide/Setting%20Up%20Clients/

https://www.maketecheasier.com/create-nas-with-glusterfs/

How to Create Your Own NAS With GlusterFS

By Sarah Li Cain / Mar 17, 2015 / Linux

GlusterFS is a system that provides network storage which has the ability to be made fault-tolerant,

redundant and scalable. It’s a great option for applications that need access to large files, such as

scientific grade storage solutions. What the file system does is aggregates and memory sources through

a single global namespace into a pool of storage and it is accessible through multi-file level protocols.

The great thing about GlusterFS is that it is very easy to use and maintain. Here’s how you can set up

your own NAS with GlusterFS.

What You Need:

    A network
    
    GlusterFS
    
    Linux Boxes

1. Set Up Your Network

Your best bet is connecting GlusterFS to Gigabit Ethernet and a huge array of servers, plus storage devices.

If you don’t have these on hand, two computers or VMs are usually sufficient, particularly if you are just

getting the hang of it. Copmuter 1 = server, Computer 2 = client

2. Install Your Server

Glusterfs is included in the repository of many Linux distros. Before installation, you can first compare the

version numbers between the website and your distro. Keep in mind you might have to manually update the clients.

If you have a pretty recent version, you can install the server by typing (in Debian-based distro):

sudo apt-get install glusterfs-server

3. Switch to Static IP and Adding/Removing Volumes

Open up the file “etc/network/interfaces”:

sudo nano /etc/network/interfaces

and remove the line (if present) ifacetho0 inet dynamic, then add the lines:

auto eth0

iface eth0 inet static

address 192.168.0.100

netmask 255.255.255.0

gateway 192.168.0.1

broadcast 192.168.0.255

network 192.168.0.0

Restart your machine and make sure the network is working. If it does, type in the following:

gluster volume create testvol 192.168.0.100:/data

Typing this will create a volume “testvol” which will be stored on the server. Your files will

then be located in the “/data” directory which is in the root system and what GlusterFS considers a brick.

To verify that it works, type:

gluster volume start testvol

You can remove the volume later on by typing both:

gluster volume stop testvol

and

gluster volume delete testvol

4. Mounting the Volume Locally

You can do this easily by finding:

mkdir /mnt/gluster

Then, use the command below to mount it.

mount.glusterfs 192.168.0.100:/ testvol /mnt/glusterfs
 
echo "It works" > /mnt/gluster/test. txt

Make sure it works before proceeding.

5. Sharing It Over NFS

More recent versions automatically give NFS access to volumes. You still need to add a portmap

package to the server in order to make it work though. To do you this, all you need to do is to

add a mount point:

sudo mkdir /mnt/nfsgluster

and type:

sudo mount -t nfs 192.168.0.100:/ testvol /mnt/nfstest/ -o tcp,vers=3

To make a client mount the share on boot, add the details of the GlusterFS NFS share to /etc/fstab

in the normal way. For our example, add the line:

192.168.0.100:7997:/testvol / mnt/nfstest nfs defaults,_netdev 0 0

That’s it!

Conclusion

Once you’re set up, you can add a new server by following the above steps. Make sure you give your new

server a different IP address. To check the status of your new server, type:

gluster peer probe 192.168.0.101

gluster peer status

If you’d like to work with names rather than IP addresses for your servers, you need to add them to the

hosts file on your admin machine. All you have to do is edit /etc/hosts with your text editor and add a

line (e.g. 192.168.0.101) for each server.


# List of alternatives to NFS and sshfs

https://en.wikipedia.org/wiki/Comparison_of_distributed_file_systems

https://en.wikipedia.org/wiki/Moose_File_System

https://github.com/moosefs/moosefs

https://burnhamforensics.com/2019/04/06/moosefs-build-and-installation-guide/

Needed:

4 CentOS servers of default storage (16-20GB) placed on your LAN

2 CentOS servers with two HDDs (16-20GB & ~15GB) attached and placed on your LAN, keeping one of the drives un-configured during OS installation
    

# Slow ARM CPU increase performance in sshfs while lowering security (use on LAN only)

https://serverfault.com/questions/924506/use-the-arcfour-cipher-with-sshfs-and-google-cloud

https://superuser.com/questions/32884/sshfs-mount-without-compression-or-encryption

https://unix.stackexchange.com/questions/163767/how-can-i-use-arcfour-encryption-over-sshfs

https://serverfault.com/questions/182671/how-could-one-speed-up-sshfs-in-lan

https://www.reddit.com/r/linuxquestions/comments/9gs0bm/samba_vs_nfs_vs_sshfs/

https://serverfault.com/questions/14577/what-network-file-sharing-protocol-has-the-best-performance-and-reliability


I would advise against NFS. Simply put - we had a web server farm, with JBoss, Apache, Tomcat and Oracle all

using NFS shares for common configuration files, and logging.

When the NFS share disappeared (admittedly a rare-ish occurrence) the whole thing just collapsed (predictable

really, and I advised the 'devlopers' against this config time shortcut).

There seems to be an issue with the version of NFS we were using that, if the target disappeared during a write,

the client would drop into a never ending wait loop, waiting for the NFS target to come back. Even if the NFS

box reattached - the loop still did not end.

We were using a mix of RHEL 3,4,5. Storage was on RHEL4, servers were on RHEL5, storage network was a separate lan,

and not running on vlans.

If there is a load balanced front end, checking single storage - would this not bottleneck your system?

Have you considered a read-only iSCSI connection to your storage, with an event driven script to move the uploaded

file to the storage via ftp/scp when a file is uploaded?

The only time I have implemented a successful centralised storage for multiple read heads was on an EMC storage array...

All other cost effective attempts had their drawbacks.

For sftp with no encryption, use sshfs + socat

On the server side run

socat TCP4-LISTEN:7777 EXEC:/usr/lib/sftp-server

And on the client side

sshfs -o directport=7777 remote:/dir /local/dir

Source: http://pl.atyp.us/wordpress/index.php/2009/09/file-transfer-fun/

share improve this answer

edited Jul 7 '13 at 21:46

WhyNotHugo

32333 silver badges1111 bronze badges

answered Nov 30 '12 at 3:15

endru

8111 silver badge11 bronze badge

    2
    
    While this may theoretically solve the problem, it would be preferred tos
    
    ummarize the link contents, and provide the link as reference – Canadian Luke Nov 30 '12 at 6:15
    
    3
    
    By default, socat TCP-LISTEN listens on all interfaces. To limit to one
    
    specific network interface (e.g., localhost), use the ,bind=127.0.0.1 option.
    
    To allow multiple connections to the server, add the ,fork option. Making a
    
    read-only server? Add -R to the EXEC command. In the end, it will look like
    
    this: socat TCP-LISTEN:7777,fork,bind=127.0.0.1 EXEC:'/usr/lib/sftp-server -R'
    
    (on Arch Linux, I had to use /usr/lib/ssh/sftp-server instead). – Lekensteyn Mar 22 '14 at 15:51
    

For a bit more security, your can also restrict the IP range with e.g. ,range=192.168.1.2/32, to only

allow one particular machine to connect. – Robin Dinse Jun 11 '18 at 13:11

Order of encription from fast to slow:

arcfour

chacha20-poly1305@openssh.com

rc4

Arcfour encription is not allowed and not supported because it is insecure encription.

The arcfour cipher is no longer supported in modern SSH servers because it is considered insecure.

You may want to try aes128-ctr or chacha20-poly1305@openssh.com

arcfour cipher deprecated and missing on most modern OpenSSH installations,

You can use chacha20-poly1305@openssh.com instead. – Mesut Tasci Oct 10 '18

Seems like the server does not want to allow it based onthe output of auth.log.

I'd try adding arcfour back into the SSH server's sshd_config file.

From the sshd_config man page:

excerpt

 Ciphers

        Specifies the ciphers allowed for protocol version 2.  Multiple
	
        ciphers must be comma-separated.  The supported ciphers are

        “3des-cbc”, “aes128-cbc”, “aes192-cbc”, “aes256-cbc”, “aes128-ctr”,
	
        “aes192-ctr”, “aes256-ctr”, “aes128-gcm@openssh.com”,
	
        “aes256-gcm@openssh.com”, “arcfour128”, “arcfour256”,
	
        “arcfour”, “blowfish-cbc”, and “cast128-cbc”.  The default is:

            aes128-ctr,aes192-ctr,aes256-ctr,arcfour256,arcfour128,
	    
            aes128-gcm@openssh.com,aes256-gcm@openssh.com,
	    
            aes128-cbc,3des-cbc,blowfish-cbc,cast128-cbc,aes192-cbc,
	    
            aes256-cbc,arcfour

Incidentally, there is nothing wrong with your -o Ciphers=arcfour switches from what I can tell.

I even found this SU Q&A titled: sshfs mount without compression or encryption that shows the same approach.

Mount with this options:

sshfs -o Ciphers=arcfour,Compression=no

compression slows on pack/unpack

arcfour is blazing fast !!!WHILE!!!! not 100% safe. LAN ONLY

Optional:

Adding auto_cache allows you to store file localy in temp somewhere so its not accessed via network second time.

with this 1GB connection to server is twice as fast for me.


# Scan network with nmap:

https://www.redhat.com/sysadmin/quick-nmap-inventory

With MAC address:

nmap -sn 192.168.1.0/24

https://vitux.com/find-devices-connected-to-your-network-with-nmap/

Less MAC address:

nmap -sP 192.168.1.0/24


# Archlinux install on Pogo computers that have uBoot

https://archlinuxarm.org/platforms/armv5/pogoplug-v2-pinkgray

https://obihoernchen.net/770/plug_computer_arch_linux/comment-page-2/

http://www.terminally-incoherent.com/blog/2012/09/19/set-up-a-home-linux-server-for-30-with-pogoplug/

https://forum.odroid.com/viewtopic.php?t=26894

https://www.hellion.org.uk/blog/posts/grub-on-uboot-on-qemu/

https://forum.doozan.com/read.php?3,10710,10816

https://www.cyrius.com/debian/kirkwood/sheevaplug/boot/

https://www.96boards.org/documentation/consumer/hikey/hikey620/guides/booting-linux-kernel-using-uboot.md.html

https://openwrt.org/docs/techref/bootloader

https://openwrt.org/docs/techref/bootloader/uboot

https://github.com/pepe2k/u-boot_mod

https://en.wikipedia.org/wiki/Linux_startup_process

http://mud-slide.blogspot.com/2013/12/installing-linux-on-zyxel-nsa-320-part_722.html#:~:text=coloured%20wires%20matching%20the%20original%20Nokia%20wiring.%20The%20unlabelled%20red

http://modernhackers.com/install-debian-linux-into-zyxel-326-nas/#:~:text=TTL%20Console%20adaptor%20was%20not%20able%20to%20communicate%20with%20my

https://watchmysys.com/blog/tag/nas/#:~:text=This%20method%20is%20not%20as%20reliable%20as%20soldering%20a%20header

https://forum.doozan.com/read.php?3,56534#:~:text=Attachments:%20open%20|%20download%20-%20back%20side%20mpcie%20usb%20card.jpg

https://gironi3.rssing.com/chan-14530749/all_p6.html

https://jelleraaijmakers.nl/2015/01/sheevaplug-with-debian-wheezy

https://github.com/mibodhi/u-boot-kirkwood/issues/2

https://github.com/mibodhi/u-boot-kirkwood

CONFIG_EFI_PARTITION   GPT partition table, common when EFI is the

				       bootloader.  Note 2TB partition limit; see
				       
				       disk/part_efi.c


# Tar extract to another directory

https://www.cyberciti.biz/faq/howto-extract-tar-file-to-specific-directory-on-unixlinux/

https://unix.stackexchange.com/questions/170699/how-to-specify-a-different-top-level-name-for-tar

Syntax

Untarring a file can be done using the following syntax. Typical Unix tar syntax:

tar -xf file.name.tar -C /path/to/directory

GNU/tar Linux syntax:

tar xf file.tar -C /path/to/directory

OR

tar xf file.tar --directory /path/to/directory

Extract .tar.gz archive:

tar -zxf file.tar --directory /path/to/directory

Extract .tar.bz2/.tar.zx archive:

tar -jxf file.tar --directory /path/to/directory

Where,

    x : Extract files

    f : Tar archive name

    --directory : Set directory name to extract files

    -C : Set dir name to extract files

    -j : Work on .tar.gz file format

    -z : Work on .tar.bz2 file format

    -v : Verbose output i.e. show progress on screen

Example: Extract files to another directory

In this example, I’m extracting $HOME/etc.backup.tar file to a directory called /tmp/data. First, you have to create the directory manually, enter:

mkdir /tmp/data

To extract a tar archive $HOME/etc.backup.tar into a /tmp/data, enter:

tar -xf $HOME/etc.backup.tar -C /tmp/data

To see a progress pass the -v option:

tar -xvf $HOME/etc.backup.tar -C /tmp/data


# Root make backup copy while mounted

https://unix.stackexchange.com/questions/256704/what-does-cp-x-do

cp -ax ,rsync -xor tar -one-file-system, all are the same.

It means not to cross file system boundaries.

A boundary between file systems is a mount point.

If you run df -a, you will see all files and mount points.

To help you understand with an example:

If you run df on your filesystem: df /, or on /usr directory: df /usr, you will

see that they are mounted on /, your installation partition. But if you run it on

/proc df /proc you will see that the mount point is different.

So if you issue a recursive copy on your filesystem cp -ax /, it won't copy /proc directory.

You could add -v option if you want to see exactly what is discarded and what is being copied.

As others pointed out, it's used with recursive and people generally use it for backup.

Also will not copy mounted partitions if using -x


# ATEN 4 HDMI ports 4K 4:4:4 switch, very good for TV, cable length 15 m not 6 m

https://www.aten.com/eu/en/products/professional-audiovideo/video-switches/vs481b/

Package Contents

    1x VS481B 4-Port HDMI Switch
    
    1x 2L-7D02H HDMI Cable (1,8m)
    
    1x 2XRT-0101G IR Remote Control
    
    1x 0AD6-1005-10AG Power Adapter
    
    1x User Instructions


# Lopifit band bike 2600 euro Netherlands

https://www.lopifit.nl/en/what-is-a-lopifit/


# OpenJDK vs OracleJDK

https://stackoverflow.com/questions/22358071/differences-between-oracle-jdk-and-openjdk

Both OpenJDK and Oracle JDK are created and maintained currently by Oracle only.

OpenJDK and Oracle JDK are implementations of the same Java specification passed the TCK (Java Technology Certification Kit).

Most of the vendors of JDK are written on top of OpenJDK by doing a few tweaks to (mostly to replace licensed proprietary

parts / replace with more high-performance items that only work on specific OS) components without breaking the TCK compatibility.

Many vendors implemented the Java specification and got TCK passed. For example, IBM J9, Azul Zulu, Azul Zing, and Oracle JDK.

Almost every existing JDK is derived from OpenJDK.

As suggested by many, licensing is a change between JDKs.

Starting with JDK 11 accessing the long time support Oracle JDK/Java SE will now require a commercial license.

You should now pay attention to which JDK you're installing as Oracle JDK without subscription could stop working. !!!


# US power get 240V if needed

https://www.youtube.com/watch?v=jMmUoZh3Hq4

-120V - 0V GND - +120V


# LibreOffice display issues

https://ask.libreoffice.org/en/question/111594/libreoffice-does-not-display-correctly/

Try to disable Tools - Options - View - Use OpenGL for all rendering

If you cannot open the 'Options' dialog using the mouse, you can try Alt+T Alt+O.


# Top 1000 de lucruri uimitoare

https://www.youtube.com/watch?v=y9TAzLJWANA


# chown and symbolic links

https://phoenixnap.com/kb/linux-chown-command-with-examples

To change the owner of a symbolic link, use the -h option. Otherwise, the ownership of the linked

file will be changed.

Tested: When using "chown -R user:user folder" it implies having -h also. Will modify permissions

on symlinks not target files. But anyway to be sure better use "chown -hR user:user folder" because

some /etc files may be symlinked and better keep root.

If the target file is in the same folder as the symlink chown -R or chown -hR do the same thing:

permissions are changed on the symlink and on the file. So -R really is -hR


# Very good 7 years old PC power supply

https://www.mygarage.ro/teste-si-review-uri/469244-review-sursa-thermaltake-smart-se-630w-gold.html

Nota: acesta este modelul SPS-0630M, a nu se confunda cu fratele SPS-630M


# Windows clock setting like Linux

https://superuser.com/questions/1336303/how-can-i-stop-linux-from-changing-windowss-clock

https://superuser.com/questions/494432/force-windows-8-to-use-utc-when-dealing-with-bios-clock

https://itectec.com/ubuntu/ubuntu-clock-time-is-off-on-dual-boot/

Linux time be like Windows:

timedatectl set-local-rtc 1 --adjust-system-clock

Windows time be like Linux:

On the Windows computer, open the Registry Editor

Run… regedit.exe and navigate to

HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\TimeZoneInformation.

Create a DWORD named RealTimeIsUniversal, and set its value to 1.

Create a file named WindowsTimeFixUTC.reg with the following contents and then double

click on it to merge the contents with the registry:

Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\TimeZoneInformation]

     "RealTimeIsUniversal"=dword:00000001

Note: Windows Time service will still write local time to the RTC regardless of the registry

setting above on shutdown, so it is handy to disable Windows Time service with this command

(if time sync is still required while in Windows use any third-party time sync solution):

sc config w32time start= disabled

Reversing the change

You can create a file with the following contents and then double-click it to merge in the original changes, as above:

Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\TimeZoneInformation]

     "RealTimeIsUniversal"=-

If Windows Time service was disabled, enable it again with the command:

sc config w32time start= demand


# WD Red white label:

https://www.techradar.com/reviews/g-technology-g-drive-4tb

WD Elements Desktop, 4TB, 3.5", USB 3.0, Black

Found inside a:

Western Digital WD40EMRX-82UZ0N0, otherwise known as the WD Red.

This is a 4TB hard disk drive spinning at 5400RPM with 64MB cache

Fine-tuned to deliver cooler temperatures in use and targets NAS users.


# Windows 10 skip login screen

https://www.isunshare.com/windows-10/2-ways-to-automatically-skip-windows-10-login-screen.html

WIN + R

netplwiz

Uncheck "User must enter a user name and password to use the computer".


# Check memory has ECC enabled:

https://www.pugetsystems.com/labs/articles/How-to-Check-ECC-RAM-Functionality-462


# Found cheaper mini PC solution with better cooling:

Asrock Deskmini A300 with Ryzen 3 3200G

https://www.youtube.com/watch?v=G62LbZHcO6o

https://www.youtube.com/watch?v=eZy4vKl9zDk

https://www.youtube.com/watch?v=U9eDqWqBt1U

AMD Ryzen 2400G has Wraith Stealth:

- just fits but scratches, no BIOS update required

AMD Ryzen 3400G has Wraith Spire:

- needs BIOS update, can be done with additional very low price and easy to find CPU:

Athlon 200GE

- cooler can fit if the fan is removed and mounted outside the case right on top of the cooler:

https://www.youtube.com/watch?v=QDlOwChawx0

Noctua NH-L9a-AM4 is the best cooler, thermals not much different than Wraith Spire:

https://www.youtube.com/watch?v=FbUm47PwpGo


# Repair non functional GPU with corrupted VBIOS

https://www.youtube.com/watch?v=gYf_gh1myEs


# Install mpv on Windows 10 Pro

https://mpv.io/installation/

https://sourceforge.net/projects/mpv-player-windows/files/

https://github.com/rossy/mpv-install/blob/master/README.md

https://www.reddit.com/r/mpv/comments/f0hsej/im_really_confused_about_how_to_get_hwdec_working/

PATH\mpv\mpv.conf

hwdec=d3d11va-copy

gpu-context=d3d11

gpu-api=auto

save-position-on-quit

audio-normalize-downmix=yes

volume=100

audio-channels=stereo


# Mount NFS share (Linux NAS) on Windows 10 Pro

https://graspingtech.com/mount-nfs-share-windows-10/

Turn Windows features on or off.

Scroll down and press + on Services for NFS and select all inside, then click OK.

Close.

mount -o anon \\10.1.1.211\mnt\vms X:


# DRM free guide

https://www.defectivebydesign.org/guide


# Make potato computer about 10% faster

https://www.youtube.com/watch?v=_kVIBfofamQ


# Mount Windows 10 share in Linux path

https://stackoverflow.com/questions/18442846/how-to-access-shared-folder-without-giving-username-and-password

https://www.thomas-krenn.com/en/wiki/Mounting_a_Windows_Share_in_Linux

Windows 10:

Network and sharing center > Change advanced sharing settings > Enable Turn Off password protect sharing option.

Do this to all by pressing ( \/ ) further down.

Enable 48 bit not 128 bit encryption.

Linux:

su -

apt install -y samba smbclient cifs-utils

mkdir /mnt/win10

sync

mount -t cifs //IP/share /mnt/win10 -o user=username


# Stop Windows 10 feature updates

https://www.askvg.com/fix-your-device-isnt-quite-ready-for-feature-update-to-windows-10/

https://www.askvg.com/how-to-prevent-or-skip-feature-update-installation-in-windows-10/

Type regedit in RUN or Start search box and press Enter. It'll open Registry Editor.

Now go to following key:

HKEY_LOCAL_MACHINE\SOFTWARE\Policies\Microsoft\Windows

Create a new key under Windows key and set its name as WindowsUpdate. If the key is already present, skip to next step.

Now in right-side pane, create a new DWORD DisableOSUpgrade and set its value to 1

Again go to following key in Registry Editor:

HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate

Create a new key under WindowsUpdate key and set its name as OSUpgrade

Now in right-side pane, create a new DWORD AllowOSUpgrade and set its value to 0

Close Registry Editor and restart your computer to take effect.

Now Windows 10 will not upgrade to new feature update in your computer.

PS: In future, if you want to install feature update, delete both DWORDs created in steps 4 and 7.


# Fix Windows 10 update corruption

Follow these instructions to download and install Windows 10 updates manually:

Open Edge (another Microsoft’s try to enforce Edge) and navigate to

https://www.catalog.update.microsoft.com/Home.aspx

In the search bar, type the exact registration number of the troubling update and search for it.

Download the update and install it.

Restart your PC.

OR

https://windowsreport.com/windows-10-update-is-not-ready/

Open Command Prompt with the administrative permission.

Copy-paste the following commands in the command line and press Enter after each:

net stop wuauserv

net stop bits

net stop cryptsvc

Ren %systemroot%SoftwareDistributionSoftwareDistribution.bak

Ren %systemroot%system32catroot2 catroot2.bak

net start wuauserv

net start bits

net start cryptsvc

Restart PC, navigate to Updates and try updating again.

OR

Right-click on the Start and open Command Prompt (Admin).

In the command line, type the following line and press Enter:

DISM /online /Cleanup-Image / ScanHealth

DISM /Online /Cleanup-Image /RestoreHealth

It’ll take up to 10 minutes for the DISM tool to repair corrupted files.

Restart your PC.

With the external installation media (if the first way fails):

Mount your Windows 10 installation media, either USB or ISO DVD.

Open Command Prompt (Admin) again.

In the command line, type the following commands and press Enter after each:

DISM /online /Cleanup-Image /CheckHealth

If there’s not any single error, enter this command and press Enter:

DISM /online /Cleanup-Image / ScanHealth

If DISM finds any errors, enter the following commands and tap Enter:

DISM /Online /Cleanup-Image /RestoreHealth /Source:repairSourceinstall.wim

Have in mind that you’ll need to replace the ”repair source” part of your installation media with the source path.

Wait for the procedure to end and restart your PC.


# Write corrupted flash BIOS chip:

https://www.flashrom.org/RaspberryPi

Prerequisites

Use latest Raspbian (or any other distribution with a recent kernel). Run the following commands (or make sure these kernel modules are loaded successfully):

modprobe spi_bcm2835 # If that fails you may wanna try the older spi_bcm2708 module instead

modprobe spidev

Connecting the flash chip

To learn more about the RPi's expansion header refer to http://elinux.org/Rpi_Low-level_peripherals . Please do not try to draw more than 50mA from the 3.3V pin. If the flash chip is still placed in a foreign circuit (e.g. soldered to a PC mainboard) please refer to ISP for further details.

RPi header	SPI flash

25	GND

24	/CS

23	SCK

21	DO

19	DI

17	VCC 3.3V (+ /HOLD, /WP)

Always connect all input pins of integrated circuits (not only flash chips).

In general the other pins (usually pin 3 is /WP and pin 7 is /HOLD) should be connected to Vcc unless they are

required to be floating or connected to GND (both extremely uncommon for SPI flash chips). Please consult the

datasheet for the flash chip in question.

If your flash chip is detected but your read/write/verify operations tend to fail, try to add decoupling capacitors

(one 100nF and one 4.7uF ceramic capacitor is preferred) close to the flash chip's power pin.

See Micron's TN-25-09: Layout Guidelines PCB Design Recommendations/VCC Power Supply Decoupling [1]

Running flashrom

Flashrom uses the Linux-native SPI driver, which is implemented by flashrom's linux_spi module. To use the RaspberryPi

with flashrom, you have to specify that driver. You should always tell it at what speed the SPI bus should run; you specify

that with the spispeed parameter (given in kHz). You also have to specify the Linux SPI device, e.g.

flashrom -p linux_spi:dev=/dev/spidev0.0,spispeed=1000


# Virtual machine in Debian Buster:

https://wiki.debian.org/KVM

https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/virtualization_host_configuration_and_guest_installation_guide/sect-virtualization_host_configuration_and_guest_installation_guide-windows_installations-installing_windows_xp_as_a_fully_virtualized_guest

https://www.linuxquestions.org/questions/linux-virtualization-and-cloud-90/virt-install-error-host-does-not-support-any-virtualization-options-4175483456/

```bash
apt-get install -y qemu-kvm libvirt-clients libvirt-daemon-system
apt-get install -y virt-manager
virt-install \
   --name=windowsos \
   --os-type=windows \
   --network network=default \
   --disk path=/var/lib/libvirt/images/windowsos.img,size=20 \
   --cdrom=/dev/sr0 \
   --graphics spice --ram=2048
```

# tar extract a file/folder not all

https://www.cyberciti.biz/faq/extracting-single-file-directory-from-tarball-2/

```bash
tar -xvf archive.tar  -C /extract-folder file-name
tar -xvf archive.tar  -C /extract-folder folder-name
tar -zxvf archive.tar.gz  -C /extract-folder file-name
tar -zxvf archive.tar.gz  -C /extract-folder folder-name
tar -jxvf archive.tar.bz2  -C /extract-folder file-name
tar -jxvf archive.tar.bz2  -C /extract-folder folder-name
```

# tar list contents

https://www.if-not-true-then-false.com/2010/list-tar-tar-gz-tar-bz2-contents/

tar -tvf archive.tar

or

tar --list --verbose --file=archive.tar

Add:

> list_of_archive.tar.txt


# tar.gz list contents

https://www.if-not-true-then-false.com/2010/list-tar-tar-gz-tar-bz2-contents/

tar -ztvf archive.tar.gz

or

tar --gzip --list --verbose --file=archive.tar


Add:

> list_of_archive.tar.gz.txt


# tar.bz2 list contents

https://www.if-not-true-then-false.com/2010/list-tar-tar-gz-tar-bz2-contents/

tar -jtvf archive.tar.bz2

or

tar --bzip2 --list --verbose --file=archive.tar

Add:

> list_of_archive.tar.bz2.txt


# Detect free space in MB for a mounted partition

https://stackoverflow.com/questions/7315587/bash-shortest-way-to-get-n-th-column-of-output/7315628

https://www.tecmint.com/how-to-check-disk-space-in-linux/

THEFREESPACE=$[$(df -m|grep /dev/sda1|awk '{print $4}')]

echo $THEFREESPACE

echo $((THEFREESPACE/80-1))


# Create random files fast avoiding /dev/random and /dev/urandom

https://superuser.com/questions/470949/how-do-i-create-a-1gb-random-file-in-linux

A little over 1 GB, base64 expands by about 33.(3) % :

openssl rand -out sample.txt -base64 $(( 2**30 * 3/4 ))

Exactly 1 GB:

The openssl command makes a file exactly 1 byte too big. The truncate command trims that byte off.

openssl rand -out sample.txt -base64 792917038; truncate -s-1 sample.txt

Exactly 65 MB:

openssl rand -out sample.txt -base64 $(( 2**26 * 3/4 ))

Exactly 80 MB:

openssl rand -out sample.txt -base64 61946641

Exactly 260 MB:

openssl rand -out sample.txt -base64 $(( 2**28 * 3/4 ))

Time for 260 MB on Marvell Kirkwood about 50 seconds.


# Log files detection

https://wiki.debian.org/RunningOnFlash

find / -xdev -printf '%T+ %P\n' | sort -n | tail -99

Edit /etc/rsyslog.conf and comment out any log files that are not required.

/var/run and /var/lock

sudoedit /etc/default/rcS

..then edit these lines:


RAMRUN=yes

RAMLOCK=yes

..so that /var/run and /var/lock are mounted in RAM.


# Gimp scale images

https://graphicdesign.stackexchange.com/questions/49941/how-can-i-shrink-these-areas-using-gimp

Open image in Gimp

CTRL+A

Select > Float

Float > Scale Layer

114 %

File > Export as...


# Debian Live

https://cdimage.debian.org/images/unofficial/non-free/images-including-firmware/current-live/


# Debian graphical OpenVPN

https://torguard.net/knowledgebase.php?action=displayarticle&id=53

apt-get install network-manager-openvpn-gnome


# Debian remove Wicd

https://help.ubuntu.com/community/WICD

Reverting to NetworkManager

For Gnome / Unity

Open up a Terminal and execute the following commands:

Install NetworkManager:

sudo apt-get install network-manager-gnome network-manager

Then remove WICD:

sudo apt-get remove wicd wicd-gtk

Restart your system.

Confirm everything is working, then remove WICD config files:

sudo dpkg --purge wicd wicd-gtk

For KDE (Kubuntu)

Open up a Terminal and execute the following commands:

Install NetworkManager:

sudo apt-get install plasma-widget-networkmanagement network-manager

Then remove WICD:

sudo apt-get remove wicd wicd-kde

Restart your system.

Confirm everything is working, then remove WICD config files:

sudo dpkg --purge wicd wicd-kde


# Trick for adding icu-config in Debian 10 which is missing this library (work in progress)

http://forum.tinycorelinux.net/index.php?topic=1884.0

https://github.com/LinearTapeFileSystem/ltfs/issues/153

nano /usr/bin/icu-config

```bash
#!/bin/sh
opts=$1
case $opts in
  '--cppflags')
  echo '' ;; 
  '--ldflags')
    echo '-licuuc -licudata' ;;
  *)
    echo '/usr/lib/x86_64-linux-gnu/icu/pkgdata.inc' ;;
esac
```

chmod +x /usr/bin/icu-config


# Change default ssh server port

https://www.ubuntu18.com/ubuntu-change-ssh-port/

https://forum.mxlinux.org/viewtopic.php?f=108&t=47652

```bash
su -
sudo -s
nano /etc/ssh/sshd_config
```

Port 22000

```bash
systemctl restart sshd
/etc/init.d/ssh restart
```

# Test hard disk reliability before using it, deletes all data

https://www.reddit.com/r/DataHoarder/comments/575vn4/is_there_anything_faster_than_badblocks_wsv/

https://explainshell.com/explain?cmd=badblocks+-wsv

su -

sudo -s

badblocks -wsv -b 4096 /dev/sdX

Just do a single pass of writes, and a short SMART test. Should be enough if you

want an initial "is the disk clean" check. After buying, or before selling a drive:

badblocks -wsv -b 4096 -t 0x00 /dev/sdX

smartctl -t short /dev/sdX


# Bucharest good food restaurant:

https://start-up.ro/ciotoianu-omul-care-ncinge-gratarele-in-popesti/

https://utopiabalcanica.net/2018/04/16/la-o-parte-cocosatu-regele-micilor-e-la-popesti-si-i-spune-ciotoianu/

# Raspberry Pi reduce power consumption

https://www.reddit.com/r/raspberry_pi/comments/aki1is/reducing_power_of_the_raspberry_pi_3_b/

https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md

https://www.raspberrypi.org/forums/viewtopic.php?t=257144

dtoverlay=pi3-disable-wifi

dtoverlay=pi3-disable-bt

arm_freq=600

arm_freq_max=600

arm_freq_min=600

gpu_freq=250

core_freq=250

core_freq_min=250

sdram_freq=250

sdram_freq_min=250

over_voltage=-3

over_voltage_min=-3

initial_turbo=30

temp_limit=60


# Raspberry Pi default analog sound driver upgraded, about from 30% CD to 90% CD

https://www.raspberrypi.org/forums/viewtopic.php?f=29&t=195178

https://www.raspberrypi-spy.co.uk/2014/07/raspberry-pi-model-b-3-5mm-audiovideo-jack/

https://www.element14.com/community/thread/42067/l/audio-line-in-for-raspberry-pi?displayFullThread=true

https://www.element14.com/community/community/raspberry-pi/raspberry-pi-accessories/cirrus_logic_audio_card?ICID=Pi-Accessories-wolfson-audio-space

https://www.richardmudhar.com/blog/2018/02/cirrus-logic-audio-card-for-the-raspberry-pi-revisited/

https://www.horus.com/~hias/cirrus-driver.html

Configuration for kernel 4.9 and newer

Enable the driver

Add the following line to /boot/config.txt to enable the Cirrus Logic card driver

dtoverlay=rpi-cirrus-wm5102

Note: MMAP support is already enabled by default in kernel 4.9, you no longer need to add the i2s-mmap overlay.

Setup module dependencies

The cirrus driver requires some manually defined module dependencies, if you fail to add them the driver won't load properly.

Create a file /etc/modprobe.d/cirrus.conf with the following content:

softdep arizona-spi pre: arizona-ldo1

Download and install "usecase" mixer scripts

Download cirrus-ng-scripts.tgz and extract it for example in /home/pi/bin.


wget http://www.horus.com/~hias/tmp/cirrus/cirrus-ng-scripts.tgz

mkdir bin

cd bin

tar zxf ../cirrus-ng-scripts.tgz

Note: If you had installed the older usecase scripts for kernel 4.4 you need to remove them.

You have to start the appropriate scripts before you can use the card. For example:

./Reset_paths.sh

./Playback_to_Lineout.sh

./Playback_to_SPDIF.sh

./Record_from_Linein.sh

Additional configuration steps

Disable RPi on-board audio device snd-bcm2835

On Raspbian the RPi on-board audio device snd-bcm2835 is enabled by default. When you run aplay -l you'll see

two cards, the Cirrus card sndrpiwsp and the on-board card bcm2835.


If you don't need on-board audio you can disable it by removing (or commenting out) the dtparam=audio=on

line from /boot/config.txt.

#dtparam=audio=on

Optionally: use fixed card number

If you don't want to disable snd-bcm2835 or if you also have an USB audio device connected you might notice that

the card numbers under which the drivers register will change. Sometimes the Cirrus card will card 0, sometimes

on-board audio. The card number depends on which driver is registered first, which is purely random.

You can manually assign fixed card (slot/index) numbers using the slot option of the snd module. For example, if

you want the Cirrus card always to be the first and on-board audio the second one, add the following line to your

/etc/modprobe.d/cirrus.conf file:

For kernel 4.9 add this line:

options snd slots=snd-soc-rpi-cirrus,snd-bcm2835


# NFS on Debian (6 times faster than sshfs transfer for very slow CPU)

https://wiki.debian.org/NFS/Kerberos

https://wiki.debian.org/NFSServerSetup

https://forum.doozan.com/read.php?2,28351

https://kerneltalks.com/troubleshooting/mount-nfs-requested-nfs-version-or-transport-protocol-is-not-supported/

https://www.cyberciti.biz/faq/howto-see-shares-on-nfs-server-exported-filesystems/

http://www.brennan.id.au/19-Network_File_System.html#nfs4

https://linuxsecurity.com/features/features/nfs-security


# BTRFS on Kirkwood Zyxel NSA3xx

https://forum.doozan.com/read.php?2,33146,33148

https://forum.doozan.com/read.php?2,23630


# Udev rules for automounting USB drives

https://forum.doozan.com/read.php?2,24139


# Zyxel NSA3xx install ssh as dropbear and nfs with 2019 repository (not needed for replacing stock firmware and uboot)

http://zyxel.diskstation.eu/Users/Mijzelf/zypkg-repo/README.html

https://linuxize.com/post/how-to-mount-an-nfs-share-in-linux/

1) Download this file

http://zyxel.diskstation.eu/Users/Mijzelf/zypkg-repo/web_prefix

2) Put the file in the directory \\<NAS>\admin\zy-pkgs\ on the NAS. Make sure your browser doesn't
	
add any extensions to the file. The filename is web_prefix, without extension, all lowercase characters.

3) More info

For the NSA212, there is no support for the web_prefix file, so the procedure is a bit different. Enable the Telnet Backdoor, login over telnet (as root), and execute

echo http://zyxel.diskstation.eu/Users/Mijzelf/zypkg-repo/Medion >/etc/package_src_url

NSA series

In the packages menu in the webinterface execute 'Retreive list from Internet'.

Install MetaRepository. Execute 'Retreive list from Internet' again.

NAS series

On the webinterface desktop, open the 'App Centre'. Goto 'Browse->All Apps'. Now there is a

cycle icon on the page. Push it. install MetaRepository. Push the cycle icon again.

Medion
In the package menu in the webinterface execute 'Retreive list from Internet'. Do not try to

install anything else than MetaRepository. When MetaRepsitory is installed, goto the MetaRepository page, and change

'ftp://ftp2.zyxel.com/+' in 'ftp://ftp2.zyxel.com/NSA310/4.40/zypkg/' (without the quotes), and if you also want the

Medion repository add a line 'ftp://nas-download:sEhtalr@download.medion.de/package/'. Now hit apply, and re-retreive

the package list. Warning: not all ZyXEL packages run on a Medion. Make sure you first install dropbear, and test it,

to have an entrance if the webinterface fails.

How to configure

NSA series, Medion

In the package menu you can open the configuration page of MetaRepository.

NAS series

On the desktop you'll find a MetaRepository icon to open the configuration page.

Here you can enter the source repositories. When a package with the same name and version can be found in several

repositories, the repository highest in this list is used. Further you can blacklist packages.

How to create your own (local) repository

Just put the package(s) in a share, in combination with a describing ZYPKGS file (case sensitive). Add the full

path to the 'Repositories' list. Done. You can also supply a ZYPKG_INFO.tgz if you like. The script first tries

to download/copy the ZYPKG_INFO.tgz file, and if that fails it tries ZYPKGS.

NFS full path not just folder name.


# Zyxel NSA3xx install latest uboot for loading modern kernels

https://forum.doozan.com/read.php?3,12381

https://forum.doozan.com/read.php?3,27280

http://davidegironi.blogspot.com/2018/12/debian-and-openmediavault-on-zyxtel.html#.XzzJPOgzaUk

http://blog.qnology.com/2015/04/hacking-pogoplug-v3oxnas-proclassic.html

https://forum.openmediavault.org/index.php?thread/4271-omv-on-zyxel-nsa320/

1) get root access in stock firmware by 

a) resetting the NAS

Locate the reset button hole between the power connector and Ethernet cable at the back of the NSA box.

Take a small pin and push the button until you hear a single beep (it will beep in about 1 or 2 seconds).

Now remove the pin (ie; dont push anymore). Now, you can login with your default admin

userid/password of admin/1234 with the web browser.

None of your data or settings are lost by doing this!

Maybe the configuration is corrupted and an over 5 second reset is needed (often I have needed this):

Wait 1 minute after started, hold reset button, hear 1 beep, wait 5 seconds, hear 1 beep, wait 1 second,

release reset button, hear 3 short beeps - this is the factory reset mode so wait 3 minutes for it to complete.

All data will be lost by doing factory reset !

b) start the NAS, find the ip address, setup and login as admin / 1234 with web browser

c) copy the full http link address while logged in from the browser into a text file, save the text file

d) with XXXXX available from the saved link make this new link and put it in browser and press enter

Firmware 4.40

http://<ip-of-nas>/zyxel/cgi-bin/remote_help-cgi?type=backdoor

Firmware 4.60

http://<ip-of-nas>/rXXXXX,/adv,/cgi-bin/remote_help-cgi?type=backdoor

e) only a white page or error page will be displayed but this has started something needed in the NAS

f) this started something will allow telnet login as admin/1234 but as regular user ($)

if you reboot the NAS, go back to b) because the started something needs to be started after every boot

g) to have root access there is NsaRescueAngel method that was used as a remote assistance tool for service:

telnet <ip-of-nas>
	
user is admin and the password is the same as you use for the web administration GUI.

This user does not have full root rights but can be used to generate the NsaRescueAngel (root) password. Run:

/sbin/makekey

This will give you some short password e.g.

FaEWaQO3

Disconnect Telnet

Telnet again and login as root

telnet <ip-of-nas>
	
This time use user NsaRescueAngel and the short password you got from step 2 above. e.g. FaEWaQO3

Now you should have full root and own the box. This is enough to install new uboot, no need for ssh

(Dropbear) to be installed, anyway it is more complex to install ssh (Dropbear) so better avoid this.

g) As NsaRescueAngel having maximum power change the root password with passwd

h) You can from now on login with root and the set up password but you still after every boot first need

to login in the browser as admin/1234 and http://<ip-of-nas>/rXXXXX,/adv,/cgi-bin/remote_help-cgi?type=backdoor

i) Telnet or Dropbear is very simple and will not allow a lot of things like complex bash scripting

Backdoor

You can open a telnet backdoor by pressing the reset button for about 6 seconds (until you hear

the second beep). This backdoor will close after about 2 minutes without activity.

On newer firmwares (>3.23?) this backdoor is closed, but now there is a webinterface backdoor;

login on the userinterface as admin, and then enter the url:

http://<ip-of-nas>/zyxel/cgi-bin/remote_help-cgi?type=backdoor

This will give a white page, there is no further conformation.

There is 1 usable login account, username NsaRescueAngel, which has root rights. The password

can be found by running /sbin/makekey. It outputs a hash on the MAC address of the box.

This is a Catch-22. On older firmwares (< 3.0?) there is a second login, username admin password

root, which has limited rights, but which can be used to determine the NsaRescueAngel password.

On newer firmwares the default shell for this user is none.

To find the password on a newer firmware, there are several options:

Make a file with name "mykey.php" containing with no spaces " " :

< ? 

  echo shell_exec('/sbin/makekey'); 
  
  exit;
  
Copy to the NSA, and open through a Web share (not File Explorer) - your

NsaRescueAngel password is returned.

Use an FFP stick, and run /sbin/makekey

Run makekey on another NSA-2xx, and provide the right MAC address (All caps,

seperated by colons) /sbin/makekey AA:BB:CC:DD:EE:FF

Download makekey and run it on your favorite ARM Linux box, providing the right

MAC address (and hope that it runs).

Update NSA310 Firmware 4.22

The backdoor is opened by calling the script /usr/local/btn/open_backdoor.sh.

The troubles with finding the key or using the NsaRscueAngel account are over!

You can now use telnet with root and use the same password you use for admin!

(Better would have been to allow login with "admin" only and then go su.)

For the NSA-310 you can also use the webinterface backdoor

http://<ip-of-nas>/zyxel/cgi-bin/remote_help-cgi?type=sshd_tdc
	
Root Access: NSA310 Firmware V4.70(AFK.1)

Open up telnet backdoor.

This is done by logging into the normal web administration GUI and then opening this URL:

http://<ip-of-nas>/r41773,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
The r41773 part is some number that changes by firmware revision but will be the same part as

the URL you normally get when opening the administration GUI.

Telnet to find root password

telnet <ip-of-nas>
	
user is admin and the password is the same as you use for the web administration GUI.

This user does not have full root rights but can be used to generate the NsaRescueAngel (root) password. Run:

/sbin/makekey

This will give you some short password e.g.

FaEWaQO3

Disconnect Telnet

Telnet again and login as root

telnet <ip-of-nas>
	
This time use user NsaRescueAngel and the short password you got from step 2 above. e.g. FaEWaQO3

Now you should have full root and own the box.

Update NSA-320/NSA-325 Firmware 4.40

The SSH webinterface backdoor does not seem to work (returns result=0). The telnet webinterface

backdoor can be used with this URL:

http://<ip-of-nas>/zyxel/cgi-bin/remote_help-cgi?type=backdoor
	
You can login using root with the admin password.

Note: If you changed the admin password, telnet login might not work.

Follow these steps to fix this problem:

Change the admin password back to default using the web interface.

Login using telnet with root and the default admin password

Change the password through telnet using "passwd root".

Afterwards, you can change the admin password on the web interface back to what you prefer.

Update NSA-300 series Firmware 4.60

The /zyxel/ part is exchanged by /r38657,/adv,/, so the complete url is

http://<ip-of-nas>/r38657,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
The number is different for each box and each firmware version. I think it's a revision number

from SVN, or somethink like that. To find out what it's for you, have a look at the urls generated

when you are entering the webinterface.

Medion boxes

On Medion boxes the /zyxel/ part of the url is obfuscated.

Firmware 1.00:

http://<ip-of-nas>/r32694,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
Firmware 1.01(UZD.0):

http://<ip-of-nas>/r34814,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
Firmware 1.01(UZD.2):

http://<ip-of-nas>/r36258,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
You can login as root, using the admin web password.

On Medion boxes it is possible to start a telnet daemon on the NAS, by requesting a webpage:

Firmware 1.00:

http://<ip-of-nas>/r32694,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
Firmware 1.01(UZD.0):

http://<ip-of-nas>/r34814,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
Firmware 1.01(UZD.2):

http://<ip-of-nas>/r36258,/adv,/cgi-bin/remote_help-cgi?type=backdoor
	
You can login as admin, or as root, using the admin web password.

If your firmware version is not listed here, login on the webinterface of your nas, and

have a look at the url to find out the 'r value' of your firmware version. Some browsers

won't show this, in that case copy the url and paste it in notepad, or something like that.

(On most NSA boxes this telnet backdoor also works. Just find the 'r value' of your firmware.)

2) get a volume formated and a public share started on that volume to put files in (can be external

on USB, but if external on USB install of ssh (Dropbear) can not be done because the needed /admin

share in which to put the link to the repository is only allowed on an internal volume

3) with Windows Download this files from Dropbox (top right of site has down arrow = Download file)

and put in the NAS share (you can see it on Windows Network):

https://bitly.com/2iC3E69 as uboot.tar

https://bitly.com/1RdYW8S as tools.tar.gz

https://bitly.com/1sMwD7b as uenv.tar

4) login as root and change directory to /e-data/xxxxxxxxx and extract all files in one folder

```bash
cd /e-data/xxxxxxxxx
tar xf uboot.tar
tar xzf tools.tar.gz
tar xf as uenv.tar
```

5) as root remove the 3 archive files, copy all the files from /e-data/xxxxxxxxx/tools in

/e-data/xxxxxxxxx , remove directory /e-data/xxxxxxxxx/tools and copy all the files from

/e-data/xxxxxxxxx to /tmp and change directory to /tmp

```bash
cd /e-data/xxxxxxxxx
rm uboot.tar
rm tools.tar.gz
rm uenv.tar
cp /e-data/xxxxxxxxx/tools/* /e-data/xxxxxxxxx/
rmdir /e-data/xxxxxxxxx/tools/
cp /e-data/xxxxxxxxx/* /tmp
```

6) save the current environment values as MAC network address, etc. will be lost !

./fw_printenv > oldenvuboot.txt

cp oldenvuboot.txt /e-data/xxxxxxxxx

Copy oldenvuboot.txt from NAS in Windows C: drive

7) Copy oldenvuboot.txt from the NAS to Windows C: drive. The MAC address is needed so it can be set up in the new uboot.

8) install as instructed from /tmp, first try ./executable an then executable

From my tests I have found often that only erasing flash, writing uboot image and writing uboot environment configuration

image works and what will not work is fw_setenv command writing to the configuration while still being in the stock firmware.

And the uboot environment configuration has wrong settings for another architecture but this can be fixed by adding to the

Debian rootfs a file boot/uEnv.txt having this:

dtb_file=/boot/dts/kirkwood-nsa320.dtb

And after this you can boot into the Debian rootfs and fix other fw_setenv commands so you have good MAC, SATA and USB

9) Anyway try to also do the environment install command updates, I see that netconsole is actually not needed so avoid adding:

fw_setenv preboot_nc 'setenv nc_ready 0; for pingstat in 1 2 3 4 5; do; sleep 1; if run if_netconsole; then setenv nc_ready 1; fi; done; if test $nc_ready -eq 1; then run start_netconsole; fi'

fw_setenv preboot 'run preboot_nc'

For safety check uboot has this info correct, if not or to be sure add to /boot/uEnv.txt

```bash
fw_setenv mtdparts 'xxxxxxxxx'
fw_setenv ethaddr 'xx:xx:xx:xx:xx:xx'
```

Also check you add this info in the uboot environment:

fw_setenv dtb_file '/boot/dts/kirkwood-nsa320.dtb'

https://www.denx.de/wiki/view/DULG/UBootEnvVariables

ipaddr = ip address of the booted device

serverip = TFTP server ip address or desktop PC to view netconsole messages

netmask = 255.255.255.0 used to know broadcast address of local network (no broadcast to internet)

https://forum.doozan.com/read.php?3,14,14

https://archlinuxarm.org/forum/viewtopic.php?f=53&t=9823

https://wiki.archlinux.org/index.php/Netconsole

```bash
fw_setenv preboot_nc 'setenv nc_ready 0; for pingstat in 1 2 3 4 5; do; sleep 1; if run if_netconsole; then setenv nc_ready 1; fi; done; if test $nc_ready -eq 1; then run start_netconsole; fi'
fw_setenv preboot 'run preboot_nc'
fw_setenv ipaddr    '192.168.1.xxx'
fw_setenv serverip '192.168.1.yyy'
fw_setenv netmask '255.255.255.0'
```

Not required to be used in this case:

ncip = netconsole uboot ip

ncipk = netconsole kernel ip

From what I understand netconsole will only transmit on local network, worst case broadcast address,

when kernel is being booted netconsole of kernel takes over and netconsole of Uboot stops sending.

No ttl serial really needed if for editing you remove storage from NSA320 and edit on another computer

LED status information from this new uboot 2017

# Blue first LED = power is on

# Green blinking second LED = kernel has been found and is being loaded

# Green second LED = boot has been OK, now ssh login to the NAS should work

# Yellow blinking second LED = rootfs device not found, kernel not booted, try using USB stick with

Debian rootfs in front USB port, makeimage commands on uImage, /boot/uEnv.txt, repartition

uboot 2017 from my experinence can not see very fast USB sticks, larger than 2 TB HDD's and any SSD,

probably uboot does not know to negociate SATA speed and USB speed for newer devices to ask to be slower

# Yellow second LED = rootfs device has been found, kernel booted but something else not ok like uInitrd

or bad information about architecture, try fixing with /boot/uEnv.txt or makeimage commands on uInitrd

# LAN green LED = eth0 working, NAS should be seen on the network

# LAN only yellow LED = eth0 not working, NAS is behaving like having LAN cable unplugged


# Zyxel NSA320 install working Debian

https://stackoverflow.com/questions/3913246/cannot-send-character-with-minicom

https://forum.doozan.com/read.php?2,12096

https://forum.doozan.com/read.php?2,25643

https://forum.doozan.com/read.php?2,88409,88424

https://forum.doozan.com/read.php?3,62649,63144

https://forum.doozan.com/read.php?2,79219,79334

https://gironi3.rssing.com/chan-14530749/all_p6.html

Configure network and ssh:

Now edit the file etc/network/interface cause eth0 will be named enp1s0 on NSA310 as follow:

  auto enp1s0
  
  allow-hotplug enp1s0
  
  iface enp1s0 inet dhcp
  
Plug the usb key to the NSA310 and boot it.

The NSA should boot up with Debian.

If you set your network interface as above you should found it on your DHCP leases.

You should be able to log in the ssh terminal using the default username and passwod: root for both.

One logged in renew the ssh keys by using the following 

rm /etc/ssh/ssh_host*

ssh-keygen -A

apt-get update

apt-get upgrade

And change your root password using passwd.

You are now ready to install things on your Debian NAS.

Setup Debian:

The most usefull software you can install in your NAS, to me, it's OpenMediaVault.

Installing OpenMediaVault in your new NAS is really simple, you just have to add the openmediavault repository package,

using the command (you can see this correctly by viewing raw text):

```bash
cat <<EOF >> /etc/apt/sources.list.d/openmediavault.list
deb http://packages.openmediavault.org/public arrakis main
# deb http://downloads.sourceforge.net/project/openmediavault/packages arrakis main
## Uncomment the following line to add software from the proposed repository.
# deb http://packages.openmediavault.org/public arrakis-proposed main
# deb http://downloads.sourceforge.net/project/openmediavault/packages arrakis-proposed main
## This software is not part of OpenMediaVault, but is offered by third-party
## developers as a service to OpenMediaVault users.
# deb http://packages.openmediavault.org/public arrakis partner
# deb http://downloads.sourceforge.net/project/openmediavault/packages arrakis partner
EOF
```

Then run the following come to install the openmediavault software:

```bash
export LANG=C

export DEBIAN_FRONTEND=noninteractive

export APT_LISTCHANGES_FRONTEND=none

apt-get update

apt-get --allow-unauthenticated install openmediavault-keyring

apt-get update

apt-get --yes --auto-remove --show-upgraded \
    --allow-downgrades --allow-change-held-packages \
    --no-install-recommends \
    --option Dpkg::Options::="--force-confdef" \
    --option DPkg::Options::="--force-confold" \
    install postfix openmediavault
```
Initialize the system and database:

omv-initsystem

Now you should be able to login to the web interface of your NSA310 openmediavault using the default username and password, that are admin and openmediavault.

From the OpenMediaVault panel you can set the static address of your network card, of course you can to this by terminal also.

You can format and mount your hard drive, and setup your sharing.

You can find a lot usefull information on the openmendiavault documentation page https://openmediavault.readthedocs.io/en/latest/

Completed openmediavault install.

Read CPU temperature:

cat /sys/class/hwmon/hwmon0/device/hwmon/hwmon0/temp1_input

Avoid Archlinuxarm for NSA320 - tested not ok

1) get the rootfs:

https://bitly.com/2Nx3XfY as debianrootfs.tar.bz2

2) extract to new ext2/3/4 partition on storage to be used inside NSA3xx

3) having newest uboot 2017xxxx add /boot/uEnv.txt having dtb_file=/boot/dts/kirkwood-nsa320.dtb

without correct dtb_file eth0 network may not work

4) make static ip:

On another computer edit /etc/network/interfaces

auto lo eth0

iface lo inet loopback

iface eth0 inet static

	address	ip.ip.ip.wanted
	
	netmask	255.255.255.0
	
	network	ip.ip.ip.0
	
	broadcast ip.ip.ip.255
	
	gateway	ip.ip.ip.1

Maybe it is not eth0 and something else:

https://wiki.debian.org/NetworkConfiguration#Using_DHCP_to_automatically_configure_the_interface

```bash
rm /etc/ssh/ssh_host*
ssh-keygen -A
apt-get update
apt-get upgrade
```

If upgrade log has update-initramfs: Generating /boot/initrd.img-5.2.9-kirkwood-tld-1

```bash
cd /boot
mkimage -A arm -O linux -T ramdisk -C gzip -a 0x00000000 -e 0x00000000 -n initramfs-5.2.9-kirkwood-tld-1 -d initrd.img-5.2.9-kirkwood-tld-1 uInitrd
```

Activate systemd:

nano /boot/uEnv.txt

Add line:

custom_params=init=/bin/systemd


# Scart to HDMI

https://www.ebay.com/itm/SCART-HDMI-To-HDMI-720-1080P-HD-Video-Converter-Box-For-HDTV-DVD-US-EU-Adapter/293202446742?hash=item44443a4996:m:msRHB4Y1RUNnc2xfZC4MKNw


# Get 3D digital to 3D print

https://old.reddit.com/r/3Dprinting/comments/et91oc/i_recently_had_a_full_head_mri_using_free/ffev4aq/


# Open book reader (small screen, audio book support)

https://github.com/joeycastillo/The-Open-Book

https://liliputing.com/2020/01/the-open-book-ereader-will-be-a-real-thing-you-can-buy-eventually.html

https://hackaday.io/project/168761-the-open-book-feather

https://hackaday.io/contest/168107-take-flight-with-feather


# Linux view CPU-Z like info

https://www.cyberciti.biz/faq/check-ram-speed-linux/

https://packages.debian.org/stretch/dmidecode

dmidecode --type 17


# OpenWRT access point

https://www.hanselman.com/blog/ConfiguringTwoWirelessRoutersWithOneSSIDNetworkNameAtHomeForFreeRoaming.aspx

https://openwrt.org/docs/guide-user/network/wifi/dumbap

https://roll.urown.net/router/bridged-ap.html


etc config network

config interface 'lan'

    option type 'bridge'
    
    option ifname 'eth0'
    
    option _orig_ifname 'eth0 wlan0 wlan1'
    
    option _orig_bridge 'true'
    
    option proto 'static'
    
    option ipaddr '192.0.2.2'
    
    option netmask '255.255.255.0'
    
    option gateway '192.0.2.1'
    
    option broadcast '192.0.2.255'
    
    option dns '192.0.2.2 192.0.2.1'
    
    option ip6gw 'fe80::4e5e:cff:fe40:11a5'
    
    option ip6addr '2001:db8:c0de:10:2cb0:5dff:fe7f:2dba/64'

config interface 'wan'

    option ifname 'eth1'
    
    option proto 'dhcp'
    
    option auto '0'

config interface 'wan6'

    option ifname 'eth1'
    
    option proto 'dhcpv6'
    
    option auto '0'
    
    option reqaddress 'try'
    
    option reqprefix 'auto'

config route

    option interface 'lan'
    
    option target '0.0.0.0'
    
    option netmask '0.0.0.0'
    
    option gateway '192.0.2.1'

config route6

    option interface 'lan'
    
    option target '::/0'
    
    option gateway 'fe80::4e5e:cff:fe40:11aa'
    
    option metric '1024'



etc config dhcp

config dhcp 'lan'

    option interface 'lan'
    
    option ignore '1'


# Play 64 bit games in Wine - EVE Online

EVE Online is not easy as Alpha, you need Omega account which costs some money.

But it is good for testing wine 64 bit functionality.

```bash
su -

dpkg --add-architecture i386 && apt-get update && apt-get install -y wine && sync

apt-get install -y winetricks mono-complete fonts-wine ttf-mscorefonts-installer

exit
```

Download EVE Online from www.eveonline.com (signup if not already)

rm -rf .wine/

Do not create 32 bit Wine prefix, leave it 64 bit.

Do not disable d3d11 library - it is working fine in the default Wine package from Debian Buster.

```bash
winecfg
winetricks d3dx9_36
winetricks ddr=opengl multisampling=enabled orm=fbo videomemorysize=2048 strictdrawordering=enabled
wine ~/Downloads/EveLauncher-1602194.exe
mv ~/.wine/drive_c/EVE/ ~/
cd EVE/
wine eve.exe 
```

# Windows 10 Home, Pro, Enterprise, Enterprise LTSB can be left unactivated but only for trial period

https://linustechtips.com/main/topic/956409-windows-10-ltsb-no-more-damn-updates/

https://superuser.com/questions/1024274/how-long-can-i-use-windows-10-without-activation

https://www.reddit.com/r/windows/comments/6tn72g/windows_10_enterprise_ltsb/


# Ryzen 5 CPU Debian Linux and Ubuntu Linux

Debian 10.2 does not have "driver" in linux 4.19 amdgpu for Ryzen 5 Picasso. Need to use Ubuntu 19.10 directly,

or you need to install Debian without any desktop, then install the linux kernel and linux firmware from backports

not needed to also update firmware-linux-free

Why Debian ? Because it has << stable >> Chromium with accelerated video decoding by default, just need to enable

in chrome://flags the "Override software rendering list" and check hardware video decoding status in chrome://gpu

and check that it works with for example:

https://www.youtube.com/watch?v=LXb3EKWsInQ

Bear in mind that actual 4k content at 60 Hz will overload the notebook integrated GPU so you need to keep the setting at

1080p 60 HZ or maybe if ok 1440p 60 Hz. This limitation is because of integrated GPU RAM speed more so if no dual channel

Procedure:

1 - update notebook BIOS (with an unactivated Windows 10 fresh install if notebook does not have Windows),

this is required because Debian is too unstable with older than year 2019 BIOS, important to fix ACPI and AMD microcode

2 - install Debian with an over 400 MB ESP EFI partition (linux kernels need space), deselected Debian Desktop Environment

and deselected all desktops, not keeping Windows 10 at all but keeping end of storage drivers partition if available

3 - Force debian to update the linux kernel to a very new version with this steps:

aaa - start in normal mode, if not working press power button, then start in recovery mode and type root password every time until otherwise stated down here

bbb - Install new kernel with this commands in console as root

ip a

Remember the name of the LAN network, for example enp1s0f1

nano /etc/network/interfaces

Add this in /etc/network/interfaces

allow-hotplug enp1s0f1
iface enp1s0f1 inet dhcp

CTRL+o, ENTER, CTRL+x

nano /etc/apt/sources.list

https://backports.debian.org/Instructions/

https://wiki.debian.org/Backports

Add this in /etc/apt/sources.list

deb http://deb.debian.org/debian buster-backports main contrib non-free

deb-src http://deb.debian.org/debian buster-backports main contrib non-free

CTRL+o, ENTER, CTRL+x

```bash
apt-get update
apt-get -t buster-backports install linux-image-amd64
apt-get -t buster-backports install firmware-linux-nonfree
apt-get -t buster-backports install btrfs-progs
sync
reboot
```

ccc - Run tasksel as root to install a Desktop environment

If you need to install another desktop than Gnome and Gnome is already installed, exit tasksel and:

https://unix.stackexchange.com/questions/110882/installing-and-uninstalling-gnome-on-debian/518850

```bash
rm /usr/share/xsessions/gnome.desktop 
rm /usr/share/xsessions/openbox.desktop 
aptitude purge `dpkg --get-selections | grep gnome | cut -f 1`
aptitude -f install
aptitude purge `dpkg --get-selections | grep deinstall | cut -f 1`
aptitude -f install
tasksel
```

In tasksel select Debian Desktop Environment and also the Desktop you want like MATE on LXDE

```bash
sync
reboot
```

ddd - if you want, use https:/github.com/iugamarian/debianims to optimize the installation


# Example Ryzen 5 3500U notebook A315-41-R93M information

https://unix.stackexchange.com/questions/183375/how-can-i-change-screen-brightness-in-debian-wheezy-that-installed-in-laptop

As root:

echo 64 > /sys/class/backlight/amdgpu_bl0/brightness

/etc/default/grub

GRUB_CMDLINE_LINUX_DEFAULT="ipv6.disable=1 video=HDMI-A-1:1920x1080@60:D"

update-grub

Use external HDMI monitor only, at each boot run in terminal, better not make startup script:

xrandr --output eDP --off

On access point it disconnects after some time:

https://ubuntuforums.org/showthread.php?p=12350385#post12350385

https://ubuntuforums.org/showthread.php?t=2181530

Solution 1 - Laptop has gigabit LAN port use the LAN, if does not have LAN use USB 3.0 LAN gigabit dongle.

Solution 2 - keep active with constant ping to home device and if disconnected disconnect and reconnect wifi.

The BIOS is actually E*.bin not D*.bin in the archive, in case you need to do BIOS recovery.

Bios recovery is made by creating FAT32 partition on a GPT USB stick and copying the BIOS in it,

rename your Bios .bin file extension to .fd file extension. Example: AB7HG.fd or maybe you need

to add x64 in your filename. Example: AB7HGx64.fd then power off, remove power adapter, plug your

USB, press FN+ESC keys, plug your AC Adapter, press power button while still holding FN+ESC keys

and Hold it for 5 secs then Release the pressed keys. (You can hear that your fan is spinning at max.)

There will be an indication on both of your USB and Power Indicator (might also blink.)

Wait for about 5 minutes until the process is finished. (Your Laptop will automatically Shuts off or

Reboot itself, Then that's it. You are done and your Waifu laptop is now or would be breathing again.)

This particular model comes with M.2 SSD and missing the special SATA cable for SATA HDD / SSD

and comes with Endless OS or Linpus Linux, where Linpus Linux is not with graphical Desktop

because linux kernel is old 4.2 and so no GPU acceleration for Ryzen 3500U.

http://teknisi-indonesia.com/resources/aspire-3-a315-41-la-g021p-schematic.11166/

https://community.acer.com/en/discussion/576988/acer-aspire-3-a315-41g-bios-1-15-upgrade-stuck-boot

https://community.acer.com/en/discussion/576990/how-to-flash-bios-on-acer-aspire-laptop

https://www.youtube.com/watch?v=A0S9BIghFnU

https://www.ebay.co.uk/itm/C5V01-NBX00026X00-DH5JV-NBX0002BY00-HDD-ACER-Aspire-A315-HDD-Jack/184046678950?hash=item2ada0973a6:m:mX7kZNGuY3QWovombR_vJyw

https://guatemaladigital.com/Deal4GO+SATA+SSD+Hard+Drive+Cable+with+2.5%22+HDD+Caddy+Bracket+for+Acer+Aspire+3+A315-41+A315-41G+A315-33+DH5JV+NBX0002BY00+50.GY9N2.003+(Color%3A+Black)/Producto.aspx?Codigo=7940076

https://www.amazon.it/Deal4GO-A315-41-A315-41G-NBX0002BY00-50-GY9N2-003/dp/B0816QWNFG

https://www.youtube.com/watch?v=KxQE5rm8vMc

https://www.youtube.com/watch?v=YKav9W2JkcI

https://www.acer.com/ac/ro/RO/content/model/NX.GY9EX.090

https://community.acer.com/en/discussion/552075/sata-cable-and-bracket-for-aspire-a315-41-part-number-nx-gy9ed-012

https://community.acer.com/en/discussion/566384/aspire-3-a315-41-r9j2-sata-cable-hdd-cradle-and-mainboard-schematic

https://www.doctor-laptop.ro/cablu-conectare-hdd-original-acer-50.gy9n2.003/

https://www.ebay.fr/itm/312167713875

https://www.ipc-computer.eu/notebook-spareparts/cable/-50.gy9n2.003

https://www.aliexpress.com/item/4000385931261.html?spm=a2g0o.productlist.0.0.73121d90IDor0d&algo_pvid=0fa5b831-5c0c-4406-8963-ff4086dc6d13&algo_expid=0fa5b831-5c0c-4406-8963-ff4086dc6d13-2&btsid=9c0509f7-767f-4dc0-a908-ff2b97570f99&ws_ab_test=searchweb0_0,searchweb201602_7,searchweb201603_55

https://www.aliexpress.com/item/4000364917729.html?spm=a2g0o.productlist.0.0.73121d90IDor0d&algo_pvid=0fa5b831-5c0c-4406-8963-ff4086dc6d13&algo_expid=0fa5b831-5c0c-4406-8963-ff4086dc6d13-1&btsid=9c0509f7-767f-4dc0-a908-ff2b97570f99&ws_ab_test=searchweb0_0,searchweb201602_7,searchweb201603_55

https://www.youtube.com/watch?v=_H_khwud0M0

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

While with BIOS/MBR systems you install to the MBR and almost never to a partition, with UEFI you always install to the efi partition.

It actually should default to install to that partition anyway and you can only have one efi partition (with boot flag) per drive.

In your case installing grub to sda3 the efi partition is correct. You should see multiple folders for each system you have installed,

in Ubuntu they are mounted at /boot/efi and on the drive you have ubuntu & Microsoft folders with boot files:

/boot/efi/EFI/ubuntu/grubx64.efi /boot/efi/EFI/Microsoft/Boot/bootmgfw.efi

Grub2's os-prober has a bug and only creates BIOS boot entries which do not work with UEFI. You do not chainload to the Windows install

like BIOS, but chain load to the efi partition. Boot-Repair can automatically create entries in 25_custom or you can manually add entries

as shown in bug report to your 40_custom.

grub2's os-prober creates wrong style (BIOS) chain boot entry https://bugs.launchpad.net/ubuntu/+source/grub2/+bug/1024383 type of entry

from Boot-Repair that should work. menuentry "Windows UEFI bkpbootmgfw.efi" { menuentry "Windows Boot UEFI loader" { Type of entry that

does not work: 'Windows ...) (on /dev/sdXY)' Some info in Post #3 on cleaning up menus, if desired. http://ubuntuforums.org/showthread.php?t=2085530


# Raspberry Pi VPN and iptables

https://www.comparitech.com/blog/vpn-privacy/raspberry-pi-vpn/


# Best available router for OpenWRT having no multiple risky versions

https://openwrt.org/toh/asus/asus_rt-ac57u

http://downloads.openwrt.org/releases/19.07.0/targets/ramips/mt7621/openwrt-19.07.0-ramips-mt7621-asus_rt-ac57u-squashfs-sysupgrade.bin


# Raspberry Pi Raspbian has Real VNC

https://www.deviceplus.com/how-tos/how-to-make-a-vnc-server-on-raspberry-pi/


# Power supply for TP-Link SG1016 that outputs 12V 2A

https://www.yoycart.com/Product/524147033928/

https://itbbs.pconline.com.cn/diy/15217927.html


# Windows 10 disable fast boot and clone drive on same PC, Legacy and UEFI

https://somoit.net/windows/windows-cannot-shrink-volume-unmovable-files

http://wiki.christophchamp.com/index.php?title=Dd_(command)

https://www.linux.com/tutorials/gnu-ddrescue-best-damaged-drive-rescue/

https://help.uaudio.com/hc/en-us/articles/213195423-How-To-Disable-Fast-Startup-in-Windows-10

https://appuals.com/enable-disable-hibernation-windows-7/

https://www.howtogeek.com/102897/whats-the-difference-between-sleep-and-hibernate-in-windows/

https://win10faq.com/how-to-clone-windows-10-with-clonezilla/

https://pureinfotech.com/clone-windows-10-drive-clonezilla/

https://www.tecmint.com/migrate-windows-10-from-hdd-to-ssd-using-clonezilla/

https://www.quora.com/If-you-clone-the-OS-hard-drive-will-Windows-require-a-new-license

https://www.tigraine.at/2014/01/04/how-to-upgrade-to-an-ssd-with-minimal-effort

https://www.technibble.com/forums/threads/gnu-ddrescue-ridiculous-learning-curve.34594/

https://en.wikipedia.org/wiki/Dd_(Unix)

dd if=/dev/zero of=path/to/file bs=512 count=1 conv=notrunc

The notrunc conversion option means do not truncate the output file — that is, if the output file already exists,

just replace the specified bytes and leave the rest of the output file alone. Without this option, dd would create

an output file 512 bytes long.

In Windows 10 three steps are required to disable fast startup, GUI, Hybrid and CMD:

1) GUI disable Fast Startup, press the Windows Key + R to bring up the Run dialog, type powercfg.cpl and hit Enter.

The Power Options window should appear. Click "Choose what the power buttons do" from the column on the left.

Click "Change settings that are currently unavailable".

Scroll down to "Shutdown settings" and uncheck the box for "Turn on fast startup".

Click the "Save changes" button to apply the changes.

2) Hybrid disable Fast Startup, press the Windows Key + R to bring up the Run dialog, type powercfg.cpl and hit Enter.

The Power Options window should appear. Click "Change when the computer sleeps" from the column on the left.

Click "Change advanced power settings".

If the computer is a laptop: Sleep -> Allow hybrid sleep -> On battery -> Off

Sleep -> Allow hybrid sleep -> Plugged in -> Off

Click OK.

Step 3 hides the option Allow hybrid sleep.

3) CMD disable Fast Startup, click Start, and then type cmd in the Start Search box.

Don’t use run since it won’t allow you to run as an administrator.

In the search results list, right-click Command Prompt or CMD, and then click Run as Administrator.

When you are prompted by User Account Control, click Continue.

At the command prompt, type:

powercfg.exe /hibernate off

and then press Enter. Type exit, and then press Enter to close the Command Prompt window.

Optional:

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

```bash
su -
echo "Europe/Bucharest" > /etc/timezone
emerge --config sys-libs/timezone-data
nano -w /etc/conf.d/keymaps
```

=====================

keymap="us"

=====================

```bash
export LC_ALL=en_US.UTF-8
nano  /etc/locale.gen
```

=====================

en_US.UTF-8 UTF-8

=====================

```bash
locale-gen
sync
locale -a
reboot
```


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

Just to be clear though, experimental isn't a complete distribution. It's just an extra repository where

maintainers can upload the latest stuff without immediately pushing it into unstable. It's meant for users

of unstable who wants to pull in the latest of only some specific package. Recently I've used it for getting

the latest mesa while Debian has been frozen these last few months, and it's great to have such a simple way

to do that. Just don't try going into your apt sources list and change unstable to experimental. That won't

work because experimental isn't an entire distribution in itself the way unstable and stable is. The way you

do it is you have both unstable and experimental in your sources list, and whenever you want to install an

experimental package you append '-t experimental' to apt, otherwise experimental packages will be ignored.

Example if I want latest gnome-shell and mutter and already have added the repo to my sources

Code:

sudo apt -t experimental install gnome-shell mutter

That way, nothing but gnome-shell and mutter will be from experimental. Everything else in my distribution

will still be from unstable. 

# android block phones

https://www.androidauthority.com/how-to-block-a-phone-number-246484/

http://andauth.co/fdmcms


# btrfs raid 1 for devices with some checksum errors

https://btrfs.wiki.kernel.org/index.php/Using_Btrfs_with_Multiple_Devices#Adding_new_devices

https://btrfs.wiki.kernel.org/index.php/FAQ

https://btrfs.wiki.kernel.org/index.php/Status#Defrag

https://wiki.archlinux.org/index.php/Btrfs#Preparation

https://wiki.gentoo.org/wiki/Btrfs

https://wiki.gentoo.org/wiki/Btrfs/System_Root_Guide

https://btrfs.wiki.kernel.org/index.php/UseCases

https://askubuntu.com/questions/32418/is-there-an-easier-way-to-chroot-than-bind-mounting

https://bartsimons.me/ubuntu-linux-chroot-guide/

https://unix.stackexchange.com/questions/405472/cannot-find-efi-directory-issue-with-grub-install

Usage

Typing long Btrfs commands can quickly become a hassle. Each command (besides the initial btrfs command)

can be reduced to a very short set of instructions. This method is helpful when working from the command

line to reduce the amount of characters typed.

For example, to defragment a filesystem located at /, the following shows the long command:

root #btrfs filesystem defragment -v /

Shorten each of the longer commands after the btrfs command by reducing them to their unique,

shortest prefix. In this context, unique means that no other btrfs commands will match the command

at the command's shortest length. The shortened version of the above command is:
root #btrfs fi de -v /

No other btrfs commands start with fi; filesystem is the only one. The same goes for the de sub-command

under the filesystem command.

Creation

Warning

The mkfs.btrfs command irreversibly destroys any content of the partition it is told to format.

Please be sure the correct drive and partition have been selected before running any mkfs command!

To create a Btrfs filesystem on the /dev/sdXN partition:

root #mkfs.btrfs /dev/sdXN

In the example above, replace N with the partition number and X with the disk letter that is to be formatted.

For example, to format the third partition of the first drive in the system with Btrfs, run:

root #mkfs.btrfs /dev/sda3

Important
The last number column in /etc/fstab should be 0 for all Btrfs partitions. fsck.btrfs and btrfsck should

not be run during each system boot.
Mount

After creation, filesystems can be mounted in several ways:

    mount - Manual mount.
    
    fstab - Defining mount points in /etc/fstab enables automatic mounts on system boot.
    
    Removable media - Automatic mounts on demand (useful for USB drives).
    
    AutoFS - Automatic mount on filesystem access.

Converting ext* based file systems

It is possible to convert ext2, ext3, and ext4 filesystems to Btrfs using the btrfs-convert utility.

The following instructions only supports the conversion of filesystems that are unmounted. To convert

the root partition, boot to a system rescue disk (SystemRescueCD works nicely) and run on the conversion

commands on the root partition.

First be sure the the mount point is unmounted:

root #umount <mounted_device>

Check the integrity of the filesystem using the appropriate fsck tool. In the next example, the filesystem is ext4:

root #fsck.ext4 -f <unmounted_device>

Use btrfs-convert to convert the ext* formatted device into a Btrfs-formatted device:

root #btrfs-convert <unmounted_device>

Be sure to edit /etc/fstab after the device has been formatted to change the filesystem column from ext4 to Btrfs:

FILE /etc/fstabChanging ext4 to btrfs

<device>   <mountpoint>  btrfs  defaults  0 0

Defragmentation

Another feature of Btrfs is online defragmentation. To defragment a root Btrfs filesystem run:

root #btrfs filesystem defragment -r -v /

Warning

Defragmenting with kernel versions < 3.9 or ≥ 3.14-rc2 as well as with Linux stable kernel

versions ≥ 3.10.31, ≥ 3.12.12 or ≥ 3.13.4 breaks up ref-links between files and their COW

copies[1] and thus may increase space usage considerably. Make sure to have enough free space

available and not too many snapshots on the drive as full btrfs partitions can get really slow.

Compression

Btrfs supports transparent compression using the zlib, lzo, and zstd (v5.1.0)[2] compression algorithms.

It is possible to compress specific files using the file attributes:

user $chattr +c

The compress mount option sets the default behavior to compress all the newly created files.

To re-compress the whole filesystem, run the following command:

root #btrfs filesystem defragment -r -v -clzo /

Depending on the CPU and disk performance, using lzo compression could improve the overall throughput.

As alternatives to lzo it is possible to use the zlib or zstd compression algorithms. Zlib is slower but

has a higher compression ratio, whereas zstd has a good ratio between the two[3].

To force zlib compression across the whole filesystem:

root #btrfs filesystem defragment -r -v -czlib /

Compression level

Since kernel version 4.15.0[4], zlib compression can now be set by levels 1-9. Since kernel version 5.1.0

zstd can be set to levels 1-15. For example, to set zlib to maximum compression at mount time:

root #mount -o compress=zlib:9 /dev/sdXY /path/to/btrfs/mountpoint

Or to set minimal compression:

root #mount -o compress=zlib:1 /dev/sdXY /path/to/btrfs/mountpoint

Or adjust compression by remounting:

root #mount -o remount,compress=zlib:3 /path/to/btrfs/mountpoint

The compression level should be visible in /proc/mounts or by checking the most recent dmesg

output using the following command:

root #dmesg | grep -i btrfs

[    0.495284] Btrfs loaded, crc32c=crc32c-intel

[ 3010.727383] BTRFS: device label My Passport devid 1 transid 31 /dev/sdd1

[ 3111.930960] BTRFS info (device sdd1): disk space caching is enabled

[ 3111.930973] BTRFS info (device sdd1): has skinny extents

[ 9428.918325] BTRFS info (device sdd1): use zlib compression, level 3

Compression ratio and disk usage

The usual userspace tools for determining used and free space like du and df may provide inaccurate

results on a Btrfs partition due to inherent design differences in the way files are written compared

to, for example, ext2/3/4[5].

It is therefore advised to use the du/df alternatives provided by the btrfs userspace tool btrfs filesystem.

In addition to that, The compsize tool found in the sys-fs/compsize package can be helpful in providing

additional information regarding compression ratios and the disk usage of compressed files. The following

are example uses of these tools for a btrfs partition mounted under /media/drive.

user $btrfs filesystem du -s /media/drive

     Total   Exclusive  Set shared  Filename
     
 848.12GiB   848.12GiB       0.00B  /media/drive/

user $btrfs filesystem df /media/drive

Data, single: total=846.00GiB, used=845.61GiB

System, DUP: total=8.00MiB, used=112.00KiB

Metadata, DUP: total=2.00GiB, used=904.30MiB

GlobalReserve, single: total=512.00MiB, used=0.00B

user $compsize /media/drive

Processed 2262 files, 112115 regular extents (112115 refs), 174 inline.

Type       Perc     Disk Usage   Uncompressed Referenced

TOTAL       99%      845G         848G         848G

none       100%      844G         844G         844G

zlib        16%      532M         3.2G         3.2G


Multiple devices (RAID)

Btrfs can be used with multiple block devices in order to create RAIDs. Using Btrfs to create

filesystems that span multiple devices is much easier than creating using mdadm since there is

no initialization time needed for creation.

BTRFS handles data and metadata separately. This is an important factor to keep in mind when using

a multi-device filesystem. It is possible to use separate profiles for data and metadata block groups.

For example, metadata could be configured across multiple devices in RAID1, while data could be configured

to RAID5. This is profile possible when using three or more block devices, since RAID5 requires a minimum

of 3 block devices.

This type of profile offers the benefit of redundancy for metadata on each device and striping for data across

devices, which increases read speeds. The drawback of this profile is more space than necessary is used for

metadata, and write speeds are reduced for data blocks, since RAID5 uses a parity bit.
Creation

The simplest method is to use the entirety of unpartitioned block devices to create a filesystem spanning multiple

devices. For example, to create a filesystem in RAID1 mode across two devices:

root #mkfs.btrfs -m raid1 <device1> <device2> -d raid1 <device1> <device2>
	
Conversion

Converting between RAID profiles is possible with the balance sub-command. For example, say three block devices

are presently configured for RAID1 and mounted at /srv. It is possible to convert the data in this profile from

RAID1 to RAID5 with using the following command:
root #btrfs balance start -dconvert=raid5 --force /srv

Conversion can be performed while the filesystem is online and in use. Possible RAID modes in btrfs include RAID0,

RAID1, RAID5, RAID6, and RAID10. See the upstream BTRFS wiki for more information.

Warning

It is currently not safe to use the RAID 5 or 6 modes[6]. RAID 5 and 6 modes have seen some fixes[7] in Linux 4.12,

but overall status is still marked as unstable.[8][9]. Users who want to use RAID5 or RAID6 functionality of btrfs

are encouraged to check the btrfs status page for stability status of said modes before utilizing the modes.

Removal

By device path

Block devices (disks) can be removed from multi-device filesystems using the btrfs device remove subcommand:

root #sudo btrfs device remove /dev/sde /srv

By device ID

Use the usage subcommand to determine the device IDs:

root #btrfs device usage /srv

Next use the device ID to remove the device, in this case /dev/sde will be removed:

root #btrfs device remove 5 /srv

Subvolumes

As mentioned above in the features list, Btrfs can create subvolumes. Subvolumes can be used to better organize

and manage data. They become especially powerful when combined with snapshots. Important distinctions must be

made between Btrfs subvolumes and subvolumes created by Logical Volume Management (LVM). Btrfs subvolumes are

not block level devices, they are POSIX file namespaces.[10] They can be created at any location in the filesystem

and will act like any other directory on the system with one caveat: subvolumes can be mounted and unmounted.

Subvolumes are nestable (subvolumes can be created inside other subvolumes), and easily created or removed.
Note
A subvolume cannot be created across different Btrfs filesystems. If /dev/sda and /dev/sdb both contain separate

(non-RAID) Btrfs filesystems, there is no way a subvolume can expand across the two filesystems. The snapshot can

be moved from one filesystem to another, but it cannot span across the two. It must be on /dev/sda or /dev/sdb.
Create

To create a subvolume, issue the following command inside a Btrfs filesystem's name space:
root #btrfs subvolume create <dest-name>

Replace <dest-name> with the desired destination and subvolume name. For example, if a Btrfs filesystem exists

at /mnt/btrfs, a subvolume could be created inside it using the following command:

root #btrfs subvolume create /mnt/btrfs/subvolume1

List

To see the subvolume(s) that have been created, use the subvolume list command followed by a Btrfs filesystem

location. If the current directory is somewhere inside a Btrfs filesystem, the following command will display

the subvolume(s) that exist on the filesystem:

root #btrfs subvolume list .

If a Btrfs filesystem with subvolumes exists at the mount point created in the example command above, the output

from the list command will look similar to the following:

root #btrfs subvolume list /mnt/btrfs

ID 309 gen 102913 top level 5 path mnt/btrfs/subvolume1

Remove

Subvolumes can be properly removed by using the subvolume delete command followed by the path to the subvolume.

All available subvolume paths in a Btrfs filesystem can be seen using the list command above.

root #btrfs subvolume delete <subvolume-path>

As above, replace <subvolume-path> with the actual path to the subvolume to be removed. To delete the subvolume

used in the examples above, the following command would be issued:

root #btrfs subvolume delete /mnt/btrfs/subvolume1

Delete subvolume (no-commit): '/mnt/btrfs/subvolume1'

Snapshots

Snapshots are subvolumes that share data and metadata with other subvolumes. This is made possible by Btrfs

Copy on Write (CoW) ability.[10] Snapshots can be used for several purposes, one of which is to create backups

of file system structures at specific points in time.

If the root filesystem is Btrfs, it is possible to create a snapshot using the subvolume snapshot commands:

root #mkdir -p /mnt/backup/rootfs

root #btrfs subvolume snapshot / /mnt/backup/rootfs/

The following small shell script can be added to a timed cron job to create a timestamped snapshot backup of

a Btrfs formatted root filesystem. The timestamps can be adjusted to whatever is preferred by the user.

FILE btrfs_snapshot.shBtrfs rootfs snapshot cron job example

```bash
#!/bin/bash
NOW=$(date +"%Y-%m-%d_%H:%M:%S")
 
if [ ! -e /mnt/backup ]; then
mkdir -p /mnt/backup
fi
 
cd /
/sbin/btrfs subvolume snapshot / "/mnt/backup/backup_${NOW}"
```

Mounting

A subvolume can be mounted in a location different from where it was created, or users can choose to not mount

them at all. For example, a user could create a Btrfs filesystem in /mnt/btrfs and create /mnt/btrfs/home and

/mnt/btrfs/gentoo-repo subvolumes. The subvolumes could then be mounted at /home and /var/db/repos/gentoo, with

the original top level subvolume left unmounted. This results in a configuration where the subvolumes' relative

path from the top level subvolume is different from their actual path.

To mount a subvolume, perform the following command, where <rel-path> is the relative path of the subvolume from

the top level subvolume, obtainable through the subvolume list command:

root #mount -o subvol=<rel-path> <device> <mountpoint>

Similarly, one can update the filesystem tab to mount their Btrfs subvolumes like so:

FILE /etc/fstabMounting Subvolumes

<device>  <mountpoint>  btrfs  subvol=<rel-path>  0 2

Troubleshooting
Using with VM disk images

When using Btrfs with virtual machine disk images, it is best to disable copy-on-write on the disk images in

order to speed up IO performance. This can only be performed on files that are newly created. It also possible

to disable CoW on all files created within a certain directory. For example, using the chattr command:
root #chattr +C /var/lib/libvirt/images
Clear the free space cache

It is possible to clear Btrfs' free space cache by mounting the filesystem with the clear_cache mount option.

For example:

root #mount -o clear_cache /path/to/device /path/to/mountpoint

Btrfs hogging memory (disk cache)

When utilizing some of Btrfs' special abilities (like making many --reflink copies or creating high amounts

of snapshots), a lot of memory can be consumed and not freed fast enough by the kernel's inode cache. This

issue can go undiscovered since memory dedicated to the disk cache might not be clearly visible in traditional

system monitoring utilities. The slabtop utility (available as part of the sys-process/procps package) was

specifically created to determine how much memory kernel objects are consuming:

root #slabtop

If the inode cache is consuming too much memory, the kernel can be manually instructed to drop the cache by

echoing an integer value to the /proc/sys/vm/drop_caches file[11].

To be safe, and to help the kernel determine the maximum amount of freeable memory, be sure to run a sync

before running the echo commands below:

user $sync

Most of the time Btrfs users will probably want to echo 2 to reclaim just the slab objects

(dentries and btrfs_inodes):

root #echo 2 > /proc/sys/vm/drop_caches

To clear the entire disk cache (slab objects and the page cache) use echo 3 instead:

root #echo 3 > /proc/sys/vm/drop_caches

Warning

While the above commands are non-destructive (as long as a sync was completed before running them),

they could seriously but temporarily slow down the system while the kernel loads only the necessary

items back into memory. Think twice before running the above commands for systems under heavy load!

More information on kernel slabs can be found in this dedoimedo blog entry. 

For defrag and autodefrag to work very well, avoid making any of this: subvolumes, reflinks and compression.

Caveat: since Linux 3.14-rc2, 3.13.4, 3.12.12 and 3.10.31 (which removed the snapshot-aware defragmentation)

defragmenting a file which has a COW copy (either a snapshot copy or one made with cp --reflink or bcp) would

produce two unrelated files. If you defragment a subvolume that had a snapshot, you would roughly double the

disk usage, as the snapshot files would no longer be COW images of the originals.

One would get this rather unexpected outcome when using defragment (together with "-c") to compress files

with the hope of saving space. Note that this is the same behaviour as observed with any pre 3.9 kernel

(which added snapshot-aware defragmentation). 

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

For Legacy BIOS:

grub-install /dev/sda

update-grub

===========================================

For UEFI BIOS:

First if Windows boots directly check in the BIOS scroll down UEFI Hardware BBS Priorities and

select Ubuntu if it is there as first boot option, this is actually GRUB,

if GRUB menu does not appear and Ubuntu loads directly, run: sudo update-grub && sync && restart

If can't find EFI directory, verify the size of the EFI partition in Windows:

This PC -> Manage -> Disk management

And then in the Linux chroot lsblk or cat /proc/partitions to find same size partition

(usually it is /dev/sda1) and run this additional commands:

mkdir -p /boot/efi

mount /dev/sda1 /boot/efi

grub-install /dev/sda1

update-grub

===========================================

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

You can use pkexec on those flavours that support this option. As of 18.04, only Xubuntu supports

this option by default, as shown in the following examples:

```bash
    pkexec thunar
    pkexec mousepad /etc/fstab
```

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

```bash
sudo -s
raspi-config -> Interfacing options -> SSH -> Enable SSH -> TAB -> Finish
apt install -y tightvncserver
chown -R pi /home/pi
```

OR

chown pi:pi .Xauthority

On the PC start tightvncserver on the Pi:

```bash
ssh pi@192.168.x.x
tightvncserver :1 -geometry 800x600 -depth 24
exit
```

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

See here file par2aaron.txt having:

https://pthree.org/2014/04/01/protect-against-bit-rot-with-parchive/

https://news.ycombinator.com/item?id=13615978

Thanks for the heads up but that's not the one. I posted it in 1998 or 1999 and I tried to find the exact

Usenet archive link similar to the direct link for Larry Page's famous 1996 post on comp.lang.java

To go back to the article, the author mentions posting the files to newsgroup "alt.binaries.backup". With

Usenet, there isn't exactly a contractual SLA (Service Level Agreement) for that group. It's a gentlemen's

agreement between those commercial Usenet providers (and non-commercials ones like universities) to replicate

messages. Maybe because I posted the message to my GTE/Verizon ISP's Usenet server meant that it only got

replicated to a few peers and it "died".

If my tiny text-based post which is 2 years newer than Larry Page's can't be recovered today, it doesn't

give me lot of confidence to use Usenet as a backup solution. I have over 1 terabyte of photos, home videos,

and tif scans of all my paper files. It's not appealing to chop that 1TB into a thousand PAR2 files with extra

20% redundant parity and posting it to alt.binaries.backup. That seems extremely fragile. Another commenter made

a suggestion for Amazon's new personal "unlimited" cloud for $60/year. That seems much more reliable.


> It's not appealing to chop that 1TB into a thousand PAR2 files with extra 20% redundant parity and posting it

to alt.binaries.backup.

For a 1 TB archive with 20% redundancy, you're looking at a block size of at least 32 MB in each par2 file (due to

the maximum block count of 32767 in the official implementation). Given that the article size limit for many news

servers is roughly 1 MB, you're looking at even a single block getting split into 32 article posts. par2 programs

typically will generate a series of files where the smallest files contain a single block and the largest files

contain 50 or more blocks. The 50 block files will each get split into 1600 articles.

For par2 recovery to work even when articles are missing, you really want the recovery block size to be less than

the article size limit, so that even if one or more articles are missing, the par2 archiver program can still read

a subset of blocks from the incomplete recovery file and still use them for recovery. That means that the maximum

archive size would be roughly 32 GB to keep the block size under the article size limit.

Going beyond that size means that it's less likely that the recovery file would be usable if some of the articles

are missing. At 32 GB, if one article is missing from a 3 block recovery file, the software will still be able find

2 blocks in that file. But, if the archive size was 100 GB, then the block size would be a minimum of 3 MB and just

missing 3 out of 9 articles that make up a 3 block recovery file would make the recovery file unusable.


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

BTRFS uses CRC32C with polynomial 0x1EDC6F41, hardware accelerated in newer CPUs and quite good for

bitrot detection, if you also have RAID1 mirror to repair automatically after detection even if it's

a whole 4 KB sector missing which BTRFS does.

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

General linux userspace tools such as df will inaccurately report free space on a Btrfs partition.

It is recommended to use btrfs filesystem usage to query Btrfs partitions. For example:

btrfs filesystem usage /

Note: The btrfs filesystem usage command does not currently work correctly with RAID5/RAID6 RAID levels.

Btrfs supports online defragmentation through the mount option autodefrag. To manually defragment your root, use:

btrfs filesystem defragment -r /

Using the above command without the -r switch will result in only the metadata held by the subvolume containing

the directory being defragmented. This allows for single file defragmentation by simply specifying the path.

Defragmenting a file which has a COW copy (either a snapshot copy or one made with cp --reflink or bcp) plus using

the -c switch with a compression algorithm may result in two unrelated files effectively increasing the disk usage.

Parity RAID (RAID 5/6) code has multiple serious data-loss bugs in it.

Bitrot protection (repair) available if you use well tested RAID 1 (mirror) or RAID 10 (mirror striped)

Bitrot protection (repair) can be done if you use two partitions of the same size on the same one harddisk / ssd

and put them in RAID 1 (mirror) or maybe even one partition with RAID1 made just inside it:


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
Debian does not support Btrfs RAID out of the box, so the way to go is to start to install BTRFS without RAID

on one of the disk drives, leave the same space on a partition on the other drive(s), and then do

btrfs device add /dev/sdb3 /

btrfs balance start -dconvert=raid1 -mconvert=raid1 /

We also add "degraded" as file system option in fstab; e.g.:

UUID=3d0ce18b-dc2c-4943-b765-b8a79f842b88 /               btrfs   degraded,strictatime        0       0

btrfs device delete missing tells btrfs to remove the first device that is described

by the filesystem metadata but not present when the FS was mounted.

...

For example if you have a raid1 layout with two devices, and a device fails, you must:

    mount in degraded mode.
    
    add a new device.
    
    remove the missing device.

The UUID (check with blkid) is the same for both partitions in our RAID, so no need to specify devices.

EFI and RAID1

There is no support for RAID1 and EFI in Linux or Debian, so what we did was to have one EFI system partition

(ESP) on each drive, let the Debian installer install the the EFI stub of grub on one of them, and then use dd

to copy the contents of the ESP to the other ESP partition(s):

dd if=/dev/sda1 of=/dev/sdb1

This has to be repeated every time the EFI partition changes, but it seems that this normally does not change,

even when running update-grub. OTOH, it does not hurt to do the dd more often than necessary.

We also needed to change /etc/grub.d/10_linux in different places than "The perfect Btrfs setup for a server"

(which seems to be written for a BIOS/MBR system) indicates: Search for " ro " (two occurences), and prepend

"rootflags=degraded". One of these lines becomes

	linux	${rel_dirname}/${basename}.efi.signed root=${linux_root_device_thisversion} rootflags=degraded ro ${args}

In order for that to take effect, we had to

update-grub

What to do on a failed disk

We disconnected one of the disks (while the system was offline, online would have been an interesting variant) to

simulate a disk failure. Due to a bug in BTRFS, it degrades nicely on the first boot, but then becomes irreversibly

read-only on the second boot. If you get there, the best option seems to be to copy the read-only file system to a

fresh and writable file system (with e.g., tar or cpio). (We actually went as far as having the read-only file system

with one of the two drives we used, so the bug is still there in Linux 4.11).

You probably want to avoid these complications, and while you are still in your first boot, you can. What we did

(with the other disk), is to convert it back from RAID1 to a single profile, then remove the failed device

(which complains that it does not know the device, but still removes it).

btrfs balance start -v -mconvert=dup -dconvert=single /

btrfs device remove /dev/sdb3

#now check that it has worked

btrfs device usage /

btrfs fi show

btrfs fi usage

We then shut down the system, plugged the replacement disk in (actually the disk we had earlier ruined by double

degraded booting, after wiping the BTRFS partition), booted and then did the usual dance to turn the now-single

BTRFS into a RAID1 again:

btrfs device add /dev/sdb3 /

btrfs balance start -dconvert=raid1 -mconvert=raid1 /

As a result, we had a RAID1 again.

If you wonder why we did not use btrfs replace: We would have to connect the new disk before the second reboot,

which is not always practical. With the method above, once we have rebalanced the file system to a single one,

we can reboot as often as we like to get the new drive online. 


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

Permanent in LXDE, with example found from LXRandR for external monitor

https://wiki.lxde.org/en/Autostart

/home/pi/.config/autostart/lxrandr-autostart.desktop

[Desktop Entry]

Type=Application

Name=LXRandR autostart

Comment=Start xrandr with settings done in LXRandR

Exec=sh -c 'xrandr --output eDP --off --output HDMI-A-0 --mode 1920x1080 --rate 60.00 --same-as eDP'

OnlyShowIn=LXDE

========================

su -

apt update

apt install -y xinput

exit

nano /home/pi/.config/autostart/mouseslower-autostart.desktop

========================

[Desktop Entry]

Type=Application

Name=Mouseslower autostart

Comment=Start Mouseslower

Exec=sh -c '/usr/bin/mouseslower'

OnlyShowIn=LXDE

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

audio-channels=stereo

cache=yes

demuxer-thread=yes

demuxer-readahead-secs=200

fullscreen

# MPV 2018 best with AMD Polaris, can't select on timeline,  configuration in ~/.config/mpv/mpv.conf:

https://www.reddit.com/r/nhl_games/comments/5ea3an/increasing_cachereadahead_in_mpv/

# sudo apt install -y ffmpeg mesa-va-drivers vainfo

hwdec=vaapi

vo=vaapi

save-position-on-quit

audio-normalize-downmix=yes

volume=100

audio-channels=stereo

cache=yes

demuxer-thread=yes

demuxer-readahead-secs=200

fullscreen


# MPV 2017 medium with AMD Polaris, configuration in ~/.config/mpv/mpv.conf:

# sudo apt install -y ffmpeg mesa-va-drivers vainfo

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

Enabling the high DPI mode in Firefox is actually pretty simple, it is just a matter of setting the

layout.css.devPixelsPerPx value in about:config to a value of 2 (i.e. double the size).

To do this, open up a new tab, and type about:config into the address bar, and press enter.

Then use the search box in this page to find the key layout.css.devPixelsPerPx and then change the value from -1.0 to 2

However, on closer inspection you will realise that the icons in the Firefox toolbar, while now the correct proportions,

are now slightly fuzzy compared to the text. This is because the default Firefox theme ships bitmap icons that don’t work

well with High DPI displays.

The easiest way to fix this, and to make your Firefox fit that much better with the Fedora Workstation, is to install the

GNOME Firefox theme. This theme uses scalable vector assets for the icons (SVGs) that look crisp and clean on a high DPI display.

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

```bash
apt-cache search dpkg-divert
sudo -s
apt-get install -y config-package-dev
dpkg-divert --local --divert /etc/grub.d/09_os-prober --rename --add /etc/grub.d/30_os-prober
update-grub
```

# Remove this divert (works to remove if bad name given to diverted file), go back to default:

```bash
sudo -s
dpkg-divert --rename --remove /etc/grub.d/30_os-prober
update-grub
```

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

```bash
touch compavr && chmod +x compavr && gedit compavr
avr-gcc -mmcu=atmega32 -Wall -Os -o $1.elf $1.c
avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex
avrdude -c avrusb500 -p atmega32 -u -U hfuse:w:0xc9:m -U lfuse:w:0xe3:m
avrdude -c avrusb500 -p atmega32 -U flash:w:$1.hex
```

# Usbasp has failed for me, found a way to fix it:

http://visionhack.de/2012/09/23/installation-usap-programmer-with-ubuntu/

Short JP3 slow sck.

Or...

https://forum.arduino.cc/index.php?topic=363772.75

It always keeps reset on the target chip - after program, to test, you need to remove connection and power it from somewhere else.

Compavr for atmega8 chip programmed with usbasp:

```bash
avr-gcc -mmcu=atmega8 -Wall -Os -o $1.elf $1.c
avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex
```

# Fuses are for fast external oscilator like 12MHz !!! Not internal.

```bash
avrdude -c usbasp -p atmega8 -u -U hfuse:w:0xc9:m -U lfuse:w:0xef:m
avrdude -c usbasp -p atmega8 -U flash:w:$1.hex
```

# Program the best USBASP firmware available on the internet, if it is already an USBASP no need to program the fuses:

https://github.com/iugamarian/usbasp/tree/1.06-alpha

# Avoid cloning the repository - it will not be 1.06-alpha activated and I do not now how to do that yet:

# Download zip,

copy bin/firmware/usbasp-v1.06-alpha-2016-05-18/usbasp-v1.06-alpha-2016-05-18-atmega8.hex to home folder and:

avrdude -c usbasp -p atmega8 -U flash:w:usbasp-v1.06-alpha-2016-05-18-atmega8.hex


0. Verify that you have a USBasp V2.0, and that it has a 12MHz crystal and an ATMEGA8 or ATMEGA8A MCU

onboard. DO NOT CONNECT IT TO THE USB PORT OF YOUR COMPUTER.

1. Short the JP2 (self-programming) jumper.

2. Connect the USBasp V2.0 to the USBtinyISP using a 10-pin ribbon cable

Picture of pins:

http://www.scienceprog.com/building-and-installing-usbasp-usb-programmer-of-avr-microcontrollers/

Firmware:

http://blog.lincomatic.com/wp-content/uploads/2014/05/usbasp.atmega8.2011-05-28.zip

Commands that work:

```bash
avrdude -c avrusb500 -p atmega8 -u -U hfuse:w:0xc9:m -U lfuse:w:0xef:m
avrdude -c avrusb500 -p atmega8 -U flash:w:usbasp.atmega8.2011-05-28.hex
```

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

```bash
sudo -s
apt-get install -y samba samba-common python-glade2 system-config-samba
cp -pf /etc/samba/smb.conf /etc/samba/smb.conf.bak
cat /dev/null  > /etc/samba/smb.conf
systemctl stop apparmor
systemctl disable apparmor
firewall-cmd --add --service=samba
```

a) In terminal run (set password that Windows asks):

sudo pdbedit -a pi

b) In terminal run:

mkdir /media/pi/EXT4DATE/dan/sambasharefolder

c) In terminal run:

```bash
chmod -R 0755 /media/pi/EXT4DATE/dan/sambasharefolder/
chown -R pi:pi /media/pi/EXT4DATE/dan/sambasharefolder/
chown pi:pi /media/pi/EXT4DATE/dan
```

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

Install samba36-server and luci-app-samba as root:

```bash
opkg install samba36-server
opkg install luci-app-samba
```

Edit /etc/config/samba

vi /etc/config/samba

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

vi /etc/passwd

root:$1$nvFplbd8$l05HR0mdTHcGprNaMg8QA1:0:0:root:/root:/bin/ash

daemon:*:1:1:daemon:/var:/bin/false

network:*:101:101:network:/var:/bin/false

nobody:*:65534:65534:nobody:/var:/bin/false

diantokam:*:65535:65534:diantokam:/var:/bin/false

Create password for user diantokam

passwd diantokam

Adding diantokam to samba user

smbpasswd -a diantokam

Enable samba service on startup

/etc/init.d/samba enable

To start samba service

/etc/init.d/samba start

To stop samba service

/etc/init.d/samba stop

To accesses samba share from windows type \\samba_server_name on address bar, in my case it is \\openwrt


# OpenWRT/LEDE Project - Samba Server 2018 firewalled only one ip allowed, also ssh and http (LuCI)

Combined from the following (each not working individually, only after merging all and testing):

http://mattventura.net/2009/08/17/a-mostly-complete-openwrt-tutorial/ - input_rule instead of INPUT

https://www.garron.me/en/bits/iptables-open-port-for-specific-ip.html - port to one IP

http://www.chiark.greenend.org.uk/~peterb/network/drop-vs-reject - drop saves upload speed

https://superuser.com/questions/924662/openwrt-iptables-rules-will-be-deleted-automatically - /etc/firewall.user

http://troy.jdmz.net/samba/fw/ - the ports used by Samba

Allow only one ip to ssh and samba no matter wan or lan, edit /etc/firewall.user

```bash
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
```

# Mounting samba shares

sudo apt install cifs-utils

sudo nano /etc/group

add your user name to the users line users:x:100:abc

sudo nano /etc/fstab

//192.168.1.50/500gbhdd /media/500gbhdd cifs username=abc,password=def 0 1

https://askubuntu.com/questions/33480/how-do-i-navigate-to-a-samba-location-ive-mounted-in-nautilus-on-the-command-li

https://askubuntu.com/questions/33480/how-do-i-navigate-to-a-samba-location-ive-mounted-in-nautilus-on-the-command-li

Upon adding myself to this group (sudo nano /etc/group, adding my name to the users line users:x:100:pi),

I am now able to access the remote directories. My /etc/fstab file has these lines:

//xxx.220.131.233/user  /mnt/rua_user  cifs  username=abc,password=def 0 1

//xxx.220.131.233/all   /mnt/rua_all   cifs  username=abc,password=def 0 1

The rights on the folders look like this (ls -lh /mnt):

drwxrwx---  8 1016 users 0 2011-03-08 11:14 rua_all

drwxrwx--- 25 1016 users 0 2011-05-25 18:21 rua_user


# Using switches ensures less or no sniffing of packets

https://blog.packet-foo.com/2016/07/how-to-use-wireshark-to-steal-passwords/
