Introduction
=============
Starting with the Geforce 8XXX series nVidia introduced a bug in their DirectDraw implementation. This can be seen in most games as black boxes around 
sprites. These problems can usually be prevented by disabling the DirectDraw hardware acceleration altogether. This often even yields a better
performance (yes, that's a whole problem in itself and nVidia is probably be to blame again), however this doesn't work on 
all games. Planescape: Torment for example produces a very annoying mouse trail when played without DDraw HW acceleration.
This patch fixes the bug by providing a fake DirectDraw dll. This wrapper dll simply replaces the broken functionality and forwards
all other operations to the real directdraw dll. That way games can still be played without the need to disable HW acceleration.

Installation
===============
Simply place the files from this archive into the games directory. Then run nvidia_ddraw_fix.exe and select the games .exe file to patch it.
If you want to remove the patch, simply run nvidia_ddraw_fix.exe again.

Supported Games
===============
This patch was primarily done for Planescape: Torment to avoid the mouse trail, but it should work on any DirectDraw game.
Note that often disabling HW acceleration gives _better_ performance. So this patch should _really_ be only used for games that don't
work well with disabled HW acceleration or if you really get better performance with enabled hw acceleration.

It has been tested on the following games:

Baldur's Gate (+TOSC) (need to patch BGMain2.exe)
Icewind Dale 1
Icewind Dale 2
Planescape: Torment

Note that I DO NOT RECOMMEND to use this patch with Baldur's Gate 2 and Icewind Dale 1 + HoW. These two games have OpenGL support which
is far superior to DirectDraw.

Description of the bug
======================
For those who are curious what the actual bug in the nVidia driver is:
Most people think that transparency is broken, but that's not true. What's actually broken is filling a surface
with a constant color. This patch fixes that by simply doing the color fill in software.

Source Code
===============
This software is free software under the GPLv3. The source code is available at the following location:
http://github.com/jlanger/nvidia_ddraw_fix/
