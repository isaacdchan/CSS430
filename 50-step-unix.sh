man ls
man -wK git
# lists all execs/path vars that start with "man"
pwd
# !!
# UP arrow key
hostname
ifconfig -a
dig google.com
uname -a && cat /proc/version && cat /etc/issue
ps -e | wc -l
ps aux --sort=-%mem | head
ps aux --sort=%cpu | head
ps --no-headers
#^C or ^D
python -c 'while True: 1 * 1' & ps | grep python
ps -ef | grep python | grep 'while'
kill 2236
whoami
groups
echo $0
touch funny
ls -la funny
chmod 754 funny
echo "ls -al" > funny
zsh funny
echo "sleep 10" >> funny
cp funny not-so-funny
chmod o-r+x not-so-funny
chmod go-rw not-so-funny | chmod +x not-so-funny
