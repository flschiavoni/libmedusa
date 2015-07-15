touch NEWS README AUTHORS ChangeLog
libtoolize -c
aclocal
automake --add-missing --gnu
autoconf
./configure
make
