#!/usr/bin/perl

use warnings;
use strict;
use Time::localtime;
use Net::FTP;

my $ftp;
my $day_cnt = 2;
my $remote_system = "192.168.1.1";
my $username = "test123";
my $passwd = "test123";
my $ORG_PATTERN = "FKKKLOG";
#my $ORG_PATTERN = "FKCCLOG";
my $NEW_PATTERN = "FKCCLOG";
#my $NEW_PATTERN = "FKKKLOG";
my $BASE_PATH = '/home/dark264sh/ftp_test/SNLOG';
my $REMOTE_PATH = '/home/dark264sh/SNLOG';
my $cur_path;
my $LOGDIR = "/home/dark264sh/ftp_test";

connect_ftp();
do_while();
close_ftp();

sub do_while
{
	my $base_day = time - 86400 * ($day_cnt - 1);

	for(my $i = 0; $i < $day_cnt; $i++)
	{
		my $today = $base_day + 86400 * $i;
		my $tm = localtime($today);
		my $DAY = sprintf("%02d%02d%02d", (($tm->year)+1900) % 100, ($tm->mon)+1, $tm->mday);
		$cur_path = "$BASE_PATH/$DAY/";
		read_file("$cur_path"."*$ORG_PATTERN*");
	}
}

sub read_file
{
	my $path = shift;
	applog("read_file", "READ DIR=$path");
	my @files = glob($path);
	foreach my $file (sort @files)
	{
		applog("read_file", "FILE=$file");
		send_ftp($file);
		convert_name($file);
	}
}

sub convert_name
{
	my $org_fullname = shift;
	my $org_name = shift;
	my $cvt_name;
	my $ext_name;
	my $cnt;
	applog("convert_name", "ORG_NAME=$org_fullname");
	my @temp_path = split(/\//, $org_fullname);
	$cnt = @temp_path;
	$cnt = $cnt - 1;
	$org_name = $temp_path[$cnt];
	my @temp_ext = split(/\./, $org_name);
	$cnt = @temp_ext;
	if($cnt eq 2)
	{
		$ext_name = $temp_ext[1];
		my @temp_name = split(/_/, $temp_ext[0]);
		$cnt = @temp_name;
		if($cnt eq 5)
		{
			$cvt_name = "$cur_path"."$temp_name[0]_$temp_name[1]_"."$NEW_PATTERN"."_$temp_name[3]_$temp_name[4].$ext_name";
			rename($org_fullname, $cvt_name) or die "Cannot rename $!";
			applog("convert_name", "RENAME $org_fullname => $cvt_name");
		}
		else
		{
			applog("convert_name", "INVALID FILE NAME=$org_name");
		}
	}
	else
	{
		applog("convert_name", "INVALID FILE EXT NAME=$org_name");
	}
}

sub connect_ftp
{
	$ftp = Net::FTP->new($remote_system, Debug => 0) or die "Cannot connect to $remote_system: $@";
	$ftp->login($username, $passwd) or die "Cannot login ", $ftp->message;
	$ftp->cwd($REMOTE_PATH) or die "Cannot change working directory ", $ftp->message;
}

sub close_ftp
{
	$ftp->quit;
}

sub send_ftp
{
	my $name = shift;
	applog("send_ftp", "SEND FILE NAME=$name");
	$ftp->binary();
	$ftp->put($name) or die "file=$name put failed ", $ftp->message;
}

sub applog
{
	my @args = @_;
	my $tm = localtime(time);
	my $log = sprintf("[%04d-%02d-%02d %02d:%02d:%02d]", ($tm->year)+1900, ($tm->mon)+1, $tm->mday, $tm->hour, $tm->min, $tm->sec);

	$log = "$log [PID:$$]";
	foreach my $arg (@args) 
	{
		$log = "$log [$arg]";
	}
	$log = "$log\n";

    my $DIR = sprintf("%04d%02d%02d", ($tm->year)+1900, ($tm->mon)+1, $tm->mday);
    my $LOGFILENAME = "$LOGDIR/FTP_$DIR.log";
	open LOG, ">> $LOGFILENAME" || die $!;
	print LOG "$log";
	close LOG;
}

