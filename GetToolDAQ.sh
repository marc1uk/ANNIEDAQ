#!/bin/bash

# rpms prerequisits needs root
#yum install make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel postgresql-devel.x86_64 emacs screennc firefox git screen libcurl-devel libusb-devel.x86_64


mkdir ToolDAQ
cd ToolDAQ

git clone https://github.com/ToolDAQ/ToolDAQFramework.git
git clone https://github.com/ToolDAQ/zeromq-4.0.7.git
wget http://downloads.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.tar.gz
tar zxvf boost_1_60_0.tar.gz

cd zeromq-4.0.7

./configure --prefix=`pwd`
make
make install

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH

cd ../boost_1_60_0

mkdir install

./bootstrap.sh --prefix=`pwd`/install/ 
./b2 install 

export LD_LIBRARY_PATH=`pwd`/install/lib:$LD_LIBRARY_PATH

cd ../ToolDAQFramework

make clean
make

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH

cd ../

wget https://root.cern.ch/download/root_v5.34.34.source.tar.gz
tar zxvf root_v5.34.34.source.tar.gz
cd root

#sudo yum install git make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel

./configure --prefix=`pwd` --enable-rpath
make
make install

cd ../
#sudo yum install postgresql-devel.x86_64


wget http://pqxx.org/download/software/libpqxx/libpqxx-4.0.1.tar.gz
tar zxvf libpqxx-4.0.1.tar.gz
cd libpqxx-4.0.1
mkdir install
./configure --enable-shared --prefix=`pwd`/install
make
make install

cd ../../

make clean
make

ln -s configfiles/LoggerToolChain ./Logger


#as root
#route add -net 224.0.0.0 netmask 240.0.0.0 dev eth1

#webpage
#cp /home/annie/ANNIEDAQ/Webpage/*.cgi /var/www/cgi-bin/
#cp /home/annie/ANNIEDAQ/Webpage/*.cpp /var/www/cgi-bin
#cp /home/annie/ANNIEDAQ/Webpage/Makefile /var/www/cgi-bin
#cp /home/annie/ANNIEDAQ/Webpage/*.html /var/www/html/
#cp /home/annie/ANNIEDAQ/Webpage/*.png /var/www/html/
#cp /home/annie/ANNIEDAQ/Webpage/*.JPG /var/www/html/
#cp /home/annie/ANNIEDAQ/Webpage/styles.css /var/www/html/
#mkdir /data/logs
#mkdir /data/output
#mkdir /data/monitoring
#cd /va/www/html/
#ln -s /data/logs/ ./
#ln -s /data/output/ ./
#ln -s /data/monitoring/ ./
#chown -R annie:apache /data/

#cd /home/annie/ANNIEDAQ/ToolDAQ/
#wget ftp://ftp.gnu.org/gnu/cgicc/cgicc-3.2.9.tar.gz
#tar -zxvf cgicc-3.2.9.tar.gz
#cd cgicc-3.2.9
#./configure --prefix=/usr/local
#make
#make install

#cd ../boost_1_60_0
#cp -r install/include/* /usr/local/include/
#cp -r install/lib/* /usr/local/lib/

#cd ../zeromq-4.0.7/
#cp -r include/* /usr/local/include/
#cp -r lib/* /usr/local/lib/

#cd ../../
#cp -r include/* /usr/local/include/
#cp -r lib/* /usr/local/lib/

#cd /var/www/cgi-bin
#make

#cp /home/annie/ANNIEDAQ/setup/rc.local /etc/
#cp /home/annie/ANNIEDAQ/setup/ANNIEcgi.conf /etc/ld.so.conf.d/
#cp /home/annie/ANNIEDAQ/setup/ANNIE.conf /etc/httpd/conf.d/
###edit the daq name to correct one
#ldconfig
#service httpd restart

##postgresql
## as root
chown -R products:annie /local
chmod -R g+w /local

## as annie
source /local/ups/etc/setups.sh 
setup upd
upd install postgres v9_3_9_x64 -f Linux+2.6 -z /local/ups/db
ups declare -c postgres v9_3_9_x64
ups tailor postgres
  > /data/postgres
  > annie
  >
  > 
  >
## no longer needed chgrp  postgres /data/postgres/*
cp /home/annie/ANNIEDAQ/setup/postgresql.conf /data/postgres/
cp /home/annie/ANNIEDAQ/setup/pg_hba.conf /data/postgres/
/local/ups/prd/postgres/v9_3_9_x64/Linux-2-6/bin/postgres --config-file=/data/postgres/postgresql.conf &

##setting up dbs
psql postgres
  > create database annie;
  > \c annie
  > create table runinformation(ID bigserial primary key, runnumber bigint, subrunnumber bigint, starttime timestamp without time zone, stoptime timestamp without time zone, runtype integer, runstatus integer, numevents bigint);
 > create role root;
 > grant all privileges on runinformation to root;
 > grant usage, select on sequence runinformation_id_seq to root;
 > grant usage, update on sequence runinformation_id_seq to root;
 > create role apache;
 > alter role apache with login;
 > grant all privileges on runinformation to apache;
 > \q

##
