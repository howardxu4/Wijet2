#!/bin/sh
# Copyright (C) 1999 - 2004 Red Hat, Inc. All rights reserved. This
# copyrighted material is made available to anyone wishing to use, modify,
# copy, or redistribute it subject to the terms and conditions of the
# GNU General Public License version 2.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Mandatorily source xinitrc-common, which is common code shared between the
# Xsession and xinitrc scripts which has been factored out to avoid duplication
#.. /etc/X11/xinit/xinitrc-common

# The user may have their own clients they want to run.  If they don't,
# fall back to system defaults.
#if [ -f $HOME/.Xclients ]; then
#	exec $DBUS_LAUNCH $HOME/.Xclients || \
#	exec $HOME/.Xclients
#elif [ -f /etc/X11/xinit/Xclients ]; then
#	exec $DBUS_LAUNCH /etc/X11/xinit/Xclients || \
#	exec /etc/X11/xinit/Xclients
#else
       # failsafe settings.  Although we should never get here
       # (we provide fallbacks in Xclients as well) it can't hurt.


# change directory, all executables are there
	cd /usr/local/bin

#show splash
	bn splash &

#  start wireless and network
	st

# tell watchdog to invoke dhcpd
	pkill -7 wd

# start app
	ct

# disable screen saver
xscreensaver 2>/dev/null &

# start window manager
        exec /usr/X11R6/bin/mwm

#fi
