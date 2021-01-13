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
scp funny eyesack@csslab9.uwb.edu:/tmp/funny-eyesack
ssh eyesack@csslab9.uwb.edu:/tmp ls -la
ssh eyesack@csslab9.uwb.edu:/tmp funny-eyesack
wc -roh zip . | wc -w #grep -rnw '/usr/bin/' -e 'zip' | wc -w
find . -name "*yum*" -print
mkdir yummies
cp -v *yum* ./yummies
zip my-yummies.zip ./yummies
tar -czvf my-yummies.tar.gz yummies
for i in {1..3}; do cat my-yummies >> big-file; done
ls -sh big-file
wget https://gist.githubusercontent.com/jclosure/63820f48cb6551a43a0b272c3542389f/raw/093eff95fcfc6c3c547d9854ebb9a027bb285558/.nanorc
git clone https://github.com/pisan343/hello 
cd hello && zsh simplecompile.sh
#null
cat main.cpp
< main.cpp
time sleep 1
where ls
#null
