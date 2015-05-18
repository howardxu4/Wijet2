# .bash_profile

# Get the aliases and functions
if [ -f ~/.bashrc ]; then
	. ~/.bashrc
fi

# Load RAM file from ROM [move to rc]
#./.dome1st

# Check Integration
./.ckall
if [ $? != 0 ]; then
echo "Invalid version found. system will shutdown now!"
/sbin/shutdown -g 0
exit
fi

# User specific environment and startup programs
unset USERNAME
cd /usr/local/bin

# Start IR
ln -s /dev/ttyS1 /dev/lirc1
lircd -d /dev/lirc1 -o /dev/lircd -P /var/run/lircd.pid  /etc/lircd.conf
lircmd -i /dev/lircd -f /dev/lircm /etc/lircmd.conf
setled 0

# Start Watchdog 
wd &

