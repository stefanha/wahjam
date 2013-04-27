#
# Regular cron jobs for the jammr-client package
#
0 4	* * *	root	[ -x /usr/bin/jammr-client_maintenance ] && /usr/bin/jammr-client_maintenance
